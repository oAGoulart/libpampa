#include <stdbool.h>
#include "acutest.h"
#include "memory.h"

void test_memory_append()
{
  data_t src = { 0, NULL }, dst = { 0, NULL };

  TEST_ASSERT(data_alloc(&src, 64) == true);
  TEST_CHECK(src.size == 64);
  memset(src.address, 20, src.size);

  TEST_CHECK(data_append(&src, &dst) == true);

  data_free(&src);
  data_free(&dst);
}

void test_memory_append_byte()
{
  data_t data = { 0, NULL };

  TEST_CHECK(data_append_byte(&data, 50) == true);
  TEST_CHECK(data.size == 1);
  TEST_CHECK(data_append_byte(&data, 60) == true);
  TEST_CHECK(data.size == 2);

  data_free(&data);
}

TEST_LIST = {
  { "memory_append", test_memory_append },
  { "memory_append_byte", test_memory_append_byte },
  { NULL, NULL }
};
