#include "unity.h"
#include "input.h"
#include "contact.h"
#include <string.h>

void setUp(void) {
    contact_clear();
}

void tearDown(void) {
}

void test_parse_int_valid_input(void) {
    int result = 0;
    TEST_ASSERT_TRUE(input_parse_int("42", 0, 100, &result));
    TEST_ASSERT_EQUAL_INT(42, result);
}

void test_parse_int_out_of_bounds(void) {
    int result = 0;
    TEST_ASSERT_FALSE(input_parse_int("150", 0, 100, &result));
}

void test_parse_int_garbage_input(void) {
    int result = 0;
    TEST_ASSERT_FALSE(input_parse_int("abc", 0, 100, &result));
    TEST_ASSERT_FALSE(input_parse_int("12abc", 0, 100, &result));
}

void test_validate_required_string(void) {
    TEST_ASSERT_FALSE(input_validate_string("", true));
    TEST_ASSERT_TRUE(input_validate_string("Ivan", true));
    TEST_ASSERT_TRUE(input_validate_string("", false));
}

void test_contact_add_and_find(void) {
    Contact c;
    memset(&c, 0, sizeof(Contact));
    strcpy(c.lastName, "Ivanov");
    strcpy(c.firstName, "Ivan");

    int add_result = contact_add(&c);
    TEST_ASSERT_EQUAL_INT(CONTACT_SUCCESS, add_result);

    int index = contact_find("Ivanov", "Ivan");
    TEST_ASSERT_NOT_EQUAL(-1, index);

    const Contact* found = contact_get(index);
    TEST_ASSERT_NOT_NULL(found);
    TEST_ASSERT_EQUAL_STRING("Ivanov", found->lastName);
    TEST_ASSERT_EQUAL_STRING("Ivan", found->firstName);
}

void test_contact_add_invalid_data(void) {
    Contact c;
    memset(&c, 0, sizeof(Contact));
    TEST_ASSERT_EQUAL_INT(CONTACT_ERR_INVALID, contact_add(&c));
}

void test_contact_not_found(void) {
    int index = contact_find("Unknown", "Person");
    TEST_ASSERT_EQUAL_INT(-1, index);
}

void test_contact_edit(void) {
    // 1. Добавляем контакт
    Contact c;
    memset(&c, 0, sizeof(Contact));
    strcpy(c.lastName, "OldLastName");
    strcpy(c.firstName, "OldFirstName");
    contact_add(&c);

    // 2. Находим его индекс
    int idx = contact_find("OldLastName", "OldFirstName");
    TEST_ASSERT_NOT_EQUAL(-1, idx);

    // 3. Создаем новые данные
    Contact new_c;
    memset(&new_c, 0, sizeof(Contact));
    strcpy(new_c.lastName, "NewLastName");
    strcpy(new_c.firstName, "NewFirstName");
    strcpy(new_c.phone, "1234567");

    // 4. Редактируем
    int edit_result = contact_edit(idx, &new_c);
    TEST_ASSERT_EQUAL_INT(CONTACT_SUCCESS, edit_result);

    // 5. Проверяем, что данные обновились
    const Contact* updated = contact_get(idx);
    TEST_ASSERT_EQUAL_STRING("NewLastName", updated->lastName);
    TEST_ASSERT_EQUAL_STRING("NewFirstName", updated->firstName);
    TEST_ASSERT_EQUAL_STRING("1234567", updated->phone);

    // 6. Проверяем, что старые данные больше не находятся
    TEST_ASSERT_EQUAL_INT(-1, contact_find("OldLastName", "OldFirstName"));
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_parse_int_valid_input);
    RUN_TEST(test_parse_int_out_of_bounds);
    RUN_TEST(test_parse_int_garbage_input);
    RUN_TEST(test_validate_required_string);
    RUN_TEST(test_contact_add_and_find);
    RUN_TEST(test_contact_add_invalid_data);
    RUN_TEST(test_contact_edit);
    RUN_TEST(test_contact_not_found);
    return UNITY_END();
}