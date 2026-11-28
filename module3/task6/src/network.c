#include "network.h"

#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

int network_open(uint16_t port)
{
    int fd;
    int enabled = 1;
    int flags;
    struct sockaddr_in address;

    fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (fd < 0) {
        return -1;
    }

    if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &enabled, sizeof(enabled)) < 0) {
        close(fd);
        return -1;
    }

    if (setsockopt(fd, SOL_SOCKET, SO_BROADCAST, &enabled, sizeof(enabled)) < 0) {
        close(fd);
        return -1;
    }

    memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = htonl(INADDR_ANY);
    address.sin_port = htons(port);

    if (bind(fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        close(fd);
        return -1;
    }

    flags = fcntl(fd, F_GETFL, 0);
    if (flags < 0 || fcntl(fd, F_SETFL, flags | O_NONBLOCK) < 0) {
        close(fd);
        return -1;
    }

    return fd;
}

int network_send(int socket_fd, const uint8_t *data, size_t length, uint16_t port)
{
    struct sockaddr_in destination;
    ssize_t sent;

    memset(&destination, 0, sizeof(destination));
    destination.sin_family = AF_INET;
    destination.sin_port = htons(port);
    destination.sin_addr.s_addr = htonl(INADDR_BROADCAST);

    sent = sendto(socket_fd, data, length, 0,
                  (struct sockaddr *)&destination, sizeof(destination));
    if (sent < 0) {
        return -1;
    }
    if ((size_t)sent != length) {
        errno = EIO;
        return -1;
    }

    return 0;
}

ssize_t network_receive(int socket_fd, uint8_t *buffer, size_t capacity)
{
    return recvfrom(socket_fd, buffer, capacity, 0, NULL, NULL);
}
