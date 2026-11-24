#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>
#include <signal.h>
#include <errno.h>
#include <poll.h>
#include <stdbool.h>
#include <time.h>

#define MAX_MSG_SIZE 256
#define MAX_QUEUES 2
#define EXIT_PRIORITY 31
#define MSG_STOP "EXIT"

#define HEARTBEAT_PRIORITY 30
#define MSG_HEARTBEAT "HEARTBEAT"
#define HEARTBEAT_INTERVAL 2
#define HEARTBEAT_TIMEOUT 5

#endif