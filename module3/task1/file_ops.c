#include "file_ops.h"
#include <fcntl.h>
#include <unistd.h>

int file_open_read(const char* filename) {
    return open(filename, O_RDONLY);
}

int file_open_write(const char* filename) {
    return open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
}

long long file_get_size(int fd) {
    long long size = lseek(fd, 0, SEEK_END);
    lseek(fd, 0, SEEK_SET);
    return size;
}

ssize_t file_read_block(int fd, void* buf, size_t count) {
    return read(fd, buf, count);
}

ssize_t file_write_block(int fd, const void* buf, size_t count) {
    return write(fd, buf, count);
}