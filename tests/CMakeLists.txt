file(GLOB TEST_SRC *.c)

add_executable(tests test_all.c cutest/CuTest.c ${TEST_SRC})
