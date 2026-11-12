#include "unity.h"
#include "network.h"

void setUp(void) {}
void tearDown(void) {}

void test_parse_ip_valid(void) {
    uint32_t ip;
    TEST_ASSERT_TRUE(net_parse_ip("192.168.1.10", &ip));
    TEST_ASSERT_EQUAL_HEX32(0xC0A8010A, ip); // C0=192, A8=168, 01=1, 0A=10
}

void test_parse_ip_invalid(void) {
    uint32_t ip;
    TEST_ASSERT_FALSE(net_parse_ip("192.168.1", &ip));    // Мало частей
    TEST_ASSERT_FALSE(net_parse_ip("192.168.1.256", &ip));// Число > 255
    TEST_ASSERT_FALSE(net_parse_ip("abc.1.1.1", &ip));    // Буквы
}

void test_ip_to_str(void) {
    char buf[20];
    net_ip_to_str(0xC0A8010A, buf);
    TEST_ASSERT_EQUAL_STRING("192.168.1.10", buf);
}

void test_same_subnet_true(void) {
    uint32_t ip1, ip2, mask;
    net_parse_ip("192.168.1.10", &ip1);
    net_parse_ip("192.168.1.50", &ip2);
    net_parse_ip("255.255.255.0", &mask);
    
    TEST_ASSERT_TRUE(net_is_same_subnet(ip1, ip2, mask));
}

void test_same_subnet_false(void) {
    uint32_t ip1, ip2, mask;
    net_parse_ip("192.168.1.10", &ip1);
    net_parse_ip("192.168.2.50", &ip2); // Другая подсеть
    net_parse_ip("255.255.255.0", &mask);
    
    TEST_ASSERT_FALSE(net_is_same_subnet(ip1, ip2, mask));
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_parse_ip_valid);
    RUN_TEST(test_parse_ip_invalid);
    RUN_TEST(test_ip_to_str);
    RUN_TEST(test_same_subnet_true);
    RUN_TEST(test_same_subnet_false);
    return UNITY_END();
}