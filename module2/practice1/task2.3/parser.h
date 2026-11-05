#ifndef PARSER_H
#define PARSER_H
#include "tokenizer.h"

typedef enum {
    NODE_NUMBER,
    NODE_ADD,
    NODE_SUB,
    NODE_MUL,
    NODE_DIV
} NodeType;

typedef struct ASTNode {
    NodeType type;
    double value;
    struct ASTNode* left;
    struct ASTNode* right;
} ASTNode;

ASTNode* parse_expression(Token* tokens);
void free_tree(ASTNode* node);

#endif