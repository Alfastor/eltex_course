#define _POSIX_C_SOURCE 200809L

#include "console.h"
#include "network.h"
#include "protocol.h"
#include "users.h"

#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <poll.h>
#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/random.h>
#include <time.h>
#include <unistd.h>

#define DEFAULT_PORT 45454
#define HEARTBEAT_INTERVAL_MS 1000ULL
#define PEER_TIMEOUT_MS 5000ULL
#define POLL_INTERVAL_MS 200
#define INPUT_BUFFER_SIZE (CHAT_MESSAGE_MAX + 2)

static volatile sig_atomic_t stop_requested = 0;

static void handle_signal(int signal_number)
{
    (void)signal_number;
    stop_requested = 1;
}

static uint64_t monotonic_ms(void)
{
    struct timespec ts;

    if (clock_gettime(CLOCK_MONOTONIC, &ts) != 0) {
        return 0;
    }

    return (uint64_t)ts.tv_sec * 1000ULL + (uint64_t)ts.tv_nsec / 1000000ULL;
}

static uint64_t create_client_id(void)
{
    uint64_t id = 0;
    ssize_t received;
    int random_fd;

    received = getrandom(&id, sizeof(id), 0);
    if (received == (ssize_t)sizeof(id) && id != 0) {
        return id;
    }

    random_fd = open("/dev/urandom", O_RDONLY);
    if (random_fd >= 0) {
        received = read(random_fd, &id, sizeof(id));
        close(random_fd);
        if (received == (ssize_t)sizeof(id) && id != 0) {
            return id;
        }
    }

    id = monotonic_ms() ^ ((uint64_t)getpid() << 32U) ^ (uint64_t)(uintptr_t)&id;
    return id == 0 ? 1 : id;
}

static int valid_name(const char *name)
{
    size_t length;
    size_t i;

    if (name == NULL) {
        return 0;
    }
    length = strlen(name);
    if (length == 0 || length > CHAT_NAME_MAX) {
        return 0;
    }
    for (i = 0; i < length; ++i) {
        if ((unsigned char)name[i] < 32U || (unsigned char)name[i] == 127U) {
            return 0;
        }
    }
    return 1;
}

static int parse_port(const char *value, uint16_t *port)
{
    char *end = NULL;
    unsigned long parsed;

    errno = 0;
    parsed = strtoul(value, &end, 10);
    if (errno != 0 || end == value || *end != '\0' || parsed == 0 || parsed > 65535) {
        return -1;
    }
    *port = (uint16_t)parsed;
    return 0;
}

static int send_packet(int socket_fd, uint16_t port, uint64_t client_id,
                       const char *name, PacketType type, const char *text)
{
    ChatPacket packet;
    uint8_t buffer[CHAT_PACKET_MAX];
    size_t length = 0;

    memset(&packet, 0, sizeof(packet));
    packet.type = type;
    packet.client_id = client_id;
    strncpy(packet.name, name, sizeof(packet.name) - 1);
    if (text != NULL) {
        strncpy(packet.text, text, sizeof(packet.text) - 1);
    }

    if (protocol_encode(&packet, buffer, sizeof(buffer), &length) != 0) {
        console_error("Не удалось сформировать UDP-пакет.");
        return -1;
    }

    if (network_send(socket_fd, buffer, length, port) != 0) {
        console_error("Ошибка отправки UDP-пакета: %s", strerror(errno));
        return -1;
    }

    return 0;
}

static void announce_new_user(const ChatPacket *packet)
{
    if (packet->type == PACKET_JOIN) {
        console_info("[Система] %s подключился к чату.", packet->name);
    } else {
        console_info("[Система] Обнаружен участник: %s.", packet->name);
    }
}

static void process_packet(const uint8_t *data, size_t length, uint64_t own_id,
                           UserTable *users)
{
    ChatPacket packet;
    User removed;
    int is_new = 0;

    if (protocol_decode(data, length, &packet) != 0 || packet.client_id == own_id) {
        return;
    }

    if (packet.type == PACKET_LEAVE) {
        if (users_remove(users, packet.client_id, &removed) == 1) {
            console_info("[Система] %s покинул чат.", removed.name);
        }
        return;
    }

    if (users_update(users, packet.client_id, packet.name, monotonic_ms(), &is_new) != 0) {
        return;
    }

    if (is_new) {
        announce_new_user(&packet);
    }

    if (packet.type == PACKET_CHAT) {
        console_chat(packet.name, packet.text);
    }
}

static void process_line(char *line, int socket_fd, uint16_t port,
                         uint64_t client_id, const char *name,
                         const UserTable *users, int *running)
{
    size_t length = strlen(line);

    while (length > 0 && (line[length - 1] == '\r' || line[length - 1] == '\n')) {
        line[--length] = '\0';
    }

    if (length == 0) {
        return;
    }

    if (strcmp(line, "/quit") == 0 || strcmp(line, "/exit") == 0) {
        *running = 0;
        return;
    }
    if (strcmp(line, "/help") == 0) {
        console_help();
        return;
    }
    if (strcmp(line, "/users") == 0) {
        console_users(name, users);
        return;
    }
    if (line[0] == '/') {
        console_info("Неизвестная команда. Введите /help для просмотра команд.");
        return;
    }

    if (send_packet(socket_fd, port, client_id, name, PACKET_CHAT, line) == 0) {
        console_chat("Вы", line);
    }
}

