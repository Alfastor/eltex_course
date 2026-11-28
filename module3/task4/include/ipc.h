#ifndef IPC_H
#define IPC_H

#include <sys/types.h>

#define IPC_PROJECT_SHM 'M'
#define IPC_PROJECT_SEM 'S'
#define IPC_KEY_FILE ".task4_ipc_key"

int ipc_create(int *shm_id, int *sem_id);
int ipc_attach(int *shm_id, int *sem_id, void **memory);
int ipc_lock(int sem_id);
int ipc_unlock(int sem_id);
int ipc_remove(int shm_id, int sem_id);
int ipc_cleanup_existing(void);
int ipc_pidfd_open(pid_t pid);
int ipc_pidfd_exited(int pidfd);

#endif
