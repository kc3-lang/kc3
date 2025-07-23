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
#include <endian.h>
#include "../libkc3/file.h"
#include "../libkc3/marshall.h"
#include "../libkc3/marshall_read.h"
#include "../libkc3/str.h"
#include "../libkc3/list.h"
#include "../libkc3/tag.h"
#include "test.h"

#define MARSHALL_READ_TEST(type, on_heap, test, expected)              \
  do {                                                                 \
    s_marshall_read m = {0};                                           \
    s_buf *buf = NULL;                                                 \
    TEST_ASSERT(marshall_read_init(&m));                               \
    TEST_ASSERT(marshall_read_## type (&m,                             \
                                       on_heap, (type) (test)));       \
    buf = (on_heap) ? &m.heap : &m.buf;                                \
    TEST_MEM_EQ(buf->ptr.pu8, sizeof(type),                            \
      (expected), sizeof(expected) - 1);                               \
    marshall_clean(&m);                                                \
  } while (0);

#define MARSHALL_READ_TEST_BUF_AND_HEAP(type, test, expected)  \
  MARSHALL_READ_TEST(type, false, test, expected)              \
  MARSHALL_READ_TEST(type, true, test, expected)


#define MARSHALL_READ_TEST_TAG(test, on_heap)                          \
  do {                                                                 \
    s_marshall_read m = {0};                                           \
    s_tag tag = {0};                                                   \
    tag_init_1(&tag, test);                                            \
    TEST_EQ(marshall_read_init(&m), &m);                               \
    TEST_EQ(marshall_read_tag(&m, (on_heap), &tag), &m);               \
    marshall_clean(&m);                                                \
    tag_clean(&tag);                                                   \
  } while (0)

#define MARSHALL_READ_TEST_TAG_BUF_AND_HEAP(test)                      \
  MARSHALL_READ_TEST_TAG(test, true);                                  \
  MARSHALL_READ_TEST_TAG(test, false)

void marshal_test (void);

TEST_CASE_PROTOTYPE(marshall_read_bool);
TEST_CASE_PROTOTYPE(marshall_read_character);
TEST_CASE_PROTOTYPE(marshall_read_s8);
TEST_CASE_PROTOTYPE(marshall_read_s16);
TEST_CASE_PROTOTYPE(marshall_read_s32);
TEST_CASE_PROTOTYPE(marshall_read_s64);
TEST_CASE_PROTOTYPE(marshall_read_sw);
TEST_CASE_PROTOTYPE(marshall_read_to_buf);
TEST_CASE_PROTOTYPE(marshall_read_to_file);
TEST_CASE_PROTOTYPE(marshall_read_to_str);
TEST_CASE_PROTOTYPE(marshall_read_u8);
TEST_CASE_PROTOTYPE(marshall_read_u16);
TEST_CASE_PROTOTYPE(marshall_read_u32);
TEST_CASE_PROTOTYPE(marshall_read_u64);
TEST_CASE_PROTOTYPE(marshall_read_uw);
TEST_CASE_PROTOTYPE(marshall_read_plist);

void marshall_read_test (void)
{
  TEST_CASE_RUN(marshall_read_bool);
  TEST_CASE_RUN(marshall_read_character);
  TEST_CASE_RUN(marshall_read_s8);
  TEST_CASE_RUN(marshall_read_s16);
  TEST_CASE_RUN(marshall_read_s32);
  TEST_CASE_RUN(marshall_read_s64);
  TEST_CASE_RUN(marshall_read_sw);
  TEST_CASE_RUN(marshall_read_to_buf);
  TEST_CASE_RUN(marshall_read_to_str);
  TEST_CASE_RUN(marshall_read_to_file);
  TEST_CASE_RUN(marshall_read_u8);
  TEST_CASE_RUN(marshall_read_u16);
  TEST_CASE_RUN(marshall_read_u32);
  TEST_CASE_RUN(marshall_read_u64);
  TEST_CASE_RUN(marshall_read_uw);
  TEST_CASE_RUN(marshall_read_plist);
}

TEST_CASE(marshall_read_bool)
{
  MARSHALL_READ_TEST_BUF_AND_HEAP(bool, false, "\x00");
  MARSHALL_READ_TEST_BUF_AND_HEAP(bool, true, "\x01");
  MARSHALL_READ_TEST_BUF_AND_HEAP(bool, false, "\x00");
  MARSHALL_READ_TEST_BUF_AND_HEAP(bool, true, "\x01");
}
TEST_CASE_END(marshall_read_bool)

TEST_CASE(marshall_read_character)
{
  MARSHALL_READ_TEST_BUF_AND_HEAP(character, 'A', "A");
  MARSHALL_READ_TEST_BUF_AND_HEAP(character, '\0', "\x00");
  MARSHALL_READ_TEST_BUF_AND_HEAP(character, 'Z', "Z");
  MARSHALL_READ_TEST_BUF_AND_HEAP(character, 'A', "A");
  MARSHALL_READ_TEST_BUF_AND_HEAP(character, '\0', "\x00");
  MARSHALL_READ_TEST_BUF_AND_HEAP(character, 'Z', "Z");
}
TEST_CASE_END(marshall_read_character)

