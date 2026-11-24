#include "common.h"
#include <signal.h>
#include <errno.h>

static int qid;
static pid_t my_pid;
static int topics_count;
static char** topics_list;

static volatile sig_atomic_t timeout_flag = 0;

void sigint_handler(int sig) {
    (void)sig;
    Message msg;
    msg.mtype = 1;
    for (int i = 0; i < topics_count; i++) {
        snprintf(msg.mtext, MAX_STR, "unsubscribe,%d,%s", my_pid, topics_list[i]);
        msgsnd(qid, &msg, strlen(msg.mtext) + 1, 0);
    }
    printf("\nПодписчик завершает работу.\n");
    exit(EXIT_SUCCESS);
}

void alarm_handler(int sig) {
    (void)sig;
    timeout_flag = 1;
}

int run_subscriber(int argc, char* argv[], int start_idx) {
    qid = msgget(MSG_QUEUE_KEY, 0666);
    if (qid == -1) {
        perror("msgget (Брокер не запущен)");
        return EXIT_FAILURE;
    }

    my_pid = getpid();
    topics_count = argc - start_idx;
    topics_list = &argv[start_idx];

    struct sigaction sa;
    sa.sa_handler = alarm_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0; 
    sigaction(SIGALRM, &sa, NULL);

    signal(SIGINT, sigint_handler);

    Message msg;
    msg.mtype = 1;
    for (int i = 0; i < topics_count; i++) {
        snprintf(msg.mtext, MAX_STR, "subscribe,%d,%s", my_pid, topics_list[i]);
        if (msgsnd(qid, &msg, strlen(msg.mtext) + 1, 0) == -1) {
            perror("msgsnd subscribe");
            return EXIT_FAILURE;
        }
        printf("Подписка на тему: %s\n", topics_list[i]);
    }

    printf("Ожидание сообщений (таймаут 5 сек)\n");

    while (1) {
        timeout_flag = 0;
        
        alarm(5); 

        ssize_t res = msgrcv(qid, &msg, MAX_STR, my_pid, 0);
        
        alarm(0); 

        if (res == -1) {
            if (errno == EINTR && timeout_flag) {
                printf("\n[Ошибка] Брокер не отвечает. Завершение...\n");
                break;
            } else {
                perror("msgrcv");
                break;
            }
        } else {
            if (strcmp(msg.mtext, "heartbeat") != 0) {
                printf("[Получено]: %s\n", msg.mtext);
            }
        }
    }
    return EXIT_SUCCESS;
}