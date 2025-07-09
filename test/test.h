/* kc3
 * Copyright from 2022 to 2025 kmx.io <contact@kmx.io>
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

#include <float.h>
#include <stdio.h>
#include "../libkc3/buf.h"

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
      fprintf(stderr, "\nAssertion failed in %s:%d %s\n%s\n",          \
              __FILE__, __LINE__, __func__, # test);                   \
      return 1;                                                        \
    }                                                                  \
  } while(0)

#define TEST_CASE(name)                                                \
  char test_case_ ## name (void)                                       \
  {                                                                    \
    g_test_case_name = # name;                                         \

#define TEST_CASE_END(name)                                            \
    test_ok();                                                         \
    return 0;                                                          \
  }

#define TEST_CASE_PROTOTYPE(name)                                      \
  char test_case_ ## name (void)

#define TEST_CASE_RUN(name)                                            \
  test_case_ ## name()

#define TEST_EQ(test, expected)                                        \
  do {                                                                 \
    long long signed TEST_EQ_test = (long long signed) (test);         \
    long long signed TEST_EQ_expected = (long long signed) (expected); \
    if (TEST_EQ_test == TEST_EQ_expected) {                            \
      g_test_assert_count++;                                           \
      g_test_assert_ok++;                                              \
    }                                                                  \
    else {                                                             \
      test_ko();                                                       \
      fprintf(stderr, "\n%sAssertion failed in %s:%d %s\n"             \
              "%s == %s\n"                                             \
              "Expected %lld got %lld.%s\n",                           \
              TEST_COLOR_KO,                                           \
              __FILE__, __LINE__, __func__,                            \
              # expected, # test,                                      \
              TEST_EQ_expected, TEST_EQ_test,                          \
              TEST_COLOR_RESET);                                       \
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
      fprintf(stderr, "\n%sAssertion failed in %s:%d %s\n"             \
              "%s == %s\n"                                             \
              "Expected %s got %.8g.%s\n",                            \
              TEST_COLOR_KO,                                           \
              __FILE__, __LINE__, __func__,                            \
              # test, # expected, # expected, TEST_FLOAT_EQ_tmp,       \
              TEST_COLOR_RESET);                                       \
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
      fprintf(stderr, "\n%sAssertion failed in %s:%d %s\n"             \
              "%s == %s\n"                                             \
              "Expected %s got %.8g.%s\n",                            \
              TEST_COLOR_KO,                                           \
              __FILE__, __LINE__, __func__,                            \
              # test, # expected1, # expected1, TEST_FLOAT_EQ2_tmp,    \
              TEST_COLOR_RESET);                                       \
      return 1;                                                        \
    }                                                                  \
  } while (0)

#define TEST_DOUBLE_EQ(test, expected)                                 \
  do {                                                                 \
    double TEST_DOUBLE_EQ_tmp = (double) (test);                       \
    if (fabs(TEST_DOUBLE_EQ_tmp - (expected)) <= DBL_EPSILON) {        \
      g_test_assert_count++;                                           \
      g_test_assert_ok++;                                              \
    }                                                                  \
    else {                                                             \
      test_ko();                                                       \
      fprintf(stderr, "\n%sAssertion failed in %s:%d %s\n"             \
              "%s == %s\n"                                             \
              "Expected %s got %.16lg.%s\n",                              \
              TEST_COLOR_KO,                                           \
              __FILE__, __LINE__, __func__,                            \
              # test, # expected, # expected, TEST_DOUBLE_EQ_tmp,      \
              TEST_COLOR_RESET);                                       \
      return 1;                                                        \
    }                                                                  \
  } while (0)

#define TEST_MEM_EQ(p1, buf1_size, p2, buf2_size)                      \
  {                                                                    \
    size_t i = 0;                                                      \
    TEST_EQ(buf1_size, buf2_size);                                     \
    while (i < buf1_size) {                                            \
      TEST_EQ(((u8 *) p1)[i], ((u8 *) p2)[i]);                         \
      i++;                                                             \
    }                                                                  \
  }

#define TEST_STR_COMPARE(a, b, expected)                               \
  do {                                                                 \
    sw TEST_STR_COMPARE_tmp = str_compare(a, b);                       \
    if (TEST_STR_COMPARE_tmp == expected) {                            \
      g_test_assert_count++;                                           \
      g_test_assert_ok++;                                              \
    }                                                                  \
    else {                                                             \
      test_ko();                                                       \
      fprintf(stderr, "\n%sAssertion failed in %s:%d %s\n"             \
              "str_compare(%s, %s) == %s\n"                            \
              "Expected %s got %ld.%s\n",                              \
              TEST_COLOR_KO,                                           \
              __FILE__, __LINE__, __func__,                            \
              # a, # b, # expected, # expected, TEST_STR_COMPARE_tmp,  \
              TEST_COLOR_RESET);                                       \
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
      fprintf(stderr, "\n%sAssertion failed in %s:%d %s\n"             \
              "strncmp(%s, %s, %ld) == 0\n",                           \
              TEST_COLOR_KO,                                           \
              __FILE__, __LINE__, __func__,                            \
              # test, # result, (long) (bytes));                       \
      fprintf(stderr, "Expected %s got \"",                            \
              # result);                                               \
      fwrite(TEST_STRNCMP_tmp, (bytes), 1, stderr);                    \
      fprintf(stderr, "\".%s\n", TEST_COLOR_RESET);                    \
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

/* Init and clean. */
void test_clean (void);
void test_init (char *argv0, int *argc, char ***argv);

/* Operators. */
void test_context (const char *context);
int  test_file_compare (const char *path_a, const char *path_b);
void test_ko (void);
void test_ok (void);
void test_summary (void);
int  test_target (const char *target);

#endif /* TEST_H */

