#include <stdio.h>
#include "contact.h"
#include "input.h"

int main(void) {
    int choice;

    while (1) {
        // В цикле while в main.c:
        printf("1. Показать все контакты\n");
        printf("2. Найти контакт\n"); // <--- Новый пункт
        printf("3. Добавить контакт\n");
        printf("4. Редактировать контакт\n");
        printf("5. Удалить контакт\n");
        printf("0. Выход\n");

        choice = input_int("Выберите действие: ", 0, 5); // <--- Изменили верхнюю границу на 5

        switch (choice) {
            case 1: print_all_contacts(); break;
            case 2: find_and_print_contact(); break; // <--- Вызов новой функции
            case 3: add_contact(); break;
            case 4: edit_contact(); break;
            case 5: delete_contact(); break;
            case 0: 
                printf("Выход из программы.\n");
                return 0;
        }
    }
    return 0;
}