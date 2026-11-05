#include "eval.h"

// --- Базовые математические функции ---
static bool do_add(double a, double b, double* result) {
    *result = a + b;
    return true;
}

static bool do_sub(double a, double b, double* result) {
    *result = a - b;
    return true;
}

static bool do_mul(double a, double b, double* result) {
    *result = a * b;
    return true;
}

static bool do_div(double a, double b, double* result) {
    if (b == 0.0) return false; // Ошибка деления на ноль
    *result = a / b;
    return true;
}

/*
static bool do_nothing(double a, double b, double* result) {
    *result = a * b * 100;
    return true;
}
*/

// --- Динамическая таблица команд ---
typedef struct {
    NodeType type;
    math_operation op;
} CommandMap;

static CommandMap commands[] = {
    {NODE_ADD, do_add},
    {NODE_SUB, do_sub},
    {NODE_MUL, do_mul},
    {NODE_DIV, do_div}
};

static const int num_commands = sizeof(commands) / sizeof(commands[0]);

// --- Вычислитель ---
double evaluate(ASTNode* node, bool* error) {
    if (*error || !node) return 0.0;

    // Число — это базовый случай рекурсии
    if (node->type == NODE_NUMBER) {
        return node->value;
    }

    // Рекурсивно вычисляем левую и правую части
    double left = evaluate(node->left, error);
    double right = evaluate(node->right, error);

    if (*error) return 0.0;

    // Ищем подходящую команду в таблице
    for (int i = 0; i < num_commands; i++) {
        if (commands[i].type == node->type) {
            double result = 0.0;
            // Вызываем функцию через указатель
            if (!commands[i].op(left, right, &result)) {
                *error = true;
                return 0.0;
            }
            return result;
        }
    }

    // Если узел неизвестного типа
    *error = true;
    return 0.0;
}