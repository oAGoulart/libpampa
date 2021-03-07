#include "acutest.h"
#include "memory.h"

void test_memory_append()
{
  data_t src = { 64, NULL};
  data_t dst = { 64, NULL};

  TEST_ASSERT(data_alloc(&src) && data_alloc(&dst));
  (void)memset(src, 20, src.size);
  (void)memset(dst, 40, dst.size);

  TEST_CHECK(data_append(&src, &dst));

  data_free(src);
  data_free(dst);
}

TEST_LIST = {
  { "memory_append", test_memory_append },
  { NULL, NULL }
};
