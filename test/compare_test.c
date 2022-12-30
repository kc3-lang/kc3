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
#include "test.h"
#include "../libc3/compare.h"
#include "../libc3/tag.h"

#define COMPARE_TEST_TAG(a, b, expected)                               \
  do {                                                                 \
    TEST_EQ(compare_tag((a), (b)), (expected));                        \
  } while (0)

void compare_test_f32 ();
void compare_test_f64 ();

void compare_test ()
{
  compare_test_f32();
  compare_test_f64();
}

void compare_test_f32 ()
{
  TEST_EQ(compare_f32('0', '0'), 0);
  TEST_EQ(compare_f32('0', '1'), -1);
  TEST_EQ(compare_f32('1', '0'), 1);
  TEST_EQ(compare_f32(1.0, 1.0), 0);
  TEST_EQ(compare_f32(1.0, 2.0), -1);
  TEST_EQ(compare_f32(1.0, 0.0), 1);
  TEST_EQ(compare_f32(10000000000000000000000.0,
                      10000000000000000000000.0),
          0);
  TEST_EQ(compare_f32(3.40282346638528859811704183484516925440e+38,
                      3.40282346638528859811704183484516925440e+38),
          0);
  TEST_EQ(compare_f32(1.797693134862315708145274237317043567981e+38,
                      1.797693134862315708145274237317043567981e+38),
          0);
  TEST_EQ(compare_f32(1.797693134862315708145274237317043567981e+38,
                      1.797693134862315708145274237317043567981e+38),
          0);
  TEST_EQ(compare_f32(1.597693134862315708145274237317043567981e+38,
                      1.797693134862315708145274237317043567980e+38),
          -1);
  TEST_EQ(compare_f32(1.797693134862315708145274237317043567980e+38,
                      1.597693134862315708145274237317043567981e+38),
          1);
}

void compare_test_f64 ()
{
  TEST_EQ(compare_f64(0.0, 0.0), 0);
  TEST_EQ(compare_f64(0.0, 1.0), -1);
  TEST_EQ(compare_f64(1.0, 0.0), 1);
  TEST_EQ(compare_f64(1.0, 1.0), 0);
  TEST_EQ(compare_f64(10000000000000000000000.0,
                      10000000000000000000000.0),
          0);
  TEST_EQ(compare_f64(3.40282346638528859811704183484516925440e+38,
                      3.40282346638528859811704183484516925440e+38),
          0);
  TEST_EQ(compare_f64(1.797693134862315708145274237317043567981e+308,
                      1.797693134862315708145274237317043567981e+308),
          0);
  TEST_EQ(compare_f64(1.597693134862315708145274237317043567981e+308,
                      1.797693134862315708145274237317043567980e+308),
          -1);
  TEST_EQ(compare_f64(1.797693134862315708145274237317043567980e+308,
                      1.597693134862315708145274237317043567981e+308),
          1);
}

