/* kc3
 * Copyright 2022,2023,2024 kmx.io <contact@kmx.io>
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
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "compare_test.h"
#include "../libkc3/str.h"
#include "../libkc3/buf.h"
#include "../libkc3/buf_parse.h"
#include "../libkc3/integer.h"
#include "../libkc3/ratio.h"
#include "test.h"

#define RATIO_TEST_INSPECT(test_num, test_den, expected)               \
  do {                                                                 \
  s_ratio ratio;                                                       \
  s_str result;                                                        \
  integer_init_u64(&ratio.numerator, test_num);                        \
  integer_init_u64(&ratio.denominator, test_den);                      \
  test_context("ratio_inspect(" # test_num "/" # test_den              \
               ") -> " # expected);                                    \
  TEST_EQ(ratio_inspect(&ratio, &result), &result);                    \
  TEST_STRNCMP(result.ptr.p, expected, result.size);                   \
  ratio_clean(&ratio);                                                 \
  str_clean(&result);                                                  \
  test_context(NULL);                                                  \
} while (0)

#define RATIO_TEST_PARSE(test, expected_num, expected_den)             \
  do {                                                                 \
    sw result_num;                                                     \
    sw result_den;                                                     \
    s_buf buf;                                                         \
  s_ratio ratio;                                                       \
  s_str str;                                                           \
  str_init_1(&str, NULL, test);                                        \
  test_context("buf_parse_ratio(" # test ") -> "                       \
               # expected_num "/" # expected_den);                     \
  buf_init_str(&buf, false, &str);                                     \
  TEST_ASSERT(buf_parse_ratio(&buf, &ratio) > 0);                      \
  result_num = integer_to_sw(&ratio.numerator);                        \
  result_den = integer_to_sw(&ratio.denominator);                      \
  TEST_EQ(result_num, expected_num);                                   \
  TEST_EQ(result_den, expected_den);                                   \
  str_clean(&str);                                                     \
  ratio_clean(&ratio);                                                 \
  test_context(NULL);                                                  \
} while (0)

TEST_CASE_PROTOTYPE(ratio_inspect);
TEST_CASE_PROTOTYPE(ratio_parse);

void ratio_test (void)
{
  TEST_CASE_RUN(ratio_inspect);
  TEST_CASE_RUN(ratio_parse);
}

TEST_CASE(ratio_inspect)
{
    RATIO_TEST_INSPECT(0, 1, "0/1");
    RATIO_TEST_INSPECT(1, 1, "1/1");
    RATIO_TEST_INSPECT(1, 10, "1/10");
    RATIO_TEST_INSPECT(1, 100, "1/100");
    RATIO_TEST_INSPECT(1, 1000, "1/1000");
    RATIO_TEST_INSPECT(10, 1, "10/1");
    RATIO_TEST_INSPECT(100, 1, "100/1");
    RATIO_TEST_INSPECT(1000, 1, "1000/1");
}
TEST_CASE_END(ratio_inspect)

TEST_CASE(ratio_parse)
{
    RATIO_TEST_PARSE("0/1", 0, 1);
    RATIO_TEST_PARSE("1/1", 1, 1);
    RATIO_TEST_PARSE("1/10", 1, 10);
    RATIO_TEST_PARSE("1/100", 1, 100);
    RATIO_TEST_PARSE("1/1000", 1, 1000);
    RATIO_TEST_PARSE("10/1", 10, 1);
    RATIO_TEST_PARSE("100/1", 100, 1);
    RATIO_TEST_PARSE("1000/1", 1000, 1);
}
TEST_CASE_END(ratio_parse)
