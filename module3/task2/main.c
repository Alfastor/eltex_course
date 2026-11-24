#include "common.h"

int run_broker();
int run_publisher(const char* topic);
int run_subscriber(int argc, char* argv[], int start_idx);

int main(int argc, char* argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Использование: %s [-b | -p <topic> | -s <topic1> <topic2> ...]\n", argv[0]);
        return EXIT_FAILURE;
    }

    if (strcmp(argv[1], "-b") == 0) {
        return run_broker();
    } else if (strcmp(argv[1], "-p") == 0) {
        if (argc < 3) {
            fprintf(stderr, "Ошибка: для издателя укажите тему (-p <topic>)\n");
            return EXIT_FAILURE;
        }
        return run_publisher(argv[2]);
    } else if (strcmp(argv[1], "-s") == 0) {
        if (argc < 3) {
            fprintf(stderr, "Ошибка: для подписчика укажите хотя бы одну тему (-s <topic>)\n");
            return EXIT_FAILURE;
        }
        return run_subscriber(argc, argv, 2);
    } else {
        fprintf(stderr, "Неизвестный ключ: %s\n", argv[1]);
        return EXIT_FAILURE;
    }
}