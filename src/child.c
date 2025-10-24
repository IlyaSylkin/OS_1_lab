#include "src/process.h"

void float_to_string(float num, char *buffer) {
    int pos = 0;
    
    // отр числа
    if (num < 0) {
        buffer[pos++] = '-';
        num = -num;
    }
    
    // Целая часть
    int integer = (int)num;
    
    if (integer == 0) {
        buffer[pos++] = '0';
    } else {

        char digits[10];
        int digit_count = 0;
        int temp = integer;
        
        while (temp > 0) {
            digits[digit_count++] = '0' + (temp % 10);
            temp /= 10;
        }
        
        // в обратном порядке
        for (int i = digit_count - 1; i >= 0; i--) {
            buffer[pos++] = digits[i];
        }
    }
    
    // Десятичная часть
    buffer[pos++] = '.';
    
    int decimal = (int)((num - integer) * 1000 + 0.05); 

    buffer[pos++] = '0' + (decimal / 100);
    buffer[pos++] = '0' + (decimal % 100 / 10);
    buffer[pos++] = '0' + (decimal % 10);
    
    buffer[pos] = '\0';
}

void process_line(char *line) {
    float numbers[MAX_NUMBERS];
    int count = 0;
    
    // Парсим числа из строки
    char *token = strtok(line, " ");
    
    while (token != NULL && count < MAX_NUMBERS) {
        numbers[count++] = atof(token);
        token = strtok(NULL, " ");
    }
    
    if (count < 2) {
        write(1, "Error: line must contain at least 2 numbers\n", 44);
        return;
    }
    
    write(1, "Command: ", 9);
    for (int i = 0; i < count; i++) {
        char buffer[20];
        float_to_string(numbers[i], buffer);
        int len = strlen(buffer);
        write(1, buffer, len);
        write(1, " ", 1);
    }
    write(1, "\n", 1);
    
    // деление
    float current_result = numbers[0];
    char num_buf[20];
    
    write(1, "Division results for ", 21);
    float_to_string(current_result, num_buf);
    write(1, num_buf, strlen(num_buf));
    write(1, ":\n", 2);
    
    for (int i = 1; i < count; i++) {
        float divisor = numbers[i];
 
        if (divisor == 0.0f) {
            write(1, "ERROR: division by zero! Exiting.\n", 34);
            exit(EXIT_FAILURE);
        }

        float previous_result = current_result;
        current_result = current_result / divisor;
 
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

int main(void) { 
    char buffer[BUFFER_SIZE * 2];
    size_t buffer_len = 0;
    int bytes_read;
    
    while ((bytes_read = read(0, buffer + buffer_len, BUFFER_SIZE - buffer_len - 1)) > 0) {
        buffer_len += bytes_read;
        buffer[buffer_len] = '\0';
        
        char *start = buffer;
        char *end;
        
        // Обрабатываем каж строку
        while ((end = strchr(start, '\n')) != NULL) {
            *end = '\0';
            
            if (strlen(start) > 0) {
                process_line(start);
            }
            
            start = end + 1;
        }
    }
    
    if (bytes_read == -1) {
        write(1, "read from file error\n", 21);
        exit(EXIT_FAILURE);
    }
    
    return 0;
}