void compare_test_tag ()
{
  s_tag a;
  s_tag b;
  /* tuple */
  COMPARE_TEST_TAG(tag_init_1(&a, "{a, b}"), &a, 0);
  COMPARE_TEST_TAG(tag_1(&a, "{a, b}"), tag_init_1(&b, "{a, b}"), 0);
  COMPARE_TEST_TAG(tag_1(&a, "{{a, b}, {c, d}}"),
               tag_1(&b, "{{a, b}, {c, d}}"), 0);
  /* integer <> s8 */
  COMPARE_TEST_TAG(tag_1(&a, "-0x10000000000000000"),
                   tag_1(&b, "-0x7F"), -1);
  COMPARE_TEST_TAG(tag_1(&a, "-0x7F"),
                   tag_1(&b, "-0x10000000000000000"), 1);
  /* integer <> s16 */
  COMPARE_TEST_TAG(tag_1(&a, "-0x10000000000000000"),
                   tag_1(&b, "-0x7FFF"), -1);
  COMPARE_TEST_TAG(tag_1(&a, "-0x7FFF"),
                   tag_1(&b, "-0x10000000000000000"), 1);
  /* integer <> s32 */
  COMPARE_TEST_TAG(tag_1(&a, "-0x10000000000000000"),
                   tag_1(&b, "-0x7FFFFFFF"), -1);
  COMPARE_TEST_TAG(tag_1(&a, "-0x7FFFFFFF"),
                   tag_1(&b, "-0x10000000000000000"), 1);
  /* integer <> s64 */
  COMPARE_TEST_TAG(tag_1(&a, "-0x10000000000000000"),
                   tag_1(&b, "-0x7FFFFFFFFFFFFFFF"), -1);
  COMPARE_TEST_TAG(tag_1(&a, "-0x7FFFFFFFFFFFFFFF"),
                   tag_1(&b, "-0x10000000000000000"), 1);
  /* integer <> u8 */
  COMPARE_TEST_TAG(tag_1(&a, "0xFF"),
                   tag_1(&b, "0x10000000000000000"), 1);
  COMPARE_TEST_TAG(tag_1(&a, "0x10000000000000000"),
                   tag_1(&b, "0xFF"), -1);
  /* integer <> u16 */
  COMPARE_TEST_TAG(tag_1(&a, "0xFFFF"),
                   tag_1(&b, "0x10000000000000000"), 1);
  COMPARE_TEST_TAG(tag_1(&a, "0x10000000000000000"),
                   tag_1(&b, "0xFFFF"), -1);
  /* integer <> u32 */
  COMPARE_TEST_TAG(tag_1(&a, "0xFFFFFFFF"),
                   tag_1(&b, "0x10000000000000000"), 1);
  COMPARE_TEST_TAG(tag_1(&a, "0x10000000000000000"),
                   tag_1(&b, "0xFFFFFFFF"), -1);
  /* integer <> u64 */
  COMPARE_TEST_TAG(tag_1(&a, "0xFFFFFFFFFFFFFFFF"),
                   tag_1(&b, "0x10000000000000000"), 1);
  COMPARE_TEST_TAG(tag_1(&a, "0x10000000000000000"),
                   tag_1(&b, "0xFFFFFFFFFFFFFFFF"), -1);
  /* s8 <> s8 */
  COMPARE_TEST_TAG(tag_1(&a, "-0x7F"), tag_1(&b, "-0x7F"), 0);
  COMPARE_TEST_TAG(tag_1(&a, "-0x7F"), tag_1(&b, "-1"), -1);
  COMPARE_TEST_TAG(tag_1(&a, "-1"), tag_1(&b, "-0x7F"), 1);
  COMPARE_TEST_TAG(tag_1(&a, "-1"), tag_1(&b, "-1"), 0);
  /* s8 <> s16 */
  COMPARE_TEST_TAG(tag_1(&a, "-0x100"), tag_1(&b, "-0x7F"), -1);
  COMPARE_TEST_TAG(tag_1(&a, "-0x7F"), tag_1(&b, "-0x100"), 1);
  /* s8 <> s32 */
  COMPARE_TEST_TAG(tag_1(&a, "-0x10000"), tag_1(&b, "-0x7F"), -1);
  COMPARE_TEST_TAG(tag_1(&a, "-0x7F"), tag_1(&b, "-0x10000"), 1);
  /* s8 <> s64 */
  COMPARE_TEST_TAG(tag_1(&a, "-0x100000000"), tag_1(&b, "-0x7F"), -1);
  COMPARE_TEST_TAG(tag_1(&a, "-0x7F"), tag_1(&b, "-0x100000000"), 1);
  /* s8 <> u8 */
  COMPARE_TEST_TAG(tag_1(&a, "-1"), tag_1(&b, "0"), -1);
  COMPARE_TEST_TAG(tag_1(&a, "0"), tag_1(&b, "-1"), 1);
  /* s8 <> u16 */
  COMPARE_TEST_TAG(tag_1(&a, "-1"), tag_1(&b, "0x100"), -1);
  COMPARE_TEST_TAG(tag_1(&a, "0x100"), tag_1(&b, "-1"), 1);
  /* s8 <> u32 */
  COMPARE_TEST_TAG(tag_1(&a, "-1"), tag_1(&b, "0x10000"), -1);
  COMPARE_TEST_TAG(tag_1(&a, "0x10000"), tag_1(&b, "-1"), 1);
  /* s8 <> u64 */
  COMPARE_TEST_TAG(tag_1(&a, "-1"), tag_1(&b, "0x100000000"), -1);
  COMPARE_TEST_TAG(tag_1(&a, "0x100000000"), tag_1(&b, "-1"), 1);
  /* s16 <> s16 */
  COMPARE_TEST_TAG(tag_1(&a, "-0x100"), tag_1(&b, "-0x100"), 0);
  COMPARE_TEST_TAG(tag_1(&a, "-0x200"), tag_1(&b, "-0x100"), -1);
  COMPARE_TEST_TAG(tag_1(&a, "-0x100"), tag_1(&b, "-0x200"), 1);
  COMPARE_TEST_TAG(tag_1(&a, "-0x200"), tag_1(&b, "-0x200"), 0);
  /* s16 <> s32 */
  COMPARE_TEST_TAG(tag_1(&a, "-0x10000"), tag_1(&b, "-0x100"), -1);
  COMPARE_TEST_TAG(tag_1(&a, "-0x100"), tag_1(&b, "-0x10000"), 1);
  /* s16 <> s64 */
  COMPARE_TEST_TAG(tag_1(&a, "-0x100000000"), tag_1(&b, "-0x100"), -1);
  COMPARE_TEST_TAG(tag_1(&a, "-0x100"), tag_1(&b, "-0x100000000"), 1);
  /* s16 <> u8 */
  COMPARE_TEST_TAG(tag_1(&a, "-0x100"), tag_1(&b, "0"), -1);
  COMPARE_TEST_TAG(tag_1(&a, "0"), tag_1(&b, "-0x100"), 1);
  /* s16 <> u16 */
  COMPARE_TEST_TAG(tag_1(&a, "-0x100"), tag_1(&b, "0x100"), -1);
  COMPARE_TEST_TAG(tag_1(&a, "0x100"), tag_1(&b, "-0x100"), 1);
  /* s16 <> u32 */
  COMPARE_TEST_TAG(tag_1(&a, "-0x100"), tag_1(&b, "0x10000"), -1);
  COMPARE_TEST_TAG(tag_1(&a, "0x10000"), tag_1(&b, "-0x100"), 1);
  /* s16 <> u64 */
  COMPARE_TEST_TAG(tag_1(&a, "-0x100"), tag_1(&b, "0x100000000"), -1);
  COMPARE_TEST_TAG(tag_1(&a, "0x100000000"), tag_1(&b, "-0x100"), 1);
  /* s32 <> s32 */
  COMPARE_TEST_TAG(tag_1(&a, "-0x10000"), tag_1(&b, "-0x10000"), 0);
  COMPARE_TEST_TAG(tag_1(&a, "-0x20000"), tag_1(&b, "-0x10000"), -1);
  COMPARE_TEST_TAG(tag_1(&a, "-0x10000"), tag_1(&b, "-0x20000"), 1);
  COMPARE_TEST_TAG(tag_1(&a, "-0x20000"), tag_1(&b, "-0x20000"), 0);
  /* s32 <> s64 */
  COMPARE_TEST_TAG(tag_1(&a, "-0x100000000"), tag_1(&b, "-0x10000"), -1);
  COMPARE_TEST_TAG(tag_1(&a, "-0x10000"), tag_1(&b, "-0x100000000"), 1);
  /* s32 <> u8 */
  COMPARE_TEST_TAG(tag_1(&a, "-0x10000"), tag_1(&b, "0"), -1);
  COMPARE_TEST_TAG(tag_1(&a, "0"), tag_1(&b, "-0x10000"), 1);
  /* s32 <> u16 */
  COMPARE_TEST_TAG(tag_1(&a, "-0x10000"), tag_1(&b, "0x100"), -1);
  COMPARE_TEST_TAG(tag_1(&a, "0x100"), tag_1(&b, "-0x10000"), 1);
  /* s32 <> u32 */
  COMPARE_TEST_TAG(tag_1(&a, "-0x10000"), tag_1(&b, "0x10000"), -1);
  COMPARE_TEST_TAG(tag_1(&a, "0x10000"), tag_1(&b, "-0x10000"), 1);
  /* s32 <> u64 */
  COMPARE_TEST_TAG(tag_1(&a, "-0x10000"),
                   tag_1(&b, "0x100000000"), -1);
  COMPARE_TEST_TAG(tag_1(&a, "0x100000000"),
                   tag_1(&b, "-0x10000"), 1);
  /* s64 <> s64 */
  COMPARE_TEST_TAG(tag_1(&a, "-0x100000000"),
                   tag_1(&b, "-0x100000000"), 0);
  COMPARE_TEST_TAG(tag_1(&a, "-0x200000000"),
                   tag_1(&b, "-0x100000000"), -1);
  COMPARE_TEST_TAG(tag_1(&a, "-0x100000000"),
                   tag_1(&b, "-0x200000000"), 1);
  COMPARE_TEST_TAG(tag_1(&a, "-0x200000000"),
                   tag_1(&b, "-0x200000000"), 0);
  /* s64 <> u8 */
  COMPARE_TEST_TAG(tag_1(&a, "-0x100000000"), tag_1(&b, "0"), -1);
  COMPARE_TEST_TAG(tag_1(&a, "0"), tag_1(&b, "-0x100000000"), 1);
  /* s64 <> u16 */
  COMPARE_TEST_TAG(tag_1(&a, "-0x100000000"), tag_1(&b, "0x100"), -1);
  COMPARE_TEST_TAG(tag_1(&a, "0x100"), tag_1(&b, "-0x100000000"), 1);
  /* s64 <> u32 */
  COMPARE_TEST_TAG(tag_1(&a, "-0x100000000"),
                   tag_1(&b, "0x10000"), -1);
  COMPARE_TEST_TAG(tag_1(&a, "0x10000"),
                   tag_1(&b, "-0x100000000"), 1);
  /* s64 <> u64 */
  COMPARE_TEST_TAG(tag_1(&a, "-0x100000000"),
                   tag_1(&b, "0x100000000"), -1);
  COMPARE_TEST_TAG(tag_1(&a, "0x100000000"),
                   tag_1(&b, "-0x100000000"), 1);
  /* u8 <> u8 */
  COMPARE_TEST_TAG(tag_1(&a, "0"), tag_1(&b, "0"), 0);
  COMPARE_TEST_TAG(tag_1(&a, "0"), tag_1(&b, "1"), -1);
  COMPARE_TEST_TAG(tag_1(&a, "1"), tag_1(&b, "0"), 1);
  COMPARE_TEST_TAG(tag_1(&a, "1"), tag_1(&b, "1"), 0);
  /* u8 <> u16 */
  COMPARE_TEST_TAG(tag_1(&a, "0"), tag_1(&b, "0x100"), -1);
  COMPARE_TEST_TAG(tag_1(&a, "0x100"), tag_1(&b, "0"), 1);
  /* u8 <> u32 */
  COMPARE_TEST_TAG(tag_1(&a, "0"), tag_1(&b, "0x10000"), -1);
  COMPARE_TEST_TAG(tag_1(&a, "0x10000"), tag_1(&b, "0"), 1);
  /* u8 <> u64 */
  COMPARE_TEST_TAG(tag_1(&a, "0"), tag_1(&b, "0x100000000"), -1);
  COMPARE_TEST_TAG(tag_1(&a, "0x100000000"), tag_1(&b, "0"), 1);
  /* u16 <> u16 */
  COMPARE_TEST_TAG(tag_1(&a, "0x100"), tag_1(&b, "0x100"), 0);
  COMPARE_TEST_TAG(tag_1(&a, "0x100"), tag_1(&b, "0x200"), -1);
  COMPARE_TEST_TAG(tag_1(&a, "0x200"), tag_1(&b, "0x100"), 1);
  COMPARE_TEST_TAG(tag_1(&a, "0x200"), tag_1(&b, "0x200"), 0);
  /* u16 <> u32 */
  COMPARE_TEST_TAG(tag_1(&a, "0x100"), tag_1(&b, "0x10000"), -1);
  COMPARE_TEST_TAG(tag_1(&a, "0x10000"), tag_1(&b, "0x100"), 1);
  /* u16 <> u64 */
  COMPARE_TEST_TAG(tag_1(&a, "0x100"), tag_1(&b, "0x100000000"), -1);
  COMPARE_TEST_TAG(tag_1(&a, "0x100000000"), tag_1(&b, "0x100"), 1);
  /* u32 <> u32 */
  COMPARE_TEST_TAG(tag_1(&a, "0x10000"), tag_1(&b, "0x10000"), 0);
  COMPARE_TEST_TAG(tag_1(&a, "0x10000"), tag_1(&b, "0x20000"), -1);
  COMPARE_TEST_TAG(tag_1(&a, "0x20000"), tag_1(&b, "0x10000"), 1);
  COMPARE_TEST_TAG(tag_1(&a, "0x20000"), tag_1(&b, "0x20000"), 0);
  /* u32 <> u64 */
  COMPARE_TEST_TAG(tag_1(&a, "0x10000"), tag_1(&b, "0x100000000"), -1);
  COMPARE_TEST_TAG(tag_1(&a, "0x100000000"), tag_1(&b, "0x10000"), 1);
  /* u64 <> u64 */
  COMPARE_TEST_TAG(tag_1(&a, "0x100000000"),
                   tag_1(&b, "0x100000000"), 0);
  COMPARE_TEST_TAG(tag_1(&a, "0x100000000"),
                   tag_1(&b, "0x200000000"), -1);
  COMPARE_TEST_TAG(tag_1(&a, "0x200000000"),
                   tag_1(&b, "0x100000000"), 1);
  COMPARE_TEST_TAG(tag_1(&a, "0x200000000"),
                   tag_1(&b, "0x200000000"), 0);
  tag_clean(&a);
  tag_clean(&b);
}
