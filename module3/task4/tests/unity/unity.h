#ifndef UNITY_H
#define UNITY_H

int UnityBegin(const char *name);
int UnityEnd(void);
void UnityRunTest(void (*test)(void), const char *name, int line);
void UnityAssertEqualInt(int expected, int actual, const char *message, int line);
void UnityAssertTrue(int condition, const char *message, int line);

#define UNITY_BEGIN() UnityBegin(__FILE__)
#define RUN_TEST(function) UnityRunTest(function, #function, __LINE__)
#define TEST_ASSERT_EQUAL_INT(expected, actual) UnityAssertEqualInt((expected), (actual), "TEST_ASSERT_EQUAL_INT", __LINE__)
#define TEST_ASSERT_TRUE(condition) UnityAssertTrue((condition), "TEST_ASSERT_TRUE", __LINE__)
#define TEST_ASSERT_FALSE(condition) UnityAssertTrue(!(condition), "TEST_ASSERT_FALSE", __LINE__)
#define TEST_ASSERT_NULL(pointer) UnityAssertTrue((pointer) == 0, "TEST_ASSERT_NULL", __LINE__)
#define TEST_ASSERT_NOT_NULL(pointer) UnityAssertTrue((pointer) != 0, "TEST_ASSERT_NOT_NULL", __LINE__)

#endif
