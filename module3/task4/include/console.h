#ifndef CONSOLE_H
#define CONSOLE_H

#include <stdint.h>
#include <sys/types.h>

void console_error(const char *message);
void console_producer_started(uint32_t size);
void console_block_created(unsigned int number, uint32_t count);
void console_producer_full(unsigned int generated);
void console_producer_waiting(void);
void console_producer_finished(void);
void console_no_consumers(void);
void console_consumer_started(pid_t pid);
void console_block_processed(pid_t pid, uint32_t count, int minimum, int maximum);
void console_consumer_finished(pid_t pid);
void console_producer_lost(pid_t pid);

#endif
