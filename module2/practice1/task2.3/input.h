#ifndef INPUT_H
#define INPUT_H
#include <stdbool.h>

#define MAX_EXPR_LEN 256

void input_read_line(char* buffer, int size);
bool input_is_empty(const char* str);

#endif