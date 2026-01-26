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
#include "compare_test.h"
#include "../libkc3/list.h"
#include "../libkc3/map.h"
#include "../libkc3/tag.h"
#include "../libkc3/tuple.h"

#define COMPARE_TEST_LIST(a, b, expected)                              \
  do {                                                                 \
    s_list *tmp_a = (a);                                               \
    s_list *tmp_b = (b);                                               \
    test_context("compare_list(" # a ", " # b ") -> " # expected);     \
    TEST_EQ(compare_list(tmp_a, tmp_b), (expected));                   \
    list_delete_all(tmp_a);                                            \
    list_delete_all(tmp_b);                                            \
    test_context(NULL);                                                \
  } while (0)

#define COMPARE_TEST_MAP(a, b, expected)                               \
  do {                                                                 \
    test_context("compare_map(" # a ", " # b ") -> " # expected);      \
    TEST_EQ(compare_map((a), (b)), (expected));                        \
    test_context(NULL);                                                \
  } while (0)

#define COMPARE_TEST_TAG(a, b, expected)                               \
  do {                                                                 \
    TEST_EQ(compare_tag((a), (b)), (expected));                        \
  } while (0)

#define COMPARE_TEST_TUPLE(a, b, expected)                             \
  do {                                                                 \
    test_context("compare_tuple(" # a ", " # b ") -> " # expected);    \
    TEST_EQ(compare_tuple((a), (b)), (expected));                      \
    test_context(NULL);                                                \
  } while (0)

TEST_CASE_PROTOTYPE(compare_bool);
TEST_CASE_PROTOTYPE(compare_character);
TEST_CASE_PROTOTYPE(compare_f32);
TEST_CASE_PROTOTYPE(compare_f64);
TEST_CASE_PROTOTYPE(compare_list);
TEST_CASE_PROTOTYPE(compare_map);
TEST_CASE_PROTOTYPE(compare_str);
TEST_CASE_PROTOTYPE(compare_tag);
TEST_CASE_PROTOTYPE(compare_tuple);

void compare_test (void)
{
  TEST_CASE_RUN(compare_bool);
  TEST_CASE_RUN(compare_character);
  TEST_CASE_RUN(compare_f32);
  TEST_CASE_RUN(compare_f64);
  TEST_CASE_RUN(compare_list);
  TEST_CASE_RUN(compare_map);
  TEST_CASE_RUN(compare_str);
  TEST_CASE_RUN(compare_tag);
  TEST_CASE_RUN(compare_tuple);
}

TEST_CASE(compare_bool)
{
  TEST_EQ(compare_bool(false, false), 0);
  TEST_EQ(compare_bool(false, true), -1);
  TEST_EQ(compare_bool(true, false), 1);
  TEST_EQ(compare_bool(true, true), 0);
}
TEST_CASE_END(compare_bool)

TEST_CASE(compare_character)
{
  TEST_EQ(compare_character('0', '0'), 0);
  TEST_EQ(compare_character('0', '1'), -1);
  TEST_EQ(compare_character('1', '0'), 1);
  TEST_EQ(compare_character('1', '1'), 0);
  TEST_EQ(compare_character('0', 'A'), -1);
  TEST_EQ(compare_character('A', '0'), 1);
  TEST_EQ(compare_character('A', 'A'), 0);
  TEST_EQ(compare_character('A', 'B'), -1);
  TEST_EQ(compare_character('B', 'A'), 1);
  TEST_EQ(compare_character('A', 'a'), -1);
  TEST_EQ(compare_character('a', 'A'), 1);
}
TEST_CASE_END(compare_character)

TEST_CASE(compare_f32)
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
TEST_CASE_END(compare_f32)

TEST_CASE(compare_f64)
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
TEST_CASE_END(compare_f64)

TEST_CASE(compare_list)
{
  COMPARE_TEST_LIST(NULL, NULL, 0);
  COMPARE_TEST_LIST(list_new_1("[A, B]"), list_new_1("[A, C]"), -1);
  COMPARE_TEST_LIST(list_new_1("[A, C]"), list_new_1("[A, B]"), 1);
}
TEST_CASE_END(compare_list)

TEST_CASE(compare_map)
{
  s_map a;
  s_map b;
  COMPARE_TEST_MAP(map_init_1(&a, "%{a: A, b: B}"),
                   map_init_1(&b, "%{a: A, b: B}"), 0);
  COMPARE_TEST_MAP(&a, &a, 0);
  map_clean(&a);
  map_clean(&b);
  COMPARE_TEST_MAP(map_init_1(&a, "%{a: A, b: A}"),
                   map_init_1(&b, "%{a: A, b: B}"), -1);
  map_clean(&a);
  map_clean(&b);
  COMPARE_TEST_MAP(map_init_1(&a, "%{a: A, b: B}"),
                   map_init_1(&b, "%{a: A, b: A}"), 1);
  map_clean(&a);
  map_clean(&b);
  COMPARE_TEST_MAP(map_init_1(&a, "%{a: A, b: B}"),
                   map_init_1(&b, "%{a: A, b: B, c: C}"), -1);
  map_clean(&a);
  map_clean(&b);
  COMPARE_TEST_MAP(map_init_1(&a, "%{a: A, b: B, c: C}"),
                   map_init_1(&b, "%{a: A, b: B}"), 1);
  map_clean(&a);
  map_clean(&b);
}
TEST_CASE_END(compare_map)

TEST_CASE(compare_str)
{
  s_str *p;
  s_str a;
  s_str b;
  TEST_EQ((p = str_new_empty(), compare_str(p, p)), 0);
  str_delete(p);
  TEST_EQ((str_init_1(&a, NULL, "abc"), compare_str(&a, &a)), 0);
  COMPARE_TEST_STR(str_init_empty(&a), str_init_empty(&b), 0);
  COMPARE_TEST_STR(str_init_empty(&a), str_init_1(&b, NULL, "0"), -1);
  COMPARE_TEST_STR(str_init_1(&a, NULL, "0"), str_init_empty(&b), 1);
  COMPARE_TEST_STR(str_init_1(&a, NULL, "0"), str_init_1(&b, NULL, "0"), 0);
  COMPARE_TEST_STR(str_init_1(&a, NULL, "0"), str_init_1(&b, NULL, "A"), -1);
  COMPARE_TEST_STR(str_init_1(&a, NULL, "01"), str_init_1(&b, NULL, "0"), 1);
  COMPARE_TEST_STR(str_init_1(&a, NULL, "01"), str_init_1(&b, NULL, "01"), 0);
  COMPARE_TEST_STR(str_init_1(&a, NULL, "01"), str_init_1(&b, NULL, "012"), -1);
  COMPARE_TEST_STR(str_init_1(&a, NULL, "01"), str_init_1(&b, NULL, "02"), -1);
  COMPARE_TEST_STR(str_init_1(&a, NULL, "01"), str_init_1(&b, NULL, "023"), -1);
  COMPARE_TEST_STR(str_init_1(&a, NULL, "01"), str_init_1(&b, NULL, "ABC"), -1);
  COMPARE_TEST_STR(str_init_1(&a, NULL, "012"), str_init_1(&b, NULL, "0"), 1);
  COMPARE_TEST_STR(str_init_1(&a, NULL, "012"), str_init_1(&b, NULL, "01"), 1);
  COMPARE_TEST_STR(str_init_1(&a, NULL, "012"), str_init_1(&b, NULL, "012"), 0);
  COMPARE_TEST_STR(str_init_1(&a, NULL, "012"), str_init_1(&b, NULL, "0123"), -1);
  COMPARE_TEST_STR(str_init_1(&a, NULL, "012"), str_init_1(&b, NULL, "013"), -1);
  COMPARE_TEST_STR(str_init_1(&a, NULL, "012"), str_init_1(&b, NULL, "0134"), -1);
  COMPARE_TEST_STR(str_init_1(&a, NULL, "012"), str_init_1(&b, NULL, "ABC"), -1);
}
TEST_CASE_END(compare_str)

TEST_CASE(compare_tag)
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
                   tag_1(&b, "0x10000000000000000"), -1);
  COMPARE_TEST_TAG(tag_1(&a, "0x10000000000000000"),
                   tag_1(&b, "0xFF"), 1);
  /* integer <> u16 */
  COMPARE_TEST_TAG(tag_1(&a, "0xFFFF"),
                   tag_1(&b, "0x10000000000000000"), -1);
  COMPARE_TEST_TAG(tag_1(&a, "0x10000000000000000"),
                   tag_1(&b, "0xFFFF"), 1);
  /* integer <> u32 */
  COMPARE_TEST_TAG(tag_1(&a, "0xFFFFFFFF"),
                   tag_1(&b, "0x10000000000000000"), -1);
  COMPARE_TEST_TAG(tag_1(&a, "0x10000000000000000"),
                   tag_1(&b, "0xFFFFFFFF"), 1);
  /* integer <> u64 */
  COMPARE_TEST_TAG(tag_1(&a, "0xFFFFFFFFFFFFFFFF"),
                   tag_1(&b, "0x10000000000000000"), -1);
  COMPARE_TEST_TAG(tag_1(&a, "0x10000000000000000"),
                   tag_1(&b, "0xFFFFFFFFFFFFFFFF"), 1);
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
TEST_CASE_END(compare_tag)

TEST_CASE(compare_tuple)
{
  s_tuple a;
  s_tuple b;
  COMPARE_TEST_TUPLE(tuple_init_1(&a, "{A, B}"),
                     tuple_init_1(&b, "{A, B}"), 0);
  COMPARE_TEST_TUPLE(&a, &a, 0);
  tuple_clean(&a);
  tuple_clean(&b);
  COMPARE_TEST_TUPLE(tuple_init_1(&a, "{A, A}"),
                     tuple_init_1(&b, "{A, B}"), -1);
  tuple_clean(&a);
  tuple_clean(&b);
  COMPARE_TEST_TUPLE(tuple_init_1(&a, "{A, B}"),
                     tuple_init_1(&b, "{A, A}"), 1);
  tuple_clean(&a);
  tuple_clean(&b);
  COMPARE_TEST_TUPLE(tuple_init_1(&a, "{A, B}"),
                     tuple_init_1(&b, "{A, B, C}"), -1);
  tuple_clean(&a);
  tuple_clean(&b);
  COMPARE_TEST_TUPLE(tuple_init_1(&a, "{A, B, C}"),
                     tuple_init_1(&b, "{A, B}"), 1);
  tuple_clean(&a);
  tuple_clean(&b);
}
TEST_CASE_END(compare_tuple)
