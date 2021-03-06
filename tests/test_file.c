#include "acutest.h"
#include "file.h"

void test_file_create()
{
  TEST_CHECK(file_create("test.log") == true);
  TEST_CHECK(file_create("test.log") == false);
  TEST_CHECK(file_create("test/test.log") == false);
  TEST_CHECK(file_create("../test.log") == true);
}

void test_file_open()
{
  file_t* file = NULL;

  TEST_CHECK(file_open(&file, NULL) == false);
  TEST_CHECK(file_open(NULL, "test.log") == false);
  TEST_CHECK(file_open(NULL, NULL) == false);
  TEST_CHECK(file_open(&file, "filethatdontexist.log") == false);

  file_create("filethatexist.log");
  TEST_CHECK(file_open(&file, "filethatexist.log") == true);
  file_close(file);
}

TEST_LIST = {
  { "file_create", test_file_create },
  { "file_open", test_file_open },
  { NULL, NULL }
};
