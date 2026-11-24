#include "ipc.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>

int ipc_init(IpcChannels* ch, const char* pipe_base_name) {
    if (pipe_base_name == NULL) {
        ch->is_named = 0;
        int p2c[2], c2p[2];
        if (pipe(p2c) == -1 || pipe(c2p) == -1) return -1;
        ch->p2c_read = p2c[0]; ch->p2c_write = p2c[1];
        ch->c2p_read = c2p[0]; ch->c2p_write = c2p[1];
    } else {
        ch->is_named = 1;
        snprintf(ch->p2c_name, 256, "%s_p2c", pipe_base_name);
        snprintf(ch->c2p_name, 256, "%s_c2p", pipe_base_name);
        
        mkfifo(ch->p2c_name, 0666);
        mkfifo(ch->c2p_name, 0666);
    }
    return 0;
}

void ipc_parent_close_unused(IpcChannels* ch) {
    if (ch->is_named) {
        ch->p2c_write = open(ch->p2c_name, O_WRONLY);
        ch->c2p_read = open(ch->c2p_name, O_RDONLY);
    } else {
        close(ch->p2c_read);
        close(ch->c2p_write);
    }
}

void ipc_child_close_unused(IpcChannels* ch) {
    if (ch->is_named) {
        ch->p2c_read = open(ch->p2c_name, O_RDONLY);
        ch->c2p_write = open(ch->c2p_name, O_WRONLY);
    } else {
        close(ch->p2c_write);
        close(ch->c2p_read);
    }
}

void ipc_cleanup(IpcChannels* ch) {
    close(ch->p2c_write);
    close(ch->c2p_read);
    if (ch->is_named) {
        unlink(ch->p2c_name);
        unlink(ch->c2p_name);
    }
}