TEST_CASE(marshall_read_plist)
{
  s_marshall_read m = {0};
  s_list *list_test;
  s_str str = {0};
  const s_str expected =
    {{0}, 51, {"KC3MARSH\x02\0\0\0\0\0\0\0"
               "\x13\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
               "\0\0\0\0\0\0\0\0\x13\0\x18\0\0\0\0\0"
               "\0\0\0"}};

  TEST_ASSERT(marshall_read_init(&m));
  list_test = list_new_1("[0, 1]");
  TEST_ASSERT(list_test);
  TEST_ASSERT(marshall_read_plist(&m, true, &list_test));
  marshall_read_to_str(&m, &str);
  TEST_STR_EQ(str, expected);
}
TEST_CASE_END(marshall_read_plist)

TEST_CASE(marshall_read_s8)
{
  MARSHALL_READ_TEST_BUF_AND_HEAP(s8, 0, "\0");
  MARSHALL_READ_TEST_BUF_AND_HEAP(s8, 0xAB, "\xAB");
  MARSHALL_READ_TEST_BUF_AND_HEAP(s8, 0, "\0");
  MARSHALL_READ_TEST_BUF_AND_HEAP(s8, 0xAB, "\xAB");
}
TEST_CASE_END(marshall_read_s8)

TEST_CASE(marshall_read_s16)
{
  MARSHALL_READ_TEST_BUF_AND_HEAP(s16, 0, "\0\0");
  MARSHALL_READ_TEST_BUF_AND_HEAP(s16, -1, "\xFF\xFF");
  MARSHALL_READ_TEST_BUF_AND_HEAP(s16, 0, "\0\0");
  MARSHALL_READ_TEST_BUF_AND_HEAP(s16, -1, "\xFF\xFF");
}
TEST_CASE_END(marshall_read_s16)

TEST_CASE(marshall_read_s32)
{
  MARSHALL_READ_TEST_BUF_AND_HEAP(s32, 0, "\0\0\0\0");
  MARSHALL_READ_TEST_BUF_AND_HEAP(s32, ~0, "\xFF\xFF\xFF\xFF");
  MARSHALL_READ_TEST_BUF_AND_HEAP(s32, 0xDEADBEEF, "\xEF\xBE\xAD\xDE");
  MARSHALL_READ_TEST_BUF_AND_HEAP(s32, 0xDEADCAFE, "\xFE\xCA\xAD\xDE");
  MARSHALL_READ_TEST_BUF_AND_HEAP(s32, 0xFEEDBABE, "\xBE\xBA\xED\xFE");
  MARSHALL_READ_TEST_BUF_AND_HEAP(s32, 0, "\0\0\0\0");
  MARSHALL_READ_TEST_BUF_AND_HEAP(s32, ~0, "\xFF\xFF\xFF\xFF");
  MARSHALL_READ_TEST_BUF_AND_HEAP(s32, 0xDEADBEEF, "\xEF\xBE\xAD\xDE");
  MARSHALL_READ_TEST_BUF_AND_HEAP(s32, 0xDEADCAFE, "\xFE\xCA\xAD\xDE");
  MARSHALL_READ_TEST_BUF_AND_HEAP(s32, 0xFEEDBABE, "\xBE\xBA\xED\xFE");
}
TEST_CASE_END(marshall_read_s32)

TEST_CASE(marshall_read_s64)
{
  MARSHALL_READ_TEST_BUF_AND_HEAP(s64, 0,
                                 "\0\0\0\0\0\0\0\0");
  MARSHALL_READ_TEST_BUF_AND_HEAP(s64, ~0,
                                 "\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF");
  MARSHALL_READ_TEST_BUF_AND_HEAP(s64, 0,
                                 "\0\0\0\0\0\0\0\0");
  MARSHALL_READ_TEST_BUF_AND_HEAP(s64, ~0,
                                 "\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF");
}
TEST_CASE_END(marshall_read_s64)

TEST_CASE(marshall_read_sw)
{
  MARSHALL_READ_TEST_BUF_AND_HEAP(s64, 0,
                                 "\0\0\0\0\0\0\0\0");
  MARSHALL_READ_TEST_BUF_AND_HEAP(s64, ~0,
                                 "\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF");
  MARSHALL_READ_TEST_BUF_AND_HEAP(s64, 0,
                                 "\0\0\0\0\0\0\0\0");
  MARSHALL_READ_TEST_BUF_AND_HEAP(s64, ~0,
                                 "\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF");
}
TEST_CASE_END(marshall_read_sw)

