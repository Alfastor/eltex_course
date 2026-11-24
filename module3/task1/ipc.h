#ifndef IPC_H
#define IPC_H

typedef struct {
    int p2c_read;
    int p2c_write;
    int c2p_read;
    int c2p_write;
    char p2c_name[256];
    char c2p_name[256];
    int is_named;
} IpcChannels;

int ipc_init(IpcChannels* ch, const char* pipe_base_name);
void ipc_parent_close_unused(IpcChannels* ch);
void ipc_child_close_unused(IpcChannels* ch);
void ipc_cleanup(IpcChannels* ch);

#endif