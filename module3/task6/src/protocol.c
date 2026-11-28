#define _POSIX_C_SOURCE 200809L

#include "protocol.h"

#include <string.h>

#define HEADER_SIZE 17
#define PROTOCOL_VERSION 1

static const uint8_t MAGIC[4] = {'U', 'D', 'C', 'H'};

static int valid_type(PacketType type)
{
    return type >= PACKET_JOIN && type <= PACKET_LEAVE;
}

int protocol_encode(const ChatPacket *packet, uint8_t *buffer, size_t capacity, size_t *written)
{
    size_t name_length;
    size_t text_length;
    size_t total_length;
    size_t i;

    if (packet == NULL || buffer == NULL || written == NULL || !valid_type(packet->type)) {
        return -1;
    }

    name_length = strnlen(packet->name, CHAT_NAME_MAX + 1);
    text_length = strnlen(packet->text, CHAT_MESSAGE_MAX + 1);

    if (name_length == 0 || name_length > CHAT_NAME_MAX || text_length > CHAT_MESSAGE_MAX) {
        return -1;
    }

    if (packet->type != PACKET_CHAT && text_length != 0) {
        return -1;
    }

    total_length = HEADER_SIZE + name_length + text_length;
    if (capacity < total_length) {
        return -1;
    }

    memcpy(buffer, MAGIC, sizeof(MAGIC));
    buffer[4] = PROTOCOL_VERSION;
    buffer[5] = (uint8_t)packet->type;

    for (i = 0; i < 8; ++i) {
        buffer[6 + i] = (uint8_t)(packet->client_id >> (56U - (unsigned int)(i * 8U)));
    }

    buffer[14] = (uint8_t)name_length;
    buffer[15] = (uint8_t)(text_length >> 8U);
    buffer[16] = (uint8_t)text_length;
    memcpy(buffer + HEADER_SIZE, packet->name, name_length);
    memcpy(buffer + HEADER_SIZE + name_length, packet->text, text_length);

    *written = total_length;
    return 0;
}

int protocol_decode(const uint8_t *buffer, size_t length, ChatPacket *packet)
{
    size_t name_length;
    size_t text_length;
    size_t i;
    uint64_t client_id = 0;
    PacketType type;

    if (buffer == NULL || packet == NULL || length < HEADER_SIZE) {
        return -1;
    }

    if (memcmp(buffer, MAGIC, sizeof(MAGIC)) != 0 || buffer[4] != PROTOCOL_VERSION) {
        return -1;
    }

    type = (PacketType)buffer[5];
    if (!valid_type(type)) {
        return -1;
    }

    name_length = buffer[14];
    text_length = ((size_t)buffer[15] << 8U) | buffer[16];

    if (name_length == 0 || name_length > CHAT_NAME_MAX || text_length > CHAT_MESSAGE_MAX) {
        return -1;
    }

    if (length != HEADER_SIZE + name_length + text_length) {
        return -1;
    }

    if (type != PACKET_CHAT && text_length != 0) {
        return -1;
    }

    for (i = 0; i < 8; ++i) {
        client_id = (client_id << 8U) | buffer[6 + i];
    }

    memset(packet, 0, sizeof(*packet));
    packet->type = type;
    packet->client_id = client_id;
    memcpy(packet->name, buffer + HEADER_SIZE, name_length);
    packet->name[name_length] = '\0';
    if (text_length > 0) {
        memcpy(packet->text, buffer + HEADER_SIZE + name_length, text_length);
    }
    packet->text[text_length] = '\0';

    return 0;
}
