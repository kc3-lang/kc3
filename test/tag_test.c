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
#include "../libkc3/assert.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "../libkc3/tag.h"
#include "../libkc3/integer.h"
#include "../libkc3/ratio.h"
#include "../libkc3/compare.h"
#include "test.h"

#define TAG_TEST_HASH_U64_COMPARE(a, b, expected)                      \
  do {                                                                 \
    u64 hash_a;                                                        \
    u64 hash_b;                                                        \
    TEST_EQ(tag_hash_u64(a, &hash_a), &hash_a);                        \
    TEST_EQ(tag_hash_u64(b, &hash_b), &hash_b);                        \
    TEST_EQ(compare_u64(hash_a, hash_b), (expected));                  \
  } while (0)

void tag_test (void);
TEST_CASE_PROTOTYPE(tag_hash_u64);
TEST_CASE_PROTOTYPE(tag_init_1);
TEST_CASE_PROTOTYPE(tag_shift);
TEST_CASE_PROTOTYPE(tag_divmod);
TEST_CASE_PROTOTYPE(tag_integer_divmod);
TEST_CASE_PROTOTYPE(tag_div_mixed);
// TEST_CASE_PROTOTYPE(tag_type__macro);

void tag_test (void)
{
  TEST_CASE_RUN(tag_init_1);
  TEST_CASE_RUN(tag_hash_u64);
  TEST_CASE_RUN(tag_shift);
  TEST_CASE_RUN(tag_divmod);
  TEST_CASE_RUN(tag_integer_divmod);
  TEST_CASE_RUN(tag_div_mixed);
  //  TEST_CASE_RUN(tag_type__macro);
}

TEST_CASE(tag_hash_u64)
{
  s_tag a = {0};
  s_tag b = {0};
  tag_init_1(&a, "{a, b}");
  TAG_TEST_HASH_U64_COMPARE(&a, &a, 0);
  TAG_TEST_HASH_U64_COMPARE(tag_1(&a, "{a, b}"), tag_init_1(&b, "{a, b}"), 0);
  TAG_TEST_HASH_U64_COMPARE(tag_1(&a, "{a, b}"), tag_1(&b, "{a, b}"), 0);
  TAG_TEST_HASH_U64_COMPARE(tag_1(&a, "{{a, b}, {c, d}}"),
                            tag_1(&b, "{{a, b}, {c, d}}"), 0);
  tag_clean(&a);
  tag_clean(&b);
}
TEST_CASE_END(tag_hash_u64)

TEST_CASE(tag_init_1)
{
  s_tag a = {0};
  TEST_ASSERT(tag_init_1(&a, "fn () { [] }"));
  tag_clean(&a);
  TEST_ASSERT(tag_init_1(&a, "fn (x) { x }"));
  tag_clean(&a);
  TEST_ASSERT(tag_init_1(&a, "fn (x, y) { x }"));
  tag_clean(&a);
}
TEST_CASE_END(tag_init_1)

/*
TEST_CASE(tag_type__macro)
{
  TEST_EQ(TAG_TYPE((u8) 0), TAG_U8);
  TEST_EQ(TAG_TYPE((u8) 1), TAG_U8);
  TEST_EQ(TAG_TYPE(256), TAG_U16);
  TEST_EQ(TAG_TYPE(65535), TAG_U16);
  TEST_EQ(TAG_TYPE(65536), TAG_U32);
  TEST_EQ(TAG_TYPE(4294967295), TAG_U32);
  TEST_EQ(TAG_TYPE(4294967296UL), TAG_U64);
  TEST_EQ(TAG_TYPE(18446744073709551615UL), TAG_U64);
  TEST_EQ(TAG_TYPE("a"), TAG_STR);
}
TEST_CASE_END(tag_type__macro)
*/

static s_tag * tag_test_init_integer (s_tag *tag, e_tag_type type, s64 value)
{
  switch (type) {
  case TAG_BOOL: return tag_init_bool(tag, (bool) value);
  case TAG_CHARACTER: return tag_init_character(tag, (character) value);
  case TAG_S8: return tag_init_s8(tag, (s8) value);
  case TAG_S16: return tag_init_s16(tag, (s16) value);
  case TAG_S32: return tag_init_s32(tag, (s32) value);
  case TAG_S64: return tag_init_s64(tag, value);
  case TAG_SW: return tag_init_sw(tag, (sw) value);
  case TAG_U8: return tag_init_u8(tag, (u8) value);
  case TAG_U16: return tag_init_u16(tag, (u16) value);
  case TAG_U32: return tag_init_u32(tag, (u32) value);
  case TAG_U64: return tag_init_u64(tag, (u64) value);
  case TAG_UW: return tag_init_uw(tag, (uw) value);
  case TAG_INTEGER:
    if (! tag_init_integer_zero(tag))
      return NULL;
    if (! integer_set_s64(&tag->data.td_integer, value)) {
      tag_clean(tag);
      return NULL;
    }
    return tag;
  default: return NULL;
  }
}

