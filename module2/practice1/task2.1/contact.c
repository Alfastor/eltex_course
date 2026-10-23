#include "contact.h"
#include "input.h"
#include <stdio.h>
#include <string.h>

// Требования 1 и 3: статический массив структур
static Contact contacts[MAX_CONTACTS];
static int contact_count = 0;

// Поиск индекса контакта по Фамилии и Имени
static int find_contact_index(const char* lname, const char* fname) {
    for (int i = 0; i < contact_count; i++) {
        if (strcmp(contacts[i].lastName, lname) == 0 && 
            strcmp(contacts[i].firstName, fname) == 0) {
            return i;
        }
    }
    return -1;
}

void add_contact(void) {
    if (contact_count >= MAX_CONTACTS) {
        printf("  [Ошибка] База контактов заполнена (максимум %d).\n", MAX_CONTACTS);
        return;
    }

    Contact new_contact;
    printf("\n--- Добавление контакта ---\n");
    
    // Обязательные поля (is_required = 1)
    input_string("Фамилия: ", new_contact.lastName, MAX_STR, 1);
    input_string("Имя: ", new_contact.firstName, MAX_STR, 1);
    
    // Необязательные поля (is_required = 0)
    input_string("Место работы: ", new_contact.company, MAX_STR, 0);
    input_string("Должность: ", new_contact.position, MAX_STR, 0);
    input_string("Телефон: ", new_contact.phone, 20, 0);
    input_string("Email: ", new_contact.email, MAX_STR, 0);
    input_string("Ссылка на соцсеть: ", new_contact.social, MAX_STR, 0);
    input_string("Профиль в мессенджере: ", new_contact.messenger, MAX_STR, 0);

    contacts[contact_count] = new_contact;
    contact_count++;
    printf("  [Успех] Контакт добавлен.\n");
}

void edit_contact(void) {
    char lname[MAX_STR], fname[MAX_STR];
    printf("\n--- Редактирование контакта ---\n");
    input_string("Введите фамилию для поиска: ", lname, MAX_STR, 1);
    input_string("Введите имя для поиска: ", fname, MAX_STR, 1);

    int idx = find_contact_index(lname, fname);
    if (idx == -1) {
        printf("  [Ошибка] Контакт не найден.\n");
        return;
    }

    printf("Редактирование: %s %s\n", contacts[idx].lastName, contacts[idx].firstName);
    
    input_string("Новая Фамилия: ", contacts[idx].lastName, MAX_STR, 1);
    input_string("Новое Имя: ", contacts[idx].firstName, MAX_STR, 1);
    input_string("Новое место работы: ", contacts[idx].company, MAX_STR, 0);
    input_string("Новая должность: ", contacts[idx].position, MAX_STR, 0);
    input_string("Новый телефон: ", contacts[idx].phone, 20, 0);
    input_string("Новый Email: ", contacts[idx].email, MAX_STR, 0);
    input_string("Новая соцсеть: ", contacts[idx].social, MAX_STR, 0);
    input_string("Новый мессенджер: ", contacts[idx].messenger, MAX_STR, 0);
    
    printf("  [Успех] Контакт обновлен.\n");
}

void delete_contact(void) {
    char lname[MAX_STR], fname[MAX_STR];
    printf("\n--- Удаление контакта ---\n");
    input_string("Введите фамилию: ", lname, MAX_STR, 1);
    input_string("Введите имя: ", fname, MAX_STR, 1);

    int idx = find_contact_index(lname, fname);
    if (idx == -1) {
        printf("  [Ошибка] Контакт не найден.\n");
        return;
    }

    // Сдвиг элементов массива влево для удаления
    for (int i = idx; i < contact_count - 1; i++) {
        contacts[i] = contacts[i + 1];
    }
    contact_count--;
    printf("  [Успех] Контакт удален.\n");
}

void print_all_contacts(void) {
    if (contact_count == 0) {
        printf("\nСписок контактов пуст.\n");
        return;
    }
    
    printf("\n=== СПИСОК КОНТАКТОВ (Всего: %d) ===\n", contact_count);
    for (int i = 0; i < contact_count; i++) {
        printf("[%d] %s %s\n", i + 1, contacts[i].lastName, contacts[i].firstName);
        
        // Выводим только заполненные поля
        if (strlen(contacts[i].company) > 0 || strlen(contacts[i].position) > 0)
            printf("    Работа: %s, %s\n", contacts[i].company, contacts[i].position);
        if (strlen(contacts[i].phone) > 0)
            printf("    Тел: %s\n", contacts[i].phone);
        if (strlen(contacts[i].email) > 0)
            printf("    Email: %s\n", contacts[i].email);
        if (strlen(contacts[i].social) > 0)
            printf("    Соцсеть: %s\n", contacts[i].social);
        if (strlen(contacts[i].messenger) > 0)
            printf("    Мессенджер: %s\n", contacts[i].messenger);
            
        printf("-----------------------------------\n");
    }
}

void find_and_print_contact(void) {
    char lname[MAX_STR], fname[MAX_STR];
    printf("\n--- Поиск контакта ---\n");
    
    // Запрашиваем данные для поиска
    input_string("Введите фамилию: ", lname, MAX_STR, 1);
    input_string("Введите имя: ", fname, MAX_STR, 1);

    // Ищем индекс
    int idx = find_contact_index(lname, fname);
    
    if (idx == -1) {
        printf("  [Ошибка] Контакт не найден.\n");
        return;
    }

    // Выводим данные найденного контакта
    printf("\n=== НАЙДЕН КОНТАКТ ===\n");
    printf("ФИО: %s %s\n", contacts[idx].lastName, contacts[idx].firstName);
    
    if (strlen(contacts[idx].company) > 0 || strlen(contacts[idx].position) > 0)
        printf("Работа: %s, %s\n", contacts[idx].company, contacts[idx].position);
    if (strlen(contacts[idx].phone) > 0)
        printf("Тел: %s\n", contacts[idx].phone);
    if (strlen(contacts[idx].email) > 0)
        printf("Email: %s\n", contacts[idx].email);
    if (strlen(contacts[idx].social) > 0)
        printf("Соцсеть: %s\n", contacts[idx].social);
    if (strlen(contacts[idx].messenger) > 0)
        printf("Мессенджер: %s\n", contacts[idx].messenger);
    
    printf("======================\n");
}