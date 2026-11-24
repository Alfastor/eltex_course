#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include "ipc.h"
#include "protocol.h"
#include "file_ops.h"

#define BLOCK_SIZE 1024

void run_child(IpcChannels* ch) {
    ipc_child_close_unused(ch);
    FileHeader header;
    char buffer[BLOCK_SIZE];

    while (1) {
        protocol_send_ready(ch->c2p_write);
        if (!protocol_recv_header(ch->p2c_read, &header)) break;
        if (header.is_finished) break;

        char copy_name[MAX_FILENAME + 10];
        snprintf(copy_name, sizeof(copy_name), "%s.copy", header.filename);

        int out_fd = file_open_write(copy_name);
        if (out_fd == -1) {
            fprintf(stderr, "[Ребенок] Ошибка создания файла %s\n", copy_name);
            continue; 
        }

        long long bytes_left = header.size;
        while (bytes_left > 0) {
            ssize_t r = read(ch->p2c_read, buffer, (bytes_left > BLOCK_SIZE) ? BLOCK_SIZE : bytes_left);
            if (r <= 0) break;
            file_write_block(out_fd, buffer, r);
            bytes_left -= r;
        }
        close(out_fd);
    }
    close(ch->p2c_read);
    close(ch->c2p_write);
    exit(EXIT_SUCCESS);
}

void run_parent(IpcChannels* ch, int start_idx, int argc, char* argv[]) {
    ipc_parent_close_unused(ch);
    char buffer[BLOCK_SIZE];

    for (int i = start_idx; i < argc; i++) {
        int in_fd = file_open_read(argv[i]);
        if (in_fd == -1) {
            fprintf(stderr, "[Родитель] Файл не найден: %s\n", argv[i]);
            continue;
        }

        if (!protocol_recv_ready(ch->c2p_read))
        {
            fprintf(stderr, "Ошибка: дочерний процесс не готов или завершился. \n");
            return;
        }

        FileHeader header = {0};
        strncpy(header.filename, argv[i], MAX_FILENAME - 1);
        header.size = file_get_size(in_fd);
        header.is_finished = 0;

        protocol_send_header(ch->p2c_write, &header);

        ssize_t bytes_read;
        while ((bytes_read = file_read_block(in_fd, buffer, BLOCK_SIZE)) > 0) {
            write(ch->p2c_write, buffer, bytes_read);
        }
        close(in_fd);
    }

    protocol_recv_ready(ch->c2p_read);
    FileHeader finish = {.is_finished = 1};
    protocol_send_header(ch->p2c_write, &finish);

    ipc_cleanup(ch);
    wait(NULL);
    printf("Копирование завершено.\n");
}

int main(int argc, char* argv[]) {
    char* pipe_name = NULL;
    int start_idx = 1;

    if (argc > 2 && strcmp(argv[1], "-p") == 0) {
        pipe_name = argv[2];
        start_idx = 3;
    }

    if (start_idx >= argc) {
        fprintf(stderr, "Использование: %s [-p имя_канала] файл1 файл2 ...\n", argv[0]);
        return EXIT_FAILURE;
    }

    IpcChannels ch;
    if (ipc_init(&ch, pipe_name) == -1) {
        perror("Ошибка инициализации каналов");
        return EXIT_FAILURE;
    }

    pid_t pid = fork();
    if (pid == -1) {
        perror("fork");
        return EXIT_FAILURE;
    }

    if (pid == 0) {
        run_child(&ch);
    } else {
        run_parent(&ch, start_idx, argc, argv);
    }

    return 0;
}