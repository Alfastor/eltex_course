#ifndef PROTOCOL_H
#define PROTOCOL_H

#define MAX_FILENAME 256

typedef struct {
    char filename[MAX_FILENAME];
    long long size;
    int is_finished;
} FileHeader;

// Возвращают 1 при успехе, 0 при ошибке/EOF
int protocol_send_header(int fd, const FileHeader* header);
int protocol_recv_header(int fd, FileHeader* header);
int protocol_send_ready(int fd);
int protocol_recv_ready(int fd);

#endif