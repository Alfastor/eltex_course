#include "ipc.h"
#include "storage.h"
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <poll.h>
#include <sys/syscall.h>

union semun {
    int val;
    struct semid_ds *buf;
    unsigned short *array;
};

static int get_keys(key_t *shm_key, key_t *sem_key)
{
    int fd = open(IPC_KEY_FILE, O_CREAT | O_RDWR, 0666);
    if (fd < 0) return -1;
    close(fd);
    *shm_key = ftok(IPC_KEY_FILE, IPC_PROJECT_SHM);
    *sem_key = ftok(IPC_KEY_FILE, IPC_PROJECT_SEM);
    return (*shm_key == (key_t)-1 || *sem_key == (key_t)-1) ? -1 : 0;
}

int ipc_create(int *shm_id, int *sem_id)
{
    key_t shm_key, sem_key;
    if (get_keys(&shm_key, &sem_key) < 0) return -1;
    *sem_id = semget(sem_key, 1, IPC_CREAT | IPC_EXCL | 0666);
    if (*sem_id < 0) return -1;
    union semun argument;
    argument.val = 1;
    if (semctl(*sem_id, 0, SETVAL, argument) < 0) {
        semctl(*sem_id, 0, IPC_RMID);
        return -1;
    }
    *shm_id = shmget(shm_key, STORAGE_SIZE, IPC_CREAT | IPC_EXCL | 0666);
    if (*shm_id < 0) {
        semctl(*sem_id, 0, IPC_RMID);
        return -1;
    }
    return 0;
}

int ipc_attach(int *shm_id, int *sem_id, void **memory)
{
    key_t shm_key, sem_key;
    if (get_keys(&shm_key, &sem_key) < 0) return -1;
    *sem_id = semget(sem_key, 1, 0666);
    if (*sem_id < 0) return -1;
    *shm_id = shmget(shm_key, STORAGE_SIZE, 0666);
    if (*shm_id < 0) return -1;
    *memory = shmat(*shm_id, NULL, 0);
    return *memory == (void *)-1 ? -1 : 0;
}

int ipc_lock(int sem_id)
{
    struct sembuf operation = {0, -1, SEM_UNDO};
    while (semop(sem_id, &operation, 1) < 0) {
        if (errno != EINTR) return -1;
    }
    return 0;
}

int ipc_unlock(int sem_id)
{
    struct sembuf operation = {0, 1, SEM_UNDO};
    while (semop(sem_id, &operation, 1) < 0) {
        if (errno != EINTR) return -1;
    }
    return 0;
}

int ipc_remove(int shm_id, int sem_id)
{
    int result = 0;
    if (shmctl(shm_id, IPC_RMID, NULL) < 0) result = -1;
    if (semctl(sem_id, 0, IPC_RMID) < 0) result = -1;
    return result;
}

int ipc_cleanup_existing(void)
{
    key_t shm_key, sem_key;
    if (get_keys(&shm_key, &sem_key) < 0) return -1;
    int shm_id = shmget(shm_key, STORAGE_SIZE, 0666);
    int sem_id = semget(sem_key, 1, 0666);
    if (shm_id >= 0) shmctl(shm_id, IPC_RMID, NULL);
    if (sem_id >= 0) semctl(sem_id, 0, IPC_RMID);
    return 0;
}

int ipc_pidfd_open(pid_t pid)
{
#ifdef SYS_pidfd_open
    return (int)syscall(SYS_pidfd_open, pid, 0);
#else
    (void)pid;
    errno = ENOSYS;
    return -1;
#endif
}

int ipc_pidfd_exited(int pidfd)
{
    struct pollfd descriptor;
    descriptor.fd = pidfd;
    descriptor.events = POLLIN;
    descriptor.revents = 0;
    int result;
    do {
        result = poll(&descriptor, 1, 0);
    } while (result < 0 && errno == EINTR);
    if (result < 0) return -1;
    return result > 0 && (descriptor.revents & (POLLIN | POLLHUP | POLLERR)) != 0;
}
