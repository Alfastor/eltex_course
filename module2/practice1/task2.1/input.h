#ifndef INPUT_H
#define INPUT_H

// Очистка буфера ввода (удаление символов до '\n' или EOF)
void clear_buffer(void);

// Ввод строки. is_required = 1 (обязательное поле), 0 (необязательное)
void input_string(const char* prompt, char* buffer, int max_len, int is_required);

// Ввод целого числа в заданном диапазоне
int input_int(const char* prompt, int min, int max);

#endif // INPUT_H