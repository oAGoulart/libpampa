#include "cutest/CuTest.h"
#include "file.h"

void test_file_create(CuTest* tc)
{
  CuAssertTrue(tc, file_create("test.log"));
  CuAssertTrue(tc, !file_create("test/test.log"));
  CuAssertTrue(tc, file_create("../test.log"));
}

CuSuite* CuGetSuite()
{
  CuSuite* suite = CuSuiteNew();

  SUITE_ADD_TEST(suite, test_file_create);

  return suite;
}
