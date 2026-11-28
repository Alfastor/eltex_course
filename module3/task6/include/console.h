#ifndef CONSOLE_H
#define CONSOLE_H

#include "users.h"
#include <stddef.h>

void console_info(const char *format, ...);
void console_error(const char *format, ...);
void console_chat(const char *name, const char *message);
void console_prompt(void);
void console_users(const char *own_name, const UserTable *table);
void console_help(void);

#endif
