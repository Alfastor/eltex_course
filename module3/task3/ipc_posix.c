#include "ipc_posix.h"

mqd_t ipc_mq_create(const char* name) {
    struct mq_attr attr;
    attr.mq_flags = 0;
    attr.mq_maxmsg = 10; // Максимум сообщений в очереди
    attr.mq_msgsize = MAX_MSG_SIZE;
    attr.mq_curmsgs = 0;
    
    return mq_open(name, O_RDWR | O_CREAT | O_EXCL, 0666, &attr);
}

mqd_t ipc_mq_open_existing(const char* name) {
    return mq_open(name, O_RDWR);
}

void ipc_mq_close(mqd_t mq) {
    mq_close(mq);
}

void ipc_mq_remove(const char* name) {
    mq_unlink(name);
}

bool ipc_mq_send(mqd_t mq, const char* msg, unsigned int prio) {
    if (mq_send(mq, msg, strlen(msg) + 1, prio) == -1) {
        perror("mq_send");
        return false;
    }
    return true;
}

ssize_t ipc_mq_receive(mqd_t mq, char* buf, size_t len, unsigned int* prio) {
    return mq_receive(mq, buf, len, prio);
}