#ifndef INPUT_H
#define INPUT_H
#include <stdbool.h>

// Парсит строку в целое число в заданном диапазоне. Возвращает true при успехе.
bool input_parse_int(const char* str, int min, int max, int* out_val);

// Парсит строку в число с плавающей точкой. Возвращает true при успехе.
bool input_parse_double(const char* str, double* out_val);

// Проверяет строку (например, на пустоту). Возвращает true, если строка валидна.
bool input_validate_string(const char* str, bool is_required);

#endif // INPUT_H