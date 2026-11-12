#include "permissions.h"
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/stat.h>

bool perm_parse_symbolic(const char* str, unsigned int* out_mode) {
    if (strlen(str) != 9) return false;
    unsigned int mode = 0;

    if (str[0] == 'r') mode |= S_IRUSR;
    if (str[1] == 'w') mode |= S_IWUSR;
    if (str[2] == 'x') mode |= S_IXUSR;
    
    if (str[3] == 'r') mode |= S_IRGRP;
    if (str[4] == 'w') mode |= S_IWGRP;
    if (str[5] == 'x') mode |= S_IXGRP;
    
    if (str[6] == 'r') mode |= S_IROTH;
    if (str[7] == 'w') mode |= S_IWOTH;
    if (str[8] == 'x') mode |= S_IXOTH;

    *out_mode = mode;
    return true;
}

bool perm_parse_numeric(const char* str, unsigned int* out_mode) {
    char* endptr;
    long val = strtol(str, &endptr, 8); // Парсим как восьмеричное число
    if (endptr == str || *endptr != '\0') return false;
    if (val < 0 || val > 0777) return false;
    
    *out_mode = (unsigned int)val;
    return true;
}

bool perm_apply_chmod(unsigned int current_mode, const char* cmd, unsigned int* out_mode) {
    int i = 0;
    unsigned int who_mask = 0;
    
    // 1. Кто? (u, g, o, a)
    while (cmd[i] == 'u' || cmd[i] == 'g' || cmd[i] == 'o' || cmd[i] == 'a') {
        if (cmd[i] == 'u' || cmd[i] == 'a') who_mask |= 0700;
        if (cmd[i] == 'g' || cmd[i] == 'a') who_mask |= 0070;
        if (cmd[i] == 'o' || cmd[i] == 'a') who_mask |= 0007;
        i++;
    }
    if (who_mask == 0) who_mask = 0777; // По умолчанию - все

    // 2. Оператор (+, -, =)
    if (cmd[i] != '+' && cmd[i] != '-' && cmd[i] != '=') return false;
    char op = cmd[i++];

    // 3. Права (r, w, x)
    unsigned int rights_mask = 0;
    while (cmd[i] != '\0') {
        if (cmd[i] == 'r') rights_mask |= 0444;
        else if (cmd[i] == 'w') rights_mask |= 0222;
        else if (cmd[i] == 'x') rights_mask |= 0111;
        else return false;
        i++;
    }
    
    unsigned int target_mask = who_mask & rights_mask;
    unsigned int new_mode = current_mode;

    if (op == '+') {
        new_mode |= target_mask;
    } else if (op == '-') {
        new_mode &= ~target_mask;
    } else if (op == '=') {
        new_mode &= ~who_mask; // Очищаем старые
        new_mode |= target_mask; // Ставим новые
    }

    *out_mode = new_mode;
    return true;
}

bool perm_get_file_mode(const char* filename, unsigned int* out_mode) {
    struct stat fileStat;
    if (stat(filename, &fileStat) == 0) {
        *out_mode = fileStat.st_mode & 0777;
        return true;
    }
    return false;
}

void perm_to_symbolic(unsigned int mode, char* out_str) {
    out_str[0] = (mode & S_IRUSR) ? 'r' : '-';
    out_str[1] = (mode & S_IWUSR) ? 'w' : '-';
    out_str[2] = (mode & S_IXUSR) ? 'x' : '-';
    out_str[3] = (mode & S_IRGRP) ? 'r' : '-';
    out_str[4] = (mode & S_IWGRP) ? 'w' : '-';
    out_str[5] = (mode & S_IXGRP) ? 'x' : '-';
    out_str[6] = (mode & S_IROTH) ? 'r' : '-';
    out_str[7] = (mode & S_IWOTH) ? 'w' : '-';
    out_str[8] = (mode & S_IXOTH) ? 'x' : '-';
    out_str[9] = '\0';
}

void perm_to_binary(unsigned int mode, char* out_str) {
    int idx = 0;
    for (int i = 8; i >= 0; i--) {
        out_str[idx++] = ((mode >> i) & 1) ? '1' : '0';
        if (i == 6 || i == 3) out_str[idx++] = ' ';
    }
    out_str[idx] = '\0';
}