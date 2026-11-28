#include "console.h"
#include <stdio.h>

void console_error(const char *message)
{
    perror(message);
}

void console_producer_started(uint32_t size)
{
    printf("Производитель запущен. Размер разделяемой памяти: %u байт\n", size);
    fflush(stdout);
}

void console_block_created(unsigned int number, uint32_t count)
{
    printf("Сформирован набор №%u, элементов: %u\n", number, count);
    fflush(stdout);
}

void console_producer_full(unsigned int generated)
{
    printf("Разделяемая память заполнена. Всего сформировано наборов: %u\n", generated);
    fflush(stdout);
}

void console_producer_waiting(void)
{
    printf("Генерация завершена. Ожидание обработки всех наборов...\n");
    fflush(stdout);
}

void console_no_consumers(void)
{
    printf("Активные потребители отсутствуют. Производитель прекращает ожидание и освобождает IPC-ресурсы.\n");
    fflush(stdout);
}

void console_producer_finished(void)
{
    printf("Все наборы обработаны. Освобождение IPC-ресурсов.\n");
    fflush(stdout);
}

void console_consumer_started(pid_t pid)
{
    printf("Потребитель PID=%ld подключен\n", (long)pid);
    fflush(stdout);
}

void console_block_processed(pid_t pid, uint32_t count, int minimum, int maximum)
{
    printf("Потребитель PID=%ld: обработан набор из %u элементов, минимум=%d, максимум=%d\n", (long)pid, count, minimum, maximum);
    fflush(stdout);
}

void console_consumer_finished(pid_t pid)
{
    printf("Потребитель PID=%ld завершил работу\n", (long)pid);
    fflush(stdout);
}

void console_producer_lost(pid_t pid)
{
    printf("Производитель PID=%ld аварийно завершился. Потребитель прекращает работу и очищает IPC-ресурсы.\n", (long)pid);
    fflush(stdout);
}
