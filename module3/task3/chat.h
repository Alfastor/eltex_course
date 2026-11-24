#ifndef CHAT_H
#define CHAT_H

#include "common.h"
#include "ipc_posix.h"

void chat_run(mqd_t mq_in, mqd_t mq_out, const char* my_name, bool is_creator, const char* q1_name, const char* q2_name);

#endif // CHAT_H