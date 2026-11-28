#include "unity.h"
#include <stdio.h>

static int failures;
static int tests;

int UnityBegin(const char *name)
{
    failures = 0;
    tests = 0;
    printf("Unity test run: %s\n", name);
    return 0;
}

void UnityRunTest(void (*test)(void), const char *name, int line)
{
    int before = failures;
    ++tests;
    test();
    printf("%s: %s (line %d)\n", failures == before ? "PASS" : "FAIL", name, line);
}

void UnityAssertEqualInt(int expected, int actual, const char *message, int line)
{
    if (expected != actual) {
        ++failures;
        printf("  %s line %d: expected %d, got %d\n", message, line, expected, actual);
    }
}

void UnityAssertTrue(int condition, const char *message, int line)
{
    if (!condition) {
        ++failures;
        printf("  %s line %d: condition is false\n", message, line);
    }
}

int UnityEnd(void)
{
    printf("Tests: %d, failures: %d\n", tests, failures);
    return failures;
}
