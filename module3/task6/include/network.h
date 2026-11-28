#ifndef NETWORK_H
#define NETWORK_H

#include <stddef.h>
#include <stdint.h>
#include <sys/types.h>

int network_open(uint16_t port);
int network_send(int socket_fd, const uint8_t *data, size_t length, uint16_t port);
ssize_t network_receive(int socket_fd, uint8_t *buffer, size_t capacity);

#endif
