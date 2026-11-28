#include "protocol.h"
#include "users.h"

#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

static void test_protocol_round_trip(void)
{
    ChatPacket source;
    ChatPacket decoded;
    uint8_t buffer[CHAT_PACKET_MAX];
    size_t written = 0;

    memset(&source, 0, sizeof(source));
    source.type = PACKET_CHAT;
    source.client_id = UINT64_C(0x123456789ABCDEF0);
    strcpy(source.name, "Алексей");
    strcpy(source.text, "Проверка UDP-чата");

    assert(protocol_encode(&source, buffer, sizeof(buffer), &written) == 0);
    assert(protocol_decode(buffer, written, &decoded) == 0);
    assert(decoded.type == source.type);
    assert(decoded.client_id == source.client_id);
    assert(strcmp(decoded.name, source.name) == 0);
    assert(strcmp(decoded.text, source.text) == 0);
}

static void test_protocol_rejects_invalid_packet(void)
{
    ChatPacket source;
    uint8_t buffer[CHAT_PACKET_MAX];
    size_t written = 0;

    memset(&source, 0, sizeof(source));
    source.type = PACKET_HEARTBEAT;
    source.client_id = 7;
    strcpy(source.name, "Tester");

    assert(protocol_encode(&source, buffer, sizeof(buffer), &written) == 0);
    assert(protocol_decode(buffer, written - 1, &(ChatPacket){0}) != 0);
    buffer[0] = 'X';
    assert(protocol_decode(buffer, written, &(ChatPacket){0}) != 0);
}

static void test_users_table(void)
{
    UserTable table;
    User removed;
    User expired[USERS_MAX];
    int is_new = 0;

    users_init(&table);
    assert(table.count == 0);

    assert(users_update(&table, 100, "Alice", 1000, &is_new) == 0);
    assert(is_new == 1);
    assert(table.count == 1);

    assert(users_update(&table, 100, "Alice", 2000, &is_new) == 0);
    assert(is_new == 0);
    assert(table.items[0].last_seen_ms == 2000);

    assert(users_update(&table, 200, "Bob", 2500, &is_new) == 0);
    assert(is_new == 1);
    assert(table.count == 2);

    assert(users_remove(&table, 100, &removed) == 1);
    assert(strcmp(removed.name, "Alice") == 0);
    assert(table.count == 1);

    assert(users_expire(&table, 8000, 5000, expired, USERS_MAX) == 1);
    assert(strcmp(expired[0].name, "Bob") == 0);
    assert(table.count == 0);
}

int main(void)
{
    test_protocol_round_trip();
    test_protocol_rejects_invalid_packet();
    test_users_table();
    puts("OK: все модульные тесты пройдены.");
    return 0;
}
