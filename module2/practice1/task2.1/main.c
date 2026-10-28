#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "contact.h"
#include "input.h"

#define BUF_SIZE 256

// Вспомогательная функция для чтения строки с клавиатуры
void read_line(char* buffer, int size) {
    if (fgets(buffer, size, stdin) == NULL) {
        exit(EXIT_FAILURE); // Прерывание по Ctrl+D
    }
    // Убираем \n
    size_t len = strlen(buffer);
    if (len > 0 && buffer[len-1] == '\n') {
        buffer[len-1] = '\0';
    } else {
        // Очистка, если ввели слишком много
        int c;
        while ((c = getchar()) != '\n' && c != EOF);
    }
}

// Обертка для запроса строки с валидацией
void prompt_string(const char* prompt, char* dest, int size, bool required) {
    while (1) {
        printf("%s", prompt);
        read_line(dest, size);
        if (input_validate_string(dest, required)) break;
        printf("  [Ошибка] Поле обязательно для заполнения!\n");
    }
}

// Обертка для запроса числа с валидацией
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

// Вспомогательная функция вывода одного контакта
void print_contact(const Contact* c) {
    printf("ФИО: %s %s\n", c->lastName, c->firstName);
    if (strlen(c->company) > 0 || strlen(c->position) > 0)
        printf("Работа: %s, %s\n", c->company, c->position);
    if (strlen(c->phone) > 0) printf("Тел: %s\n", c->phone);
    if (strlen(c->email) > 0) printf("Email: %s\n", c->email);
    if (strlen(c->social) > 0) printf("Соцсеть: %s\n", c->social);
    if (strlen(c->messenger) > 0) printf("Мессенджер: %s\n", c->messenger);
}

// Интерфейсные функции (собирают данные и вызывают contact.c)
void ui_add_contact() {
    Contact c;
    memset(&c, 0, sizeof(Contact)); // Инициализация нулями
    
    printf("\n--- Добавление контакта ---\n");
    prompt_string("Фамилия: ", c.lastName, MAX_STR, true);
    prompt_string("Имя: ", c.firstName, MAX_STR, true);
    prompt_string("Место работы: ", c.company, MAX_STR, false);
    prompt_string("Должность: ", c.position, MAX_STR, false);
    prompt_string("Телефон: ", c.phone, 20, false);
    prompt_string("Email: ", c.email, MAX_STR, false);
    prompt_string("Ссылка на соцсеть: ", c.social, MAX_STR, false);
    prompt_string("Профиль в мессенджере: ", c.messenger, MAX_STR, false);

    int res = contact_add(&c);
    if (res == CONTACT_SUCCESS) printf("  [Успех] Контакт добавлен.\n");
    else printf("  [Ошибка] Не удалось добавить контакт (база переполнена).\n");
}

void ui_print_all() {
    int count = contact_get_count();
    if (count == 0) {
        printf("\nСписок контактов пуст.\n");
        return;
    }
    printf("\n=== СПИСОК КОНТАКТОВ (%d) ===\n", count);
    for (int i = 0; i < count; i++) {
        printf("[%d] ", i + 1);
        print_contact(contact_get(i));
        printf("-----------------------------------\n");
    }
}

void ui_find_contact() {
    char lname[MAX_STR], fname[MAX_STR];
    prompt_string("Введите фамилию: ", lname, MAX_STR, true);
    prompt_string("Введите имя: ", fname, MAX_STR, true);
    
    int idx = contact_find(lname, fname);
    if (idx != -1) {
        printf("\n=== НАЙДЕН КОНТАКТ ===\n");
        print_contact(contact_get(idx));
        printf("======================\n");
    } else {
        printf("  [Ошибка] Контакт не найден.\n");
    }
}

void ui_delete_contact() {
    char lname[MAX_STR], fname[MAX_STR];
    prompt_string("Введите фамилию: ", lname, MAX_STR, true);
    prompt_string("Введите имя: ", fname, MAX_STR, true);
    
    int idx = contact_find(lname, fname);
    if (idx != -1) {
        if (contact_delete(idx) == CONTACT_SUCCESS) {
            printf("  [Успех] Контакт удален.\n");
        }
    } else {
        printf("  [Ошибка] Контакт не найден.\n");
    }
}

int main(void) {
    int choice;
    while (1) {
        printf("\n=== ТЕЛЕФОННАЯ КНИГА ===\n");
        printf("1. Показать все контакты\n");
        printf("2. Найти контакт\n");
        printf("3. Добавить контакт\n");
        printf("4. Удалить контакт\n");
        printf("0. Выход\n");

        choice = prompt_int("Выберите действие: ", 0, 4);
        switch (choice) {
            case 1: ui_print_all(); break;
            case 2: ui_find_contact(); break;
            case 3: ui_add_contact(); break;
            case 4: ui_delete_contact(); break;
            case 0: return 0;
        }
    }
}