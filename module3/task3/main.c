#include "common.h"
#include "ipc_posix.h"
#include "chat.h"

int main(int argc, char* argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Использование: %s <base_name>\n", argv[0]);
        return EXIT_FAILURE;
    }

    char q1_name[64], q2_name[64];
    snprintf(q1_name, sizeof(q1_name), "/%s_1", argv[1]);
    snprintf(q2_name, sizeof(q2_name), "/%s_2", argv[1]);

    mqd_t mq_in, mq_out;
    bool is_creator = false;

    mq_in = ipc_mq_create(q1_name);
    
    if (mq_in != (mqd_t)-1) {
        is_creator = true;
        mq_out = ipc_mq_create(q2_name);
        if (mq_out == (mqd_t)-1) {
            perror("mq_open _2");
            ipc_mq_remove(q1_name);
            return EXIT_FAILURE;
        }
        printf("[Система] Вы - Создатель. Принимаете из %s, отправляете в %s\n", q1_name, q2_name);
        chat_run(mq_in, mq_out, "Создатель", is_creator, q1_name, q2_name); // <------------------ ВЫЗОВ chat_run
    } 
    else if (errno == EEXIST) {
        is_creator = false;
        mq_in = ipc_mq_open_existing(q2_name); // Читаем из _2
        mq_out = ipc_mq_open_existing(q1_name); // Пишем в _1
        
        if (mq_in == (mqd_t)-1 || mq_out == (mqd_t)-1) {
            perror("mq_open existing");
            return EXIT_FAILURE;
        }
        printf("[Система] Вы - Участник. Принимаете из %s, отправляете в %s\n", q2_name, q1_name);
        chat_run(mq_in, mq_out, "Участник", is_creator, q1_name, q2_name); // <------------------ ВЫЗОВ chat_run
    } 
    else {
        perror("mq_open _1");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}