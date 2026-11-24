#include "protocol.h"
#include <unistd.h>
#include <string.h>

int protocol_send_header(int fd, const FileHeader* header) {
    return write(fd, header, sizeof(FileHeader)) == sizeof(FileHeader);
}

int protocol_recv_header(int fd, FileHeader* header) {
    return read(fd, header, sizeof(FileHeader)) == sizeof(FileHeader);
}

int protocol_send_ready(int fd) {
    char msg = 'R';
    return write(fd, &msg, 1) == 1;
}

int protocol_recv_ready(int fd) {
    char msg;
    return read(fd, &msg, 1) == 1;
}