#include "unity.h"
#include "storage.h"
#include <string.h>
#include <stddef.h>

static unsigned char memory[512];

static void set_up_storage(void)
{
    storage_initialize(memory, sizeof(memory), 1234u);
}

static void test_initialization(void)
{
    set_up_storage();
    StorageHeader *header = (StorageHeader *)memory;
    TEST_ASSERT_EQUAL_INT(STORAGE_MAGIC, header->magic);
    TEST_ASSERT_EQUAL_INT(0, header->first_offset);
    TEST_ASSERT_EQUAL_INT(0, header->producer_done);
    TEST_ASSERT_EQUAL_INT(1234, header->producer_pid);
}

static void test_append_and_read(void)
{
    set_up_storage();
    int values[] = {7, -3, 18, 4};
    TEST_ASSERT_EQUAL_INT(1, storage_append(memory, sizeof(memory), values, 4));
    DataBlock *block = storage_next_pending(memory, sizeof(memory));
    TEST_ASSERT_NOT_NULL(block);
    TEST_ASSERT_EQUAL_INT(4, block->count);
    TEST_ASSERT_EQUAL_INT(-3, block->values[1]);
}

static void test_minmax(void)
{
    set_up_storage();
    int values[] = {7, -3, 18, 4};
    storage_append(memory, sizeof(memory), values, 4);
    MinMax result;
    TEST_ASSERT_EQUAL_INT(0, storage_minmax(storage_next_pending(memory, sizeof(memory)), &result));
    TEST_ASSERT_EQUAL_INT(-3, result.minimum);
    TEST_ASSERT_EQUAL_INT(18, result.maximum);
}

static void test_processed_state(void)
{
    set_up_storage();
    int values[] = {1, 2};
    storage_append(memory, sizeof(memory), values, 2);
    storage_finish(memory);
    TEST_ASSERT_FALSE(storage_all_processed(memory, sizeof(memory)));
    storage_next_pending(memory, sizeof(memory))->count = 0;
    TEST_ASSERT_TRUE(storage_all_processed(memory, sizeof(memory)));
}

static void test_invalid_append(void)
{
    set_up_storage();
    int values[] = {1};
    TEST_ASSERT_EQUAL_INT(-1, storage_append(memory, sizeof(memory), values, 0));
    TEST_ASSERT_EQUAL_INT(-1, storage_append(memory, sizeof(memory), values, STORAGE_MAX_VALUES + 1));
}

static void test_capacity_limit(void)
{
    unsigned char small_memory[sizeof(StorageHeader) + 8];
    storage_initialize(small_memory, sizeof(small_memory), 1234u);
    int values[STORAGE_MAX_VALUES] = {0,1,2,3,4,5,6,7};
    TEST_ASSERT_EQUAL_INT(0, storage_append(small_memory, sizeof(small_memory), values, STORAGE_MAX_VALUES));
}


static void test_consumer_registry(void)
{
    set_up_storage();
    TEST_ASSERT_EQUAL_INT(0, storage_register_consumer(memory, 2001));
    TEST_ASSERT_EQUAL_INT(1, storage_active_consumers(memory));
    TEST_ASSERT_EQUAL_INT(0, storage_touch_consumer(memory, 2001));
    storage_unregister_consumer(memory, 2001);
    TEST_ASSERT_EQUAL_INT(0, storage_active_consumers(memory));
}

static void test_consumer_reaping(void)
{
    set_up_storage();
    TEST_ASSERT_EQUAL_INT(0, storage_register_consumer(memory, 2002));
    uint64_t now = storage_monotonic_ns();
    TEST_ASSERT_EQUAL_INT(1, storage_reap_consumers(memory, now));
    TEST_ASSERT_EQUAL_INT(0, storage_reap_consumers(memory, now + STORAGE_CONSUMER_TIMEOUT_NS + 1));
    TEST_ASSERT_EQUAL_INT(0, storage_active_consumers(memory));
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_initialization);
    RUN_TEST(test_append_and_read);
    RUN_TEST(test_minmax);
    RUN_TEST(test_processed_state);
    RUN_TEST(test_invalid_append);
    RUN_TEST(test_capacity_limit);
    RUN_TEST(test_consumer_registry);
    RUN_TEST(test_consumer_reaping);
    return UnityEnd();
}
