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

  TEST_ASSERT(file_create("filethatexist.log") == true);
  TEST_CHECK(file_open(&file, "filethatexist.log") == true);

  file_close(file);
}

void test_file_write()
{
  file_t* file = NULL;
  TEST_ASSERT(file_open(&file, "filethatexist.log") == true);

  TEST_ASSERT(data_alloc(&file->buffer, file->buffer.size + 64) == true);
  memset(file->buffer.address, 72, 64);

  TEST_CHECK(file_write(file, 0, 32, false) == true);
  TEST_CHECK(file_write(file, 32, 32, true) == true);
  TEST_CHECK(file_write(file, 99, 64, false) == false);

  file_close(file);
}

void test_file_read()
{
  file_t* file = NULL;
  TEST_ASSERT(file_open(&file, "filethatexist.log") == true);

  TEST_CHECK(file_read(file, file->offset + 1, 1, false) == true);
  TEST_CHECK(file->buffer.size == 1);
  TEST_CHECK(file_read(file, file->offset, 4, true) == true);
  TEST_CHECK(file->buffer.size == 4);
  TEST_CHECK(file_read(file, file->offset, 512, true) == true);
  TEST_CHECK(file->buffer.size == 60);
  TEST_CHECK(file->offset == 64);

  file_close(file);
}

void test_file_read_line()
{
  file_t* file = NULL;
  TEST_ASSERT(file_open(&file, "filethatexist.log") == true);

  TEST_ASSERT(data_alloc(&file->buffer, 1) == true);
  memset(file->buffer.address, 10, 1);
  TEST_CHECK(file_write(file, 4, 1, false) == true);

  TEST_CHECK(file_read_line(file, true) == true);
  TEST_CHECK(file->buffer.size == 5);
  TEST_CHECK(file->offset == 5);

  file_close(file);
}

TEST_LIST = {
  { "file_create", test_file_create },
  { "file_open", test_file_open },
  { "file_write", test_file_write },
  { "file_read", test_file_read },
  { "file_read_line", test_file_read_line },
  { NULL, NULL }
};
