#include "unity.h"
#include "ipc.h"
#include "storage.h"
#include <string.h>

static void test_posix_shared_memory_and_posix_semaphore(void)
{
    int creator_fd = -1;
    sem_t *creator_sem = SEM_FAILED;
    void *creator_memory = NULL;
    int attached_fd = -1;
    sem_t *attached_sem = SEM_FAILED;
    void *attached_memory = NULL;

    TEST_ASSERT_EQUAL_INT(0, ipc_cleanup_existing());
    TEST_ASSERT_EQUAL_INT(0, ipc_create(&creator_fd, &creator_sem, &creator_memory));
    TEST_ASSERT_NOT_NULL(creator_memory);
    TEST_ASSERT_TRUE(creator_sem != SEM_FAILED);

    memset(creator_memory, 0, STORAGE_SIZE);
    ((StorageHeader *)creator_memory)->magic = STORAGE_MAGIC;

    TEST_ASSERT_EQUAL_INT(0, ipc_attach(&attached_fd, &attached_sem, &attached_memory));
    TEST_ASSERT_NOT_NULL(attached_memory);
    TEST_ASSERT_TRUE(attached_sem != SEM_FAILED);
    TEST_ASSERT_EQUAL_INT(STORAGE_MAGIC, ((StorageHeader *)attached_memory)->magic);

    TEST_ASSERT_EQUAL_INT(0, ipc_lock(creator_sem));
    ((StorageHeader *)attached_memory)->producer_done = 1;
    TEST_ASSERT_EQUAL_INT(0, ipc_unlock(creator_sem));
    TEST_ASSERT_EQUAL_INT(1, ((StorageHeader *)creator_memory)->producer_done);

    TEST_ASSERT_EQUAL_INT(0, ipc_detach(attached_fd, attached_sem, attached_memory));
    TEST_ASSERT_EQUAL_INT(0, ipc_detach(creator_fd, creator_sem, creator_memory));
    TEST_ASSERT_EQUAL_INT(0, ipc_remove());
    TEST_ASSERT_EQUAL_INT(0, ipc_cleanup_existing());
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_posix_shared_memory_and_posix_semaphore);
    return UnityEnd();
}