static char tag_shift_test_result (s_tag *a, s_tag *b, s_tag *expected,
                                  bool right)
{
  s_tag result = {0};
  result.ref_count = 17;
  TEST_EQ(right ? tag_shift_right(a, b, &result) :
           tag_shift_left(a, b, &result), &result);
  TEST_EQ(result.type, expected->type);
  TEST_EQ(compare_tag(&result, expected), 0);
  if (result.type == TAG_BOOL)
    TEST_EQ(result.data.td_bool_, expected->data.td_bool_);
  TEST_EQ(result.ref_count, 17);
  tag_clean(&result);
  return 0;
}

static char tag_shift_test_error (s_tag *a, s_tag *b, bool right)
{
  s_tag result = {0};
  s_tag saved;
  TEST_ASSERT(tag_init_u8(&result, 77));
  result.ref_count = 17;
  saved = result;
  TEST_EQ(right ? tag_shift_right(a, b, &result) :
           tag_shift_left(a, b, &result), NULL);
  TEST_MEMCMP((const char *) &result, (const char *) &saved,
               sizeof(result));
  return 0;
}

TEST_CASE(tag_shift)
{
  const e_tag_type types[] = {
    TAG_BOOL, TAG_CHARACTER, TAG_S8, TAG_S16, TAG_S32, TAG_S64, TAG_SW,
    TAG_U8, TAG_U16, TAG_U32, TAG_U64, TAG_UW, TAG_INTEGER
  };
  const uw widths[] = {8, 32, 8, 16, 32, 64, 8 * sizeof(sw),
                       8, 16, 32, 64, 8 * sizeof(uw), 0};
  const s64 minima[] = {S8_MIN, S16_MIN, S32_MIN, S64_MIN, SW_MIN};
  const s64 integer_floors[] = {0, -1, -1, -2};
  s_tag a = {0};
  s_tag b = {0};
  s_tag expected = {0};
  uw i;
  uw j;
  s_integer integer;
  mp_digit *integer_digits;
  s_integer integer_result;
  uw n;
  /* Every supported left/count type pair must agree for counts 0 and 1. */
  for (i = 0; i < sizeof(types) / sizeof(*types); i++) {
    TEST_ASSERT(tag_test_init_integer(&a, types[i], 1));
    for (j = 0; j < sizeof(types) / sizeof(*types); j++)
      for (n = 0; n <= 1; n++) {
        TEST_ASSERT(tag_test_init_integer(&b, types[j], n));
        TEST_ASSERT(tag_test_init_integer(&expected, types[i], n ? 2 : 1));
        TEST_EQ(tag_shift_test_result(&a, &b, &expected, false), 0);
        tag_clean(&expected);
        TEST_ASSERT(tag_test_init_integer(&expected, types[i], n ? 0 : 1));
        TEST_EQ(tag_shift_test_result(&a, &b, &expected, true), 0);
        tag_clean(&expected);
        tag_clean(&b);
      }
    if (widths[i]) {
      TEST_ASSERT(tag_test_init_integer(&expected, types[i], 0));
      for (n = widths[i]; n <= widths[i] + 1; n++) {
        TEST_ASSERT(tag_init_u64(&b, n));
        TEST_EQ(tag_shift_test_result(&a, &b, &expected, false), 0);
        TEST_EQ(tag_shift_test_result(&a, &b, &expected, true), 0);
      }
      tag_clean(&expected);
    }
    /* Negative counts must leave an existing destination untouched. */
    for (j = 2; j <= 6; j++) {
      TEST_ASSERT(tag_test_init_integer(&b, types[j], -1));
      TEST_EQ(tag_shift_test_error(&a, &b, false), 0);
      TEST_EQ(tag_shift_test_error(&a, &b, true), 0);
    }
    TEST_ASSERT(tag_init_integer_1(&b, "-18446744073709551616"));
    TEST_EQ(tag_shift_test_error(&a, &b, false), 0);
    TEST_EQ(tag_shift_test_error(&a, &b, true), 0);
    tag_clean(&b);
    /* Counts must be classified before any conversion to a signed word. */
    for (j = 0; j < 3; j++) {
      if (! j)
        TEST_ASSERT(tag_init_u64(&b, U64_MAX));
      else
        TEST_ASSERT(tag_init_integer_1(&b, j == 1 ?
                     "18446744073709551616" : "18446744073709551617"));
      TEST_ASSERT(tag_test_init_integer(&expected, types[i], 0));
      TEST_EQ(tag_shift_test_result(&a, &b, &expected, true), 0);
      if (types[i] == TAG_INTEGER)
        TEST_EQ(tag_shift_test_error(&a, &b, false), 0);
      else
        TEST_EQ(tag_shift_test_result(&a, &b, &expected, false), 0);
      tag_clean(&expected);
      tag_clean(&b);
    }
    tag_clean(&a);
  }
  /* Signed minima, sign extension, wrapping, and the top nominal bit. */
  for (i = 2; i <= 6; i++) {
    TEST_ASSERT(tag_test_init_integer(&a, types[i], minima[i - 2]));
    TEST_ASSERT(tag_init_u8(&b, 1));
    TEST_ASSERT(tag_test_init_integer(&expected, types[i], 0));
    TEST_EQ(tag_shift_test_result(&a, &b, &expected, false), 0);
    TEST_ASSERT(tag_test_init_integer(&expected, types[i], minima[i - 2] / 2));
    TEST_EQ(tag_shift_test_result(&a, &b, &expected, true), 0);
    TEST_ASSERT(tag_test_init_integer(&expected, types[i], -1));
    for (n = widths[i] - 1; n <= widths[i] + 1; n++) {
      TEST_ASSERT(tag_init_u64(&b, n));
      TEST_EQ(tag_shift_test_result(&a, &b, &expected, true), 0);
    }
    TEST_ASSERT(tag_test_init_integer(&a, types[i], 1));
    TEST_ASSERT(tag_init_u8(&b, widths[i] - 1));
    TEST_ASSERT(tag_test_init_integer(&expected, types[i], minima[i - 2]));
    TEST_EQ(tag_shift_test_result(&a, &b, &expected, false), 0);
    TEST_ASSERT(tag_test_init_integer(&a, types[i], -3));
    TEST_ASSERT(tag_init_u8(&b, 1));
    TEST_ASSERT(tag_test_init_integer(&expected, types[i], -6));
    TEST_EQ(tag_shift_test_result(&a, &b, &expected, false), 0);
    TEST_ASSERT(tag_test_init_integer(&expected, types[i], -2));
    TEST_EQ(tag_shift_test_result(&a, &b, &expected, true), 0);
  }
  TEST_ASSERT(tag_init_u64(&a, U64_MAX));
  TEST_ASSERT(tag_init_u64(&expected, U64_MAX - 1));
  TEST_EQ(tag_shift_test_result(&a, &b, &expected, false), 0);
  TEST_ASSERT(tag_init_u64(&expected, U64_MAX / 2));
  TEST_EQ(tag_shift_test_result(&a, &b, &expected, true), 0);
  TEST_ASSERT(tag_init_bool(&a, 255));
  TEST_ASSERT(tag_init_bool(&b, 255));
  TEST_ASSERT(tag_init_bool(&expected, 2));
  TEST_EQ(tag_shift_test_result(&a, &b, &expected, false), 0);
  /* Exact Integer shifts and flooring for negative odd values. */
  TEST_ASSERT(tag_init_integer_1(&a, "3"));
  TEST_ASSERT(tag_init_u8(&b, 64));
  TEST_ASSERT(tag_init_integer_1(&expected, "55340232221128654848"));
  TEST_EQ(tag_shift_test_result(&a, &b, &expected, false), 0);
  TEST_EQ(tag_shift_test_result(&expected, &b, &a, true), 0);
  tag_clean(&expected);
  tag_clean(&a);
  for (i = 0; i < 4; i++) {
    TEST_ASSERT(tag_test_init_integer(&a, TAG_INTEGER, -(s64) i));
    TEST_ASSERT(tag_init_u8(&b, 1));
    TEST_ASSERT(tag_test_init_integer(&expected, TAG_INTEGER,
                                    integer_floors[i]));
    TEST_EQ(tag_shift_test_result(&a, &b, &expected, true), 0);
    tag_clean(&expected);
    TEST_ASSERT(tag_init_u64(&b, U64_MAX));
    TEST_ASSERT(tag_test_init_integer(&expected, TAG_INTEGER, i ? -1 : 0));
    TEST_EQ(tag_shift_test_result(&a, &b, &expected, true), 0);
    tag_clean(&expected);
    if (! i) {
      TEST_ASSERT(tag_init_integer_zero(&expected));
      TEST_EQ(tag_shift_test_result(&a, &b, &expected, false), 0);
      tag_clean(&expected);
    }
    tag_clean(&a);
  }
  TEST_ASSERT(tag_init_u8(&a, 1));
  TEST_ASSERT(tag_init_f64(&b, 1.0));
  TEST_EQ(tag_shift_test_error(&a, &b, false), 0);
  TEST_EQ(tag_shift_test_error(&a, &b, true), 0);
  a = (s_tag) {0};
  TEST_ASSERT(tag_init_u8(&b, 1));
  TEST_EQ(tag_shift_test_error(&a, &b, false), 0);
  TEST_EQ(tag_shift_test_error(&a, &b, true), 0);
  /* Direct helper errors must also preserve initialized destinations. */
  TEST_ASSERT(integer_init_s8(&integer, 3));
  TEST_ASSERT(integer_init_s8(&integer_result, 77));
  integer_digits = integer_result.in_int.dp;
  TEST_EQ(integer_lshift(&integer, -1, &integer_result), NULL);
  TEST_EQ(integer_rshift(&integer, -1, &integer_result), NULL);
  TEST_EQ(integer_lshift(&integer, SW_MAX, &integer_result), NULL);
  TEST_EQ(integer_result.in_int.dp, integer_digits);
  TEST_EQ(integer_to_s8(&integer_result), 77);
  integer_clean(&integer_result);
  integer_clean(&integer);
}
TEST_CASE_END(tag_shift)