TEST_CASE(marshall_read_tag)
{
  MARSHALL_READ_TEST_TAG_BUF_AND_HEAP("0");
  MARSHALL_READ_TEST_TAG_BUF_AND_HEAP("1");
  MARSHALL_READ_TEST_TAG_BUF_AND_HEAP("2");
  MARSHALL_READ_TEST_TAG_BUF_AND_HEAP("256");
  MARSHALL_READ_TEST_TAG_BUF_AND_HEAP("-2147483648");
  MARSHALL_READ_TEST_TAG_BUF_AND_HEAP("92233720368547");
  MARSHALL_READ_TEST_TAG_BUF_AND_HEAP("100000000000000000000000000000");
  MARSHALL_READ_TEST_TAG_BUF_AND_HEAP("[1, 2]");
  MARSHALL_READ_TEST_TAG_BUF_AND_HEAP("defmodule Test do end");
  MARSHALL_READ_TEST_TAG_BUF_AND_HEAP("%{a: 1, b: 2}");
  MARSHALL_READ_TEST_TAG_BUF_AND_HEAP("List.reverse([])");
  MARSHALL_READ_TEST_TAG_BUF_AND_HEAP("a = 1");
}
TEST_CASE_END(marshall_read_tag)

TEST_CASE(marshall_read_to_buf)
{
  char b[1024024] = {0};
  s_buf buf = {0};
  s_marshall_read m = {0};
  buf_init(&buf, false, sizeof(b), b);
  TEST_ASSERT(marshall_read_init(&m) != NULL);
  TEST_EQ(marshall_read_to_buf(&m, &buf), sizeof(s_marshall_read));
  marshall_clean(&m);
}
TEST_CASE_END(marshall_read_to_buf)

TEST_CASE(marshall_read_to_file)
{
  u32 value = 42;
  s_marshall_read m = {0};
  s_str path = {0};
  TEST_ASSERT(marshall_read_init(&m));
  TEST_ASSERT(marshall_read_u32(&m, false, value));
  TEST_EQ(marshall_read_to_file(&m, ".marshall_read_test_to_file.1.kc3m"),
          sizeof(s_marshall_read) +
          sizeof(u32));
  TEST_EQ(test_file_compare(".marshall_read_test_to_file.1.kc3m",
                             "marshall_read_test_to_file.1.expected.kc3m"),
          0);
  TEST_EQ(str_init_1(&path, NULL, ".marshall_read_test_to_file.1.kc3m"),
          &path);
  TEST_ASSERT(file_unlink(&path));
  marshall_clean(&m);
}
TEST_CASE_END(marshall_read_to_file)

TEST_CASE(marshall_read_to_str)
{
  u32 value = 424242;
  s_marshall_read m = {0};
  s_str str = {0};
  s_str expected = {0};
  TEST_ASSERT(marshall_read_init(&m));
  TEST_ASSERT(marshall_read_u32(&m, false, value));
  TEST_EQ(marshall_read_to_str(&m, &str), &str);
  marshall_clean(&m);
  expected = (s_str) {{0}, 36, {"KC3MARSH"
                                "\x00\x00\x00\x00\x00\x00\x00\x00"
                                "\x00\x00\x00\x00\x00\x00\x00\x00"
                                "\x04\x00\x00\x00\x00\x00\x00\x00"
                                "\x32\x79\x06\x00"}};
  TEST_STR_EQ(str, expected);
  str_clean(&str);
}
TEST_CASE_END(marshall_read_to_str)

TEST_CASE(marshall_read_u8)
{
  MARSHALL_READ_TEST_BUF_AND_HEAP(u8, 0x00, "\x00");
  MARSHALL_READ_TEST_BUF_AND_HEAP(u8, 0xFF, "\xFF");
  MARSHALL_READ_TEST_BUF_AND_HEAP(u8, 0x00, "\x00");
  MARSHALL_READ_TEST_BUF_AND_HEAP(u8, 0xFF, "\xFF");
}
TEST_CASE_END(marshall_read_u8)

TEST_CASE(marshall_read_u16)
{
  MARSHALL_READ_TEST_BUF_AND_HEAP(u16, 0xFFFF, "\xFF\xFF");
  MARSHALL_READ_TEST_BUF_AND_HEAP(u16, 0xFFFF, "\xFF\xFF");
}
TEST_CASE_END(marshall_read_u16)

TEST_CASE(marshall_read_u32)
{
  MARSHALL_READ_TEST_BUF_AND_HEAP(u32, 0xFFFFFFFF, "\xFF\xFF\xFF\xFF");
  MARSHALL_READ_TEST_BUF_AND_HEAP(u32, 0xFFFFFFFF, "\xFF\xFF\xFF\xFF");
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
  MARSHALL_READ_TEST_BUF_AND_HEAP(uw, 0, "\0\0\0\0\0\0\0\0");
  MARSHALL_READ_TEST_BUF_AND_HEAP(uw, ~0,
                                  "\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF");
  MARSHALL_READ_TEST_BUF_AND_HEAP(uw, 0, "\0\0\0\0\0\0\0\0");
  MARSHALL_READ_TEST_BUF_AND_HEAP(uw, ~0, "\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF");
}
TEST_CASE_END(marshall_read_uw)
