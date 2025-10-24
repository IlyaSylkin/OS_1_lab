#include "process.h"

void redirect_fd(int old_fd, int new_fd) {
    if (dup2(old_fd, new_fd) == -1) {
        write(1, "dup2 error\n", 11);
        exit(EXIT_FAILURE);
    }
    close(old_fd);
}