int main(int argc, char **argv)
{
    const char *name;
    uint16_t port = DEFAULT_PORT;
    uint64_t client_id;
    uint64_t next_heartbeat;
    int socket_fd;
    int running = 1;
    int input_open = 1;
    int input_overflow = 0;
    char input_buffer[INPUT_BUFFER_SIZE];
    size_t input_used = 0;
    UserTable users;
    struct sigaction action;
    int is_terminal;

    if (argc < 2 || argc > 3) {
        fprintf(stderr, "Использование: %s <имя> [порт]\n", argv[0]);
        return EXIT_FAILURE;
    }

    name = argv[1];
    if (!valid_name(name)) {
        fprintf(stderr, "Имя должно содержать от 1 до %d байт и не содержать управляющих символов.\n",
                CHAT_NAME_MAX);
        return EXIT_FAILURE;
    }

    if (argc == 3 && parse_port(argv[2], &port) != 0) {
        fprintf(stderr, "Некорректный номер UDP-порта. Допустимый диапазон: 1–65535.\n");
        return EXIT_FAILURE;
    }

    memset(&action, 0, sizeof(action));
    action.sa_handler = handle_signal;
    sigemptyset(&action.sa_mask);
    if (sigaction(SIGINT, &action, NULL) != 0 ||
        sigaction(SIGTERM, &action, NULL) != 0) {
        perror("sigaction");
        return EXIT_FAILURE;
    }

    socket_fd = network_open(port);
    if (socket_fd < 0) {
        perror("Не удалось создать UDP-сокет");
        return EXIT_FAILURE;
    }

    client_id = create_client_id();
    users_init(&users);
    is_terminal = isatty(STDIN_FILENO);

    console_info("Вы вошли в чат под именем %s.", name);
    console_info("UDP broadcast: 255.255.255.255:%u", (unsigned int)port);
    console_info("Heartbeat: %llu мс; таймаут участника: %llu мс.",
                 (unsigned long long)HEARTBEAT_INTERVAL_MS,
                 (unsigned long long)PEER_TIMEOUT_MS);
    console_help();

    if (send_packet(socket_fd, port, client_id, name, PACKET_JOIN, NULL) != 0) {
        close(socket_fd);
        return EXIT_FAILURE;
    }
    next_heartbeat = monotonic_ms() + HEARTBEAT_INTERVAL_MS;

    if (is_terminal) {
        console_prompt();
    }

    while (running && !stop_requested) {
        struct pollfd descriptors[2];
        int descriptor_count = 1;
        int poll_result;
        uint8_t received_data[CHAT_PACKET_MAX + 1];
        ssize_t received;
        uint64_t now;

        descriptors[0].fd = socket_fd;
        descriptors[0].events = POLLIN;
        descriptors[0].revents = 0;

        if (input_open) {
            descriptors[1].fd = STDIN_FILENO;
            descriptors[1].events = POLLIN | POLLHUP;
            descriptors[1].revents = 0;
            descriptor_count = 2;
        }

        poll_result = poll(descriptors, (nfds_t)descriptor_count, POLL_INTERVAL_MS);
        if (poll_result < 0 && errno != EINTR) {
            console_error("Ошибка poll(): %s", strerror(errno));
            break;
        }

        if (descriptor_count == 2 &&
            (descriptors[1].revents & (POLLIN | POLLHUP | POLLERR)) != 0) {
            char chunk[256];
            ssize_t amount = read(STDIN_FILENO, chunk, sizeof(chunk));

            if (amount == 0) {
                input_open = 0;
                running = 0;
            } else if (amount < 0 && errno != EINTR && errno != EAGAIN) {
                console_error("Ошибка чтения ввода: %s", strerror(errno));
                running = 0;
            } else if (amount > 0) {
                ssize_t i;
                for (i = 0; i < amount; ++i) {
                    if (chunk[i] == '\n') {
                        if (!input_overflow) {
                            input_buffer[input_used] = '\0';
                            process_line(input_buffer, socket_fd, port, client_id,
                                         name, &users, &running);
                        } else {
                            console_info("Сообщение слишком длинное; максимум %d байт.", CHAT_MESSAGE_MAX);
                        }
                        input_used = 0;
                        input_overflow = 0;
                        if (running && is_terminal) {
                            console_prompt();
                        }
                    } else if (!input_overflow) {
                        if (input_used < CHAT_MESSAGE_MAX) {
                            input_buffer[input_used++] = chunk[i];
                        } else {
                            input_overflow = 1;
                        }
                    }
                }
            }
        }

        if (descriptors[0].revents & POLLIN) {
            for (;;) {
                received = network_receive(socket_fd, received_data, sizeof(received_data));
                if (received < 0) {
                    if (errno != EAGAIN && errno != EWOULDBLOCK && errno != EINTR) {
                        console_error("Ошибка приёма UDP-пакета: %s", strerror(errno));
                    }
                    break;
                }
                process_packet(received_data, (size_t)received, client_id, &users);
            }
        }

        now = monotonic_ms();
        if (now >= next_heartbeat) {
            send_packet(socket_fd, port, client_id, name, PACKET_HEARTBEAT, NULL);
            next_heartbeat = now + HEARTBEAT_INTERVAL_MS;
        }

        {
            User expired[USERS_MAX];
            size_t expired_count = users_expire(&users, now, PEER_TIMEOUT_MS,
                                                expired, USERS_MAX);
            size_t i;
            for (i = 0; i < expired_count && i < USERS_MAX; ++i) {
                console_info("[Система] %s отключился: нет Heartbeat более %llu мс.",
                             expired[i].name, (unsigned long long)PEER_TIMEOUT_MS);
            }
        }
    }

    send_packet(socket_fd, port, client_id, name, PACKET_LEAVE, NULL);
    close(socket_fd);
    console_info("Вы вышли из чата.");
    return EXIT_SUCCESS;
}
