#ifndef PERMISSIONS_H
#define PERMISSIONS_H
#include <stdbool.h>

// Парсит строку вида "rwxr-xr-x" или "755" и записывает результат по указателю
bool perm_parse_symbolic(const char* str, unsigned int* out_mode);
bool perm_parse_numeric(const char* str, unsigned int* out_mode);

// Применяет команду chmod (например, "u+x") к текущим правам
bool perm_apply_chmod(unsigned int current_mode, const char* cmd, unsigned int* out_mode);

// Получает права файла через stat()
bool perm_get_file_mode(const char* filename, unsigned int* out_mode);

// Конвертирует права в строки (НЕ печатает на экран!)
void perm_to_symbolic(unsigned int mode, char* out_str);
void perm_to_binary(unsigned int mode, char* out_str);

#endif // PERMISSIONS_H