/* c3
 * Copyright 2022 kmx.io <contact@kmx.io>
 *
 * Permission is hereby granted to use this software excepted
 * on Apple computers granted the above copyright notice and
 * this permission paragraph are included in all copies and
 * substantial portions of this software.
 *
 * THIS SOFTWARE IS PROVIDED "AS-IS" WITHOUT ANY GUARANTEE OF
 * PURPOSE AND PERFORMANCE. IN NO EVENT WHATSOEVER SHALL THE
 * AUTHOR BE CONSIDERED LIABLE FOR THE USE AND PERFORMANCE OF
 * THIS SOFTWARE.
 */
#ifndef TEST_H
#define TEST_H

#include <stdio.h>
#define TEST_COLOR_KO "\033[91m"
#define TEST_COLOR_OK "\033[92m"
#define TEST_COLOR_RESET "\033[39m"

#define TEST_ASSERT(test)                                              \
  do {                                                                 \
    if (test) {                                                        \
      test_ok();                                                       \
    }                                                                  \
    else {                                                             \
      test_ko();                                                       \
      printf("\nAssertion failed in %s:%d %s\n%s\n",                   \
             __FILE__, __LINE__, __func__, # test);                    \
    }                                                                  \
  } while(0)

#define TEST_EQ(test, expected)                                        \
  do {                                                                 \
    long long signed tmp = (long long signed) (test);                  \
    if (tmp == (long long signed) (expected)) {                        \
      test_ok();                                                       \
    }                                                                  \
    else {                                                             \
      test_ko();                                                       \
      printf("\n%sAssertion failed in %s:%d %s\n"                      \
             "%s == %s\n"                                              \
             "Expected %s got %lld.%s\n",                              \
             TEST_COLOR_KO,                                            \
             __FILE__, __LINE__, __func__,                             \
             # test, # expected, # expected, tmp,                      \
             TEST_COLOR_RESET);                                        \
    }                                                                  \
  } while (0)

#define TEST_FLOAT_EQ(test, expected)                                  \
  do {                                                                 \
    f32 tmp = (f32) (test);                                            \
    if (fabsf(tmp - (f32) (expected)) <= FLT_EPSILON)                  \
      test_ok();                                                       \
    else {                                                             \
      test_ko();                                                       \
      printf("\n%sAssertion failed in %s:%d %s\n"                      \
             "%s == %s\n"                                              \
             "Expected %s got %.16g.%s\n",                             \
             TEST_COLOR_KO,                                            \
             __FILE__, __LINE__, __func__,                             \
             # test, # expected, # expected, tmp,                      \
             TEST_COLOR_RESET);                                        \
    }                                                                  \
  } while (0)

#define TEST_FLOAT_EQ2(test, expected1, expected2)                     \
  do {                                                                 \
    f32 tmp = (f32) (test);                                            \
    if (fabsf(tmp - (f32) (expected1)) <= FLT_EPSILON ||               \
        fabsf(tmp - (f32) (expected2)) <= FLT_EPSILON)                 \
      test_ok();                                                       \
    else {                                                             \
      test_ko();                                                       \
      printf("\n%sAssertion failed in %s:%d %s\n"                      \
             "%s == %s\n"                                              \
             "Expected %s got %.16g.%s\n",                             \
             TEST_COLOR_KO,                                            \
             __FILE__, __LINE__, __func__,                             \
             # test, # expected1, # expected1, tmp,                    \
             TEST_COLOR_RESET);                                        \
    }                                                                  \
  } while (0)

#define TEST_DOUBLE_EQ(test, expected)                                 \
  do {                                                                 \
    f64 tmp = (f64) (test);                                            \
    if (fabs(tmp - (expected)) <= DBL_EPSILON)                         \
      test_ok();                                                       \
    else {                                                             \
      test_ko();                                                       \
      printf("\n%sAssertion failed in %s:%d %s\n"                      \
             "%s == %s\n"                                              \
             "Expected %s got %lf.%s\n",                                \
             TEST_COLOR_KO,                                            \
             __FILE__, __LINE__, __func__,                             \
             # test, # expected, # expected, tmp,                      \
             TEST_COLOR_RESET);                                        \
    }                                                                  \
  } while (0)

#define TEST_STR_CMP(a, b, expected)			               \
  do {                                                                 \
    sw tmp = str_compare(a, b);                                        \
    if (tmp == expected) {                                             \
      test_ok();                                                       \
    }                                                                  \
    else {                                                             \
      test_ko();                                                       \
      printf("\n%sAssertion failed in %s:%d %s\n"                      \
             "str_compare(%s, %s) == %s\n"                             \
             "Expected %s got %ld.%s\n",                               \
             TEST_COLOR_KO,                                            \
             __FILE__, __LINE__, __func__,                             \
             # a, # b, # expected, # expected, tmp,                    \
             TEST_COLOR_RESET);                                        \
    }                                                                  \
  } while (0)

#define TEST_STRNCMP(test, result, bytes)                              \
  do {                                                                 \
    const char *tmp = (test);                                          \
    if (strncmp(tmp, (result), (bytes)) == 0) {                        \
      test_ok();                                                       \
    }                                                                  \
    else {                                                             \
      test_ko();                                                       \
      printf("\n%sAssertion failed in %s:%d %s\n"                      \
             "strncmp(%s, %s, %ld) == 0\n",                            \
             TEST_COLOR_KO,                                            \
             __FILE__, __LINE__, __func__,                             \
             # test, # result, (long) (bytes));                        \
      printf("Expected %s got \"",                                     \
             # result);                                                \
      fwrite(tmp, (bytes), 1, stdout);                                 \
      printf("\".%s\n", TEST_COLOR_RESET);                             \
    }                                                                  \
  } while (0)

extern long         g_test_count;
extern long         g_test_ko;
extern long         g_test_last_ok;
extern long         g_test_ok;
extern const char **g_test_targets;

void test_context (const char *context);
int  test_file_compare (const char *path_a, const char *path_b);
void test_init (int argc, char **argv);
void test_ko ();
void test_ok ();
void test_shutdown ();
void test_summary ();
int  test_target (const char *target);

#endif /* TEST_H */


