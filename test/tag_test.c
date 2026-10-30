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
#include <stdlib.h>
#include <string.h>
#include "../libkc3/tag.h"
#include "../libkc3/integer.h"
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
// TEST_CASE_PROTOTYPE(tag_type__macro);

void tag_test (void)
{
  TEST_CASE_RUN(tag_init_1);
  TEST_CASE_RUN(tag_hash_u64);
  TEST_CASE_RUN(tag_shift);
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

static s_tag * tag_shift_test_init (s_tag *tag, e_tag_type type, s64 value)
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
    TEST_ASSERT(tag_shift_test_init(&a, types[i], 1));
    for (j = 0; j < sizeof(types) / sizeof(*types); j++)
      for (n = 0; n <= 1; n++) {
        TEST_ASSERT(tag_shift_test_init(&b, types[j], n));
        TEST_ASSERT(tag_shift_test_init(&expected, types[i], n ? 2 : 1));
        TEST_EQ(tag_shift_test_result(&a, &b, &expected, false), 0);
        tag_clean(&expected);
        TEST_ASSERT(tag_shift_test_init(&expected, types[i], n ? 0 : 1));
        TEST_EQ(tag_shift_test_result(&a, &b, &expected, true), 0);
        tag_clean(&expected);
        tag_clean(&b);
      }
    if (widths[i]) {
      TEST_ASSERT(tag_shift_test_init(&expected, types[i], 0));
      for (n = widths[i]; n <= widths[i] + 1; n++) {
        TEST_ASSERT(tag_init_u64(&b, n));
        TEST_EQ(tag_shift_test_result(&a, &b, &expected, false), 0);
        TEST_EQ(tag_shift_test_result(&a, &b, &expected, true), 0);
      }
      tag_clean(&expected);
    }
    /* Negative counts must leave an existing destination untouched. */
    for (j = 2; j <= 6; j++) {
      TEST_ASSERT(tag_shift_test_init(&b, types[j], -1));
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
      TEST_ASSERT(tag_shift_test_init(&expected, types[i], 0));
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
    TEST_ASSERT(tag_shift_test_init(&a, types[i], minima[i - 2]));
    TEST_ASSERT(tag_init_u8(&b, 1));
    TEST_ASSERT(tag_shift_test_init(&expected, types[i], 0));
    TEST_EQ(tag_shift_test_result(&a, &b, &expected, false), 0);
    TEST_ASSERT(tag_shift_test_init(&expected, types[i], minima[i - 2] / 2));
    TEST_EQ(tag_shift_test_result(&a, &b, &expected, true), 0);
    TEST_ASSERT(tag_shift_test_init(&expected, types[i], -1));
    for (n = widths[i] - 1; n <= widths[i] + 1; n++) {
      TEST_ASSERT(tag_init_u64(&b, n));
      TEST_EQ(tag_shift_test_result(&a, &b, &expected, true), 0);
    }
    TEST_ASSERT(tag_shift_test_init(&a, types[i], 1));
    TEST_ASSERT(tag_init_u8(&b, widths[i] - 1));
    TEST_ASSERT(tag_shift_test_init(&expected, types[i], minima[i - 2]));
    TEST_EQ(tag_shift_test_result(&a, &b, &expected, false), 0);
    TEST_ASSERT(tag_shift_test_init(&a, types[i], -3));
    TEST_ASSERT(tag_init_u8(&b, 1));
    TEST_ASSERT(tag_shift_test_init(&expected, types[i], -6));
    TEST_EQ(tag_shift_test_result(&a, &b, &expected, false), 0);
    TEST_ASSERT(tag_shift_test_init(&expected, types[i], -2));
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
    TEST_ASSERT(tag_shift_test_init(&a, TAG_INTEGER, -(s64) i));
    TEST_ASSERT(tag_init_u8(&b, 1));
    TEST_ASSERT(tag_shift_test_init(&expected, TAG_INTEGER,
                                    integer_floors[i]));
    TEST_EQ(tag_shift_test_result(&a, &b, &expected, true), 0);
    tag_clean(&expected);
    TEST_ASSERT(tag_init_u64(&b, U64_MAX));
    TEST_ASSERT(tag_shift_test_init(&expected, TAG_INTEGER, i ? -1 : 0));
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
