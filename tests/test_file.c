#include "acutest.h"
#include "file.h"

void test_file_create()
{
  TEST_CHECK(file_create("test.log") == true);
  TEST_CHECK(file_create("test/test.log") == false);
  TEST_CHECK(file_create("../test.log") == true);
}

TEST_LIST = {
    { "file_create", test_file_create },
    { NULL, NULL }
};
