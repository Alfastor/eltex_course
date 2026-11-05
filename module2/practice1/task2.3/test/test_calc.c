#include "unity.h"
#include "tokenizer.h"
#include "parser.h"
#include "eval.h"
#include <stdbool.h>
#include <string.h>

void setUp(void) {}
void tearDown(void) {}

// Вспомогательная функция: принимает строку, возвращает результат вычисления
double calculate_string(const char* expr, bool* error) {
    Token tokens[256];
    *error = false;
    
    int count = tokenize(expr, tokens, 256);
    if (count < 0) {
        *error = true;
        return 0.0;
    }
    
    ASTNode* tree = parse_expression(tokens);
    if (!tree) {
        *error = true;
        return 0.0;
    }
    
    double result = evaluate(tree, error);
    free_tree(tree);
    return result;
}

// ТЕСТЫ ТОКЕНИЗАТОРА
void test_tokenizer_valid(void) {
    Token tokens[10];
    int count = tokenize("12.5 + 3", tokens, 10);
    
    TEST_ASSERT_EQUAL_INT(3, count);
    TEST_ASSERT_EQUAL_INT(TOKEN_NUMBER, tokens[0].type);
    TEST_ASSERT_FLOAT_WITHIN(0.001, 12.5, tokens[0].value);
    TEST_ASSERT_EQUAL_INT(TOKEN_PLUS, tokens[1].type);
    TEST_ASSERT_EQUAL_INT(TOKEN_NUMBER, tokens[2].type);
    TEST_ASSERT_FLOAT_WITHIN(0.001, 3.0, tokens[2].value);
}

void test_tokenizer_invalid_char(void) {
    Token tokens[10];
    int count = tokenize("2 & 3", tokens, 10);
    TEST_ASSERT_EQUAL_INT(-1, count);
}

// ТЕСТЫ ВЫЧИСЛЕНИЙ (ПАРСЕР + EVAL)
void test_eval_simple(void) {
    bool error = false;
    TEST_ASSERT_FLOAT_WITHIN(0.001, 5.0, calculate_string("2 + 3", &error));
    TEST_ASSERT_FALSE(error);
}

void test_eval_precedence(void) {
    bool error = false;
    TEST_ASSERT_FLOAT_WITHIN(0.001, 14.0, calculate_string("2 + 3 * 4", &error));
    TEST_ASSERT_FALSE(error);
}

void test_eval_parentheses(void) {
    bool error = false;
    TEST_ASSERT_FLOAT_WITHIN(0.001, 20.0, calculate_string("(2 + 3) * 4", &error));
    TEST_ASSERT_FALSE(error);
}

void test_eval_division(void) {
    bool error = false;
    TEST_ASSERT_FLOAT_WITHIN(0.001, 2.5, calculate_string("10 / 4", &error));
    TEST_ASSERT_FALSE(error);
}

void test_eval_div_by_zero(void) {
    bool error = false;
    calculate_string("5 / 0", &error);
    TEST_ASSERT_TRUE(error);
}

// ТЕСТЫ ОБРАБОТКИ ОШИБОК СИНТАКСИСА
void test_syntax_error_incomplete(void) {
    bool error = false;
    calculate_string("2 + ", &error);
    TEST_ASSERT_TRUE(error);
}

void test_syntax_error_unclosed_paren(void) {
    bool error = false;
    calculate_string("(2 + 3", &error);
    TEST_ASSERT_TRUE(error);
}

void test_syntax_error_extra_tokens(void) {
    bool error = false;
    calculate_string("2 + 3 4", &error);
    TEST_ASSERT_TRUE(error);
}

int main(void) {
    UNITY_BEGIN();
    
    RUN_TEST(test_tokenizer_valid);
    RUN_TEST(test_tokenizer_invalid_char);
    
    RUN_TEST(test_eval_simple);
    RUN_TEST(test_eval_precedence);
    RUN_TEST(test_eval_parentheses);
    RUN_TEST(test_eval_division);
    RUN_TEST(test_eval_div_by_zero);
    
    RUN_TEST(test_syntax_error_incomplete);
    RUN_TEST(test_syntax_error_unclosed_paren);
    RUN_TEST(test_syntax_error_extra_tokens);
    
    return UNITY_END();
}