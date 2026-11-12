#include "network.h"
#include <stdlib.h>
#include <stdio.h>

bool net_parse_ip(const char* str, uint32_t* out_ip) {
    uint32_t ip = 0;
    int part = 0;
    char* endptr;
    const char* p = str;

    while (*p != '\0' && part < 4) {
        long val = strtol(p, &endptr, 10);
        if (endptr == p || val < 0 || val > 255) return false;
        ip = (ip << 8) | (uint32_t)val;
        p = endptr;
        if (*p == '.') {
            p++;
            part++;
        } else if (*p != '\0') {
            return false;
        }
    }
    if (part != 3 || *p != '\0') return false;
    *out_ip = ip;
    return true;
}

void net_ip_to_str(uint32_t ip, char* out_str) {
    unsigned int a = (ip >> 24) & 0xFF;
    unsigned int b = (ip >> 16) & 0xFF;
    unsigned int c = (ip >> 8) & 0xFF;
    unsigned int d = ip & 0xFF;
    sprintf(out_str, "%u.%u.%u.%u", a, b, c, d);
}

bool net_is_same_subnet(uint32_t ip1, uint32_t ip2, uint32_t mask) {
    uint32_t net1 = ip1 & mask;
    uint32_t net2 = ip2 & mask;
    return (net1 ^ net2) == 0;
}

uint32_t net_generate_random_ip(void) {
    return ((uint32_t)(rand() & 0xFF) << 24) |
           ((uint32_t)(rand() & 0xFF) << 16) |
           ((uint32_t)(rand() & 0xFF) << 8)  |
           ((uint32_t)(rand() & 0xFF));
}