#ifndef NETWORK_H
#define NETWORK_H
#include <stdint.h>
#include <stdbool.h>

// Парсит строку "a.b.c.d" в 32-битное число
bool net_parse_ip(const char* str, uint32_t* out_ip);

// Переводит 32-битное число обратно в строку "a.b.c.d"
void net_ip_to_str(uint32_t ip, char* out_str);

// Проверяет, находятся ли два IP в одной подсети с заданной маской
bool net_is_same_subnet(uint32_t ip1, uint32_t ip2, uint32_t mask);

// Генерирует случайный 32-битный IP
uint32_t net_generate_random_ip(void);

#endif // NETWORK_H