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

void add_contact(void);
void edit_contact(void);
void delete_contact(void);
void print_all_contacts(void);
void find_and_print_contact(void);

#endif // CONTACT_H