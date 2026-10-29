#ifndef INPUT_H
#define INPUT_H
#include <stdbool.h>

bool input_parse_int(const char* str, int min, int max, int* out_val);
bool input_parse_double(const char* str, double* out_val);

#endif // INPUT_H