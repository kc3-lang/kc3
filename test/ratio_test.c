/* kc3
 * Copyright from 2022 to 2026 kmx.io <contact@kmx.io>
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
#include "../libkc3/str.h"
#include "test.h"

#define RATIO_TEST_INIT_1(test, expected)                              \
  do {                                                                 \
    s_ratio ratio;                                                     \
    s_str str;                                                         \
    test_context("ratio_init_1(" # test ") -> " # expected);           \
    TEST_EQ(ratio_init_1(&ratio, (test)), &ratio);                     \
    TEST_ASSERT(str_init_ratio(&str, &ratio));                         \
    TEST_STRNCMP(str.ptr.pchar, (expected), str.size);                 \
    str_clean(&str);                                                   \
    ratio_clean(&ratio);                                               \
    test_context(NULL);                                                \
  } while (0)

#define RATIO_TEST_PARSE(test, expected_num, expected_den)             \
  do {                                                                 \
    sw result_num;                                                     \
    sw result_den;                                                     \
    s_buf buf;                                                         \
    s_ratio ratio;                                                     \
    test_context("buf_parse_ratio(" # test ") -> "                     \
                 # expected_num "/" # expected_den);                   \
    buf_init_1_const(&buf, (test));                                    \
    TEST_ASSERT(buf_parse_ratio(&buf, &ratio) > 0);                    \
    result_num = integer_to_sw(&ratio.numerator);                      \
    result_den = integer_to_sw(&ratio.denominator);                    \
    TEST_EQ(result_num, expected_num);                                 \
    TEST_EQ(result_den, expected_den);                                 \
    ratio_clean(&ratio);                                               \
    buf_clean(&buf);						       \
    test_context(NULL);                                                \
  } while (0)

#define RATIO_TEST_POW(base, exponent, expected)                       \
  do {                                                                 \
  s_ratio test_ratio;						       \
    s_ratio base_ratio;                                                \
    s_ratio exponent_ratio;                                            \
    s_ratio expected_ratio;                                            \
    test_context("ratio_pow(" # base ", " # exponent ") => "	       \
    # expected);						       \
    ratio_init_1(&base_ratio, base);				       \
    ratio_init_1(&exponent_ratio, exponent);			       \
    ratio_init_1(&expected_ratio, expected);			       \
    ratio_pow(&base_ratio, &exponent_ratio, &test_ratio);	       \
    ratio_clean(&test_ratio);					       \
    ratio_clean(&base_ratio);				               \
    ratio_clean(&exponent_ratio);				       \
    ratio_clean(&expected_ratio);				       \
    test_context(NULL);                                                \
  } while (0)

TEST_CASE_PROTOTYPE(ratio_init_1);
TEST_CASE_PROTOTYPE(ratio_parse);
TEST_CASE_PROTOTYPE(ratio_pow);

void ratio_test (void)
{
  TEST_CASE_RUN(ratio_init_1);
  TEST_CASE_RUN(ratio_parse);
  TEST_CASE_RUN(ratio_pow);
}

TEST_CASE(ratio_init_1)
{
  RATIO_TEST_INIT_1("0/1", "0/1");
  RATIO_TEST_INIT_1("1/1", "1/1");
  RATIO_TEST_INIT_1("1/10", "1/10");
  RATIO_TEST_INIT_1("1/100", "1/100");
  RATIO_TEST_INIT_1("1/1000", "1/1000");
  RATIO_TEST_INIT_1("10/1", "10/1");
  RATIO_TEST_INIT_1("100/1", "100/1");
  RATIO_TEST_INIT_1("1000/1", "1000/1");
}
TEST_CASE_END(ratio_init_1)

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

TEST_CASE(ratio_pow)
{
  RATIO_TEST_POW("0/1", "1/1", "0/1");
}
TEST_CASE_END(ratio_pow)
