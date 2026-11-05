#include "parser.h"
#include <stdlib.h>
#include <stdio.h>

static ASTNode* parse_term(Token** token_ptr);
static ASTNode* parse_factor(Token** token_ptr);

static ASTNode* create_node(NodeType type, double value, ASTNode* left, ASTNode* right) {
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
    if (!node) exit(EXIT_FAILURE);
    node->type = type;
    node->value = value;
    node->left = left;
    node->right = right;
    return node;
}

static ASTNode* parse_expression_ext(Token** token_ptr) {
    ASTNode* left = parse_term(token_ptr);
    if (!left) return NULL;

    while ((*token_ptr)->type == TOKEN_PLUS || (*token_ptr)->type == TOKEN_MINUS) {
        TokenType op = (*token_ptr)->type;
        (*token_ptr)++;

        ASTNode* right = parse_term(token_ptr);
        if (!right) { 
            free_tree(left); 
            return NULL; 
        }

        left = create_node(op == TOKEN_PLUS ? NODE_ADD : NODE_SUB, 0, left, right);
    }
    return left;
}

static ASTNode* parse_term(Token** token_ptr) {
    ASTNode* left = parse_factor(token_ptr);
    if (!left) return NULL;

    while ((*token_ptr)->type == TOKEN_MUL || (*token_ptr)->type == TOKEN_DIV) {
        TokenType op = (*token_ptr)->type;
        (*token_ptr)++;

        ASTNode* right = parse_factor(token_ptr);
        if (!right) { 
            free_tree(left); 
            return NULL; 
        }

        left = create_node(op == TOKEN_MUL ? NODE_MUL : NODE_DIV, 0, left, right);
    }
    return left;
}

static ASTNode* parse_factor(Token** token_ptr) {
    Token* current = *token_ptr;

    if (current->type == TOKEN_NUMBER) {
        (*token_ptr)++;
        return create_node(NODE_NUMBER, current->value, NULL, NULL);
    }

    if (current->type == TOKEN_LPAREN) {
        (*token_ptr)++;
        ASTNode* node = parse_expression_ext(token_ptr);
        if (!node) return NULL;

        if ((*token_ptr)->type != TOKEN_RPAREN) {
            free_tree(node);
            return NULL;
        }
        (*token_ptr)++;
        return node;
    }

    return NULL;
}

ASTNode* parse_expression(Token* tokens) {
    Token* ptr = tokens;
    ASTNode* root = parse_expression_ext(&ptr);
    if (root && ptr->type != TOKEN_EOF) {
        free_tree(root);
        return NULL;
    }
    return root;
}

void free_tree(ASTNode* node) {
    if (!node) return;
    free_tree(node->left);
    free_tree(node->right);
    free(node);
}