#include "app.h"
#include "ipc.h"
#include "storage.h"
#include "console.h"
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>

#define PRODUCER_NO_CONSUMER_TIMEOUT_NS 5000000000ull
#define PRODUCER_STARTUP_TIMEOUT_NS 10000000000ull
#define PRODUCER_MIN_VALUE (-100)
#define PRODUCER_MAX_VALUE 100

static volatile sig_atomic_t stop_requested;

static void handle_signal(int signal_number)
{
    (void)signal_number;
    stop_requested = 1;
}

static uint32_t update_consumer_count(void *memory)
{
    return storage_reap_consumers(memory, storage_monotonic_ns());
}

int run_producer(void)
{
    int shm_fd;
    sem_t *semaphore;
    void *memory;
    if (ipc_create(&shm_fd, &semaphore, &memory) < 0) {
        console_error("Не удалось создать POSIX shared memory или POSIX semaphore");
        return 1;
    }
    signal(SIGINT, handle_signal);
    signal(SIGTERM, handle_signal);
    storage_initialize(memory, STORAGE_SIZE, (uint32_t)getpid());
    srand((unsigned int)(time(NULL) ^ getpid()));
    console_producer_started(STORAGE_SIZE);
    unsigned int generated = 0;
    uint64_t no_consumers_since = storage_monotonic_ns();
    int abandoned = 0;

    while (!stop_requested) {
        if (ipc_lock(semaphore) < 0) break;
        uint32_t active = update_consumer_count(memory);
        StorageHeader *header = memory;
        uint64_t now = storage_monotonic_ns();
        if (active > 0) {
            no_consumers_since = 0;
        } else {
            if (no_consumers_since == 0) no_consumers_since = now;
            uint64_t timeout = header->consumer_seen ? PRODUCER_NO_CONSUMER_TIMEOUT_NS : PRODUCER_STARTUP_TIMEOUT_NS;
            if (now >= no_consumers_since && now - no_consumers_since >= timeout) {
                storage_finish(memory);
                ipc_unlock(semaphore);
                console_no_consumers();
                abandoned = 1;
                break;
            }
        }

        int values[STORAGE_MAX_VALUES];
        uint32_t count = (uint32_t)(rand() % STORAGE_MAX_VALUES + 1);
        for (uint32_t i = 0; i < count; ++i) values[i] = rand() % (PRODUCER_MAX_VALUE - PRODUCER_MIN_VALUE + 1) + PRODUCER_MIN_VALUE;
        int result = storage_append(memory, STORAGE_SIZE, values, count);
        if (result == 0 || result < 0) storage_finish(memory);
        if (ipc_unlock(semaphore) < 0) break;
        if (result != 1) {
            console_producer_full(generated);
            break;
        }
        ++generated;
        console_block_created(generated, count);
        usleep(250000);
    }

    if (!abandoned) {
        if (ipc_lock(semaphore) == 0) {
            storage_finish(memory);
            ipc_unlock(semaphore);
        }
    }
    console_producer_waiting();

    if (!abandoned) {
        no_consumers_since = 0;
        for (;;) {
            if (ipc_lock(semaphore) < 0) break;
            int finished = storage_all_processed(memory, STORAGE_SIZE);
            uint32_t active = update_consumer_count(memory);
            uint64_t now = storage_monotonic_ns();
            if (active > 0) {
                no_consumers_since = 0;
            } else if (no_consumers_since == 0) {
                no_consumers_since = now;
            }
            int no_consumers_timeout = active == 0 && now >= no_consumers_since && now - no_consumers_since >= ( ((StorageHeader *)memory)->consumer_seen ? PRODUCER_NO_CONSUMER_TIMEOUT_NS : PRODUCER_STARTUP_TIMEOUT_NS );
            if (no_consumers_timeout) {
                ipc_unlock(semaphore);
                console_no_consumers();
                abandoned = 1;
                break;
            }
            if (ipc_unlock(semaphore) < 0) break;
            if (finished) break;
            usleep(250000);
        }
    }

    if (!abandoned) console_producer_finished();
    ipc_detach(shm_fd, semaphore, memory);
    ipc_remove();
    return 0;
}
