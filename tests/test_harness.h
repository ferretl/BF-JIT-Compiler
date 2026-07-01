#ifndef BF_JIT_TEST_HARNESS_H
#define BF_JIT_TEST_HARNESS_H

#include <stdio.h>

static int tests_run = 0;
static int tests_failed = 0;
static int current_test_failed = false;

#define CHECK(cond)                                                                                                    \
  do {                                                                                                                 \
	if (!(cond)) {                                                                                                     \
	  current_test_failed = true;                                                                                      \
	  fprintf(stderr, "  FAIL: %s:%d: %s\n", __FILE__, __LINE__, #cond);                                               \
	}                                                                                                                  \
  } while (false)

#define RUN_TEST(fn)                                                                                                   \
  do {                                                                                                                 \
	current_test_failed = 0;                                                                                           \
	fn();                                                                                                              \
	tests_run++;                                                                                                       \
	if (current_test_failed) {                                                                                         \
	  tests_failed++;                                                                                                  \
	  printf("  [FAIL] %s\n", #fn);                                                                                    \
	} else {                                                                                                           \
	  printf("  [ok]   %s\n", #fn);                                                                                    \
	}                                                                                                                  \
  } while (false)

#define TEST_SUMMARY() (printf("\n%d tests, %d failed\n", tests_run, tests_failed), tests_failed == 0 ? 0 : 1)

#endif // BF_JIT_TEST_HARNESS_H
