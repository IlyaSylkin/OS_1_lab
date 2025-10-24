#include <sys/wait.h>
#include "src/process.h"



int main() {
    int pipe1[2];
    pid_t pid;
    char filename[100];
    char buffer[BUFFER_SIZE];
    size_t bytes_read;

    // Создаем pipe1
    if (pipe(pipe1) == -1) {
        write(2, "pipe error\n", 11);
        exit(EXIT_FAILURE);
    }

    write(1, "Enter filename: ", 16);
    bytes_read = read(0, filename, sizeof(filename) - 1);
    if (bytes_read <= 0) {
        write(2, "read error\n", 11);
        exit(EXIT_FAILURE);
    }
    filename[bytes_read - 1] = '\0'; // Убираем \n

    int file_fd = open(filename, O_RDONLY);
    if (file_fd == -1) {
        write(2, "Error: cannot open file\n", 24);
        exit(EXIT_FAILURE);
    }

    pid = fork();
    if (pid == -1) {
        write(2, "fork error\n", 11);
        exit(EXIT_FAILURE);
    }

    if (pid == 0) {
        close(pipe1[0]); 
        
        // Перенаправляем вывод в pipe1[1]
        redirect_fd(pipe1[1], 1);
        
        redirect_fd(file_fd, 0);

        execl("./child", "child", NULL); 

        write(2, "execl error\n", 12);
        exit(EXIT_FAILURE);
    } else {
        close(pipe1[1]);  
        close(file_fd); 
        
        while ((bytes_read = read(pipe1[0], buffer, BUFFER_SIZE - 1)) > 0) {
            write(1, buffer, bytes_read);
        }
        
        close(pipe1[0]);
        
        // Ждем доч
        int status;
        waitpid(pid, &status, 0);
        
        if (WIFEXITED(status)) {
            char msg[50] = "Child process exited with code: ";
            char code_str[10];
            
            int exit_code = WEXITSTATUS(status);
            int msg_len = strlen(msg);  

            int i = 0;
            int temp = exit_code;
            
            do {
                code_str[i++] = '0' + (temp % 10);
                temp /= 10;
            } while (temp > 0);

            // Добавляем цифры
            for (int j = i - 1; j >= 0; j--) {
                msg[msg_len++] = code_str[j];
            }
            
            msg[msg_len++] = '\n';
            msg[msg_len] = '\0'; 
            
            write(1, msg, msg_len);
        }
    }

    return 0;
}