static char tag_divmod_test_result (s_tag *a, s_tag *b,
                                   const char *value, bool mod)
{
  s_tag expected = {0};
  s_tag result = {0};
  TEST_ASSERT(tag_init_1(&expected, value));
  result.ref_count = 17;
  TEST_EQ(mod ? tag_mod(a, b, &result) : tag_div(a, b, &result), &result);
  TEST_EQ(result.type, expected.type);
  TEST_EQ(compare_tag(&result, &expected), 0);
  TEST_EQ(result.ref_count, 17);
  tag_clean(&result);
  tag_clean(&expected);
  return 0;
}

TEST_CASE(tag_divmod)
{
  const e_tag_type types[] = {
    TAG_S8, TAG_S16, TAG_S32, TAG_S64, TAG_SW,
    TAG_U8, TAG_U16, TAG_U32, TAG_U64, TAG_UW
  };
  const s64 minima[] = {S8_MIN, S16_MIN, S32_MIN, S64_MIN, SW_MIN};
  const char *quotients[] = {
    "128", "32768", "2147483648", "9223372036854775808",
    sizeof(sw) == 8 ? "9223372036854775808" : "2147483648"
  };
  s_tag a = {0};
  s_tag b = {0};
  uw i;
  uw j;
  uw mod;
  s_tag result = {0};
  s_tag saved;
  test_context("F58: signed minima / -1 widen; minima mod -1 is zero");
  for (i = 0; i < sizeof(minima) / sizeof(*minima); i++) {
    TEST_ASSERT(tag_test_init_integer(&a, types[i], minima[i]));
    for (j = 0; j < sizeof(minima) / sizeof(*minima); j++) {
      TEST_ASSERT(tag_test_init_integer(&b, types[j], -1));
      TEST_EQ(tag_divmod_test_result(&a, &b, quotients[i], false), 0);
      TEST_EQ(tag_divmod_test_result(&a, &b, "0", true), 0);
    }
  }
  test_context("F58: every fixed-width type pair rejects zero");
  for (i = 0; i < sizeof(types) / sizeof(*types); i++) {
    TEST_ASSERT(tag_test_init_integer(&a, types[i], 1));
    for (j = 0; j < sizeof(types) / sizeof(*types); j++) {
      TEST_ASSERT(tag_test_init_integer(&b, types[j], 0));
      for (mod = 0; mod < 2; mod++) {
        TEST_ASSERT(tag_init_u8(&result, 77));
        result.ref_count = 17;
        saved = result;
        TEST_EQ(mod ? tag_mod(&a, &b, &result) :
                 tag_div(&a, &b, &result), NULL);
        TEST_MEMCMP((const char *) &result, (const char *) &saved,
                     sizeof(result));
        tag_clean(&result);
      }
    }
  }
  test_context("ordinary signed division still truncates toward zero");
  for (i = 0; i < sizeof(minima) / sizeof(*minima); i++) {
    TEST_ASSERT(tag_test_init_integer(&a, types[i], -7));
    for (j = 0; j < sizeof(minima) / sizeof(*minima); j++) {
      TEST_ASSERT(tag_test_init_integer(&b, types[j], 2));
      TEST_EQ(tag_divmod_test_result(&a, &b, "-3", false), 0);
      TEST_EQ(tag_divmod_test_result(&a, &b, "-1", true), 0);
      TEST_ASSERT(tag_test_init_integer(&b, types[j], -1));
      TEST_EQ(tag_divmod_test_result(&a, &b, "7", false), 0);
      TEST_EQ(tag_divmod_test_result(&a, &b, "0", true), 0);
    }
  }
  test_context("word-size conversions preserve nonzero divisors and values");
  TEST_ASSERT(tag_init_sw(&a, SW_MIN));
  TEST_ASSERT(tag_init_u32(&b, U32_MAX));
  TEST_EQ(tag_divmod_test_result(&a, &b,
                                sizeof(sw) == 8 ? "-2147483648" : "0",
                                false), 0);
  TEST_ASSERT(tag_init_u32(&a, 0x80000000));
  TEST_ASSERT(tag_init_sw(&b, -1));
  TEST_EQ(tag_divmod_test_result(&a, &b, "-2147483648", false), 0);
  TEST_ASSERT(tag_init_uw(&a, 1));
  TEST_ASSERT(tag_init_u64(&b, (u64) 1 << 32));
  TEST_EQ(tag_divmod_test_result(&a, &b, "0", false), 0);
  TEST_ASSERT(tag_init_u64(&a, U64_MAX));
  TEST_ASSERT(tag_init_uw(&b, 1));
  TEST_EQ(tag_divmod_test_result(&a, &b, "18446744073709551615", false), 0);
  test_context("floating-point zero divisors keep IEEE behavior");
  TEST_ASSERT(tag_init_f64(&a, 1.0));
  TEST_ASSERT(tag_init_u8(&b, 0));
  TEST_EQ(tag_div(&a, &b, &result), &result);
  TEST_EQ(result.type, TAG_F64);
  TEST_ASSERT(isinf(result.data.td_f64));
  tag_clean(&result);
  TEST_EQ(tag_mod(&a, &b, &result), &result);
  TEST_EQ(result.type, TAG_F64);
  TEST_ASSERT(isnan(result.data.td_f64));
  tag_clean(&result);
  TEST_ASSERT(tag_init_u8(&a, 1));
  TEST_ASSERT(tag_init_f64(&b, 0.0));
  TEST_EQ(tag_div(&a, &b, &result), &result);
  TEST_EQ(result.type, TAG_F64);
  TEST_ASSERT(isinf(result.data.td_f64));
  tag_clean(&result);
  TEST_EQ(tag_mod(&a, &b, &result), &result);
  TEST_EQ(result.type, TAG_F64);
  TEST_ASSERT(isnan(result.data.td_f64));
  tag_clean(&result);
  tag_clean(&a);
  tag_clean(&b);
  test_context(NULL);
}
TEST_CASE_END(tag_divmod)

