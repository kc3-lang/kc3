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
#include "../libkc3/inspect.h"
#include "../libkc3/marshall.h"
#include "../libkc3/marshall_read.h"
#include "../libkc3/str.h"
#include "../libkc3/list.h"
#include "../libkc3/tag.h"
#include "../libkc3/tag_init.h"
#include "test.h"
#include "tag_test.h"

#define MARSHALL_READ_TEST(type, on_heap, test, expected)              \
  do {                                                                 \
    type tmp = {0};                                                    \
    s_str tmp_str = {{0}, sizeof(expected) - 1, {expected}};           \
    s_marshall_read m = {0};                                           \
    s_str test_str = {0};                                              \
    test_context("marshall_read_" # type "(" # on_heap ", " # test     \
                 ") -> " # expected);                                  \
    TEST_EQ(marshall_read_init_1(&m, (test), sizeof(test) - 1), &m);   \
    TEST_EQ(marshall_read_## type (&m, (on_heap), &tmp), &m);          \
    marshall_read_clean(&m);                                           \
    inspect_ ## type(&tmp, &test_str);                                 \
    TEST_STR_EQ(test_str, tmp_str);                                    \
    test_context(NULL);                                                \
  } while (0)

#define MARSHALL_READ_TEST_TAG(test, on_heap)                          \
  do {                                                                 \
    s_marshall_read m = {0};                                           \
    s_tag tag = {0};                                                   \
    tag_init_1(&tag, test);                                            \
    TEST_EQ(marshall_read_init_str(&m, (test)), &m);                   \
    TEST_EQ(marshall_read_tag(&m, (on_heap), &tag), &m);               \
    marhall_read_clean(&m);                                            \
    tag_clean(&tag);                                                   \
  } while (0)

void marshal_test (void);

TEST_CASE_PROTOTYPE(marshall_read_bool);
TEST_CASE_PROTOTYPE(marshall_read_character);
TEST_CASE_PROTOTYPE(marshall_read_s8);
TEST_CASE_PROTOTYPE(marshall_read_s16);
TEST_CASE_PROTOTYPE(marshall_read_s32);
TEST_CASE_PROTOTYPE(marshall_read_s64);
TEST_CASE_PROTOTYPE(marshall_read_sw);
TEST_CASE_PROTOTYPE(marshall_read_init_buf);
TEST_CASE_PROTOTYPE(marshall_read_init_file);
TEST_CASE_PROTOTYPE(marshall_read_init_str);
TEST_CASE_PROTOTYPE(marshall_read_u8);
TEST_CASE_PROTOTYPE(marshall_read_u16);
TEST_CASE_PROTOTYPE(marshall_read_u32);
TEST_CASE_PROTOTYPE(marshall_read_u64);
TEST_CASE_PROTOTYPE(marshall_read_uw);
TEST_CASE_PROTOTYPE(marshall_read_plist);

void marshall_read_test (void)
{
  TEST_CASE_RUN(marshall_read_bool);
  /*
  TEST_CASE_RUN(marshall_read_character);
  TEST_CASE_RUN(marshall_read_s8);
  TEST_CASE_RUN(marshall_read_s16);
  TEST_CASE_RUN(marshall_read_s32);
  TEST_CASE_RUN(marshall_read_s64);
  TEST_CASE_RUN(marshall_read_sw);
  TEST_CASE_RUN(marshall_read_u8);
  TEST_CASE_RUN(marshall_read_u16);
  TEST_CASE_RUN(marshall_read_u32);
  TEST_CASE_RUN(marshall_read_u64);
  TEST_CASE_RUN(marshall_read_uw);
  TEST_CASE_RUN(marshall_read_plist);
  */
}

TEST_CASE(marshall_read_bool)
{
  MARSHALL_READ_TEST(bool, false,
                     "KC3MARSH\0\0\0\0\0\0\0\0"
                     "\0\0\0\0\0\0\0\0"
                     "\x01\0\0\0\0\0\0\0"
                     "\0",
                     "false");
  MARSHALL_READ_TEST(bool, false,
                     "KC3MARSH\0\0\0\0\0\0\0\0"
                     "\0\0\0\0\0\0\0\0"
                     "\x01\0\0\0\0\0\0\0"
                     "\0",
                     "true");
  MARSHALL_READ_TEST(bool, true,
                     "KC3MARSH\0\0\0\0\0\0\0\0"
                     "\x01\0\0\0\0\0\0\0"
                     "\0\0\0\0\0\0\0\0"
                     "\0",
                     "false");
  MARSHALL_READ_TEST(bool, true,
                     "KC3MARSH\0\0\0\0\0\0\0\0"
                     "\x01\0\0\0\0\0\0\0"
                     "\0\0\0\0\0\0\0\0"
                     "\x01",
                     "true");
}
TEST_CASE_END(marshall_read_bool)

TEST_CASE(marshall_read_plist)
{
  s_marshall_read mr = {0};
  s_tag expected = {0};
  s_tag test_tag = {0};
  const char test_str[] = "KC3MARSH\x02\0\0\0\0\0\0\0"
                          "\x13\0\0\0\0\0\0\0"
                          "\0\0\0\0\0\0\0\0"
                          "\0\0\0\0\0\0\0\0"
                          "\x13\0\x18\0\0\0\0\0"
                          "\0\0\0";
  TEST_EQ(marshall_read_init_1(&mr, test_str, sizeof(test_str) - 1),
          &mr);
  TEST_EQ(marshall_read_plist(&mr, true, &test_tag.data.plist), &mr);
  test_tag.type = TAG_LIST;
  tag_init_plist_1(&expected, "[0, 1]");
  TAG_TEST_EQ(&test_tag, &expected);
  tag_clean(&test_tag);
  tag_clean(&expected);
}
TEST_CASE_END(marshall_read_plist)

/* if 0

TEST_CASE(marshall_read_character)
{
  MARSHALL_READ_TEST(character, 'A', "A");
  MARSHALL_READ_TEST(character, '\0', "\x00");
  MARSHALL_READ_TEST(character, 'Z', "Z");
  MARSHALL_READ_TEST(character, 'A', "A");
  MARSHALL_READ_TEST(character, '\0', "\x00");
  MARSHALL_READ_TEST(character, 'Z', "Z");
}
TEST_CASE_END(marshall_read_character)


TEST_CASE(marshall_read_s8)
{
  MARSHALL_READ_TEST(s8, 0, "\0");
  MARSHALL_READ_TEST(s8, 0xAB, "\xAB");
  MARSHALL_READ_TEST(s8, 0, "\0");
  MARSHALL_READ_TEST(s8, 0xAB, "\xAB");
}
TEST_CASE_END(marshall_read_s8)

TEST_CASE(marshall_read_s16)
{
  MARSHALL_READ_TEST(s16, 0, "\0\0");
  MARSHALL_READ_TEST(s16, -1, "\xFF\xFF");
  MARSHALL_READ_TEST(s16, 0, "\0\0");
  MARSHALL_READ_TEST(s16, -1, "\xFF\xFF");
}
TEST_CASE_END(marshall_read_s16)

TEST_CASE(marshall_read_s32)
{
  MARSHALL_READ_TEST(s32, 0, "\0\0\0\0");
  MARSHALL_READ_TEST(s32, ~0, "\xFF\xFF\xFF\xFF");
  MARSHALL_READ_TEST(s32, 0xDEADBEEF, "\xEF\xBE\xAD\xDE");
  MARSHALL_READ_TEST(s32, 0xDEADCAFE, "\xFE\xCA\xAD\xDE");
  MARSHALL_READ_TEST(s32, 0xFEEDBABE, "\xBE\xBA\xED\xFE");
  MARSHALL_READ_TEST(s32, 0, "\0\0\0\0");
  MARSHALL_READ_TEST(s32, ~0, "\xFF\xFF\xFF\xFF");
  MARSHALL_READ_TEST(s32, 0xDEADBEEF, "\xEF\xBE\xAD\xDE");
  MARSHALL_READ_TEST(s32, 0xDEADCAFE, "\xFE\xCA\xAD\xDE");
  MARSHALL_READ_TEST(s32, 0xFEEDBABE, "\xBE\xBA\xED\xFE");
}
TEST_CASE_END(marshall_read_s32)

TEST_CASE(marshall_read_s64)
{
  MARSHALL_READ_TEST(s64, 0,
                                 "\0\0\0\0\0\0\0\0");
  MARSHALL_READ_TEST(s64, ~0,
                                 "\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF");
  MARSHALL_READ_TEST(s64, 0,
                                 "\0\0\0\0\0\0\0\0");
  MARSHALL_READ_TEST(s64, ~0,
                                 "\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF");
}
TEST_CASE_END(marshall_read_s64)

TEST_CASE(marshall_read_sw)
{
  MARSHALL_READ_TEST(s64, 0,
                                 "\0\0\0\0\0\0\0\0");
  MARSHALL_READ_TEST(s64, ~0,
                                 "\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF");
  MARSHALL_READ_TEST(s64, 0,
                                 "\0\0\0\0\0\0\0\0");
  MARSHALL_READ_TEST(s64, ~0,
                                 "\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF");
}
TEST_CASE_END(marshall_read_sw)

TEST_CASE(marshall_read_tag)
{
  MARSHALL_READ_TEST_TAG("0");
  MARSHALL_READ_TEST_TAG("1");
  MARSHALL_READ_TEST_TAG("2");
  MARSHALL_READ_TEST_TAG("256");
  MARSHALL_READ_TEST_TAG("-2147483648");
  MARSHALL_READ_TEST_TAG("92233720368547");
  MARSHALL_READ_TEST_TAG("100000000000000000000000000000");
  MARSHALL_READ_TEST_TAG("[1, 2]");
  MARSHALL_READ_TEST_TAG("defmodule Test do end");
  MARSHALL_READ_TEST_TAG("%{a: 1, b: 2}");
  MARSHALL_READ_TEST_TAG("List.reverse([])");
  MARSHALL_READ_TEST_TAG("a = 1");
}
TEST_CASE_END(marshall_read_tag)

TEST_CASE(marshall_read_u8)
{
  MARSHALL_READ_TEST(u8, 0x00, "\x00");
  MARSHALL_READ_TEST(u8, 0xFF, "\xFF");
  MARSHALL_READ_TEST(u8, 0x00, "\x00");
  MARSHALL_READ_TEST(u8, 0xFF, "\xFF");
}
TEST_CASE_END(marshall_read_u8)

TEST_CASE(marshall_read_u16)
{
  MARSHALL_READ_TEST(u16, 0xFFFF, "\xFF\xFF");
  MARSHALL_READ_TEST(u16, 0xFFFF, "\xFF\xFF");
}
TEST_CASE_END(marshall_read_u16)

TEST_CASE(marshall_read_u32)
{
  MARSHALL_READ_TEST(u32, 0xFFFFFFFF, "\xFF\xFF\xFF\xFF");
  MARSHALL_READ_TEST(u32, 0xFFFFFFFF, "\xFF\xFF\xFF\xFF");
}
TEST_CASE_END(marshall_read_u32)

TEST_CASE(marshall_read_u64)
{
  U64(0xFFFFFFFFFFFFFFFF,
    "\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF");
  U64(0xFFFFFFFFFFFFFFFF,
    "\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF");
}
TEST_CASE_END(marshall_read_u64)

TEST_CASE(marshall_read_uw)
{
  MARSHALL_READ_TEST(uw, 0, "\0\0\0\0\0\0\0\0");
  MARSHALL_READ_TEST(uw, ~0,
                                  "\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF");
  MARSHALL_READ_TEST(uw, 0, "\0\0\0\0\0\0\0\0");
  MARSHALL_READ_TEST(uw, ~0, "\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF");
}
TEST_CASE_END(marshall_read_uw)
*/
