#include "tokenizer.h"
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

int tokenize(const char* expr, Token* tokens, int max_tokens) {
    int count = 0;
    int i = 0;

    while (expr[i] != '\0' && count < max_tokens - 1) {
        if (isspace(expr[i])) {
            i++;
            continue;
        }

        if (isdigit(expr[i]) || (expr[i] == '.' && isdigit(expr[i+1]))) {
            char* endptr;
            tokens[count].type = TOKEN_NUMBER;
            tokens[count].value = strtod(&expr[i], &endptr);
            i += (endptr - &expr[i]);
            count++;
            continue;
        }

        TokenType type = TOKEN_ERROR;
        switch (expr[i]) {
            case '+': type = TOKEN_PLUS; break;
            case '-': type = TOKEN_MINUS; break;
            case '*': type = TOKEN_MUL; break;
            case '/': type = TOKEN_DIV; break;
            case '(': type = TOKEN_LPAREN; break;
            case ')': type = TOKEN_RPAREN; break;
            default: return -1;
        }
        
        tokens[count].type = type;
        count++;
        i++;
    }

    tokens[count].type = TOKEN_EOF;
    return count;
}