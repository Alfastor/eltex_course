#ifndef IPC_POSIX_H
#define IPC_POSIX_H

#include "common.h"

mqd_t ipc_mq_create(const char* name);
mqd_t ipc_mq_open_existing(const char* name);
void ipc_mq_close(mqd_t mq);
void ipc_mq_remove(const char* name);
bool ipc_mq_send(mqd_t mq, const char* msg, unsigned int prio);
ssize_t ipc_mq_receive(mqd_t mq, char* buf, size_t len, unsigned int* prio);

#endif // IPC_POSIX_H