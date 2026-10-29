#include "input.h"
#include <string.h>
#include <stdlib.h>
#include <float.h>
#include <math.h>
#include <errno.h>

bool input_parse_int(const char* str, int min, int max, int* out_val) {
    char* endptr;
    long val = strtol(str, &endptr, 10);
    
    if (endptr == str || (*endptr != '\0' && *endptr != '\n')) return false;
    if (val < min || val > max) return false;
    
    *out_val = (int)val;
    return true;
}

bool input_parse_double(const char* str, double* out_val) {
    if (strlen(str) > 18) return false;

    char* endptr;
    
    // ВАЖНО: сбрасываем errno перед вызовом strtod
    errno = 0; 
    double val = strtod(str, &endptr);

    if (endptr == str || (*endptr != '\0' && *endptr != '\n')) return false;

    // Проверка на переполнение
    if (errno == ERANGE || isinf(val)) {
        return false;
    }

    *out_val = val;
    return true;
}