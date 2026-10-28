#include "contact.h"
#include <string.h>

static Contact contacts[MAX_CONTACTS];
static int contact_count = 0;

int contact_add(const Contact* c) {
    if (contact_count >= MAX_CONTACTS) return CONTACT_ERR_FULL;
    if (strlen(c->lastName) == 0 || strlen(c->firstName) == 0) return CONTACT_ERR_INVALID;
    
    contacts[contact_count] = *c;
    contact_count++;
    return CONTACT_SUCCESS;
}

int contact_find(const char* lname, const char* fname) {
    for (int i = 0; i < contact_count; i++) {
        if (strcmp(contacts[i].lastName, lname) == 0 && 
            strcmp(contacts[i].firstName, fname) == 0) {
            return i;
        }
    }
    return -1;
}

int contact_edit(int index, const Contact* new_data) {
    if (index < 0 || index >= contact_count) return CONTACT_ERR_NOT_FOUND;
    if (strlen(new_data->lastName) == 0 || strlen(new_data->firstName) == 0) return CONTACT_ERR_INVALID;
    
    contacts[index] = *new_data;
    return CONTACT_SUCCESS;
}

int contact_delete(int index) {
    if (index < 0 || index >= contact_count) return CONTACT_ERR_NOT_FOUND;
    
    for (int i = index; i < contact_count - 1; i++) {
        contacts[i] = contacts[i + 1];
    }
    contact_count--;
    return CONTACT_SUCCESS;
}

const Contact* contact_get(int index) {
    if (index < 0 || index >= contact_count) return NULL;
    return &contacts[index];
}

int contact_get_count(void) {
    return contact_count;
}

void contact_clear(void) {
    contact_count = 0;
}