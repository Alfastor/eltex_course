#include "unity.h"
#include "permissions.h"
#include <string.h>

void setUp(void) {}
void tearDown(void) {}

// 1. Тест парсинга буквенного ввода
void test_parse_symbolic_valid(void) {
    unsigned int mode = 0;
    // rwxr-xr-x должно превратиться в 0755
    TEST_ASSERT_TRUE(perm_parse_symbolic("rwxr-xr-x", &mode));
    TEST_ASSERT_EQUAL_UINT(0755, mode);
    
    // rw-r--r-- превращается в 0644
    TEST_ASSERT_TRUE(perm_parse_symbolic("rw-r--r--", &mode));
    TEST_ASSERT_EQUAL_UINT(0644, mode);
}

void test_parse_symbolic_invalid(void) {
    unsigned int mode = 0;
    // Слишком короткая строка
    TEST_ASSERT_FALSE(perm_parse_symbolic("rwx", &mode));
}

// 2. Тест парсинга цифрового ввода
void test_parse_numeric_valid(void) {
    unsigned int mode = 0;
    TEST_ASSERT_TRUE(perm_parse_numeric("755", &mode));
    TEST_ASSERT_EQUAL_UINT(0755, mode);
    
    TEST_ASSERT_TRUE(perm_parse_numeric("644", &mode));
    TEST_ASSERT_EQUAL_UINT(0644, mode);
}

void test_parse_numeric_invalid(void) {
    unsigned int mode = 0;
    // Число 8 в восьмеричной системе недопустимо
    TEST_ASSERT_FALSE(perm_parse_numeric("888", &mode)); 
    TEST_ASSERT_FALSE(perm_parse_numeric("abc", &mode));
}

// 3. Тест изменения прав (chmod)
void test_apply_chmod_add(void) {
    unsigned int new_mode;
    // 0644 (rw-r--r--) + u+x -> 0744 (rwxr--r--)
    TEST_ASSERT_TRUE(perm_apply_chmod(0644, "u+x", &new_mode));
    TEST_ASSERT_EQUAL_UINT(0744, new_mode);
}

void test_apply_chmod_remove(void) {
    unsigned int new_mode;
    // 0777 (rwxrwxrwx) - go-w -> 0755 (rwxr-xr-x)
    TEST_ASSERT_TRUE(perm_apply_chmod(0777, "go-w", &new_mode));
    TEST_ASSERT_EQUAL_UINT(0755, new_mode);
}

void test_apply_chmod_assign(void) {
    unsigned int new_mode;
    // 0777 (rwxrwxrwx) a=rw -> 0666 (rw-rw-rw-)
    TEST_ASSERT_TRUE(perm_apply_chmod(0777, "a=rw", &new_mode));
    TEST_ASSERT_EQUAL_UINT(0666, new_mode);
}

// 4. Тест конвертации в строки
void test_to_symbolic(void) {
    char str[10];
    perm_to_symbolic(0755, str);
    TEST_ASSERT_EQUAL_STRING("rwxr-xr-x", str);
}

void test_to_binary(void) {
    char str[16];
    // 0755 это 111 101 101
    perm_to_binary(0755, str);
    TEST_ASSERT_EQUAL_STRING("111 101 101", str);
}

int main(void) {
    UNITY_BEGIN();
    
    RUN_TEST(test_parse_symbolic_valid);
    RUN_TEST(test_parse_symbolic_invalid);
    RUN_TEST(test_parse_numeric_valid);
    RUN_TEST(test_parse_numeric_invalid);
    
    RUN_TEST(test_apply_chmod_add);
    RUN_TEST(test_apply_chmod_remove);
    RUN_TEST(test_apply_chmod_assign);
    
    RUN_TEST(test_to_symbolic);
    RUN_TEST(test_to_binary);
    
    return UNITY_END();
}