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
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    // имя файла от пользователя
    write(1, "Enter filename: ", 16);
    bytes_read = read(0, filename, sizeof(filename) - 1);
    if (bytes_read <= 0) {
        perror("read");
        exit(EXIT_FAILURE);
    }
    filename[bytes_read - 1] = '\0'; // Убираем \n

    // форкаем
    pid = fork();
    if (pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) {
        // ДОЧЕРНИЙ ПРОЦЕСС
        close(pipe1[0]);  // Закрываем чтение 
        
        // Перенаправляем вывод в pipe1[1]
        redirect_fd(pipe1[1], 1);
        
        // Запускаем дочернюю программу
        execl("./child", "child", filename, NULL);
        
        perror("execl");
        exit(EXIT_FAILURE);
    } else {
        // РОДИТЕЛЬСКИЙ ПРОЦЕСС
        close(pipe1[1]);  // Закрываем запись 
        
        write(1, "Parent process started reading from pipe...\n", 44);
        
        // Читаем данные из pipe1
        while ((bytes_read = read(pipe1[0], buffer, BUFFER_SIZE - 1)) > 0) {
            write(1, buffer, bytes_read);
        }
        
        close(pipe1[0]);
        
        // Ждем завершения дочернего процесса
        int status;
        waitpid(pid, &status, 0);
        
        if (WIFEXITED(status)) {
        char msg[50] = "Child process exited with code: ";
        char code_str[10];
        
        int exit_code = WEXITSTATUS(status);

        int i = 0;
        int temp = exit_code;
        
        do {
            code_str[i++] = '0' + (temp % 10);
            temp /= 10;
        } while (temp > 0);

        for (int j = i - 1; j >= 0; j--) {
            msg[strlen(msg)] = code_str[j];
        }
        
        msg[strlen(msg)] = '\n';
        
        write(1, msg, strlen(msg));
    }
    }

    return 0;
}