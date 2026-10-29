#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "contact.h"
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

void prompt_string(const char* prompt, char* dest, int size, bool required) {
    while (1) {
        printf("%s", prompt);
        read_line(dest, size);
        if (input_validate_string(dest, required)) break;
        printf("  [Ошибка] Поле обязательно для заполнения!\n");
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

void print_contact(const Contact* c) {
    printf("ФИО: %s %s\n", c->lastName, c->firstName);
    if (strlen(c->company) > 0 || strlen(c->position) > 0)
        printf("Работа: %s, %s\n", c->company, c->position);
    if (strlen(c->phone) > 0) printf("Тел: %s\n", c->phone);
    if (strlen(c->email) > 0) printf("Email: %s\n", c->email);
    if (strlen(c->social) > 0) printf("Соцсеть: %s\n", c->social);
    if (strlen(c->messenger) > 0) printf("Мессенджер: %s\n", c->messenger);
}

// Функция для редактирования одной строки
void prompt_edit_string(const char* prompt, char* dest, int size, bool required) {
    char buf[BUF_SIZE];
    
    // Выводим подсказку и текущее значение (если оно есть)
    printf("%s [%s]: ", prompt, strlen(dest) > 0 ? dest : "пусто");
    read_line(buf, BUF_SIZE);

    if (strcmp(buf, "-") == 0) {
        // Пользователь ввел минус - пытаемся очистить
        if (required) {
            printf("  [Ошибка] Это поле обязательное, его нельзя очистить!\n");
        } else {
            dest[0] = '\0'; // Очищаем строку
        }
    } else if (strlen(buf) > 0) {
        // Пользователь ввел новый текст - заменяем старое значение
        strncpy(dest, buf, size - 1);
        dest[size - 1] = '\0'; // Страховка для нуль-терминатора
    }
    // Если buf пуст (пользователь нажал Enter), мы просто ничего не делаем, 
    // и в dest остается старое значение.
}

void ui_edit_contact() {
    char lname[MAX_STR], fname[MAX_STR];
    prompt_string("Введите фамилию для поиска: ", lname, MAX_STR, true);
    prompt_string("Введите имя для поиска: ", fname, MAX_STR, true);
    
    int idx = contact_find(lname, fname);
    if (idx == -1) {
        printf("  [Ошибка] Контакт не найден.\n");
        return;
    }

    // Копируем текущие данные контакта во временную структуру
    Contact temp_data = *contact_get(idx);

    printf("\n--- Редактирование: %s %s ---\n", temp_data.lastName, temp_data.firstName);
    printf("Нажмите Enter, чтобы оставить поле без изменений.\n");
    printf("Введите '-' чтобы очистить необязательное поле.\n\n");
    
    // Вызываем новую функцию для каждого поля
    prompt_edit_string("Фамилия", temp_data.lastName, MAX_STR, true);
    prompt_edit_string("Имя", temp_data.firstName, MAX_STR, true);
    prompt_edit_string("Место работы", temp_data.company, MAX_STR, false);
    prompt_edit_string("Должность", temp_data.position, MAX_STR, false);
    prompt_edit_string("Телефон", temp_data.phone, 20, false);
    prompt_edit_string("Email", temp_data.email, MAX_STR, false);
    prompt_edit_string("Ссылка на соцсеть", temp_data.social, MAX_STR, false);
    prompt_edit_string("Профиль в мессенджере", temp_data.messenger, MAX_STR, false);

    // Сохраняем изменения в базу
    if (contact_edit(idx, &temp_data) == CONTACT_SUCCESS) {
        printf("  [Успех] Контакт обновлен.\n");
    } else {
        printf("  [Ошибка] Не удалось обновить контакт.\n");
    }
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

// --- НОВАЯ ФУНКЦИЯ РЕДАКТИРОВАНИЯ ---
void ui_edit_contact() {
    char lname[MAX_STR], fname[MAX_STR];
    prompt_string("Введите фамилию для поиска: ", lname, MAX_STR, true);
    prompt_string("Введите имя для поиска: ", fname, MAX_STR, true);
    
    int idx = contact_find(lname, fname);
    if (idx == -1) {
        printf("  [Ошибка] Контакт не найден.\n");
        return;
    }

    printf("\n--- Редактирование: %s %s ---\n", contact_get(idx)->lastName, contact_get(idx)->firstName);
    
    Contact new_data;
    memset(&new_data, 0, sizeof(Contact));
    
    prompt_string("Новая Фамилия: ", new_data.lastName, MAX_STR, true);
    prompt_string("Новое Имя: ", new_data.firstName, MAX_STR, true);
    prompt_string("Новое место работы: ", new_data.company, MAX_STR, false);
    prompt_string("Новая должность: ", new_data.position, MAX_STR, false);
    prompt_string("Новый телефон: ", new_data.phone, 20, false);
    prompt_string("Новый Email: ", new_data.email, MAX_STR, false);
    prompt_string("Новая соцсеть: ", new_data.social, MAX_STR, false);
    prompt_string("Новый мессенджер: ", new_data.messenger, MAX_STR, false);

    if (contact_edit(idx, &new_data) == CONTACT_SUCCESS) {
        printf("  [Успех] Контакт обновлен.\n");
    } else {
        printf("  [Ошибка] Не удалось обновить контакт.\n");
    }
}
// -----------------------------------

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
        printf("4. Редактировать контакт\n"); // <-- Обновили меню
        printf("5. Удалить контакт\n");        // <-- Сдвинули нумерацию
        printf("0. Выход\n");

        choice = prompt_int("Выберите действие: ", 0, 5); // <-- Максимум теперь 5
        switch (choice) {
            case 1: ui_print_all(); break;
            case 2: ui_find_contact(); break;
            case 3: ui_add_contact(); break;
            case 4: ui_edit_contact(); break; // <-- Вызов новой функции
            case 5: ui_delete_contact(); break;
            case 0: return 0;
        }
    }
}