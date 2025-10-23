#ifndef PROCESS_H
#define PROCESS_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#define MAX_NUMBERS 100
#define BUFFER_SIZE 1024

// Объявление функции вместо определения
void redirect_fd(int old_fd, int new_fd);

#endif