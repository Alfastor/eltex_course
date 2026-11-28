#include "app.h"
#include "ipc.h"
#include <stdio.h>
#include <string.h>

int main(int argc, char **argv)
{
    if (argc != 2) {
        fprintf(stderr, "Использование: %s producer|consumer|cleanup\n", argv[0]);
        return 2;
    }
    if (strcmp(argv[1], "cleanup") == 0) return ipc_cleanup_existing() == 0 ? 0 : 1;
    if (strcmp(argv[1], "producer") == 0) return run_producer();
    if (strcmp(argv[1], "consumer") == 0) return run_consumer();
    fprintf(stderr, "Неизвестный режим: %s\n", argv[1]);
    return 2;
}
