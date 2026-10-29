#include "unity.h"
#include "calc.h"
#include "input.h"
#include <string.h>

// ==========================================
// СЕКЦИЯ НАСТРОЙКИ (SETUP / TEARDOWN)
// ==========================================
void setUp(void) {
    // Для калькулятора очищать нечего, все функции чистые
}

void tearDown(void) {
}

// ==========================================
// ТЕСТЫ ДЛЯ МОДУЛЯ МАТЕМАТИКИ (calc.c)
// ==========================================

void test_calc_add(void) {
    // Проверяем обычные числа
    TEST_ASSERT_FLOAT_WITHIN(0.001, 5.0, calc_add(2.0, 3.0));
    // Проверяем отрицательные и дроби
    TEST_ASSERT_FLOAT_WITHIN(0.001, -2.5, calc_add(-5.0, 2.5));
    // Проверяем ноль
    TEST_ASSERT_FLOAT_WITHIN(0.001, 0.0, calc_add(0.0, 0.0));
}

void test_calc_sub(void) {
    TEST_ASSERT_FLOAT_WITHIN(0.001, 2.0, calc_sub(5.0, 3.0));
    TEST_ASSERT_FLOAT_WITHIN(0.001, -10.0, calc_sub(-5.0, 5.0));
}

void test_calc_mul(void) {
    TEST_ASSERT_FLOAT_WITHIN(0.001, 15.0, calc_mul(5.0, 3.0));
    // Умножение на ноль
    TEST_ASSERT_FLOAT_WITHIN(0.001, 0.0, calc_mul(999.0, 0.0));
    // Умножение двух отрицательных (должно быть положительное)
    TEST_ASSERT_FLOAT_WITHIN(0.001, 6.0, calc_mul(-2.0, -3.0));
}

void test_calc_div_normal(void) {
    double result = 0;
    // Проверяем успешное деление
    int status = calc_div(10.0, 2.0, &result);
    TEST_ASSERT_EQUAL_INT(CALC_SUCCESS, status);
    TEST_ASSERT_FLOAT_WITHIN(0.001, 5.0, result);

    // Проверяем деление с дробным результатом (1 / 3)
    status = calc_div(1.0, 3.0, &result);
    TEST_ASSERT_EQUAL_INT(CALC_SUCCESS, status);
    // Так как 1/3 = 0.33333... проверяем с точностью до 3 знаков
    TEST_ASSERT_FLOAT_WITHIN(0.001, 0.3333, result); 
}

void test_calc_div_by_zero(void) {
    double result = 0;
    // Проверяем деление на ноль
    int status = calc_div(10.0, 0.0, &result);
    // Ожидаем, что вернется код ошибки, а не CALC_SUCCESS
    TEST_ASSERT_EQUAL_INT(CALC_ERR_DIV_ZERO, status);
}

// ==========================================
// ТЕСТЫ ДЛЯ МОДУЛЯ ВВОДА (input.c)
// ==========================================

void test_parse_int_valid(void) {
    int val = 0;
    TEST_ASSERT_TRUE(input_parse_int("42", 0, 100, &val));
    TEST_ASSERT_EQUAL_INT(42, val);

    // Граничные значения (0 и 100)
    TEST_ASSERT_TRUE(input_parse_int("0", 0, 100, &val));
    TEST_ASSERT_TRUE(input_parse_int("100", 0, 100, &val));
}

void test_parse_int_invalid(void) {
    int val = 0;
    // Вне диапазона
    TEST_ASSERT_FALSE(input_parse_int("101", 0, 100, &val));
    TEST_ASSERT_FALSE(input_parse_int("-5", 0, 100, &val));
    // Буквы и мусор
    TEST_ASSERT_FALSE(input_parse_int("abc", 0, 100, &val));
    TEST_ASSERT_FALSE(input_parse_int("12abc", 0, 100, &val));
}

void test_parse_double_valid(void) {
    double val = 0.0;
    // Целые и дробные числа
    TEST_ASSERT_TRUE(input_parse_double("3.14", &val));
    TEST_ASSERT_FLOAT_WITHIN(0.001, 3.14, val);

    TEST_ASSERT_TRUE(input_parse_double("-2.5", &val));
    TEST_ASSERT_FLOAT_WITHIN(0.001, -2.5, val);
    
    TEST_ASSERT_TRUE(input_parse_double("42", &val));
    TEST_ASSERT_FLOAT_WITHIN(0.001, 42.0, val);
}

void test_parse_double_invalid(void) {
    double val = 0.0;
    // Буквы
    TEST_ASSERT_FALSE(input_parse_double("abc", &val));
    // Дробь с мусором в конце
    TEST_ASSERT_FALSE(input_parse_double("3.14abc", &val));
}

// ==========================================
// ГЛАВНАЯ ФУНКЦИЯ ЗАПУСКА ТЕСТОВ
// ==========================================
int main(void) {
    UNITY_BEGIN();

    // Тесты математики
    RUN_TEST(test_calc_add);
    RUN_TEST(test_calc_sub);
    RUN_TEST(test_calc_mul);
    RUN_TEST(test_calc_div_normal);
    RUN_TEST(test_calc_div_by_zero);

    // Тесты парсера
    RUN_TEST(test_parse_int_valid);
    RUN_TEST(test_parse_int_invalid);
    RUN_TEST(test_parse_double_valid);
    RUN_TEST(test_parse_double_invalid);

    return UNITY_END();
}