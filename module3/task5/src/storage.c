#include "storage.h"
#include <string.h>
#include <stddef.h>
#include <time.h>

static size_t align_size(size_t value)
{
    size_t alignment = _Alignof(DataBlock);
    return (value + alignment - 1u) & ~(alignment - 1u);
}

void storage_initialize(void *memory, size_t capacity, uint32_t producer_pid)
{
    if (memory == NULL || capacity < sizeof(StorageHeader)) return;
    memset(memory, 0, capacity);
    StorageHeader *header = memory;
    header->magic = STORAGE_MAGIC;
    header->producer_pid = producer_pid;
    header->used = (uint32_t)align_size(sizeof(StorageHeader));
}

int storage_append(void *memory, size_t capacity, const int *values, uint32_t count)
{
    if (memory == NULL || values == NULL || count == 0 || count > STORAGE_MAX_VALUES) return -1;
    StorageHeader *header = memory;
    if (header->magic != STORAGE_MAGIC) return -1;
    size_t offset = align_size(header->used);
    size_t required = sizeof(DataBlock) + (size_t)count * sizeof(int);
    if (offset > capacity || required > capacity - offset) return 0;
    DataBlock *block = (DataBlock *)((unsigned char *)memory + offset);
    block->count = count;
    block->next_offset = 0;
    memcpy(block->values, values, (size_t)count * sizeof(int));
    if (header->last_offset != 0) {
        DataBlock *last = (DataBlock *)((unsigned char *)memory + header->last_offset);
        last->next_offset = (uint32_t)offset;
    } else {
        header->first_offset = (uint32_t)offset;
    }
    header->last_offset = (uint32_t)offset;
    header->used = (uint32_t)(offset + required);
    return 1;
}

DataBlock *storage_next_pending(void *memory, size_t capacity)
{
    if (memory == NULL) return NULL;
    StorageHeader *header = memory;
    if (header->magic != STORAGE_MAGIC) return NULL;
    uint32_t offset = header->first_offset;
    while (offset != 0) {
        if (offset > capacity || sizeof(DataBlock) > capacity - offset) return NULL;
        DataBlock *block = (DataBlock *)((unsigned char *)memory + offset);
        if (block->count > 0 && block->count <= STORAGE_MAX_VALUES && sizeof(DataBlock) + (size_t)block->count * sizeof(int) <= capacity - offset) return block;
        offset = block->next_offset;
    }
    return NULL;
}

int storage_minmax(const DataBlock *block, MinMax *result)
{
    if (block == NULL || result == NULL || block->count == 0 || block->count > STORAGE_MAX_VALUES) return -1;
    result->minimum = block->values[0];
    result->maximum = block->values[0];
    for (uint32_t i = 1; i < block->count; ++i) {
        if (block->values[i] < result->minimum) result->minimum = block->values[i];
        if (block->values[i] > result->maximum) result->maximum = block->values[i];
    }
    return 0;
}

int storage_all_processed(const void *memory, size_t capacity)
{
    if (memory == NULL) return 0;
    const StorageHeader *header = memory;
    if (header->magic != STORAGE_MAGIC || !header->producer_done) return 0;
    uint32_t offset = header->first_offset;
    while (offset != 0) {
        if (offset > capacity || sizeof(DataBlock) > capacity - offset) return 0;
        const DataBlock *block = (const DataBlock *)((const unsigned char *)memory + offset);
        if (block->count != 0) return 0;
        offset = block->next_offset;
    }
    return 1;
}

void storage_finish(void *memory)
{
    if (memory == NULL) return;
    StorageHeader *header = memory;
    if (header->magic == STORAGE_MAGIC) header->producer_done = 1;
}

uint64_t storage_monotonic_ns(void)
{
    struct timespec value;
    if (clock_gettime(CLOCK_MONOTONIC, &value) < 0) return 0;
    return (uint64_t)value.tv_sec * 1000000000ull + (uint64_t)value.tv_nsec;
}

int storage_register_consumer(void *memory, pid_t pid)
{
    if (memory == NULL || pid <= 0) return -1;
    StorageHeader *header = memory;
    if (header->magic != STORAGE_MAGIC) return -1;
    for (uint32_t i = 0; i < STORAGE_MAX_CONSUMERS; ++i) {
        if (header->consumer_pids[i] == (uint32_t)pid) {
            header->consumer_heartbeat_ns[i] = storage_monotonic_ns();
            header->consumer_seen = 1;
            return 0;
        }
    }
    for (uint32_t i = 0; i < STORAGE_MAX_CONSUMERS; ++i) {
        if (header->consumer_pids[i] == 0) {
            header->consumer_pids[i] = (uint32_t)pid;
            header->consumer_heartbeat_ns[i] = storage_monotonic_ns();
            header->consumer_seen = 1;
            return 0;
        }
    }
    return -1;
}

int storage_touch_consumer(void *memory, pid_t pid)
{
    if (memory == NULL || pid <= 0) return -1;
    StorageHeader *header = memory;
    if (header->magic != STORAGE_MAGIC) return -1;
    for (uint32_t i = 0; i < STORAGE_MAX_CONSUMERS; ++i) {
        if (header->consumer_pids[i] == (uint32_t)pid) {
            header->consumer_heartbeat_ns[i] = storage_monotonic_ns();
            return 0;
        }
    }
    return -1;
}

void storage_unregister_consumer(void *memory, pid_t pid)
{
    if (memory == NULL || pid <= 0) return;
    StorageHeader *header = memory;
    if (header->magic != STORAGE_MAGIC) return;
    for (uint32_t i = 0; i < STORAGE_MAX_CONSUMERS; ++i) {
        if (header->consumer_pids[i] == (uint32_t)pid) {
            header->consumer_pids[i] = 0;
            header->consumer_heartbeat_ns[i] = 0;
            return;
        }
    }
}

uint32_t storage_reap_consumers(void *memory, uint64_t now_ns)
{
    if (memory == NULL) return 0;
    StorageHeader *header = memory;
    if (header->magic != STORAGE_MAGIC) return 0;
    uint32_t active = 0;
    for (uint32_t i = 0; i < STORAGE_MAX_CONSUMERS; ++i) {
        if (header->consumer_pids[i] == 0) continue;
        uint64_t heartbeat = header->consumer_heartbeat_ns[i];
        if (now_ns >= heartbeat && now_ns - heartbeat > STORAGE_CONSUMER_TIMEOUT_NS) {
            header->consumer_pids[i] = 0;
            header->consumer_heartbeat_ns[i] = 0;
        } else {
            ++active;
        }
    }
    return active;
}

uint32_t storage_active_consumers(const void *memory)
{
    if (memory == NULL) return 0;
    const StorageHeader *header = memory;
    if (header->magic != STORAGE_MAGIC) return 0;
    uint32_t active = 0;
    for (uint32_t i = 0; i < STORAGE_MAX_CONSUMERS; ++i) {
        if (header->consumer_pids[i] != 0) ++active;
    }
    return active;
}
