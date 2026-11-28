#ifndef STORAGE_H
#define STORAGE_H

#include <stddef.h>
#include <stdint.h>
#include <sys/types.h>

#define STORAGE_MAGIC 0x4D334D50u
#define STORAGE_SIZE 4096u
#define STORAGE_MAX_VALUES 8u
#define STORAGE_MAX_CONSUMERS 32u
#define STORAGE_CONSUMER_TIMEOUT_NS 3000000000ull

typedef struct {
    uint32_t magic;
    uint32_t used;
    uint32_t first_offset;
    uint32_t last_offset;
    uint32_t producer_done;
    uint32_t producer_pid;
    uint32_t consumer_seen;
    uint32_t consumer_pids[STORAGE_MAX_CONSUMERS];
    uint64_t consumer_heartbeat_ns[STORAGE_MAX_CONSUMERS];
} StorageHeader;

typedef struct {
    uint32_t count;
    uint32_t next_offset;
    int values[];
} DataBlock;

typedef struct {
    int minimum;
    int maximum;
} MinMax;

void storage_initialize(void *memory, size_t capacity, uint32_t producer_pid);
int storage_append(void *memory, size_t capacity, const int *values, uint32_t count);
DataBlock *storage_next_pending(void *memory, size_t capacity);
int storage_minmax(const DataBlock *block, MinMax *result);
int storage_all_processed(const void *memory, size_t capacity);
void storage_finish(void *memory);
uint64_t storage_monotonic_ns(void);
int storage_register_consumer(void *memory, pid_t pid);
int storage_touch_consumer(void *memory, pid_t pid);
void storage_unregister_consumer(void *memory, pid_t pid);
uint32_t storage_reap_consumers(void *memory, uint64_t now_ns);
uint32_t storage_active_consumers(const void *memory);

#endif
