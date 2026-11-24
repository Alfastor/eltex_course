#ifndef FILE_OPS_H
#define FILE_OPS_H
#include <sys/types.h>

int file_open_read(const char* filename);
int file_open_write(const char* filename);
long long file_get_size(int fd);
ssize_t file_read_block(int fd, void* buf, size_t count);
ssize_t file_write_block(int fd, const void* buf, size_t count);

#endif