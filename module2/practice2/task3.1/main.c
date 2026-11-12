#include <stdio.h>
#include <string.h>
#include "input.h"
#include "permissions.h"
#include <stdlib.h> 
#include <ctype.h>  

void print_all_formats(unsigned int mode) {
    char sym[10];
    char bin[16];
    perm_to_symbolic(mode, sym);
    perm_to_binary(mode, bin);
    
    printf("Буквенно: %s\n", sym);
    printf("Цифрой:   %03o\n", mode);
    printf("Битово:   %s\n", bin);
}

int main(void) {
    int choice;
    unsigned int current_mode = 0;
    bool mode_set = false;
    char input[MAX_STR];

    while (1) {
        printf("\n=== РАСЧЕТ ПРАВ ДОСТУПА ===\n");
        printf("1. Ввести права вручную\n");
        printf("2. Получить права файла (stat)\n");
        printf("3. Изменить права (chmod)\n");
        printf("0. Выход\n");
        printf("Выбор: ");
        
        input_read_line(input, MAX_STR);
        choice = atoi(input); // Простейший парсинг для меню

        if (choice == 0) break;

        if (choice == 1) {
            printf("Введите права (например, rwxr-xr-x или 755): ");
            input_read_line(input, MAX_STR);
            
            bool ok = false;
            if (isdigit(input[0])) {
                ok = perm_parse_numeric(input, &current_mode);
            } else {
                ok = perm_parse_symbolic(input, &current_mode);
            }

            if (ok) {
                print_all_formats(current_mode);
                mode_set = true;
            } else {
                printf("  [Ошибка] Неверный формат ввода.\n");
            }

        } else if (choice == 2) {
            printf("Введите имя файла: ");
            input_read_line(input, MAX_STR);

            if (perm_get_file_mode(input, &current_mode)) {
                print_all_formats(current_mode);
                mode_set = true;
            } else {
                printf("  [Ошибка] Файл не найден или нет доступа.\n");
            }

        } else if (choice == 3) {
            if (!mode_set) {
                printf("  [Ошибка] Сначала задайте исходные права.\n");
                continue;
            }
            printf("Текущие права: ");
            char sym[10];
            perm_to_symbolic(current_mode, sym);
            printf("%s\n", sym);

            printf("Введите команду (например, u+x, go-w, a=rw): ");
            input_read_line(input, MAX_STR);

            unsigned int new_mode;
            if (perm_apply_chmod(current_mode, input, &new_mode)) {
                current_mode = new_mode;
                printf("Новые права:\n");
                print_all_formats(current_mode);
            } else {
                printf("  [Ошибка] Неверный синтаксис команды chmod.\n");
            }
        }
    }
    return 0;
}