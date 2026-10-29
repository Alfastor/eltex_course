#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "calc.h"
#include "input.h"

#define BUF_SIZE 256

void read_line(char* buffer, int size) {
    if (fgets(buffer, size, stdin) == NULL) {
        exit(EXIT_FAILURE);
    }
    size_t len = strlen(buffer);
    if (len > 0 && buffer[len-1] == '\n') {
        buffer[len-1] = '\0';
    } else {
        int c;
        while ((c = getchar()) != '\n' && c != EOF);
    }
}

int prompt_int(const char* prompt, int min, int max) {
    char buf[BUF_SIZE];
    int val;
    while (1) {
        printf("%s", prompt);
        read_line(buf, BUF_SIZE);
        if (input_parse_int(buf, min, max, &val)) return val;
        printf("  [Ошибка] Введите число от %d до %d.\n", min, max);
    }
}

double prompt_double(const char* prompt) {
    char buf[BUF_SIZE];
    double val;
    while (1) {
        printf("%s", prompt);
        read_line(buf, BUF_SIZE);
        if (input_parse_double(buf, &val)) return val;
        printf("  [Ошибка] Введите корректное число.\n");
    }
}

int main(void) {
    int choice;
    double a, b, result;

    while (1) {
        printf("\n=== КАЛЬКУЛЯТОР ===\n");
        printf("1. Сложение\n");
        printf("2. Вычитание\n");
        printf("3. Умножение\n");
        printf("4. Деление\n");
        printf("0. Выход\n");

        choice = prompt_int("Выберите действие: ", 0, 4);

        if (choice == 0) {
            printf("Выход из программы.\n");
            break;
        }

        a = prompt_double("Введите первое число: ");
        b = prompt_double("Введите второе число: ");

        switch (choice) {
            case 1:
                result = calc_add(a, b);
                printf("Результат: %g + %g = %g\n", a, b, result);
                break;
            case 2:
                result = calc_sub(a, b);
                printf("Результат: %g - %g = %g\n", a, b, result);
                break;
            case 3:
                result = calc_mul(a, b);
                printf("Результат: %g * %g = %g\n", a, b, result);
                break;
            case 4:
                if (calc_div(a, b, &result) == CALC_SUCCESS) {
                    printf("Результат: %g / %g = %g\n", a, b, result);
                } else {
                    printf("  [Ошибка] Деление на ноль невозможно!\n");
                }
                break;
        }
    }

    return 0;
}