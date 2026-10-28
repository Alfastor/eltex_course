#ifndef CONTACT_H
#define CONTACT_H

#define MAX_STR 50
#define MAX_CONTACTS 100

typedef struct {
    char lastName[MAX_STR];
    char firstName[MAX_STR];
    char company[MAX_STR];
    char position[MAX_STR];
    char phone[20];
    char email[MAX_STR];
    char social[MAX_STR];
    char messenger[MAX_STR];
} Contact;

// Коды возврата для удобства тестирования
#define CONTACT_SUCCESS 0
#define CONTACT_ERR_FULL -1
#define CONTACT_ERR_NOT_FOUND -2
#define CONTACT_ERR_INVALID -3

// Управление базой
int contact_add(const Contact* c);
int contact_edit(int index, const Contact* new_data);
int contact_delete(int index);
void contact_clear(void);

// Поиск и получение данных
int contact_find(const char* lname, const char* fname);
const Contact* contact_get(int index);
int contact_get_count(void);

#endif // CONTACT_H