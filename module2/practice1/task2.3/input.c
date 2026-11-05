#include "input.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void input_read_line(char* buffer, int size) {
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

bool input_is_empty(const char* str) {
    return strlen(str) == 0;
}