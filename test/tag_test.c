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
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "../libc3/u64.h"
#include "../libc3/tag.h"
#include "test.h"

#define TAG_TEST_COMPARE(a, b, expected)                                   \
  do {                                                                 \
    TEST_EQ(tag_compare((a), (b)), (expected));                            \
  } while (0)

#define TAG_TEST_HASH_COMPARE(a, b, expected)                              \
  do {                                                                 \
    TEST_EQ(u64_compare(tag_hash(a), tag_hash(b)), (expected));            \
  } while (0)

void tag_test ();
void tag_test_compare ();
void tag_test_hash ();
void tag_to_ffi_type_test ();

void tag_test ()
{
  tag_test_compare();
  tag_test_hash();
}

void tag_test_compare ()
{
  s_tag a;
  s_tag b;
  /* tuple */
  TAG_TEST_COMPARE(tag_init_1(&a, "{a, b}"), &a, 0);
  TAG_TEST_COMPARE(tag_1(&a, "{a, b}"), tag_init_1(&b, "{a, b}"), 0);
  TAG_TEST_COMPARE(tag_1(&a, "{{a, b}, {c, d}}"),
               tag_1(&b, "{{a, b}, {c, d}}"), 0);
  /* integer <> s8 */
  TAG_TEST_COMPARE(tag_1(&a, "-0x10000000000000000"),
                   tag_1(&b, "-0x7F"), -1);
  TAG_TEST_COMPARE(tag_1(&a, "-0x7F"),
                   tag_1(&b, "-0x10000000000000000"), 1);
  /* integer <> s16 */
  TAG_TEST_COMPARE(tag_1(&a, "-0x10000000000000000"),
                   tag_1(&b, "-0x7FFF"), -1);
  TAG_TEST_COMPARE(tag_1(&a, "-0x7FFF"),
                   tag_1(&b, "-0x10000000000000000"), 1);
  /* integer <> s32 */
  TAG_TEST_COMPARE(tag_1(&a, "-0x10000000000000000"),
                   tag_1(&b, "-0x7FFFFFFF"), -1);
  TAG_TEST_COMPARE(tag_1(&a, "-0x7FFFFFFF"),
                   tag_1(&b, "-0x10000000000000000"), 1);
  /* integer <> s64 */
  TAG_TEST_COMPARE(tag_1(&a, "-0x10000000000000000"),
                   tag_1(&b, "-0x7FFFFFFFFFFFFFFF"), -1);
  TAG_TEST_COMPARE(tag_1(&a, "-0x7FFFFFFFFFFFFFFF"),
                   tag_1(&b, "-0x10000000000000000"), 1);
  /* integer <> u8 */
  TAG_TEST_COMPARE(tag_1(&a, "0xFF"),
                   tag_1(&b, "0x10000000000000000"), 1);
  TAG_TEST_COMPARE(tag_1(&a, "0x10000000000000000"),
                   tag_1(&b, "0xFF"), -1);
  /* integer <> u16 */
  TAG_TEST_COMPARE(tag_1(&a, "0xFFFF"),
                   tag_1(&b, "0x10000000000000000"), 1);
  TAG_TEST_COMPARE(tag_1(&a, "0x10000000000000000"),
                   tag_1(&b, "0xFFFF"), -1);
  /* integer <> u32 */
  TAG_TEST_COMPARE(tag_1(&a, "0xFFFFFFFF"),
                   tag_1(&b, "0x10000000000000000"), 1);
  TAG_TEST_COMPARE(tag_1(&a, "0x10000000000000000"),
                   tag_1(&b, "0xFFFFFFFF"), -1);
  /* integer <> u64 */
  TAG_TEST_COMPARE(tag_1(&a, "0xFFFFFFFFFFFFFFFF"),
                   tag_1(&b, "0x10000000000000000"), 1);
  TAG_TEST_COMPARE(tag_1(&a, "0x10000000000000000"),
                   tag_1(&b, "0xFFFFFFFFFFFFFFFF"), -1);
  /* s8 <> s8 */
  TAG_TEST_COMPARE(tag_1(&a, "-0x7F"), tag_1(&b, "-0x7F"), 0);
  TAG_TEST_COMPARE(tag_1(&a, "-0x7F"), tag_1(&b, "-1"), -1);
  TAG_TEST_COMPARE(tag_1(&a, "-1"), tag_1(&b, "-0x7F"), 1);
  TAG_TEST_COMPARE(tag_1(&a, "-1"), tag_1(&b, "-1"), 0);
  /* s8 <> s16 */
  TAG_TEST_COMPARE(tag_1(&a, "-0x100"), tag_1(&b, "-0x7F"), -1);
  TAG_TEST_COMPARE(tag_1(&a, "-0x7F"), tag_1(&b, "-0x100"), 1);
  /* s8 <> s32 */
  TAG_TEST_COMPARE(tag_1(&a, "-0x10000"), tag_1(&b, "-0x7F"), -1);
  TAG_TEST_COMPARE(tag_1(&a, "-0x7F"), tag_1(&b, "-0x10000"), 1);
  /* s8 <> s64 */
  TAG_TEST_COMPARE(tag_1(&a, "-0x100000000"), tag_1(&b, "-0x7F"), -1);
  TAG_TEST_COMPARE(tag_1(&a, "-0x7F"), tag_1(&b, "-0x100000000"), 1);
  /* s8 <> u8 */
  TAG_TEST_COMPARE(tag_1(&a, "-1"), tag_1(&b, "0"), -1);
  TAG_TEST_COMPARE(tag_1(&a, "0"), tag_1(&b, "-1"), 1);
  /* s8 <> u16 */
  TAG_TEST_COMPARE(tag_1(&a, "-1"), tag_1(&b, "0x100"), -1);
  TAG_TEST_COMPARE(tag_1(&a, "0x100"), tag_1(&b, "-1"), 1);
  /* s8 <> u32 */
  TAG_TEST_COMPARE(tag_1(&a, "-1"), tag_1(&b, "0x10000"), -1);
  TAG_TEST_COMPARE(tag_1(&a, "0x10000"), tag_1(&b, "-1"), 1);
  /* s8 <> u64 */
  TAG_TEST_COMPARE(tag_1(&a, "-1"), tag_1(&b, "0x100000000"), -1);
  TAG_TEST_COMPARE(tag_1(&a, "0x100000000"), tag_1(&b, "-1"), 1);
  /* s16 <> s16 */
  TAG_TEST_COMPARE(tag_1(&a, "-0x100"), tag_1(&b, "-0x100"), 0);
  TAG_TEST_COMPARE(tag_1(&a, "-0x200"), tag_1(&b, "-0x100"), -1);
  TAG_TEST_COMPARE(tag_1(&a, "-0x100"), tag_1(&b, "-0x200"), 1);
  TAG_TEST_COMPARE(tag_1(&a, "-0x200"), tag_1(&b, "-0x200"), 0);
  /* s16 <> s32 */
  TAG_TEST_COMPARE(tag_1(&a, "-0x10000"), tag_1(&b, "-0x100"), -1);
  TAG_TEST_COMPARE(tag_1(&a, "-0x100"), tag_1(&b, "-0x10000"), 1);
  /* s16 <> s64 */
  TAG_TEST_COMPARE(tag_1(&a, "-0x100000000"), tag_1(&b, "-0x100"), -1);
  TAG_TEST_COMPARE(tag_1(&a, "-0x100"), tag_1(&b, "-0x100000000"), 1);
  /* s16 <> u8 */
  TAG_TEST_COMPARE(tag_1(&a, "-0x100"), tag_1(&b, "0"), -1);
  TAG_TEST_COMPARE(tag_1(&a, "0"), tag_1(&b, "-0x100"), 1);
  /* s16 <> u16 */
  TAG_TEST_COMPARE(tag_1(&a, "-0x100"), tag_1(&b, "0x100"), -1);
  TAG_TEST_COMPARE(tag_1(&a, "0x100"), tag_1(&b, "-0x100"), 1);
  /* s16 <> u32 */
  TAG_TEST_COMPARE(tag_1(&a, "-0x100"), tag_1(&b, "0x10000"), -1);
  TAG_TEST_COMPARE(tag_1(&a, "0x10000"), tag_1(&b, "-0x100"), 1);
  /* s16 <> u64 */
  TAG_TEST_COMPARE(tag_1(&a, "-0x100"), tag_1(&b, "0x100000000"), -1);
  TAG_TEST_COMPARE(tag_1(&a, "0x100000000"), tag_1(&b, "-0x100"), 1);
  /* s32 <> s32 */
  TAG_TEST_COMPARE(tag_1(&a, "-0x10000"), tag_1(&b, "-0x10000"), 0);
  TAG_TEST_COMPARE(tag_1(&a, "-0x20000"), tag_1(&b, "-0x10000"), -1);
  TAG_TEST_COMPARE(tag_1(&a, "-0x10000"), tag_1(&b, "-0x20000"), 1);
  TAG_TEST_COMPARE(tag_1(&a, "-0x20000"), tag_1(&b, "-0x20000"), 0);
  /* s32 <> s64 */
  TAG_TEST_COMPARE(tag_1(&a, "-0x100000000"), tag_1(&b, "-0x10000"), -1);
  TAG_TEST_COMPARE(tag_1(&a, "-0x10000"), tag_1(&b, "-0x100000000"), 1);
  /* s32 <> u8 */
  TAG_TEST_COMPARE(tag_1(&a, "-0x10000"), tag_1(&b, "0"), -1);
  TAG_TEST_COMPARE(tag_1(&a, "0"), tag_1(&b, "-0x10000"), 1);
  /* s32 <> u16 */
  TAG_TEST_COMPARE(tag_1(&a, "-0x10000"), tag_1(&b, "0x100"), -1);
  TAG_TEST_COMPARE(tag_1(&a, "0x100"), tag_1(&b, "-0x10000"), 1);
  /* s32 <> u32 */
  TAG_TEST_COMPARE(tag_1(&a, "-0x10000"), tag_1(&b, "0x10000"), -1);
  TAG_TEST_COMPARE(tag_1(&a, "0x10000"), tag_1(&b, "-0x10000"), 1);
  /* s32 <> u64 */
  TAG_TEST_COMPARE(tag_1(&a, "-0x10000"),
                   tag_1(&b, "0x100000000"), -1);
  TAG_TEST_COMPARE(tag_1(&a, "0x100000000"),
                   tag_1(&b, "-0x10000"), 1);
  /* s64 <> s64 */
  TAG_TEST_COMPARE(tag_1(&a, "-0x100000000"),
                   tag_1(&b, "-0x100000000"), 0);
  TAG_TEST_COMPARE(tag_1(&a, "-0x200000000"),
                   tag_1(&b, "-0x100000000"), -1);
  TAG_TEST_COMPARE(tag_1(&a, "-0x100000000"),
                   tag_1(&b, "-0x200000000"), 1);
  TAG_TEST_COMPARE(tag_1(&a, "-0x200000000"),
                   tag_1(&b, "-0x200000000"), 0);
  /* s64 <> u8 */
  TAG_TEST_COMPARE(tag_1(&a, "-0x100000000"), tag_1(&b, "0"), -1);
  TAG_TEST_COMPARE(tag_1(&a, "0"), tag_1(&b, "-0x100000000"), 1);
  /* s64 <> u16 */
  TAG_TEST_COMPARE(tag_1(&a, "-0x100000000"), tag_1(&b, "0x100"), -1);
  TAG_TEST_COMPARE(tag_1(&a, "0x100"), tag_1(&b, "-0x100000000"), 1);
  /* s64 <> u32 */
  TAG_TEST_COMPARE(tag_1(&a, "-0x100000000"),
                   tag_1(&b, "0x10000"), -1);
  TAG_TEST_COMPARE(tag_1(&a, "0x10000"),
                   tag_1(&b, "-0x100000000"), 1);
  /* s64 <> u64 */
  TAG_TEST_COMPARE(tag_1(&a, "-0x100000000"),
                   tag_1(&b, "0x100000000"), -1);
  TAG_TEST_COMPARE(tag_1(&a, "0x100000000"),
                   tag_1(&b, "-0x100000000"), 1);
  /* u8 <> u8 */
  TAG_TEST_COMPARE(tag_1(&a, "0"), tag_1(&b, "0"), 0);
  TAG_TEST_COMPARE(tag_1(&a, "0"), tag_1(&b, "1"), -1);
  TAG_TEST_COMPARE(tag_1(&a, "1"), tag_1(&b, "0"), 1);
  TAG_TEST_COMPARE(tag_1(&a, "1"), tag_1(&b, "1"), 0);
  /* u8 <> u16 */
  TAG_TEST_COMPARE(tag_1(&a, "0"), tag_1(&b, "0x100"), -1);
  TAG_TEST_COMPARE(tag_1(&a, "0x100"), tag_1(&b, "0"), 1);
  /* u8 <> u32 */
  TAG_TEST_COMPARE(tag_1(&a, "0"), tag_1(&b, "0x10000"), -1);
  TAG_TEST_COMPARE(tag_1(&a, "0x10000"), tag_1(&b, "0"), 1);
  /* u8 <> u64 */
  TAG_TEST_COMPARE(tag_1(&a, "0"), tag_1(&b, "0x100000000"), -1);
  TAG_TEST_COMPARE(tag_1(&a, "0x100000000"), tag_1(&b, "0"), 1);
  /* u16 <> u16 */
  TAG_TEST_COMPARE(tag_1(&a, "0x100"), tag_1(&b, "0x100"), 0);
  TAG_TEST_COMPARE(tag_1(&a, "0x100"), tag_1(&b, "0x200"), -1);
  TAG_TEST_COMPARE(tag_1(&a, "0x200"), tag_1(&b, "0x100"), 1);
  TAG_TEST_COMPARE(tag_1(&a, "0x200"), tag_1(&b, "0x200"), 0);
  /* u16 <> u32 */
  TAG_TEST_COMPARE(tag_1(&a, "0x100"), tag_1(&b, "0x10000"), -1);
  TAG_TEST_COMPARE(tag_1(&a, "0x10000"), tag_1(&b, "0x100"), 1);
  /* u16 <> u64 */
  TAG_TEST_COMPARE(tag_1(&a, "0x100"), tag_1(&b, "0x100000000"), -1);
  TAG_TEST_COMPARE(tag_1(&a, "0x100000000"), tag_1(&b, "0x100"), 1);
  /* u32 <> u32 */
  TAG_TEST_COMPARE(tag_1(&a, "0x10000"), tag_1(&b, "0x10000"), 0);
  TAG_TEST_COMPARE(tag_1(&a, "0x10000"), tag_1(&b, "0x20000"), -1);
  TAG_TEST_COMPARE(tag_1(&a, "0x20000"), tag_1(&b, "0x10000"), 1);
  TAG_TEST_COMPARE(tag_1(&a, "0x20000"), tag_1(&b, "0x20000"), 0);
  /* u32 <> u64 */
  TAG_TEST_COMPARE(tag_1(&a, "0x10000"), tag_1(&b, "0x100000000"), -1);
  TAG_TEST_COMPARE(tag_1(&a, "0x100000000"), tag_1(&b, "0x10000"), 1);
  /* u64 <> u64 */
  TAG_TEST_COMPARE(tag_1(&a, "0x100000000"),
                   tag_1(&b, "0x100000000"), 0);
  TAG_TEST_COMPARE(tag_1(&a, "0x100000000"),
                   tag_1(&b, "0x200000000"), -1);
  TAG_TEST_COMPARE(tag_1(&a, "0x200000000"),
                   tag_1(&b, "0x100000000"), 1);
  TAG_TEST_COMPARE(tag_1(&a, "0x200000000"),
                   tag_1(&b, "0x200000000"), 0);
  tag_clean(&a);
  tag_clean(&b);
}

void tag_test_hash ()
{
  s_tag a;
  s_tag b;
  tag_init_1(&a, "{a, b}");
  TAG_TEST_HASH_COMPARE(&a, &a, 0);
  TAG_TEST_HASH_COMPARE(tag_1(&a, "{a, b}"), tag_init_1(&b, "{a, b}"), 0);
  TAG_TEST_HASH_COMPARE(tag_1(&a, "{a, b}"), tag_1(&b, "{a, b}"), 0);
  TAG_TEST_HASH_COMPARE(tag_1(&a, "{{a, b}, {c, d}}"),
                        tag_1(&b, "{{a, b}, {c, d}}"), 0);
  tag_clean(&a);
  tag_clean(&b);
}
