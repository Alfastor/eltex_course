#ifndef USERS_H
#define USERS_H

#include <stddef.h>
#include <stdint.h>

#define USERS_MAX 128

typedef struct {
    uint64_t client_id;
    char name[32];
    uint64_t last_seen_ms;
} User;

typedef struct {
    User items[USERS_MAX];
    size_t count;
} UserTable;

void users_init(UserTable *table);
int users_update(UserTable *table, uint64_t client_id, const char *name, uint64_t now_ms, int *is_new);
int users_remove(UserTable *table, uint64_t client_id, User *removed);
size_t users_expire(UserTable *table, uint64_t now_ms, uint64_t timeout_ms, User *expired, size_t capacity);

#endif
