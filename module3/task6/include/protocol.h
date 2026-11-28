#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <stddef.h>
#include <stdint.h>

#define CHAT_NAME_MAX 31
#define CHAT_MESSAGE_MAX 500
#define CHAT_PACKET_MAX (17 + CHAT_NAME_MAX + CHAT_MESSAGE_MAX)

typedef enum {
    PACKET_JOIN = 1,
    PACKET_CHAT = 2,
    PACKET_HEARTBEAT = 3,
    PACKET_LEAVE = 4
} PacketType;

typedef struct {
    PacketType type;
    uint64_t client_id;
    char name[CHAT_NAME_MAX + 1];
    char text[CHAT_MESSAGE_MAX + 1];
} ChatPacket;

int protocol_encode(const ChatPacket *packet, uint8_t *buffer, size_t capacity, size_t *written);
int protocol_decode(const uint8_t *buffer, size_t length, ChatPacket *packet);

#endif
