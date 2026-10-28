#include "input.h"
#include <string.h>
#include <stdlib.h>

bool input_parse_int(const char* str, int min, int max, int* out_val) {
    char* endptr;
    long val = strtol(str, &endptr, 10);
    
    // Если не прочитали ни одной цифры или остались лишние символы (кроме \n)
    if (endptr == str || (*endptr != '\0' && *endptr != '\n')) return false;
    if (val < min || val > max) return false;
    
    *out_val = (int)val;
    return true;
}

bool input_parse_double(const char* str, double* out_val) {
    char* endptr;
    double val = strtod(str, &endptr);
    
    if (endptr == str || (*endptr != '\0' && *endptr != '\n')) return false;
    
    *out_val = val;
    return true;
}

bool input_validate_string(const char* str, bool is_required) {
    if (is_required && strlen(str) == 0) return false;
    return true;
}