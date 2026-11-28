#include "ipc.h"
#include "storage.h"
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <poll.h>
#include <sys/syscall.h>
#include <stdio.h>

static int get_ipc_names(char *shm_name, size_t shm_name_size,
                         char *sem_name, size_t sem_name_size)
{
    int shm_length = snprintf(shm_name, shm_name_size, "/task5_shm_%lu", (unsigned long)getuid());
    int sem_length = snprintf(sem_name, sem_name_size, "/task5_sem_%lu", (unsigned long)getuid());

    if (shm_length < 0 || (size_t)shm_length >= shm_name_size ||
        sem_length < 0 || (size_t)sem_length >= sem_name_size) {
        errno = ENAMETOOLONG;
        return -1;
    }
    return 0;
}

static int unlink_ipc_names(void)
{
    char shm_name[64];
    char sem_name[64];
    int result = 0;

    if (get_ipc_names(shm_name, sizeof(shm_name), sem_name, sizeof(sem_name)) < 0) return -1;

    if (shm_unlink(shm_name) < 0 && errno != ENOENT) result = -1;
    if (sem_unlink(sem_name) < 0 && errno != ENOENT) result = -1;
    return result;
}

int ipc_create(int *shm_fd, sem_t **semaphore, void **memory)
{
    char shm_name[64];
    char sem_name[64];
    if (shm_fd == NULL || semaphore == NULL || memory == NULL) {
        errno = EINVAL;
        return -1;
    }
    if (get_ipc_names(shm_name, sizeof(shm_name), sem_name, sizeof(sem_name)) < 0) return -1;

    sem_t *created_sem = sem_open(sem_name, O_CREAT | O_EXCL, 0666, 1);
    if (created_sem == SEM_FAILED) return -1;

    int created_shm = shm_open(shm_name, O_CREAT | O_EXCL | O_RDWR, 0666);
    if (created_shm < 0) {
        int saved_errno = errno;
        sem_close(created_sem);
        sem_unlink(sem_name);
        errno = saved_errno;
        return -1;
    }

    if (ftruncate(created_shm, (off_t)STORAGE_SIZE) < 0) {
        int saved_errno = errno;
        close(created_shm);
        shm_unlink(shm_name);
        sem_close(created_sem);
        sem_unlink(sem_name);
        errno = saved_errno;
        return -1;
    }

    void *mapping = mmap(NULL, STORAGE_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, created_shm, 0);
    if (mapping == MAP_FAILED) {
        int saved_errno = errno;
        close(created_shm);
        shm_unlink(shm_name);
        sem_close(created_sem);
        sem_unlink(sem_name);
        errno = saved_errno;
        return -1;
    }

    *shm_fd = created_shm;
    *semaphore = created_sem;
    *memory = mapping;
    return 0;
}

int ipc_attach(int *shm_fd, sem_t **semaphore, void **memory)
{
    char shm_name[64];
    char sem_name[64];
    if (shm_fd == NULL || semaphore == NULL || memory == NULL) {
        errno = EINVAL;
        return -1;
    }
    if (get_ipc_names(shm_name, sizeof(shm_name), sem_name, sizeof(sem_name)) < 0) return -1;

    sem_t *attached_sem = sem_open(sem_name, 0);
    if (attached_sem == SEM_FAILED) return -1;

    int attached_shm = shm_open(shm_name, O_RDWR, 0666);
    if (attached_shm < 0) {
        int saved_errno = errno;
        sem_close(attached_sem);
        errno = saved_errno;
        return -1;
    }

    struct stat information;
    if (fstat(attached_shm, &information) < 0) {
        int saved_errno = errno;
        close(attached_shm);
        sem_close(attached_sem);
        errno = saved_errno;
        return -1;
    }
    if (information.st_size < (off_t)STORAGE_SIZE) {
        close(attached_shm);
        sem_close(attached_sem);
        errno = EINVAL;
        return -1;
    }

    void *mapping = mmap(NULL, STORAGE_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, attached_shm, 0);
    if (mapping == MAP_FAILED) {
        int saved_errno = errno;
        close(attached_shm);
        sem_close(attached_sem);
        errno = saved_errno;
        return -1;
    }

    *shm_fd = attached_shm;
    *semaphore = attached_sem;
    *memory = mapping;
    return 0;
}

int ipc_detach(int shm_fd, sem_t *semaphore, void *memory)
{
    int result = 0;
    if (memory != NULL && memory != MAP_FAILED && munmap(memory, STORAGE_SIZE) < 0) result = -1;
    if (shm_fd >= 0 && close(shm_fd) < 0) result = -1;
    if (semaphore != NULL && semaphore != SEM_FAILED && sem_close(semaphore) < 0) result = -1;
    return result;
}

int ipc_lock(sem_t *semaphore)
{
    int result;
    do {
        result = sem_wait(semaphore);
    } while (result < 0 && errno == EINTR);
    return result;
}

int ipc_unlock(sem_t *semaphore)
{
    return sem_post(semaphore);
}

int ipc_remove(void)
{
    return unlink_ipc_names();
}

int ipc_cleanup_existing(void)
{
    return unlink_ipc_names();
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
