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

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_parse_int_valid_input);
    RUN_TEST(test_parse_int_out_of_bounds);
    RUN_TEST(test_parse_int_garbage_input);
    RUN_TEST(test_validate_required_string);
    RUN_TEST(test_contact_add_and_find);
    RUN_TEST(test_contact_add_invalid_data);
    RUN_TEST(test_contact_not_found);
    return UNITY_END();
}