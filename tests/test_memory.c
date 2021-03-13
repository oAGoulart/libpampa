#include "acutest.h"
#include "memory.h"

void test_memory_append()
{
  data_t src, dst;

  TEST_ASSERT(data_alloc(&src, 64) && data_alloc(&dst, 64));
  memset(src.address, 20, src.size);
  memset(dst.address, 40, dst.size);

  TEST_CHECK(data_append(&src, &dst));

  data_free(&src);
  data_free(&dst);
}

TEST_LIST = {
  { "memory_append", test_memory_append },
  { NULL, NULL }
};
