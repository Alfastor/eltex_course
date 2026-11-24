#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <signal.h>
#include <time.h>

#define MAX_TOPICS 10
#define MAX_STR 256
#define MAX_SUBS 100
#define MAX_PUBS 100
#define MSG_TIMER 2

#define MSG_QUEUE_KEY 12345

typedef struct {
    long mtype;          
    char mtext[MAX_STR];
} Message;

typedef struct {
    pid_t pid;
    char topics[MAX_TOPICS][MAX_STR];
    int topic_count;
} SubscriberInfo;

#endif