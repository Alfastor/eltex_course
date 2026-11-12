#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "network.h"

int main(int argc, char* argv[]) {
    if (argc != 4) {
        printf("Использование: %s <IP шлюза> <Маска> <Кол-во пакетов>\n", argv[0]);
        printf("Пример: %s 192.168.1.10 255.255.255.0 1000\n", argv[0]);
        return 1;
    }

    uint32_t gateway_ip, mask;
    int n_packets;

    if (!net_parse_ip(argv[1], &gateway_ip)) {
        printf("[Ошибка] Неверный формат IP шлюза.\n");
        return 1;
    }
    if (!net_parse_ip(argv[2], &mask)) {
        printf("[Ошибка] Неверный формат маски.\n");
        return 1;
    }
    n_packets = atoi(argv[3]);
    if (n_packets <= 0) {
        printf("[Ошибка] Количество пакетов должно быть больше 0.\n");
        return 1;
    }

    srand(time(NULL));

    int same_subnet_count = 0;
    char ip_str[20];

    printf("Шлюз: ");
    net_ip_to_str(gateway_ip, ip_str);
    printf("%s\n", ip_str);

    printf("Маска: ");
    net_ip_to_str(mask, ip_str);
    printf("%s\n", ip_str);

    printf("Генерация %d пакетов...\n\n", n_packets);

    for (int i = 0; i < n_packets; i++) {
        uint32_t dest_ip = net_generate_random_ip();
        
        if (net_is_same_subnet(gateway_ip, dest_ip, mask)) {
            same_subnet_count++;
        }
    }

    int other_subnet_count = n_packets - same_subnet_count;
    
    // Вывод статистики
    printf("СТАТИСТИКА\n");
    printf("Всего пакетов: %d\n", n_packets);
    printf("В своей подсети: %d (%.2f%%)\n", same_subnet_count, (float)same_subnet_count / n_packets * 100);
    printf("В других сетях:  %d (%.2f%%)\n", other_subnet_count, (float)other_subnet_count / n_packets * 100);

    return 0;
}