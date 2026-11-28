#ifndef IPC_H
#define IPC_H

#include <semaphore.h>
#include <sys/types.h>

int ipc_create(int *shm_fd, sem_t **semaphore, void **memory);
int ipc_attach(int *shm_fd, sem_t **semaphore, void **memory);
int ipc_detach(int shm_fd, sem_t *semaphore, void *memory);
int ipc_lock(sem_t *semaphore);
int ipc_unlock(sem_t *semaphore);
int ipc_remove(void);
int ipc_cleanup_existing(void);
int ipc_pidfd_open(pid_t pid);
int ipc_pidfd_exited(int pidfd);

#endif
