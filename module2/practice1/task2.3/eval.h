#ifndef EVAL_H
#define EVAL_H
#include "parser.h"
#include <stdbool.h>

// Указатель на функцию для математических операций
typedef bool (*math_operation)(double a, double b, double* result);

double evaluate(ASTNode* node, bool* error);

#endif