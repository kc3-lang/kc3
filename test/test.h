/* c3
 * Copyright 2022,2023 kmx.io <contact@kmx.io>
 *
 * Permission is hereby granted to use this software granted the above
 * copyright notice and this permission paragraph are included in all
 * copies and substantial portions of this software.
 *
 * THIS SOFTWARE IS PROVIDED "AS-IS" WITHOUT ANY GUARANTEE OF
 * PURPOSE AND PERFORMANCE. IN NO EVENT WHATSOEVER SHALL THE
 * AUTHOR BE CONSIDERED LIABLE FOR THE USE AND PERFORMANCE OF
 * THIS SOFTWARE.
 */
#ifndef TEST_H
#define TEST_H

#include <stdio.h>

#define TEST_COLOR_KO "\033[0;91m"
#define TEST_COLOR_OK "\033[0;92m"
#define TEST_COLOR_RESET "\033[0m"

#define TEST_ASSERT(test)                                              \
  do {                                                                 \
    if (test) {                                                        \
      g_test_assert_count++;                                           \
      g_test_assert_ok++;                                              \
    }                                                                  \
    else {                                                             \
      test_ko();                                                       \
      printf("\nAssertion failed in %s:%d %s\n%s\n",                   \
             __FILE__, __LINE__, __func__, # test);                    \
      return 1;                                                        \
    }                                                                  \
  } while(0)

#define TEST_CASE(name)                                                \
  char test_case_ ## name ()                                           \
  {                                                                    \
    g_test_case_name = # name;                                         \

#define TEST_CASE_END(name)                                            \
    test_ok();                                                         \
    return 0;                                                          \
  }

#define TEST_CASE_PROTOTYPE(name)                                      \
  char test_case_ ## name ()

#define TEST_CASE_RUN(name)                                            \
  test_case_ ## name ()

#define TEST_EQ(test, expected)                                        \
  do {                                                                 \
    long long signed TEST_EQ_tmp = (long long signed) (test);          \
    if (TEST_EQ_tmp == (long long signed) (expected)) {                \
      g_test_assert_count++;                                           \
      g_test_assert_ok++;                                              \
    }                                                                  \
    else {                                                             \
      test_ko();                                                       \
      printf("\n%sAssertion failed in %s:%d %s\n"                      \
             "%s == %s\n"                                              \
             "Expected %s got %lld.%s\n",                              \
             TEST_COLOR_KO,                                            \
             __FILE__, __LINE__, __func__,                             \
             # test, # expected, # expected, TEST_EQ_tmp,              \
             TEST_COLOR_RESET);                                        \
      return 1;                                                        \
    }                                                                  \
  } while (0)

#define TEST_FLOAT_EQ(test, expected)                                  \
  do {                                                                 \
    float TEST_FLOAT_EQ_tmp = (float) (test);                          \
    if (fabsf(TEST_FLOAT_EQ_tmp - (float) (expected)) <=               \
        FLT_EPSILON) {                                                 \
      g_test_assert_count++;                                           \
      g_test_assert_ok++;                                              \
    }                                                                  \
    else {                                                             \
      test_ko();                                                       \
      printf("\n%sAssertion failed in %s:%d %s\n"                      \
             "%s == %s\n"                                              \
             "Expected %s got %.16g.%s\n",                             \
             TEST_COLOR_KO,                                            \
             __FILE__, __LINE__, __func__,                             \
             # test, # expected, # expected, TEST_FLOAT_EQ_tmp,        \
             TEST_COLOR_RESET);                                        \
      return 1;                                                        \
    }                                                                  \
  } while (0)

#define TEST_FLOAT_EQ2(test, expected1, expected2)                     \
  do {                                                                 \
    float TEST_FLOAT_EQ2_tmp = (float) (test);                         \
    if (fabsf(TEST_FLOAT_EQ2_tmp -                                     \
              (float) (expected1)) <= FLT_EPSILON ||                   \
        fabsf(TEST_FLOAT_EQ2_tmp -                                     \
              (float) (expected2)) <= FLT_EPSILON) {                   \
      g_test_assert_count++;                                           \
      g_test_assert_ok++;                                              \
    }                                                                  \
    else {                                                             \
      test_ko();                                                       \
      printf("\n%sAssertion failed in %s:%d %s\n"                      \
             "%s == %s\n"                                              \
             "Expected %s got %.16g.%s\n",                             \
             TEST_COLOR_KO,                                            \
             __FILE__, __LINE__, __func__,                             \
             # test, # expected1, # expected1, TEST_FLOAT_EQ2_tmp,     \
             TEST_COLOR_RESET);                                        \
      return 1;                                                        \
    }                                                                  \
  } while (0)

#define TEST_DOUBLE_EQ(test, expected)                                 \
  do {                                                                 \
    f64 TEST_DOUBLE_EQ_tmp = (double) (test);                          \
    if (fabs(TEST_DOUBLE_EQ_tmp - (expected)) <= DBL_EPSILON) {        \
      g_test_assert_count++;                                           \
      g_test_assert_ok++;                                              \
    }                                                                  \
    else {                                                             \
      test_ko();                                                       \
      printf("\n%sAssertion failed in %s:%d %s\n"                      \
             "%s == %s\n"                                              \
             "Expected %s got %lf.%s\n",                               \
             TEST_COLOR_KO,                                            \
             __FILE__, __LINE__, __func__,                             \
             # test, # expected, # expected, TEST_DOUBLE_EQ_tmp,       \
             TEST_COLOR_RESET);                                        \
      return 1;                                                        \
    }                                                                  \
  } while (0)

#define TEST_STR_COMPARE(a, b, expected)                               \
  do {                                                                 \
    sw TEST_STR_COMPARE_tmp = str_compare(a, b);                       \
    if (TEST_STR_COMPARE_tmp == expected) {                            \
      g_test_assert_count++;                                           \
      g_test_assert_ok++;                                              \
    }                                                                  \
    else {                                                             \
      test_ko();                                                       \
      printf("\n%sAssertion failed in %s:%d %s\n"                      \
             "str_compare(%s, %s) == %s\n"                             \
             "Expected %s got %ld.%s\n",                               \
             TEST_COLOR_KO,                                            \
             __FILE__, __LINE__, __func__,                             \
             # a, # b, # expected, # expected, TEST_STR_COMPARE_tmp,   \
             TEST_COLOR_RESET);                                        \
      return 1;                                                        \
    }                                                                  \
  } while (0)

#define TEST_STRNCMP(test, result, bytes)                              \
  do {                                                                 \
    const char *TEST_STRNCMP_tmp = (test);                             \
    if (strncmp(TEST_STRNCMP_tmp, (result), (bytes)) == 0) {           \
      g_test_assert_count++;                                           \
      g_test_assert_ok++;                                              \
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
      fwrite(TEST_STRNCMP_tmp, (bytes), 1, stdout);                    \
      printf("\".%s\n", TEST_COLOR_RESET);                             \
      return 1;                                                        \
    }                                                                  \
  } while (0)

extern long         g_test_assert_count;
extern long         g_test_assert_ko;
extern long         g_test_assert_ok;
extern const char  *g_test_case_name;
extern long         g_test_count;
extern long         g_test_ko;
extern long         g_test_last_ok;
extern long         g_test_ok;
extern const char **g_test_targets;

void test_clean ();
void test_context (const char *context);
int  test_file_compare (const char *path_a, const char *path_b);
void test_init (int argc, char **argv);
void test_ko ();
void test_ok ();
void test_summary ();
int  test_target (const char *target);

#endif /* TEST_H */


