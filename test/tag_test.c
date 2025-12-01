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
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "../libkc3/tag.h"
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

void tag_test (void)
{
  TEST_CASE_RUN(tag_init_1);
  TEST_CASE_RUN(tag_hash_u64);
}

TEST_CASE(tag_hash_u64)
{
  s_tag a;
  s_tag b;
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
  s_tag a;
  TEST_ASSERT(tag_init_1(&a, "fn () { [] }"));
  tag_clean(&a);
  TEST_ASSERT(tag_init_1(&a, "fn (x) { x }"));
  tag_clean(&a);
  TEST_ASSERT(tag_init_1(&a, "fn (x, y) { x }"));
  tag_clean(&a);
}
TEST_CASE_END(tag_init_1)

TEST_CASE(tag_type__macro)
{
  TEST_EQ(TAG_TYPE(0), TAG_U8);
  TEST_EQ(TAG_TYPE(1), TAG_U8);
  TEST_EQ(TAG_TYPE(256), TAG_U16);
  TEST_EQ(TAG_TYPE(65535), TAG_U16);
  TEST_EQ(TAG_TYPE(65536), TAG_U32);
  TEST_EQ(TAG_TYPE(4294967295), TAG_U32);
  TEST_EQ(TAG_TYPE(4294967296), TAG_U64);
  TEST_EQ(TAG_TYPE(18446744073709551615), TAG_U64);
  TEST_EQ(TAG_TYPE("a"), TAG_STR);
}
TEST_CASE_END(tag_type__macro)
