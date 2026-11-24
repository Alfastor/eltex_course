#include "common.h"

int run_publisher(const char* topic) {
    int qid = msgget(MSG_QUEUE_KEY, 0666);
    if (qid == -1) {
        perror("msgget (Брокер не запущен)");
        return EXIT_FAILURE;
    }

    Message reg_msg;
    reg_msg.mtype = 1;
    snprintf(reg_msg.mtext, MAX_STR, "register,<pid>"); 
    
    Message msg;
    msg.mtype = 1;
    int counter = 1;

    printf("Издатель запущен. Тема: %s. PID: %d\n", topic, getpid());

    while (1) {
        snprintf(msg.mtext, MAX_STR, "send,%d,%s,Message %d", getpid(), topic, counter++);
        
        if (msgsnd(qid, &msg, strlen(msg.mtext) + 1, 0) == -1) {
            perror("msgsnd");
            break;
        }
        printf("Отправлено: %s\n", msg.mtext);
        sleep(MSG_TIMER);
    }
    return EXIT_SUCCESS;
}