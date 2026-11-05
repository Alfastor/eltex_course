#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "input.h"
#include "tokenizer.h"
#include "parser.h"
#include "eval.h"

int main(void) {
    char expr[MAX_EXPR_LEN];
    Token tokens[MAX_EXPR_LEN];

    printf("=== МАТЕМАТИЧЕСКИЙ ПАРСЕР ===\n");
    printf("Вводите выражения (например, 2 + 3 * (4 - 1)).\n");
    printf("Для выхода введите 'exit' или пустую строку.\n\n");

    while (1) {
        printf("Введите выражение: ");
        input_read_line(expr, MAX_EXPR_LEN);

        if (input_is_empty(expr) || strcmp(expr, "exit") == 0) {
            break;
        }

        int token_count = tokenize(expr, tokens, MAX_EXPR_LEN);
        if (token_count < 0) {
            printf("  [Ошибка] Недопустимый символ в выражении.\n");
            continue;
        }

        ASTNode* tree = parse_expression(tokens);
        if (!tree) {
            printf("  [Ошибка] Синтаксическая ошибка в выражении.\n");
            continue;
        }

        bool eval_error = false;
        double result = evaluate(tree, &eval_error);

        if (eval_error) {
            printf("  [Ошибка] Деление на ноль.\n");
        } else {
            printf("  = %g\n", result);
        }

        free_tree(tree);
    }

    printf("Выход из программы.\n");
    return 0;
}