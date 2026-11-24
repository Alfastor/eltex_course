#include "common.h"
#include <signal.h>
#include <errno.h>
#include <time.h>

#define PID_FILE "/tmp/msg_broker.pid"

static int qid;
static SubscriberInfo subscribers[MAX_SUBS];
static int sub_count = 0;

int is_process_alive(pid_t pid) {
    return (kill(pid, 0) == 0);
}

void broker_sigint_handler(int sig) {
    (void)sig;
    printf("\n[Брокер] Получен сигнал завершения. Рассылаем SIGINT и удаляем очередь...\n");
    
    for (int i = 0; i < sub_count; i++) {
        kill(subscribers[i].pid, SIGINT);
    }
    
    msgctl(qid, IPC_RMID, NULL);
    unlink(PID_FILE);
    exit(EXIT_SUCCESS);
}

int find_subscriber(pid_t pid) {
    for (int i = 0; i < sub_count; i++) {
        if (subscribers[i].pid == pid) return i;
    }
    return -1;
}

int run_broker() {
    qid = msgget(MSG_QUEUE_KEY, 0666);
    
    if (qid != -1) {
        FILE* f = fopen(PID_FILE, "r");
        if (f) {
            pid_t old_pid = 0;
            if (fscanf(f, "%d", &old_pid) == 1) {
                if (is_process_alive(old_pid)) {
                    fprintf(stderr, "Ошибка: Брокер уже запущен (PID: %d).\n", old_pid);
                    fclose(f);
                    return EXIT_FAILURE;
                }
            }
            fclose(f);
        }
        
        printf("[Брокер] Обнаружена зависшая очередь. Удаление...\n");
        msgctl(qid, IPC_RMID, NULL);
    }

    qid = msgget(MSG_QUEUE_KEY, IPC_CREAT | 0666);
    if (qid == -1) {
        perror("msgget");
        return EXIT_FAILURE;
    }

    FILE* f = fopen(PID_FILE, "w");
    if (f) {
        fprintf(f, "%d", getpid());
        fclose(f);
    }

    signal(SIGINT, broker_sigint_handler);
    printf("[Брокер] Запущен. Ожидание сообщений...\n");

    Message msg;
    time_t last_heartbeat = time(NULL);

    while (1) {
        ssize_t res = msgrcv(qid, &msg, MAX_STR, 1, IPC_NOWAIT);
        
        if (res != -1) {
            char* command = strtok(msg.mtext, ",");
            if (!command) continue;

            if (strcmp(command, "subscribe") == 0) {
                char* pid_str = strtok(NULL, ",");
                char* topic = strtok(NULL, ",");
                if (pid_str && topic) {
                    pid_t pid = atoi(pid_str);
                    int idx = find_subscriber(pid);
                    if (idx == -1) {
                        idx = sub_count++;
                        subscribers[idx].pid = pid;
                        subscribers[idx].topic_count = 0;
                    }
                    if (subscribers[idx].topic_count < MAX_TOPICS) {
                        strncpy(subscribers[idx].topics[subscribers[idx].topic_count++], topic, MAX_STR);
                        printf("[Брокер] Подписка: PID %d -> %s\n", pid, topic);
                    }
                }
            } 
            else if (strcmp(command, "unsubscribe") == 0) {
                char* pid_str = strtok(NULL, ",");
                char* topic = strtok(NULL, ",");
                if (pid_str && topic) {
                    pid_t pid = atoi(pid_str);
                    int idx = find_subscriber(pid);
                    if (idx != -1) {
                        for (int i = 0; i < subscribers[idx].topic_count; i++) {
                            if (strcmp(subscribers[idx].topics[i], topic) == 0) {
                                for (int j = i; j < subscribers[idx].topic_count - 1; j++) {
                                    strcpy(subscribers[idx].topics[j], subscribers[idx].topics[j+1]);
                                }
                                subscribers[idx].topic_count--;
                                break;
                            }
                        }
                    }
                }
            } 
            else if (strcmp(command, "send") == 0) {
                char* pid_str = strtok(NULL, ",");
                char* topic = strtok(NULL, ",");
                char* payload = strtok(NULL, ""); 
                
                if (pid_str && topic && payload) {
                    printf("[Брокер] Получено сообщение по теме '%s'\n", topic);
                    for (int i = 0; i < sub_count; i++) {
                        for (int j = 0; j < subscribers[i].topic_count; j++) {
                            if (strcmp(subscribers[i].topics[j], topic) == 0) {
                                Message out_msg;
                                out_msg.mtype = subscribers[i].pid; 
                                snprintf(out_msg.mtext, MAX_STR, "Topic [%s]: %s", topic, payload);
                                msgsnd(qid, &out_msg, strlen(out_msg.mtext) + 1, IPC_NOWAIT);
                                break;
                            }
                        }
                    }
                }
            }
        } else if (errno != ENOMSG) {
            perror("msgrcv");
        }

        time_t now = time(NULL);
        if (now - last_heartbeat >= 2) {
            Message hb;
            strcpy(hb.mtext, "heartbeat");
            for (int i = 0; i < sub_count; i++) {
                hb.mtype = subscribers[i].pid;
                msgsnd(qid, &hb, strlen(hb.mtext) + 1, IPC_NOWAIT);
            }
            last_heartbeat = now;
        }

        // 3. Пауза 100 мс для снижения нагрузки на CPU
        usleep(100000); 
    }
    return EXIT_SUCCESS;
}