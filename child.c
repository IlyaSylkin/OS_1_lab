#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <ctype.h>

#define MAX_NUMBERS 100
#define BUFFER_SIZE 1024

// Обертка для dup2
void redirect_fd(int old_fd, int new_fd) {
    if (dup2(old_fd, new_fd) == -1) {
        perror("dup2");
        exit(EXIT_FAILURE);
    }
    close(old_fd);
}

void float_to_string(float num, char *buffer) {
    int pos = 0;
    
    // Обрабатываем отрицательные числа
    if (num < 0) {
        buffer[pos++] = '-';
        num = -num;
    }
    
    // Целая часть
    int integer = (int)num;
    
    if (integer == 0) {
        buffer[pos++] = '0';
    } else {
        // Конвертируем целую часть в строку
        char digits[10];
        int digit_count = 0;
        int temp = integer;
        
        while (temp > 0) {
            digits[digit_count++] = '0' + (temp % 10);
            temp /= 10;
        }
        
        // Записываем в обратном порядке
        for (int i = digit_count - 1; i >= 0; i--) {
            buffer[pos++] = digits[i];
        }
    }
    
    // Десятичная часть
    buffer[pos++] = '.';
    
    int decimal = (int)((num - integer) * 1000 + 0.05); // Округление
    
    // Всегда 3 цифры после точки
    buffer[pos++] = '0' + (decimal / 100);
    buffer[pos++] = '0' + (decimal % 100 / 10);
    buffer[pos++] = '0' + (decimal % 10);
    
    buffer[pos] = '\0';
}

void process_line(char *line) {
    float numbers[MAX_NUMBERS];
    int count = 0;
    
    // Парсим числа из строки помещаем в массив nubmers
    char *token = strtok(line, " "); // возвр указатель на начало текщей подстроки line
    
    while (token != NULL && count < MAX_NUMBERS) {
        numbers[count++] = atof(token); //строку в double
        token = strtok(NULL, " ");
    }
    
    if (count < 2) {
        write(1, "Error: line must contain at least 2 numbers\n", 44);
        return;
    }
    
    // Выводим команду
    write(1, "Command: ", 9);
    for (int i = 0; i < count; i++) {
        char buffer[20];
        float_to_string(numbers[i], buffer);
        int len = strlen(buffer);
        write(1, buffer, len);
        write(1, " ", 1);
    }
    write(1, "\n", 1);
    
    // Последовательное деление
    float current_result = numbers[0];
    char num_buf[20];
    
    write(1, "Division results for ", 21);
    float_to_string(current_result, num_buf);
    write(1, num_buf, strlen(num_buf));
    write(1, ":\n", 2);
    
    for (int i = 1; i < count; i++) {
        float divisor = numbers[i];
        
        // Проверка деления на 0
        if (divisor == 0.0f) {
            write(1, "ERROR: division by zero! Exiting.\n", 34);
            exit(EXIT_FAILURE);
        }
        
        // Выполняем деление
        float previous_result = current_result;
        current_result = current_result / divisor;
        
        // Выводим шаг деления
        write(1, "  ", 2);
        float_to_string(previous_result, num_buf);
        write(1, num_buf, strlen(num_buf));
        
        write(1, " / ", 3);
        float_to_string(divisor, num_buf);
        write(1, num_buf, strlen(num_buf));
        
        write(1, " = ", 3);
        float_to_string(current_result, num_buf);
        write(1, num_buf, strlen(num_buf));
        write(1, "\n", 1);
    }
    write(1, "\n", 1);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        write(2, "Usage: child <filename>\n", 24);
        exit(EXIT_FAILURE);
    }

    const char *filename = argv[1];
    char line[BUFFER_SIZE];
    char buffer[BUFFER_SIZE * 2];
    size_t buffer_len = 0;
    size_t bytes_read;
    
    // Открываем файл для чтения
    int file_fd = open(filename, O_RDONLY);
    if (file_fd == -1) {
        write(2, "Error: cannot open file\n", 24);
        exit(EXIT_FAILURE);
    }
    
    // Перенаправляем стандартный ввод на файл
    redirect_fd(file_fd, 0);

    // Читаем и обрабатываем команды из файла с буферизацией
    while ((bytes_read = read(0, buffer + buffer_len, BUFFER_SIZE - buffer_len - 1)) > 0) {
        buffer_len += bytes_read;
        buffer[buffer_len] = '\0';
        
        char *start = buffer;
        char *end;
        
        // Обрабатываем каждую строку
        // первое вхождение '\n'
        while ((end = strchr(start, '\n')) != NULL) {
            *end = '\0';
            
            // strlen работает до '\0'
            if (strlen(start) > 0) {
                process_line(start);
            }
            
            start = end + 1;
        }
        
        // Переносим необработанный остаток в начало буфера
        // if (start > buffer) {
        //     buffer_len = buffer_len - (start - buffer);
        //     memmove(buffer, start, buffer_len);
        // } else if (buffer_len == BUFFER_SIZE - 1) {
        //     // Буфер полон, но нет символа новой строки
        //     write(2, "Error: line too long\n", 21);
        //     buffer_len = 0;
        // }
    }
    
    if (bytes_read == -1) {
        perror("read from file");
        exit(EXIT_FAILURE);
    }
    
    return 0;
}