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

#include <endian.h>
#include "../libkc3/file.h"
#include "../libkc3/marshall.h"
#include "../libkc3/str.h"
#include "../libkc3/list.h"
#include "../libkc3/tag.h"
#include "test.h"

#define MARSHALL_TEST(type, test, expected)                           \
  do {                                                                \
    s_marshall m = {0};                                               \
    const s_str expected_str = STR_1(expected);                       \
    s_str test_str = {0};                                             \
    test_context("marshall_" # type "(&m, " # test ") -> "            \
                 # expected);                                         \
    TEST_ASSERT(marshall_init(&m));                                   \
    TEST_ASSERT(marshall_ ## type (&m, false, (type) (test)));        \
    TEST_ASSERT(marshall_to_str(&m, &test_str));                      \
    TEST_STR_EQ(test_str, expected_str);                              \
    marshall_clean(&m);                                               \
    test_context(NULL);                                               \
  } while (0)

#define MARSHALL_TEST_STR(test, expected)                             \
  do {                                                                \
    s_marshall m = {0};                                               \
    s_str str_expected = STR_1(expected);                             \
    s_str str_test = {0};                                             \
    s_str str_result = {0};                                           \
    test_context("marshall_str(&m, " # test ") -> " # expected);      \
    TEST_EQ(marshall_init(&m), &m);                                   \
    TEST_EQ(str_init(&str_test, NULL, sizeof(test) - 1, (test)),      \
            &str_test);                                               \
    TEST_EQ(marshall_str(&m, false, &str_test), &m);                  \
    TEST_EQ(marshall_to_str(&m, &str_result), &str_result);           \
    TEST_STR_EQ(str_result, str_expected);                            \
    str_clean(&str_result);                                           \
    str_clean(&str_test);                                             \
    marshall_clean(&m);                                               \
    test_context(NULL);                                               \
  } while (0)

#define MARSHALL_TEST_TAG(test, expected)                             \
  do {                                                                \
    s_marshall m = {0};                                               \
    const s_str expected_str = STR_1(expected);                       \
    s_tag tag = {0};                                                  \
    s_str test_str = {0};                                             \
    test_context("marshall_tag(" # test ") -> " # expected);          \
    TEST_ASSERT(tag_init_1(&tag, test));                              \
    TEST_EQ(marshall_init(&m), &m);                                   \
    TEST_EQ(marshall_tag(&m, false, &tag), &m);                       \
    TEST_ASSERT(marshall_to_str(&m, &test_str));                      \
    TEST_STR_EQ(test_str, expected_str);                              \
    marshall_clean(&m);                                               \
    tag_clean(&tag);                                                  \
    test_context(NULL);                                               \
  } while (0)

void marshal_test (void);

TEST_CASE_PROTOTYPE(marshall_bool);
TEST_CASE_PROTOTYPE(marshall_character);
TEST_CASE_PROTOTYPE(marshall_s8);
TEST_CASE_PROTOTYPE(marshall_s16);
TEST_CASE_PROTOTYPE(marshall_s32);
TEST_CASE_PROTOTYPE(marshall_s64);
TEST_CASE_PROTOTYPE(marshall_sw);
TEST_CASE_PROTOTYPE(marshall_to_buf);
TEST_CASE_PROTOTYPE(marshall_to_file);
TEST_CASE_PROTOTYPE(marshall_to_str);
TEST_CASE_PROTOTYPE(marshall_u8);
TEST_CASE_PROTOTYPE(marshall_u16);
TEST_CASE_PROTOTYPE(marshall_u32);
TEST_CASE_PROTOTYPE(marshall_u64);
TEST_CASE_PROTOTYPE(marshall_uw);
TEST_CASE_PROTOTYPE(marshall_plist);
TEST_CASE_PROTOTYPE(marshall_plist_twice);
TEST_CASE_PROTOTYPE(marshall_str);
TEST_CASE_PROTOTYPE(marshall_tag);

void marshall_test (void)
{
  TEST_CASE_RUN(marshall_bool);
  TEST_CASE_RUN(marshall_character);
  TEST_CASE_RUN(marshall_s8);
  TEST_CASE_RUN(marshall_s16);
  TEST_CASE_RUN(marshall_s32);
  TEST_CASE_RUN(marshall_s64);
  TEST_CASE_RUN(marshall_sw);
  TEST_CASE_RUN(marshall_to_buf);
  TEST_CASE_RUN(marshall_to_str);
  TEST_CASE_RUN(marshall_to_file);
  TEST_CASE_RUN(marshall_u8);
  TEST_CASE_RUN(marshall_u16);
  TEST_CASE_RUN(marshall_u32);
  TEST_CASE_RUN(marshall_u64);
  TEST_CASE_RUN(marshall_uw);
  TEST_CASE_RUN(marshall_plist);
  TEST_CASE_RUN(marshall_plist_twice);
  TEST_CASE_RUN(marshall_str);
  TEST_CASE_RUN(marshall_tag);
}

TEST_CASE(marshall_bool)
{
  // MARSHALL_TEST_BOOL(bool
  MARSHALL_TEST(bool, false,
                "KC3MARSH"
                "\x00\x00\x00\x00\x00\x00\x00\x00"
                "\x00\x00\x00\x00\x00\x00\x00\x00"
                "\x01\x00\x00\x00\x00\x00\x00\x00"
                "\x00");
  MARSHALL_TEST(bool, true,
                "KC3MARSH"
                "\x00\x00\x00\x00\x00\x00\x00\x00"
                "\x00\x00\x00\x00\x00\x00\x00\x00"
                "\x01\x00\x00\x00\x00\x00\x00\x00"
                "\x01");
}
TEST_CASE_END(marshall_bool)

TEST_CASE(marshall_character)
{
  MARSHALL_TEST(character, 'A',
                "KC3MARSH"
                "\x00\x00\x00\x00\x00\x00\x00\x00"
                "\x00\x00\x00\x00\x00\x00\x00\x00"
                "\x01\x00\x00\x00\x00\x00\x00\x00"
                "A");
  MARSHALL_TEST(character, '\0',
                "KC3MARSH"
                "\x00\x00\x00\x00\x00\x00\x00\x00"
                "\x00\x00\x00\x00\x00\x00\x00\x00"
                "\x01\x00\x00\x00\x00\x00\x00\x00"
                "\x00");
  MARSHALL_TEST(character, 'Z',
                "KC3MARSH"
                "\x00\x00\x00\x00\x00\x00\x00\x00"
                "\x00\x00\x00\x00\x00\x00\x00\x00"
                "\x01\x00\x00\x00\x00\x00\x00\x00"
                "Z");
}
TEST_CASE_END(marshall_character)

TEST_CASE(marshall_plist)
{
  s_marshall m = {0};
  s_list *list_test;
  s_str str = {0};
  const s_str expected = STR_1("KC3MARSH"
                               "\x02\x00\x00\x00\x00\x00\x00\x00"
                               "\x16\x00\x00\x00\x00\x00\x00\x00"
                               "\x08\x00\x00\x00\x00\x00\x00\x00"
                               "\x11\x00\x16#\x00\x00\x00\x00"
                               "\x00\x00\x00\x11\x01\x16.\x00"
                               "\x00\x00\x00\x00\x00\x00 \x00"
                               "\x00\x00\x00\x00\x00\x00");

  TEST_ASSERT(marshall_init(&m));
  list_test = list_new_1("[0, 1]");
  TEST_ASSERT(list_test);
  TEST_ASSERT(marshall_plist(&m, false, list_test));
  marshall_to_str(&m, &str);
  TEST_STR_EQ(str, expected);
  str_clean(&str);
  list_delete_all(list_test);
  marshall_clean(&m);
}
TEST_CASE_END(marshall_plist)

TEST_CASE(marshall_plist_twice)
{
  s_marshall m = {0};
  s_list *list_test;
  s_str str = {0};
  const s_str expected = STR_1("KC3MARSH"
                               "\x04\x00\x00\x00\x00\x00\x00\x00"
                               ",\x00\x00\x00\x00\x00\x00\x00"
                               "\x10\x00\x00\x00\x00\x00\x00\x00"
                               "\x11\x00\x16#\x00\x00\x00\x00"
                               "\x00\x00\x00\x11\x01\x16.\x00"
                               "\x00\x00\x00\x00\x00\x00\x11\x00"
                               "\x16\x39\x00\x00\x00\x00\x00\x00"
                               "\x00\x11\x01\x16\x44\x00\x00\x00"
                               "\x00\x00\x00\x00 \x00\x00\x00"
                               "\x00\x00\x00\x00\x36\x00\x00\x00"
                               "\x00\x00\x00\x00");

  TEST_ASSERT(marshall_init(&m));
  list_test = list_new_1("[0, 1]");
  TEST_ASSERT(list_test);
  TEST_ASSERT(marshall_plist(&m, false, list_test));
  TEST_ASSERT(marshall_plist(&m, false, list_test));
  marshall_to_str(&m, &str);
  TEST_STR_EQ(str, expected);
  str_clean(&str);
  list_delete_all(list_test);
  marshall_clean(&m);
}
TEST_CASE_END(marshall_plist_twice)

TEST_CASE(marshall_s8)
{
  MARSHALL_TEST(s8, 0,
                "KC3MARSH"
                "\x00\x00\x00\x00\x00\x00\x00\x00"
                "\x00\x00\x00\x00\x00\x00\x00\x00"
                "\x01\x00\x00\x00\x00\x00\x00\x00"
                "\x00");
  MARSHALL_TEST(s8, 0xAB,
                "KC3MARSH"
                "\x00\x00\x00\x00\x00\x00\x00\x00"
                "\x00\x00\x00\x00\x00\x00\x00\x00"
                "\x01\x00\x00\x00\x00\x00\x00\x00"
                "\xAB");
}
TEST_CASE_END(marshall_s8)

TEST_CASE(marshall_s16)
{
  MARSHALL_TEST(s16, 0,
                "KC3MARSH"
                "\x00\x00\x00\x00\x00\x00\x00\x00"
                "\x00\x00\x00\x00\x00\x00\x00\x00"
                "\x02\x00\x00\x00\x00\x00\x00\x00"
                "\x00\x00");
  MARSHALL_TEST(s16, -1,
                "KC3MARSH"
                "\x00\x00\x00\x00\x00\x00\x00\x00"
                "\x00\x00\x00\x00\x00\x00\x00\x00"
                "\x02\x00\x00\x00\x00\x00\x00\x00"
                "\xFF\xFF");
}
TEST_CASE_END(marshall_s16)

TEST_CASE(marshall_s32)
{
  MARSHALL_TEST(s32, 0,
                "KC3MARSH"
                "\x00\x00\x00\x00\x00\x00\x00\x00"
                "\x00\x00\x00\x00\x00\x00\x00\x00"
                "\x04\x00\x00\x00\x00\x00\x00\x00"
                "\x00\x00\x00\x00");
  MARSHALL_TEST(s32, ~0,
                "KC3MARSH"
                "\x00\x00\x00\x00\x00\x00\x00\x00"
                "\x00\x00\x00\x00\x00\x00\x00\x00"
                "\x04\x00\x00\x00\x00\x00\x00\x00"
                "\xFF\xFF\xFF\xFF");
  MARSHALL_TEST(s32, 0xDEADBEEF,
                "KC3MARSH"
                "\x00\x00\x00\x00\x00\x00\x00\x00"
                "\x00\x00\x00\x00\x00\x00\x00\x00"
                "\x04\x00\x00\x00\x00\x00\x00\x00"
                "\xEF\xBE\xAD\xDE");
  MARSHALL_TEST(s32, 0xDEADCAFE,
                "KC3MARSH"
                "\x00\x00\x00\x00\x00\x00\x00\x00"
                "\x00\x00\x00\x00\x00\x00\x00\x00"
                "\x04\x00\x00\x00\x00\x00\x00\x00"
                "\xFE\xCA\xAD\xDE");
  MARSHALL_TEST(s32, 0xFEEDBABE,
                "KC3MARSH"
                "\x00\x00\x00\x00\x00\x00\x00\x00"
                "\x00\x00\x00\x00\x00\x00\x00\x00"
                "\x04\x00\x00\x00\x00\x00\x00\x00"
                "\xBE\xBA\xED\xFE");
}
TEST_CASE_END(marshall_s32)

TEST_CASE(marshall_s64)
{
  MARSHALL_TEST(s64, 0,
                "KC3MARSH"
                "\x00\x00\x00\x00\x00\x00\x00\x00"
                "\x00\x00\x00\x00\x00\x00\x00\x00"
                "\x08\x00\x00\x00\x00\x00\x00\x00"
                "\x00\x00\x00\x00\x00\x00\x00\x00");
  MARSHALL_TEST(s64, ~0,
                "KC3MARSH"
                "\x00\x00\x00\x00\x00\x00\x00\x00"
                "\x00\x00\x00\x00\x00\x00\x00\x00"
                "\x08\x00\x00\x00\x00\x00\x00\x00"
                "\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF");
}
TEST_CASE_END(marshall_s64)

TEST_CASE(marshall_str)
{
  MARSHALL_TEST_STR("",
                    "KC3MARSH"
                    "\x00\x00\x00\x00\x00\x00\x00\x00"
                    "\x00\x00\x00\x00\x00\x00\x00\x00"
                    "\x04\x00\x00\x00\x00\x00\x00\x00"
                    "\x00\x00\x00\x00");
  MARSHALL_TEST_STR("hello",
                    "KC3MARSH"
                    "\x00\x00\x00\x00\x00\x00\x00\x00"
                    "\x00\x00\x00\x00\x00\x00\x00\x00"
                    "\t\x00\x00\x00\x00\x00\x00\x00"
                    "\x05\x00\x00\x00hell"
                    "o");
  MARSHALL_TEST_STR("hello\xFF\x00world",
                    "KC3MARSH"
                    "\x00\x00\x00\x00\x00\x00\x00\x00"
                    "\x00\x00\x00\x00\x00\x00\x00\x00"
                    "\x10\x00\x00\x00\x00\x00\x00\x00"
                    "\x0C\x00\x00\x00hell"
                    "o\xFF\x00world");
  MARSHALL_TEST_STR("héllo wörld",
                    "KC3MARSH"
                    "\x00\x00\x00\x00\x00\x00\x00\x00"
                    "\x00\x00\x00\x00\x00\x00\x00\x00"
                    "\x11\x00\x00\x00\x00\x00\x00\x00"
                    "\r\x00\x00\x00héllo wörld");
}
TEST_CASE_END(marshall_str)

TEST_CASE(marshall_sw)
{
  MARSHALL_TEST(sw, 0,
                "KC3MARSH"
                "\x00\x00\x00\x00\x00\x00\x00\x00"
                "\x00\x00\x00\x00\x00\x00\x00\x00"
                "\x08\x00\x00\x00\x00\x00\x00\x00"
                "\x00\x00\x00\x00\x00\x00\x00\x00");
  MARSHALL_TEST(sw, ~0,
                "KC3MARSH"
                "\x00\x00\x00\x00\x00\x00\x00\x00"
                "\x00\x00\x00\x00\x00\x00\x00\x00"
                "\x08\x00\x00\x00\x00\x00\x00\x00"
                "\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF");
}
TEST_CASE_END(marshall_sw)

TEST_CASE(marshall_tag)
{
  // U8
  MARSHALL_TEST_TAG("0",
                    "KC3MARSH"
                    "\x00\x00\x00\x00\x00\x00\x00\x00"
                    "\x00\x00\x00\x00\x00\x00\x00\x00"
                    "\x02\x00\x00\x00\x00\x00\x00\x00"
                    "\x11\x00");
  MARSHALL_TEST_TAG("1",
                    "KC3MARSH"
                    "\x00\x00\x00\x00\x00\x00\x00\x00"
                    "\x00\x00\x00\x00\x00\x00\x00\x00"
                    "\x02\x00\x00\x00\x00\x00\x00\x00"
                    "\x11\x01");
  MARSHALL_TEST_TAG("2",
                    "KC3MARSH"
                    "\x00\x00\x00\x00\x00\x00\x00\x00"
                    "\x00\x00\x00\x00\x00\x00\x00\x00"
                    "\x02\x00\x00\x00\x00\x00\x00\x00"
                    "\x11\x02");
  // U16
  MARSHALL_TEST_TAG("256",
                    "KC3MARSH"
                    "\x00\x00\x00\x00\x00\x00\x00\x00"
                    "\x00\x00\x00\x00\x00\x00\x00\x00"
                    "\x03\x00\x00\x00\x00\x00\x00\x00"
                    "\x12\x00\x01");
  // S32
  MARSHALL_TEST_TAG("-2147483648",
                    "KC3MARSH"
                    "\x00\x00\x00\x00\x00\x00\x00\x00"
                    "\x00\x00\x00\x00\x00\x00\x00\x00"
                    "\x05\x00\x00\x00\x00\x00\x00\x00"
                    "\x0E\x00\x00\x00\x80");
  // U64
  MARSHALL_TEST_TAG("92233720368547",
                    "KC3MARSH"
                    "\x00\x00\x00\x00\x00\x00\x00\x00"
                    "\x00\x00\x00\x00\x00\x00\x00\x00"
                    "\t\x00\x00\x00\x00\x00\x00\x00"
                    "\x14\xA3\x8D#\xD6\xE2S\x00"
                    "\x00");
  // Integer
  MARSHALL_TEST_TAG("(Integer) 1024",
                    "KC3MARSH"
                    "\x00\x00\x00\x00\x00\x00\x00\x00"
                    "\x00\x00\x00\x00\x00\x00\x00\x00"
                    "\x0C\x00\x00\x00\x00\x00\x00\x00"
                    "\n\x03\x00\x00\x00\x00\x00\x00"
                    "\x00\x00\x04\x00");
  MARSHALL_TEST_TAG("10000000000000000000000000000000000",
                    "KC3MARSH"
                    "\x00\x00\x00\x00\x00\x00\x00\x00"
                    "\x00\x00\x00\x00\x00\x00\x00\x00"
                    "\x19\x00\x00\x00\x00\x00\x00\x00"
                    "\n\x10\x00\x00\x00\x00\x00\x00"
                    "\x00\x00\x01\xED\t\xBE\xAD\x87"
                    "\xC0\x37\x8D\x8E\x64\x00\x00\x00"
                    "\x00");
  // List
  MARSHALL_TEST_TAG("[1, 2]",
                    "KC3MARSH"
                    "\x02\x00\x00\x00\x00\x00\x00\x00"
                    "\x16\x00\x00\x00\x00\x00\x00\x00"
                    "\t\x00\x00\x00\x00\x00\x00\x00"
                    "\x11\x01\x16#\x00\x00\x00\x00"
                    "\x00\x00\x00\x11\x02\x16.\x00"
                    "\x00\x00\x00\x00\x00\x00\x16 "
                    "\x00\x00\x00\x00\x00\x00\x00");

  // Float
  MARSHALL_TEST_TAG("(F32) 3.141592643",
                    "KC3MARSH"
                    "\x00\x00\x00\x00\x00\x00\x00\x00"
                    "\x00\x00\x00\x00\x00\x00\x00\x00"
                    "\x05\x00\x00\x00\x00\x00\x00\x00"
                    "\x06\xDB\x0FI@");
  MARSHALL_TEST_TAG("(F64) 3.141592643",
                    "KC3MARSH"
                    "\x00\x00\x00\x00\x00\x00\x00\x00"
                    "\x00\x00\x00\x00\x00\x00\x00\x00"
                    "\t\x00\x00\x00\x00\x00\x00\x00"
                    "\x07JP\xD8R\xFB!"
                    "\t@");
  MARSHALL_TEST_TAG("(F128) 3.141592643",
                    "KC3MARSH"
                    "\x00\x00\x00\x00\x00\x00\x00\x00"
                    "\x00\x00\x00\x00\x00\x00\x00\x00"
                    "\x11\x00\x00\x00\x00\x00\x00\x00"
                    "\x08\xACN\x82\xC2\x96\xDA\x0F"
                    "\xC9\x00@\x00\x00\x00\x00\x00"
                    "\x00");
  // Do block
  MARSHALL_TEST_TAG("do\n"
                    "  1\n"
                    "  2\n"
                    "  3\n"
                    "end",
                    "KC3MARSH"
                    "\x00\x00\x00\x00\x00\x00\x00\x00"
                    "\x00\x00\x00\x00\x00\x00\x00\x00"
                    "\x10\x00\x00\x00\x00\x00\x00\x00"
                    "\x02\x03\x00\x00\x00\x00\x00\x00"
                    "\x00\x00\x11\x01\x11\x02\x11\x03");
  // Tuple
  MARSHALL_TEST_TAG("{1, 2, [3, 4]}",
                    "KC3MARSH"
                    "\x02\x00\x00\x00\x00\x00\x00\x00"
                    "\x16\x00\x00\x00\x00\x00\x00\x00"
                    "\x16\x00\x00\x00\x00\x00\x00\x00"
                    "\x11\x03\x16#\x00\x00\x00\x00"
                    "\x00\x00\x00\x11\x04\x16.\x00"
                    "\x00\x00\x00\x00\x00\x00%\x03"
                    "\x00\x00\x00\x00\x00\x00\x00\x11"
                    "\x01\x11\x02\x16 \x00\x00\x00"
                    "\x00\x00\x00\x00");
  // Psym
  MARSHALL_TEST_TAG("Module",
                    "KC3MARSH"
                    "\x00\x00\x00\x00\x00\x00\x00\x00"
                    "\x00\x00\x00\x00\x00\x00\x00\x00"
                    "\x0C\x00\x00\x00\x00\x00\x00\x00"
                    "\x1D\x01\x06\x00\x00\x00Mo"
                    "dule");
  // Map
  MARSHALL_TEST_TAG("%{a: 1, b: 2}",
                    "KC3MARSH"
                    "\x00\x00\x00\x00\x00\x00\x00\x00"
                    "\x00\x00\x00\x00\x00\x00\x00\x00"
                    "\x1B\x00\x00\x00\x00\x00\x00\x00"
                    "\x17\x02\x00\x00\x00\x00\x00\x00"
                    "\x00\x1D\x01\x01\x00\x00\x00\x61"
                    "\x11\x01\x1D\x01\x01\x00\x00\x00"
                    "\x62\x11\x02");

  MARSHALL_TEST_TAG("cfn Tag \"tag_add\" (Tag, Tag, Result)",
                    "KC3MARSH"
                    "\x04\x00\x00\x00\x00\x00\x00\x00"
                    "X\x00\x00\x00\x00\x00\x00\x00"
                    "\t\x00\x00\x00\x00\x00\x00\x00"
                    "\x01\x00\x00\x01\x03\x00\x00\x00"
                    "Tag\x01\x07\x00\x00"
                    "\x00tag_add"
                    "7\x00\x00\x00\x00\x00\x00\x00"
                    "\x1D\x01\x03\x00\x00\x00Ta"
                    "g\x16I\x00\x00\x00\x00\x00"
                    "\x00\x00\x1D\x01\x03\x00\x00\x00"
                    "Tag\x16[\x00\x00\x00"
                    "\x00\x00\x00\x00\x1D\x01\x06\x00"
                    "\x00\x00Result"
                    "\x16p\x00\x00\x00\x00\x00\x00"
                    "\x00\x18 \x00\x00\x00\x00\x00"
                    "\x00\x00");

  // Call
  MARSHALL_TEST_TAG("defmodule Test do end",
                    "KC3MARSH"
                    "\x00\x00\x00\x00\x00\x00\x00\x00"
                    "\x00\x00\x00\x00\x00\x00\x00\x00"
                    "\x34\x00\x00\x00\x00\x00\x00\x00"
                    "\x04\x00\x01\t\x00\x00\x00\x64"
                    "efmodule"
                    "\x02\x00\x00\x00\x00\x00\x00\x00"
                    "\x1D\x01\x04\x00\x00\x00Te"
                    "st\x02\x00\x00\x00\x00\x00"
                    "\x00\x00\x00\x00 \x00\x00\x00"
                    "\x00\x00\x00\x00");
  MARSHALL_TEST_TAG("List.reverse([])",
                    "KC3MARSH"
                    "\x00\x00\x00\x00\x00\x00\x00\x00"
                    "\x00\x00\x00\x00\x00\x00\x00\x00"
                    "/\x00\x00\x00\x00\x00\x00\x00"
                    "\x04\x01\x04\x00\x00\x00Li"
                    "st\x01\x07\x00\x00\x00r"
                    "everse\x01\x00"
                    "\x00\x00\x00\x00\x00\x00\x16 "
                    "\x00\x00\x00\x00\x00\x00\x00 "
                    "\x00\x00\x00\x00\x00\x00\x00");
  MARSHALL_TEST_TAG("a = 1",
                    "KC3MARSH"
                    "\x00\x00\x00\x00\x00\x00\x00\x00"
                    "\x00\x00\x00\x00\x00\x00\x00\x00"
                    "\"\x00\x00\x00\x00\x00\x00\x00"
                    "\x04\x00\x01\x01\x00\x00\x00="
                    "\x02\x00\x00\x00\x00\x00\x00\x00"
                    "'\x00\x01\x01\x00\x00\x00\x61"
                    "\x11\x01 \x00\x00\x00\x00\x00"
                    "\x00\x00");

  MARSHALL_TEST_TAG("fn (x) { x * 2 }",
                    "KC3MARSH"
                    "\x02\x00\x00\x00\x00\x00\x00\x00"
                    "B\x00\x00\x00\x00\x00\x00\x00"
                    "\t\x00\x00\x00\x00\x00\x00\x00"
                    "\x02\x00\x00\x00\x00\x00'\x00"
                    "\x01\x01\x00\x00\x00x\x16/"
                    "\x00\x00\x00\x00\x00\x00\x00\x01"
                    "\x00\x00\x00\x00\x00\x00\x00\x01"
                    "\x04\x00\x01\x01\x00\x00\x00*"
                    "\x02\x00\x00\x00\x00\x00\x00\x00"
                    "'\x00\x01\x01\x00\x00\x00x"
                    "\x11\x02Z\x00\x00\x00\x00\x00"
                    "\x00\x00\x18 \x00\x00\x00\x00"
                    "\x00\x00\x00");
  // Ratio
  MARSHALL_TEST_TAG("2/6",
                    "KC3MARSH"
                    "\x00\x00\x00\x00\x00\x00\x00\x00"
                    "\x00\x00\x00\x00\x00\x00\x00\x00"
                    "\x15\x00\x00\x00\x00\x00\x00\x00"
                    "\v\x02\x00\x00\x00\x00\x00\x00"
                    "\x00\x00\x02\x02\x00\x00\x00\x00"
                    "\x00\x00\x00\x00\x06");
  // Quote
  MARSHALL_TEST_TAG("quote 1 + 1",
                    "KC3MARSH"
                    "\x00\x00\x00\x00\x00\x00\x00\x00"
                    "\x00\x00\x00\x00\x00\x00\x00\x00"
                    "\x1D\x00\x00\x00\x00\x00\x00\x00"
                    "\"\x04\x00\x01\x01\x00\x00\x00"
                    "+\x02\x00\x00\x00\x00\x00\x00"
                    "\x00\x11\x01\x11\x01 \x00\x00"
                    "\x00\x00\x00\x00\x00");
  // Var
  MARSHALL_TEST_TAG("(U8) ?",
                    "KC3MARSH"
                    "\x00\x00\x00\x00\x00\x00\x00\x00"
                    "\t\x00\x00\x00\x00\x00\x00\x00"
                    "\t\x00\x00\x00\x00\x00\x00\x00"
                    "\x00\x01\x02\x00\x00\x00U8"
                    "\x00! \x00\x00\x00\x00\x00"
                    "\x00\x00");
  MARSHALL_TEST_TAG("(Tag) ?",
                    "KC3MARSH"
                    "\x00\x00\x00\x00\x00\x00\x00\x00"
                    "\n\x00\x00\x00\x00\x00\x00\x00"
                    "\t\x00\x00\x00\x00\x00\x00\x00"
                    "\x00\x01\x03\x00\x00\x00Ta"
                    "g\x00! \x00\x00\x00\x00"
                    "\x00\x00\x00");
  // Complex
  MARSHALL_TEST_TAG("1 +i 2",
                    "KC3MARSH"
                    "\x00\x00\x00\x00\x00\x00\x00\x00"
                    "\x00\x00\x00\x00\x00\x00\x00\x00"
                    "\x1D\x00\x00\x00\x00\x00\x00\x00"
                    "\x04\x00\x01\x02\x00\x00\x00+"
                    "i\x02\x00\x00\x00\x00\x00\x00"
                    "\x00\x11\x01\x11\x02 \x00\x00"
                    "\x00\x00\x00\x00\x00");
  // Unquote
  MARSHALL_TEST_TAG("unquote(123)",
                    "KC3MARSH"
                    "\x00\x00\x00\x00\x00\x00\x00\x00"
                    "\x00\x00\x00\x00\x00\x00\x00\x00"
                    "\x03\x00\x00\x00\x00\x00\x00\x00"
                    "&\x11{");
  MARSHALL_TEST_TAG("%Time{}",
                    "KC3MARSH"
                    "\x00\x00\x00\x00\x00\x00\x00\x00"
                    "\x00\x00\x00\x00\x00\x00\x00\x00"
                    "\x12\x00\x00\x00\x00\x00\x00\x00"
                    "$\x00\x00\x00\x00\x00\x00\x00"
                    "\x00\x00\x00\x00\x00\x00\x00\x00"
                    "\x00\x00");
  MARSHALL_TEST_TAG("%Time{tv_sec: 1000, tv_nsec: 2000}",
                    "KC3MARSH"
                    "\x00\x00\x00\x00\x00\x00\x00\x00"
                    "\x00\x00\x00\x00\x00\x00\x00\x00"
                    "\x12\x00\x00\x00\x00\x00\x00\x00"
                    "$\x00\xE8\x03\x00\x00\x00\x00"
                    "\x00\x00\xD0\x07\x00\x00\x00\x00"
                    "\x00\x00");
  MARSHALL_TEST_TAG("%Time{tv_sec: 1 + 1, tv_nsec: 2 + 2}", "");
}
TEST_CASE_END(marshall_tag)

TEST_CASE(marshall_to_buf)
{
  char b[1024024] = {0};
  s_buf buf = {0};
  s_marshall m = {0};
  buf_init(&buf, false, sizeof(b), b);
  TEST_ASSERT(marshall_init(&m) != NULL);
  TEST_EQ(marshall_to_buf(&m, &buf), sizeof(s_marshall_header));
  marshall_clean(&m);
}
TEST_CASE_END(marshall_to_buf)

TEST_CASE(marshall_to_file)
{
  u32 value = 42;
  s_marshall m = {0};
  s_str path = {0};
  TEST_ASSERT(marshall_init(&m));
  TEST_ASSERT(marshall_u32(&m, false, value));
  TEST_EQ(marshall_to_file(&m, ".marshall_test_to_file.1.kc3m"),
          sizeof(s_marshall_header) +
          sizeof(u32));
  TEST_EQ(test_file_compare(".marshall_test_to_file.1.kc3m",
                            "marshall_test_to_file.1.expected.kc3m"),
          0);
  TEST_EQ(str_init_1(&path, NULL, ".marshall_test_to_file.1.kc3m"),
          &path);
  TEST_ASSERT(file_unlink(&path));
  str_clean(&path);
  marshall_clean(&m);
}
TEST_CASE_END(marshall_to_file)

TEST_CASE(marshall_to_str)
{
  u32 value = 424242;
  s_marshall m = {0};
  s_str str = {0};
  s_str expected = {0};
  TEST_ASSERT(marshall_init(&m));
  TEST_ASSERT(marshall_u32(&m, false, value));
  TEST_EQ(marshall_to_str(&m, &str), &str);
  marshall_clean(&m);
  expected = STR_1("KC3MARSH"
                   "\x00\x00\x00\x00\x00\x00\x00\x00"
                   "\x00\x00\x00\x00\x00\x00\x00\x00"
                   "\x04\x00\x00\x00\x00\x00\x00\x00"
                   "\x32\x79\x06\x00");
  TEST_STR_EQ(str, expected);
  str_clean(&str);
}
TEST_CASE_END(marshall_to_str)

TEST_CASE(marshall_u8)
{
  MARSHALL_TEST(u8, 0x00,
                "KC3MARSH"
                "\x00\x00\x00\x00\x00\x00\x00\x00"
                "\x00\x00\x00\x00\x00\x00\x00\x00"
                "\x01\x00\x00\x00\x00\x00\x00\x00"
                "\x00");
  MARSHALL_TEST(u8, 0xFF,
                "KC3MARSH"
                "\x00\x00\x00\x00\x00\x00\x00\x00"
                "\x00\x00\x00\x00\x00\x00\x00\x00"
                "\x01\x00\x00\x00\x00\x00\x00\x00"
                "\xFF");
}
TEST_CASE_END(marshall_u8)

TEST_CASE(marshall_u16)
{
  MARSHALL_TEST(u16, 0xFFFF,
                "KC3MARSH"
                "\x00\x00\x00\x00\x00\x00\x00\x00"
                "\x00\x00\x00\x00\x00\x00\x00\x00"
                "\x02\x00\x00\x00\x00\x00\x00\x00"
                "\xFF\xFF");
}

TEST_CASE_END(marshall_u16)

TEST_CASE(marshall_u32)
{
  MARSHALL_TEST(u32, 0xFFFFFFFF,
                "KC3MARSH"
                "\x00\x00\x00\x00\x00\x00\x00\x00"
                "\x00\x00\x00\x00\x00\x00\x00\x00"
                "\x04\x00\x00\x00\x00\x00\x00\x00"
                "\xFF\xFF\xFF\xFF");
}
TEST_CASE_END(marshall_u32)

TEST_CASE(marshall_u64)
{
  MARSHALL_TEST(u64, 0xFFFFFFFFFFFFFFFF,
                "KC3MARSH"
                "\x00\x00\x00\x00\x00\x00\x00\x00"
                "\x00\x00\x00\x00\x00\x00\x00\x00"
                "\x08\x00\x00\x00\x00\x00\x00\x00"
                "\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF");
}
TEST_CASE_END(marshall_u64)

TEST_CASE(marshall_uw)
{
  MARSHALL_TEST(uw, 0,
                "KC3MARSH"
                "\x00\x00\x00\x00\x00\x00\x00\x00"
                "\x00\x00\x00\x00\x00\x00\x00\x00"
                "\x08\x00\x00\x00\x00\x00\x00\x00"
                "\x00\x00\x00\x00\x00\x00\x00\x00");
  MARSHALL_TEST(uw, ~0,
                "KC3MARSH"
                "\x00\x00\x00\x00\x00\x00\x00\x00"
                "\x00\x00\x00\x00\x00\x00\x00\x00"
                "\x08\x00\x00\x00\x00\x00\x00\x00"
                "\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF");
}
TEST_CASE_END(marshall_uw)