static struct {
  uw fail;
  uw step;
  uw live;
} g_tag_integer_test;

/* Execute the dispatchers with failures at their real helper boundaries. */
#define TAG_INTEGER_TEST_INIT(name, type)                              \
  static s_integer * tag_integer_test_init_ ## name (s_integer *i,     \
                                                       type value)    \
  {                                                                   \
    if (++g_tag_integer_test.step == g_tag_integer_test.fail)          \
      return NULL;                                                    \
    if (! integer_init_ ## name(i, value))                            \
      return NULL;                                                    \
    g_tag_integer_test.live++;                                         \
    return i;                                                         \
  }

TAG_INTEGER_TEST_INIT(s8, s8)
TAG_INTEGER_TEST_INIT(s16, s16)
TAG_INTEGER_TEST_INIT(s32, s32)
TAG_INTEGER_TEST_INIT(s64, s64)
TAG_INTEGER_TEST_INIT(sw, sw)
TAG_INTEGER_TEST_INIT(u8, u8)
TAG_INTEGER_TEST_INIT(u16, u16)
TAG_INTEGER_TEST_INIT(u32, u32)
TAG_INTEGER_TEST_INIT(u64, u64)
TAG_INTEGER_TEST_INIT(uw, uw)

#undef TAG_INTEGER_TEST_INIT

static s_integer * tag_integer_test_div_integer
(const s_integer *a, const s_integer *b, s_integer *dest)
{
  if (++g_tag_integer_test.step == g_tag_integer_test.fail)
    return NULL;
  if (! integer_div(a, b, dest))
    return NULL;
  g_tag_integer_test.live++;
  return dest;
}

static s_integer * tag_integer_test_mod_integer
(const s_integer *a, const s_integer *b, s_integer *dest)
{
  if (++g_tag_integer_test.step == g_tag_integer_test.fail)
    return NULL;
  if (! integer_mod(a, b, dest))
    return NULL;
  g_tag_integer_test.live++;
  return dest;
}

static void tag_integer_test_clean (s_integer *i)
{
  if (i->in_int.dp)
    g_tag_integer_test.live--;
  integer_clean(i);
}

static s_ratio * tag_integer_test_ratio_init_s8 (s_ratio *r, s8 value)
{
  if (++g_tag_integer_test.step == g_tag_integer_test.fail)
    return NULL;
  if (! ratio_init_s8(r, value))
    return NULL;
  g_tag_integer_test.live += 2;
  return r;
}

static s_ratio * tag_integer_test_ratio_div (const s_ratio *a,
                                            const s_ratio *b,
                                            s_ratio *dest)
{
  if (++g_tag_integer_test.step == g_tag_integer_test.fail)
    return NULL;
  if (! ratio_div(a, b, dest))
    return NULL;
  g_tag_integer_test.live += 2;
  return dest;
}

static void tag_integer_test_ratio_clean (s_ratio *r)
{
  if (r->numerator.in_int.dp)
    g_tag_integer_test.live--;
  if (r->denominator.in_int.dp)
    g_tag_integer_test.live--;
  ratio_clean(r);
}

static void tag_integer_test_tag_clean (s_tag *tag)
{
  if (tag->type == TAG_INTEGER && tag->data.td_integer.in_int.dp)
    g_tag_integer_test.live--;
  if (tag->type == TAG_RATIO) {
    if (tag->data.td_ratio.numerator.in_int.dp)
      g_tag_integer_test.live--;
    if (tag->data.td_ratio.denominator.in_int.dp)
      g_tag_integer_test.live--;
  }
  tag_clean(tag);
}

static s_tag * tag_integer_test_reduce (s_tag *tag, s_tag *dest)
{
  if (++g_tag_integer_test.step == g_tag_integer_test.fail)
    return NULL;
  return tag_integer_reduce(tag, dest);
}

#define tag_div tag_integer_test_div
#define tag_mod tag_integer_test_mod
#define integer_div tag_integer_test_div_integer
#define integer_mod tag_integer_test_mod_integer
#define integer_clean tag_integer_test_clean
#define tag_clean tag_integer_test_tag_clean
#define tag_integer_reduce tag_integer_test_reduce
#define integer_init_s8 tag_integer_test_init_s8
#define integer_init_s16 tag_integer_test_init_s16
#define integer_init_s32 tag_integer_test_init_s32
#define integer_init_s64 tag_integer_test_init_s64
#define integer_init_sw tag_integer_test_init_sw
#define integer_init_u8 tag_integer_test_init_u8
#define integer_init_u16 tag_integer_test_init_u16
#define integer_init_u32 tag_integer_test_init_u32
#define integer_init_u64 tag_integer_test_init_u64
#define integer_init_uw tag_integer_test_init_uw
#define ratio_init_s8 tag_integer_test_ratio_init_s8
#define ratio_div tag_integer_test_ratio_div
#define ratio_clean tag_integer_test_ratio_clean
#include "../libkc3/tag_div.c"
#include "../libkc3/tag_mod.c"
#undef tag_div
#undef tag_mod
#undef integer_div
#undef integer_mod
#undef integer_clean
#undef tag_clean
#undef tag_integer_reduce
#undef integer_init_s8
#undef integer_init_s16
#undef integer_init_s32
#undef integer_init_s64
#undef integer_init_sw
#undef integer_init_u8
#undef integer_init_u16
#undef integer_init_u32
#undef integer_init_u64
#undef integer_init_uw
#undef ratio_init_s8
#undef ratio_div
#undef ratio_clean

TEST_CASE(tag_integer_divmod)
{
  const e_tag_type types[] = {
    TAG_INTEGER, TAG_S8, TAG_S16, TAG_S32, TAG_S64, TAG_SW,
    TAG_U8, TAG_U16, TAG_U32, TAG_U64, TAG_UW
  };
  const struct {
    e_tag_type a_type;
    e_tag_type b_type;
    s64 a_value;
  } paths[] = {
    {TAG_INTEGER, TAG_INTEGER, 97}, {TAG_INTEGER, TAG_SW, 97},
    {TAG_S8, TAG_INTEGER, 97}, {TAG_SW, TAG_INTEGER, 97},
    {TAG_S8, TAG_U64, 97}, {TAG_U64, TAG_S8, 97},
    {TAG_UW, TAG_SW, -1}
  };
  uw fail;
  uw steps;
  s_tag a = {0};
  s_tag b = {0};
  uw i;
  uw j;
  uw mod;
  s_tag result = {0};
  s_tag saved;
  s_integer x = {0};
  s_integer y = {0};
  s_integer integer_result = {0};
  s_integer integer_saved;
  test_context("F59: zero divisors fail for every Integer/fixed-width pair");
  for (i = 0; i < sizeof(types) / sizeof(*types); i++) {
    TEST_ASSERT(tag_test_init_integer(&a, types[i], 1));
    for (j = 0; j < sizeof(types) / sizeof(*types); j++) {
      if (i && j)
        continue;
      TEST_ASSERT(tag_test_init_integer(&b, types[j], 0));
      for (mod = 0; mod < 2; mod++) {
        TEST_ASSERT(tag_init_u8(&result, 77));
        result.ref_count = 17;
        saved = result;
        TEST_EQ(mod ? tag_mod(&a, &b, &result) :
                 tag_div(&a, &b, &result), NULL);
        TEST_MEMCMP((const char *) &result, (const char *) &saved,
                     sizeof(result));
        tag_clean(&result);
      }
      tag_clean(&b);
    }
    tag_clean(&a);
  }
  test_context("direct Integer helpers preserve the destination on failure");
  TEST_ASSERT(integer_init_u8(&x, 1));
  TEST_ASSERT(integer_init_u8(&y, 0));
  for (mod = 0; mod < 2; mod++) {
    TEST_ASSERT(integer_init_u8(&integer_result, 77));
    integer_saved = integer_result;
    TEST_EQ(mod ? integer_mod(&x, &y, &integer_result) :
             integer_div(&x, &y, &integer_result), NULL);
    TEST_MEMCMP((const char *) &integer_result,
                 (const char *) &integer_saved, sizeof(integer_result));
    TEST_EQ(integer_to_u8(&integer_result), 77);
    integer_clean(&integer_result);
  }
  integer_clean(&x);
  integer_clean(&y);
  test_context("valid Integer conversions and arithmetic still succeed");
  for (i = 0; i < sizeof(types) / sizeof(*types); i++) {
    TEST_ASSERT(tag_test_init_integer(&a, types[i], 97));
    for (j = 0; j < sizeof(types) / sizeof(*types); j++) {
      if (i && j)
        continue;
      TEST_ASSERT(tag_test_init_integer(&b, types[j], 5));
      TEST_EQ(tag_divmod_test_result(&a, &b, "19", false), 0);
      TEST_EQ(tag_divmod_test_result(&a, &b, "2", true), 0);
      tag_clean(&b);
    }
    tag_clean(&a);
  }
  TEST_ASSERT(tag_init_integer_1(&a, "-7"));
  TEST_ASSERT(tag_init_integer_1(&b, "3"));
  TEST_EQ(tag_divmod_test_result(&a, &b, "-2", false), 0);
  TEST_EQ(tag_divmod_test_result(&a, &b, "2", true), 0);
  tag_clean(&b);
  TEST_ASSERT(tag_init_integer_1(&b, "-3"));
  TEST_EQ(tag_divmod_test_result(&a, &b, "2", false), 0);
  TEST_EQ(tag_divmod_test_result(&a, &b, "-1", true), 0);
  tag_clean(&a);
  tag_clean(&b);
  test_context("large Integer results remain exact after reduction");
  TEST_ASSERT(tag_init_integer_1(&a,
                   "340282366920938463463374607431768211457"));
  TEST_ASSERT(tag_init_integer_1(&b, "2"));
  TEST_EQ(tag_divmod_test_result(&a, &b,
                   "170141183460469231731687303715884105728", false), 0);
  TEST_EQ(tag_divmod_test_result(&a, &b, "1", true), 0);
  tag_clean(&a);
  tag_clean(&b);
  test_context("F59: helper failures clean every initialized temporary");
  for (i = 0; i < sizeof(paths) / sizeof(*paths); i++) {
    TEST_ASSERT(tag_test_init_integer(&a, paths[i].a_type,
                                     paths[i].a_value));
    TEST_ASSERT(tag_test_init_integer(&b, paths[i].b_type, 5));
    for (mod = 0; mod < 2; mod++) {
      g_tag_integer_test.fail = 0;
      g_tag_integer_test.step = 0;
      g_tag_integer_test.live = 0;
      result = (s_tag) {0};
      TEST_EQ(mod ? tag_integer_test_mod(&a, &b, &result) :
               tag_integer_test_div(&a, &b, &result), &result);
      steps = g_tag_integer_test.step;
      TEST_ASSERT(steps >= 2);
      TEST_EQ(g_tag_integer_test.live, 0);
      tag_clean(&result);
      for (fail = 1; fail <= steps; fail++) {
        g_tag_integer_test.fail = fail;
        g_tag_integer_test.step = 0;
        g_tag_integer_test.live = 0;
        TEST_ASSERT(tag_init_u8(&result, 77));
        result.ref_count = 17;
        saved = result;
        TEST_EQ(mod ? tag_integer_test_mod(&a, &b, &result) :
                 tag_integer_test_div(&a, &b, &result), NULL);
        TEST_EQ(g_tag_integer_test.step, fail);
        TEST_EQ(g_tag_integer_test.live, 0);
        TEST_MEMCMP((const char *) &result, (const char *) &saved,
                     sizeof(result));
        tag_clean(&result);
      }
    }
    tag_clean(&a);
    tag_clean(&b);
  }
  test_context(NULL);
}
TEST_CASE_END(tag_integer_divmod)

TEST_CASE(tag_div_mixed)
{
  const e_tag_type types[] = {TAG_S16, TAG_S32, TAG_S64};
  const struct {
    uw a;
    s64 b;
    const char *expected;
  } word[] = {
    {6, 2, "3"}, {0, 2, "0"}, {1, 2, "0"}, {6, -2, "-3"},
    {7, 2, "3"}, {7, -2, "-3"},
    {UW_MAX >> 1, 1,
     sizeof(uw) == 8 ? "9223372036854775807" : "2147483647"},
    {UW_MAX, 1,
     sizeof(uw) == 8 ? "18446744073709551615" : "4294967295"},
    {UW_MAX, -1,
     sizeof(uw) == 8 ? "-18446744073709551615" : "-4294967295"},
    {UW_MAX, 2,
     sizeof(uw) == 8 ? "9223372036854775807" : "2147483647"},
    {UW_MAX, -2,
     sizeof(uw) == 8 ? "-9223372036854775807" : "-2147483647"}
  };
  const struct {
    s8 a;
    const char *b;
    const char *expected;
  } rational[] = {
    {1, "1/2", "2/1"}, {0, "1/2", "0/1"}, {-1, "1/2", "-2/1"},
    {1, "-1/2", "-2/1"}, {1, "2/3", "3/2"},
    {-128, "3/2", "-256/3"}, {127, "-2/3", "-381/2"}
  };
  s_tag a = {0};
  s_tag b = {0};
  s_tag expected = {0};
  uw fail;
  uw i;
  uw j;
  s_tag result = {0};
  s_tag saved;
  test_context("F60: Uw/signed division reduces the computed result");
  for (i = 0; i < sizeof(types) / sizeof(*types); i++)
    for (j = 0; j < sizeof(word) / sizeof(*word); j++) {
      TEST_ASSERT(tag_init_uw(&a, word[j].a));
      TEST_ASSERT(tag_test_init_integer(&b, types[i], word[j].b));
      TEST_EQ(tag_divmod_test_result(&a, &b, word[j].expected, false), 0);
      tag_clean(&a);
      tag_clean(&b);
    }
  test_context("F60: S8/Ratio returns an exact Ratio");
  for (i = 0; i < sizeof(rational) / sizeof(*rational); i++) {
    TEST_ASSERT(tag_init_s8(&a, rational[i].a));
    TEST_ASSERT(tag_init_ratio_1(&b, rational[i].b));
    TEST_EQ(tag_divmod_test_result(&a, &b, rational[i].expected, false), 0);
    tag_clean(&a);
    tag_clean(&b);
  }
  test_context("Ratio result owns its data after input cleanup");
  TEST_ASSERT(tag_init_s8(&a, 1));
  TEST_ASSERT(tag_init_ratio_1(&b, "1/2"));
  TEST_EQ(tag_div(&a, &b, &result), &result);
  tag_clean(&a);
  tag_clean(&b);
  TEST_ASSERT(tag_init_ratio_1(&expected, "2/1"));
  TEST_EQ(result.type, TAG_RATIO);
  TEST_EQ(compare_tag(&result, &expected), 0);
  tag_clean(&result);
  tag_clean(&expected);
  test_context("S8/Ratio rejects a zero numerator in the divisor");
  TEST_ASSERT(tag_init_s8(&a, 1));
  TEST_ASSERT(tag_init_ratio_zero(&b));
  TEST_ASSERT(tag_init_u8(&result, 77));
  result.ref_count = 17;
  saved = result;
  TEST_EQ(tag_div(&a, &b, &result), NULL);
  TEST_MEMCMP((const char *) &result, (const char *) &saved,
               sizeof(result));
  tag_clean(&result);
  tag_clean(&a);
  tag_clean(&b);
  test_context("F60: Uw reducer failures leave the destination unchanged");
  TEST_ASSERT(tag_init_uw(&a, 6));
  for (i = 0; i < sizeof(types) / sizeof(*types); i++) {
    TEST_ASSERT(tag_test_init_integer(&b, types[i], 2));
    g_tag_integer_test.fail = 0;
    g_tag_integer_test.step = 0;
    g_tag_integer_test.live = 0;
    result = (s_tag) {0};
    TEST_EQ(tag_integer_test_div(&a, &b, &result), &result);
    TEST_EQ(g_tag_integer_test.step, 1);
    TEST_EQ(g_tag_integer_test.live, 0);
    tag_clean(&result);
    g_tag_integer_test.fail = 1;
    g_tag_integer_test.step = 0;
    TEST_ASSERT(tag_init_u8(&result, 77));
    result.ref_count = 17;
    saved = result;
    TEST_EQ(tag_integer_test_div(&a, &b, &result), NULL);
    TEST_EQ(g_tag_integer_test.step, 1);
    TEST_EQ(g_tag_integer_test.live, 0);
    TEST_MEMCMP((const char *) &result, (const char *) &saved,
                 sizeof(result));
    tag_clean(&result);
    tag_clean(&b);
  }
  tag_clean(&a);
  test_context("F60: Ratio conversion and division failures clean up");
  TEST_ASSERT(tag_init_s8(&a, 1));
  TEST_ASSERT(tag_init_ratio_1(&b, "1/2"));
  g_tag_integer_test.fail = 0;
  g_tag_integer_test.step = 0;
  g_tag_integer_test.live = 0;
  result = (s_tag) {0};
  TEST_EQ(tag_integer_test_div(&a, &b, &result), &result);
  TEST_EQ(g_tag_integer_test.step, 2);
  TEST_EQ(g_tag_integer_test.live, 2);
  TEST_EQ(result.type, TAG_RATIO);
  tag_integer_test_tag_clean(&result);
  TEST_EQ(g_tag_integer_test.live, 0);
  for (fail = 1; fail <= 2; fail++) {
    g_tag_integer_test.fail = fail;
    g_tag_integer_test.step = 0;
    TEST_ASSERT(tag_init_u8(&result, 77));
    result.ref_count = 17;
    saved = result;
    TEST_EQ(tag_integer_test_div(&a, &b, &result), NULL);
    TEST_EQ(g_tag_integer_test.step, fail);
    TEST_EQ(g_tag_integer_test.live, 0);
    TEST_MEMCMP((const char *) &result, (const char *) &saved,
                 sizeof(result));
    tag_clean(&result);
  }
  tag_clean(&a);
  tag_clean(&b);
  test_context(NULL);
}
TEST_CASE_END(tag_div_mixed)
