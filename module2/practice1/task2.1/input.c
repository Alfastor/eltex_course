#include "input.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void clear_buffer(void) {
    int c;
    // Читаем символы, пока не встретим конец строки или конец файла
    while ((c = getchar()) != '\n' && c != EOF);
}

void input_string(const char* prompt, char* buffer, int max_len, int is_required) {
    while (1) {
        printf("%s", prompt);
        
        if (fgets(buffer, max_len, stdin) == NULL) {
            printf("\n[Ошибка] Прерывание ввода (EOF).\n");
            exit(EXIT_FAILURE); 
        }

        // Поиск символа перевода строки
        char* newline = strchr(buffer, '\n');
        if (newline) {
            *newline = '\0'; // Удаляем '\n', заменяя на '\0'
        } else {
            // Если '\n' не найден, значит буфер заполнен полностью, 
            // а остаток строки остался в stdin.
            clear_buffer(); 
        }

        if (is_required && strlen(buffer) == 0) {
            printf("  [Ошибка] Это поле обязательно для заполнения!\n");
            continue;
        }
        
        break; 
    }
}

int input_int(const char* prompt, int min, int max) {
    char buf[32];
    int value;
    char* endptr;

    while (1) {
        printf("%s", prompt);
        
        if (fgets(buf, sizeof(buf), stdin) == NULL) {
            printf("\n[Ошибка] Прерывание ввода (EOF).\n");
            exit(EXIT_FAILURE);
        }

        // Если не нашли '\n', значит ввели слишком много символов
        if (strchr(buf, '\n') == NULL) {
            clear_buffer(); // Очищаем остаток
        }

        // Преобразование строки в число
        value = (int)strtol(buf, &endptr, 10);

        // Проверка: был ли вообще введен текст и нет ли лишних символов
        if (endptr == buf || (*endptr != '\n' && *endptr != '\0')) {
            printf("  [Ошибка] Введите корректное число.\n");
            continue;
        }
        
        if (value < min || value > max) {
            printf("  [Ошибка] Число должно быть от %d до %d.\n", min, max);
            continue;
        }
        
        return value;
    }
}


