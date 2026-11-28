#include "console.h"

#include <stdarg.h>
#include <stdio.h>

void console_info(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
    putchar('\n');
    fflush(stdout);
}

void console_error(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);
    fputc('\n', stderr);
    fflush(stderr);
}

void console_chat(const char *name, const char *message)
{
    printf("[%s] %s\n", name, message);
    fflush(stdout);
}

void console_prompt(void)
{
    fputs("> ", stdout);
    fflush(stdout);
}

void console_users(const char *own_name, const UserTable *table)
{
    size_t i;

    printf("Участники чата:\n");
    printf("  %s (вы)\n", own_name);
    for (i = 0; i < table->count; ++i) {
        printf("  %s\n", table->items[i].name);
    }
    fflush(stdout);
}

void console_help(void)
{
    console_info("Команды: /users — список участников, /help — помощь, /quit — выход.");
}
