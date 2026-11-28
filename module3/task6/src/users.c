#include "users.h"

#include <string.h>

void users_init(UserTable *table)
{
    if (table != NULL) {
        memset(table, 0, sizeof(*table));
    }
}

int users_update(UserTable *table, uint64_t client_id, const char *name, uint64_t now_ms, int *is_new)
{
    size_t i;

    if (table == NULL || name == NULL || name[0] == '\0') {
        return -1;
    }

    for (i = 0; i < table->count; ++i) {
        if (table->items[i].client_id == client_id) {
            strncpy(table->items[i].name, name, sizeof(table->items[i].name) - 1);
            table->items[i].name[sizeof(table->items[i].name) - 1] = '\0';
            table->items[i].last_seen_ms = now_ms;
            if (is_new != NULL) {
                *is_new = 0;
            }
            return 0;
        }
    }

    if (table->count >= USERS_MAX) {
        return -1;
    }

    table->items[table->count].client_id = client_id;
    strncpy(table->items[table->count].name, name, sizeof(table->items[table->count].name) - 1);
    table->items[table->count].name[sizeof(table->items[table->count].name) - 1] = '\0';
    table->items[table->count].last_seen_ms = now_ms;
    ++table->count;

    if (is_new != NULL) {
        *is_new = 1;
    }

    return 0;
}

int users_remove(UserTable *table, uint64_t client_id, User *removed)
{
    size_t i;

    if (table == NULL) {
        return -1;
    }

    for (i = 0; i < table->count; ++i) {
        if (table->items[i].client_id == client_id) {
            if (removed != NULL) {
                *removed = table->items[i];
            }
            if (i + 1 < table->count) {
                memmove(&table->items[i], &table->items[i + 1],
                        (table->count - i - 1) * sizeof(table->items[0]));
            }
            --table->count;
            return 1;
        }
    }

    return 0;
}

size_t users_expire(UserTable *table, uint64_t now_ms, uint64_t timeout_ms, User *expired, size_t capacity)
{
    size_t i = 0;
    size_t removed_count = 0;

    if (table == NULL) {
        return 0;
    }

    while (i < table->count) {
        if (now_ms >= table->items[i].last_seen_ms &&
            now_ms - table->items[i].last_seen_ms > timeout_ms) {
            if (expired != NULL && removed_count < capacity) {
                expired[removed_count] = table->items[i];
            }
            ++removed_count;
            if (i + 1 < table->count) {
                memmove(&table->items[i], &table->items[i + 1],
                        (table->count - i - 1) * sizeof(table->items[0]));
            }
            --table->count;
        } else {
            ++i;
        }
    }

    return removed_count;
}
