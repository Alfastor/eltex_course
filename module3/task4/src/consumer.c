#include "app.h"
#include "ipc.h"
#include "storage.h"
#include "console.h"
#include <sys/shm.h>
#include <unistd.h>
#include <signal.h>
#include <stdint.h>
#include <errno.h>

static volatile sig_atomic_t stop_requested;

static void handle_signal(int signal_number)
{
    (void)signal_number;
    stop_requested = 1;
}

int run_consumer(void)
{
    int shm_id, sem_id;
    void *memory;
    int pidfd = -1;
    unsigned int startup_retries = 0;
    pid_t producer_pid = 0;
    pid_t consumer_pid = getpid();
    int registered = 0;
    int ipc_removed = 0;

    if (ipc_attach(&shm_id, &sem_id, &memory) < 0) {
        console_error("Не удалось подключиться к IPC. Сначала запустите производителя");
        return 1;
    }

    signal(SIGINT, handle_signal);
    signal(SIGTERM, handle_signal);
    console_consumer_started(consumer_pid);

    while (!stop_requested) {
        if (ipc_lock(sem_id) < 0) break;

        StorageHeader *header = memory;
        if (header->magic != STORAGE_MAGIC || header->producer_pid == 0) {
            ipc_unlock(sem_id);
            if (++startup_retries >= 30) {
                console_error("Не удалось получить данные о производителе");
                break;
            }
            usleep(100000);
            continue;
        }

        producer_pid = (pid_t)header->producer_pid;
        if (pidfd < 0) {
            pidfd = ipc_pidfd_open(producer_pid);
            if (pidfd < 0) {
                if (errno == ESRCH) {
                    console_producer_lost(producer_pid);
                    ipc_remove(shm_id, sem_id);
                    ipc_removed = 1;
                    break;
                }
                ipc_unlock(sem_id);
                console_error("Не удалось отслеживать завершение производителя");
                break;
            }
        }

        int producer_exited = ipc_pidfd_exited(pidfd);
        if (producer_exited != 0) {
            if (producer_exited > 0) {
                console_producer_lost(producer_pid);
                ipc_remove(shm_id, sem_id);
                ipc_removed = 1;
            } else {
                ipc_unlock(sem_id);
                console_error("Ошибка проверки состояния производителя");
            }
            break;
        }

        if (!registered) {
            if (storage_register_consumer(memory, consumer_pid) < 0) {
                ipc_unlock(sem_id);
                console_error("Не удалось зарегистрировать потребителя: достигнут лимит процессов");
                break;
            }
            registered = 1;
        } else if (storage_touch_consumer(memory, consumer_pid) < 0) {
            ipc_unlock(sem_id);
            break;
        }

        DataBlock *block = storage_next_pending(memory, STORAGE_SIZE);
        if (block != NULL) {
            MinMax result;
            uint32_t count = block->count;
            int valid = storage_minmax(block, &result);
            block->count = 0;
            if (ipc_unlock(sem_id) < 0) break;
            if (valid == 0) {
                console_block_processed(consumer_pid, count, result.minimum, result.maximum);
            }
            usleep(500000);
            continue;
        }

        int finished = storage_all_processed(memory, STORAGE_SIZE);
        if (finished) {
            storage_unregister_consumer(memory, consumer_pid);
            registered = 0;
        }
        if (ipc_unlock(sem_id) < 0) break;
        if (finished) break;
        usleep(200000);
    }

    if (registered && !ipc_removed && ipc_lock(sem_id) == 0) {
        storage_unregister_consumer(memory, consumer_pid);
        ipc_unlock(sem_id);
    }
    if (pidfd >= 0) close(pidfd);
    shmdt(memory);
    console_consumer_finished(consumer_pid);
    return 0;
}
