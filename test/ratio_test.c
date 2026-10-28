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
#include "../libkc3/assert.h"
#include "compare_test.h"
#include "../libkc3/str.h"
#include "../libkc3/buf.h"
#include "../libkc3/buf_parse.h"
#include "../libkc3/integer.h"
#include "../libkc3/ratio.h"
#include "../libkc3/str.h"
#include "../libkc3/tag.h"
#include "test.h"

s_tag * tag_pow (s_tag *a, s_tag *b, s_tag *dest);

#define RATIO_TEST_CAST_TYPE(type)                                     \
  TEST_EQ(ratio_to_ ## type(&ratio), integer_to_ ## type(&quotient))

#define RATIO_TEST_CAST(test, expected)                                \
  do {                                                                 \
    s_ratio ratio;                                                     \
    s_integer quotient;                                                \
    test_context("ratio casts: " test " -> " expected);                \
    TEST_EQ(ratio_init_1(&ratio, test), &ratio);                         \
    TEST_EQ(integer_init_1(&quotient, expected), &quotient);               \
    RATIO_TEST_CAST_TYPE(s8);                                          \
    RATIO_TEST_CAST_TYPE(s16);                                         \
    RATIO_TEST_CAST_TYPE(s32);                                         \
    RATIO_TEST_CAST_TYPE(s64);                                         \
    RATIO_TEST_CAST_TYPE(sw);                                          \
    RATIO_TEST_CAST_TYPE(u8);                                          \
    RATIO_TEST_CAST_TYPE(u16);                                         \
    RATIO_TEST_CAST_TYPE(u32);                                         \
    RATIO_TEST_CAST_TYPE(u64);                                         \
    RATIO_TEST_CAST_TYPE(uw);                                          \
    integer_clean(&quotient);                                          \
    ratio_clean(&ratio);                                               \
    test_context(NULL);                                                \
  } while (0)

#define RATIO_TEST_INIT_1(test, expected)                              \
  do {                                                                 \
    s_ratio ratio;                                                     \
    s_str str;                                                         \
    test_context("ratio_init_1(" # test ") -> " # expected);           \
    TEST_EQ(ratio_init_1(&ratio, (test)), &ratio);                     \
    TEST_ASSERT(str_init_ratio(&str, &ratio));                         \
    TEST_STRNCMP(str.ptr.p_pchar, (expected), str.size);                 \
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

#define RATIO_TEST_POW(base, exponent, bits, expected)                  \
  do {                                                                 \
  s_ratio test_ratio;						       \
    s_ratio base_ratio;                                                \
    s_ratio exponent_ratio;                                            \
    s_ratio expected_ratio;                                            \
    test_context("ratio_pow(" # base ", " # exponent ") => "	       \
    # expected);						       \
    TEST_ASSERT(ratio_init_1(&base_ratio, base));                        \
    TEST_ASSERT(ratio_init_1(&exponent_ratio, exponent));                \
    TEST_ASSERT(ratio_init_1(&expected_ratio, expected));                \
    TEST_EQ(ratio_pow(&base_ratio, &exponent_ratio, bits, &test_ratio),   \
             &test_ratio);                                            \
    TEST_EQ(compare_integer(&test_ratio.numerator,                     \
                             &expected_ratio.numerator), 0);           \
    TEST_EQ(compare_integer(&test_ratio.denominator,                   \
                             &expected_ratio.denominator), 0);         \
    ratio_clean(&test_ratio);					       \
    ratio_clean(&base_ratio);				               \
    ratio_clean(&exponent_ratio);				       \
    ratio_clean(&expected_ratio);				       \
    test_context(NULL);                                                \
  } while (0)

#define RATIO_TEST_POW_ERROR(base, exponent, bits)                      \
  do {                                                                 \
    s_ratio a;                                                         \
    s_ratio b;                                                         \
    s_ratio result;                                                    \
    mp_digit *numerator;                                               \
    mp_digit *denominator;                                             \
    test_context("ratio_pow error: " base " ** " exponent);             \
    TEST_ASSERT(ratio_init_1(&a, base));                                \
    TEST_ASSERT(ratio_init_1(&b, exponent));                            \
    TEST_ASSERT(ratio_init_1(&result, "7/11"));                          \
    numerator = result.numerator.in_int.dp;                            \
    denominator = result.denominator.in_int.dp;                        \
    TEST_EQ(ratio_pow(&a, &b, bits, &result), NULL);                     \
    TEST_EQ(result.numerator.in_int.dp, numerator);                     \
    TEST_EQ(result.denominator.in_int.dp, denominator);                 \
    TEST_EQ(integer_to_s8(&result.numerator), 7);                        \
    TEST_EQ(integer_to_s8(&result.denominator), 11);                     \
    ratio_clean(&result);                                              \
    ratio_clean(&b);                                                   \
    ratio_clean(&a);                                                   \
    test_context(NULL);                                                \
  } while (0)

static char ratio_test_pow_approx (const char *base,
                                   const char *exponent, uw bits)
{
  s_ratio a;
  s_ratio b;
  mp_int bound;
  mp_int left;
  s32 p;
  u32 q;
  s_ratio result;
  mp_int right;
  mp_int scale;
  mp_int target_n;
  mp_int target_d;
  mp_int tmp;
  test_context("ratio_pow relative error");
  TEST_ASSERT(ratio_init_1(&a, base));
  TEST_ASSERT(ratio_init_1(&b, exponent));
  TEST_EQ(ratio_pow(&a, &b, bits, &result), &result);
  p = integer_to_s32(&b.numerator);
  q = integer_to_u32(&b.denominator);
  TEST_EQ(integer_is_negative(&result.numerator),
           integer_is_negative(&a.numerator) && (p % 2 != 0));
  TEST_ASSERT(integer_is_positive(&result.denominator));
  TEST_EQ(mp_init_multi(&bound, &left, &right, &scale, &target_n,
                        &target_d, &tmp, NULL), MP_OKAY);
  TEST_EQ(mp_abs(&a.numerator.in_int, &target_n), MP_OKAY);
  TEST_EQ(mp_copy(&a.denominator.in_int, &target_d), MP_OKAY);
  if (p < 0) {
    mp_exch(&target_n, &target_d);
    p = -p;
  }
  TEST_EQ(mp_expt_u32(&target_n, p, &target_n), MP_OKAY);
  TEST_EQ(mp_expt_u32(&target_d, p, &target_d), MP_OKAY);
  TEST_EQ(mp_2expt(&scale, bits), MP_OKAY);
  TEST_EQ(mp_abs(&result.numerator.in_int, &left), MP_OKAY);
  TEST_EQ(mp_mul(&left, &scale, &left), MP_OKAY);
  TEST_EQ(mp_expt_u32(&left, q, &left), MP_OKAY);
  TEST_EQ(mp_mul(&left, &target_d, &left), MP_OKAY);
  TEST_EQ(mp_add_d(&scale, 1, &bound), MP_OKAY);
  TEST_EQ(mp_mul(&result.denominator.in_int, &bound, &tmp), MP_OKAY);
  TEST_EQ(mp_expt_u32(&tmp, q, &tmp), MP_OKAY);
  TEST_EQ(mp_mul(&tmp, &target_n, &right), MP_OKAY);
  TEST_ASSERT(mp_cmp(&left, &right) != MP_GT);
  TEST_EQ(mp_sub_d(&scale, 1, &bound), MP_OKAY);
  TEST_EQ(mp_mul(&result.denominator.in_int, &bound, &tmp), MP_OKAY);
  TEST_EQ(mp_expt_u32(&tmp, q, &tmp), MP_OKAY);
  TEST_EQ(mp_mul(&tmp, &target_n, &right), MP_OKAY);
  TEST_ASSERT(mp_cmp(&left, &right) != MP_LT);
  mp_clear_multi(&bound, &left, &right, &scale, &target_n, &target_d,
                 &tmp, NULL);
  ratio_clean(&result);
  ratio_clean(&b);
  ratio_clean(&a);
  test_context(NULL);
  return 0;
}

TEST_CASE_PROTOTYPE(ratio_cast);
TEST_CASE_PROTOTYPE(ratio_init_1);
TEST_CASE_PROTOTYPE(ratio_parse);
TEST_CASE_PROTOTYPE(ratio_pow);

void ratio_test (void)
{
  TEST_CASE_RUN(ratio_cast);
  TEST_CASE_RUN(ratio_init_1);
  TEST_CASE_RUN(ratio_parse);
  TEST_CASE_RUN(ratio_pow);
}

TEST_CASE(ratio_cast)
{
  RATIO_TEST_CAST("257/2", "128");
  RATIO_TEST_CAST("-257/2", "-128");
  RATIO_TEST_CAST("1/256", "0");
  RATIO_TEST_CAST("-1/256", "0");
  RATIO_TEST_CAST("0/1", "0");
  RATIO_TEST_CAST("1/1", "1");
  RATIO_TEST_CAST("1/18446744073709551616", "0");
  RATIO_TEST_CAST("18446744073709551617/18446744073709551616", "1");
  RATIO_TEST_CAST("-18446744073709551617/18446744073709551616", "-1");
  RATIO_TEST_CAST("18446744073709551617/1", "18446744073709551617");
  RATIO_TEST_CAST("-18446744073709551617/1", "-18446744073709551617");
  RATIO_TEST_CAST("36893488147419103231/2", "18446744073709551615");
  RATIO_TEST_CAST("-36893488147419103231/2", "-18446744073709551615");
}
TEST_CASE_END(ratio_cast)

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
  s_tag a = {0};
  s_tag b = {0};
  uw i;
  uw j;
  s_tag result = {0};
  s_ratio wide_base;
  s_ratio wide_exponent;
  s_ratio wide_lower;
  s_ratio wide_result;
  s_ratio wide_upper;
  const uw bits[] = {1, 8, 64, 128};
  const char *approx[][2] = {
    {"2/1", "1/2"},
    {"3/1", "1/2"},
    {"2/3", "1/2"},
    {"2/1", "3/2"},
    {"2/1", "-1/2"},
    {"-2/1", "1/3"},
    {"-2/1", "-1/3"},
    {"18446744073709551617/18446744073709551616", "1/2"},
    {"18446744073709551616/18446744073709551617", "1/2"},
    {"200000000000000000000000000000000000000000000000000/1", "1/2"},
    {"1/200000000000000000000000000000000000000000000000000", "1/2"}
  };
  const struct {
    const char *base;
    const char *exponent;
    uw bits;
    const char *lower;
    const char *upper;
  } vectors[] = {
    {"2/1", "1/2", 128,
     "141421356237309504880/100000000000000000000",
     "141421356237309504881/100000000000000000000"},
    {"3/1", "1/2", 128,
     "173205080756887729352/100000000000000000000",
     "173205080756887729353/100000000000000000000"},
    {"2/1", "1/18446744073709551616", 64, "1/1",
     "18446744073709551617/18446744073709551616"},
    {"18446744073709551617/18446744073709551616",
     "18446744073709551616/3", 64, "1/1", "2/1"}
  };
  RATIO_TEST_POW("4/9", "1/2", 64, "2/3");
  RATIO_TEST_POW("16/81", "3/4", 32, "8/27");
  RATIO_TEST_POW("2/3", "4/2", 32, "4/9");
  RATIO_TEST_POW("4/2", "2/1", 32, "4/1");
  RATIO_TEST_POW("-8/27", "1/3", 32, "-2/3");
  RATIO_TEST_POW("-8/27", "2/3", 32, "4/9");
  RATIO_TEST_POW("-8/27", "-1/3", 32, "-3/2");
  RATIO_TEST_POW("4/9", "-1/2", 32, "3/2");
  RATIO_TEST_POW("0/1", "0/1", 32, "1/1");
  RATIO_TEST_POW("2/3", "0/2", 32, "1/1");
  RATIO_TEST_POW("0/1", "1/1", 64, "0/1");
  RATIO_TEST_POW("0/1", "1/2", 32, "0/1");
  RATIO_TEST_POW("1/1", "18446744073709551616/1", 32, "1/1");
  RATIO_TEST_POW("-1/1", "18446744073709551617/1", 32, "-1/1");
  RATIO_TEST_POW("1/1", "1/18446744073709551616", 32, "1/1");
  RATIO_TEST_POW("4/9", "18446744073709551616/36893488147419103232",
                  32, "2/3");
  RATIO_TEST_POW_ERROR("0/1", "-1/2", 32);
  RATIO_TEST_POW_ERROR("-2/1", "1/2", 32);
  RATIO_TEST_POW_ERROR("2/1", "1/2", 0);
  RATIO_TEST_POW_ERROR("2/1", "1/2", (uw) -1);
  for (i = 0; i < sizeof(approx) / sizeof(*approx); i++)
    for (j = 0; j < sizeof(bits) / sizeof(*bits); j++)
      TEST_EQ(ratio_test_pow_approx(approx[i][0], approx[i][1],
                                    bits[j]), 0);
  for (i = 0; i < sizeof(vectors) / sizeof(*vectors); i++) {
    test_context(vectors[i].base);
    TEST_ASSERT(ratio_init_1(&wide_base, vectors[i].base));
    TEST_ASSERT(ratio_init_1(&wide_exponent, vectors[i].exponent));
    TEST_ASSERT(ratio_init_1(&wide_lower, vectors[i].lower));
    TEST_ASSERT(ratio_init_1(&wide_upper, vectors[i].upper));
    TEST_EQ(ratio_pow(&wide_base, &wide_exponent, vectors[i].bits,
                      &wide_result),
             &wide_result);
    TEST_ASSERT(compare_ratio(&wide_lower, &wide_result) != 1);
    TEST_ASSERT(compare_ratio(&wide_result, &wide_upper) != 1);
    ratio_clean(&wide_upper);
    ratio_clean(&wide_result);
    ratio_clean(&wide_lower);
    ratio_clean(&wide_exponent);
    ratio_clean(&wide_base);
  }
  test_context("Ratio operator requires explicit precision");
  TEST_ASSERT(tag_init_1(&a, "4/9"));
  TEST_ASSERT(tag_init_1(&b, "1/2"));
  TEST_ASSERT(tag_init_s8(&result, 7));
  TEST_EQ(tag_pow(&a, &b, &result), NULL);
  TEST_EQ(result.type, TAG_S8);
  TEST_EQ(result.data.td_s8, 7);
  tag_clean(&b);
  TEST_ASSERT(tag_init_s8(&b, 2));
  TEST_EQ(tag_pow(&a, &b, &result), NULL);
  TEST_EQ(tag_pow(&b, &a, &result), NULL);
  TEST_EQ(result.type, TAG_S8);
  TEST_EQ(result.data.td_s8, 7);
  tag_clean(&a);
  TEST_ASSERT(tag_init_s8(&a, 2));
  TEST_ASSERT(tag_init_s8(&b, 3));
  TEST_EQ(tag_pow(&a, &b, &result), &result);
  TEST_EQ(result.type, TAG_U8);
  TEST_EQ(result.data.td_u8, 8);
  tag_clean(&result);
  tag_clean(&b);
  tag_clean(&a);
  test_context(NULL);
}
TEST_CASE_END(ratio_pow)
