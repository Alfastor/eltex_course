#include "chat.h"

static mqd_t g_mq_out;
static bool g_running = true;

void sigint_handler(int sig) {
    (void)sig;
    ipc_mq_send(g_mq_out, MSG_STOP, EXIT_PRIORITY);
    g_running = false;
}

void chat_run(mqd_t mq_in, mqd_t mq_out, const char* my_name, bool is_creator, const char* q1_name, const char* q2_name) {
    g_mq_out = mq_out;
    signal(SIGINT, sigint_handler);

    struct pollfd fds[2];
    fds[0].fd = STDIN_FILENO;
    fds[0].events = POLLIN;
    fds[1].fd = mq_in;
    fds[1].events = POLLIN;

    char buf[MAX_MSG_SIZE];
    char input[MAX_MSG_SIZE];

    time_t last_recv_time = time(NULL);
    time_t last_send_time = time(NULL);
    
    printf("[%s] Чат запущен. Введите сообщение (или 'exit' для выхода):\n", my_name);

    while (g_running) {
        int ret = poll(fds, 2, 500);
        
        if (ret == -1) {
            if (errno == EINTR) continue;
            perror("poll");
            break;
        }

        time_t now = time(NULL);

        if (now - last_recv_time > HEARTBEAT_TIMEOUT) {
            printf("\n[Система] Собеседник не отвечает (Heartbeat timeout). Завершение...\n");
            break;
        }

        if (now - last_send_time >= HEARTBEAT_INTERVAL) {
            ipc_mq_send(mq_out, MSG_HEARTBEAT, HEARTBEAT_PRIORITY);
            last_send_time = now;
        }

        if (ret > 0 && (fds[0].revents & POLLIN)) {
            if (fgets(input, sizeof(input), stdin) != NULL) {
                input[strcspn(input, "\n")] = 0; 
                
                if (strcmp(input, "exit") == 0) {
                    ipc_mq_send(mq_out, MSG_STOP, EXIT_PRIORITY);
                    break;
                }
                
                if (strlen(input) > 0) {
                    ipc_mq_send(mq_out, input, 1);
                    last_send_time = now;
                }
            }
        }

        if (ret > 0 && (fds[1].revents & POLLIN)) {
            unsigned int prio;
            ssize_t bytes_read = ipc_mq_receive(mq_in, buf, sizeof(buf), &prio);
            if (bytes_read > 0) {
                last_recv_time = now; // Получили сообщение - обновили таймер

                if (prio == EXIT_PRIORITY || strcmp(buf, MSG_STOP) == 0) {
                    printf("\n[Система] Собеседник покинул чат.\n");
                    break;
                }
                else if (prio == HEARTBEAT_PRIORITY || strcmp(buf, MSG_HEARTBEAT) == 0) {
                }
                else {
                    printf("[Собеседник]: %s\n", buf);
                }
            }
        }
    }

    ipc_mq_close(mq_in);
    ipc_mq_close(mq_out);
    
    if (is_creator) {
        printf("[Система] Удаление очередей...\n");
        ipc_mq_remove(q1_name);
        ipc_mq_remove(q2_name);
    }
    printf("Чат завершен.\n");
}