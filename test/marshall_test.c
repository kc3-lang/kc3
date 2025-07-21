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
#include "../libkc3/str.h"
#include "../libkc3/list.h"
#include "../libkc3/tag.h"
#include "test.h"

#define MARSHALL_TEST(type, on_heap, test, expected)                   \
  do {                                                                 \
    s_marshall m = {0};                                                \
    s_buf *buf = NULL;                                                 \
    TEST_ASSERT(marshall_init(&m));                                    \
    TEST_ASSERT(marshall_ ## type (&m, on_heap, (type) (test)));       \
    buf = (on_heap) ? &m.heap : &m.buf;                                \
    TEST_MEM_EQ(buf->ptr.pu8, sizeof(type),                            \
      (expected), sizeof(expected) - 1);                               \
    marshall_clean(&m);                                                \
  } while (0)

#define MARSHALL_TEST_BUF_BOOL(test, expected)      \
  MARSHALL_TEST(bool, false, test, expected)

#define MARSHALL_TEST_CHARACTER(on_heap, test, expected)               \
  do {                                                                 \
    s_marshall m = {0};                                                \
    s_buf *buf = NULL;                                                 \
    TEST_ASSERT(marshall_init(&m));                                    \
    TEST_ASSERT(marshall_character(&m, (on_heap), (test)));            \
    buf = ((on_heap)) ? &m.heap : &m.buf;                              \
    TEST_MEM_EQ(buf->ptr.pu8, sizeof(expected) - 1,                    \
      (expected), sizeof(expected) - 1);                               \
    marshall_clean(&m);                                                \
  } while (0)

#define MARSHALL_TEST_BUF_CHARACTER(test, expected) \
  MARSHALL_TEST_CHARACTER(false, test, expected)

#define MARSHALL_TEST_BUF_S8(test, expected)        \
  MARSHALL_TEST(s8, false, test, expected)

#define MARSHALL_TEST_BUF_S16(test, expected)       \
  MARSHALL_TEST(s16, false, test, expected)

#define MARSHALL_TEST_BUF_S32(test, expected)       \
  MARSHALL_TEST(s32, false, test, expected)

#define MARSHALL_TEST_BUF_S64(test, expected)       \
  MARSHALL_TEST(s64, false, test, expected)

#define MARSHALL_TEST_BUF_SW(test, expected)        \
  MARSHALL_TEST(sw, false, test, expected)

#define MARSHALL_TEST_BUF_U8(test, expected)        \
  MARSHALL_TEST(u8, false, test, expected)

#define MARSHALL_TEST_BUF_U16(test, expected)       \
  MARSHALL_TEST(u16, false, test, expected)

#define MARSHALL_TEST_BUF_U32(test, expected)       \
  MARSHALL_TEST(u32, false, test, expected)

#define MARSHALL_TEST_BUF_U64(test, expected)       \
  MARSHALL_TEST(u64, false, test, expected)

#define MARSHALL_TEST_BUF_UW(test, expected)        \
  MARSHALL_TEST(uw, false, test, expected)

#define MARSHALL_TEST_HEAP_BOOL(test, expected)     \
  MARSHALL_TEST(bool, true, test, expected)

#define MARSHALL_TEST_HEAP_CHARACTER(test, expected) \
  MARSHALL_TEST_CHARACTER(true, test, expected)

#define MARSHALL_TEST_HEAP_S8(test, expected)       \
  MARSHALL_TEST(s8, true, test, expected)

#define MARSHALL_TEST_HEAP_S16(test, expected)      \
  MARSHALL_TEST(s16, true, test, expected)

#define MARSHALL_TEST_HEAP_S32(test, expected)      \
  MARSHALL_TEST(s32, true, test, expected)

#define MARSHALL_TEST_HEAP_S64(test, expected)      \
  MARSHALL_TEST(s64, true, test, expected)

#define MARSHALL_TEST_HEAP_SW(test, expected)       \
  MARSHALL_TEST(sw, true, test, expected)

#define MARSHALL_TEST_HEAP_U8(test, expected)       \
  MARSHALL_TEST(u8, true, test, expected)

#define MARSHALL_TEST_HEAP_U16(test, expected)      \
  MARSHALL_TEST(u16, true, test, expected)

#define MARSHALL_TEST_HEAP_U32(test, expected)      \
  MARSHALL_TEST(u32, true, test, expected)

#define MARSHALL_TEST_HEAP_U64(test, expected)      \
  MARSHALL_TEST(u64, true, test, expected)

#define MARSHALL_TEST_HEAP_UW(test, expected)       \
  MARSHALL_TEST(uw, true, test, expected)

#define MARSHALL_TEST_TAG(test, on_heap)                               \
  do {                                                                 \
    s_marshall m = {0};                                                \
    s_tag tag = {0};                                                   \
    tag_init_1(&tag, test);                                            \
    TEST_EQ(marshall_init(&m), &m);                                    \
    TEST_EQ(marshall_tag(&m, (on_heap), &tag), &m);                    \
    marshall_clean(&m);                                                \
    tag_clean(&tag);                                                   \
  } while (0)

#define MARSHALL_TEST_TAG_BUF_AND_HEAP(test)                           \
  MARSHALL_TEST_TAG(test, true);                                       \
  MARSHALL_TEST_TAG(test, false)

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
}

TEST_CASE(marshall_bool)
{
  MARSHALL_TEST_BUF_BOOL(false, "\x00");
  MARSHALL_TEST_BUF_BOOL(true, "\x01");
  MARSHALL_TEST_HEAP_BOOL(false, "\x00");
  MARSHALL_TEST_HEAP_BOOL(true, "\x01");
}
TEST_CASE_END(marshall_bool)

TEST_CASE(marshall_character)
{
  MARSHALL_TEST_BUF_CHARACTER('A', "A");
  MARSHALL_TEST_BUF_CHARACTER('\0', "\x00");
  MARSHALL_TEST_BUF_CHARACTER('Z', "Z");
  MARSHALL_TEST_HEAP_CHARACTER('A', "A");
  MARSHALL_TEST_HEAP_CHARACTER('\0', "\x00");
  MARSHALL_TEST_HEAP_CHARACTER('Z', "Z");
}
TEST_CASE_END(marshall_character)

TEST_CASE(marshall_s8)
{
  MARSHALL_TEST_BUF_S8(0, "\0");
  MARSHALL_TEST_BUF_S8(0xAB, "\xAB");
  MARSHALL_TEST_HEAP_S8(0, "\0");
  MARSHALL_TEST_HEAP_S8(0xAB, "\xAB");
}
TEST_CASE_END(marshall_s8)

TEST_CASE(marshall_s16)
{
  MARSHALL_TEST_BUF_S16(0, "\0\0");
  MARSHALL_TEST_BUF_S16(-1, "\xFF\xFF");
  MARSHALL_TEST_HEAP_S16(0, "\0\0");
  MARSHALL_TEST_HEAP_S16(-1, "\xFF\xFF");
}
TEST_CASE_END(marshall_s16)

TEST_CASE(marshall_s32)
{
  MARSHALL_TEST_BUF_S32(0, "\0\0\0\0");
  MARSHALL_TEST_BUF_S32(~0, "\xFF\xFF\xFF\xFF");
  MARSHALL_TEST_BUF_S32(0xDEADBEEF, "\xEF\xBE\xAD\xDE");
  MARSHALL_TEST_BUF_S32(0xDEADCAFE, "\xFE\xCA\xAD\xDE");
  MARSHALL_TEST_BUF_S32(0xFEEDBABE, "\xBE\xBA\xED\xFE");
  MARSHALL_TEST_HEAP_S32(0, "\0\0\0\0");
  MARSHALL_TEST_HEAP_S32(~0, "\xFF\xFF\xFF\xFF");
  MARSHALL_TEST_HEAP_S32(0xDEADBEEF, "\xEF\xBE\xAD\xDE");
  MARSHALL_TEST_HEAP_S32(0xDEADCAFE, "\xFE\xCA\xAD\xDE");
  MARSHALL_TEST_HEAP_S32(0xFEEDBABE, "\xBE\xBA\xED\xFE");
}
TEST_CASE_END(marshall_s32)

TEST_CASE(marshall_s64)
{
  MARSHALL_TEST_BUF_S64(0, "\0\0\0\0\0\0\0\0");
  MARSHALL_TEST_BUF_S64(~0, "\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF");
  MARSHALL_TEST_HEAP_S64(0, "\0\0\0\0\0\0\0\0");
  MARSHALL_TEST_HEAP_S64(~0, "\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF");
}
TEST_CASE_END(marshall_s64)

TEST_CASE(marshall_sw)
{
  MARSHALL_TEST_BUF_SW(0, "\0\0\0\0\0\0\0\0");
  MARSHALL_TEST_BUF_SW(~0, "\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF");
  MARSHALL_TEST_HEAP_SW(0, "\0\0\0\0\0\0\0\0");
  MARSHALL_TEST_HEAP_SW(~0, "\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF");
}
TEST_CASE_END(marshall_sw)


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
  expected = (s_str) {{0}, 36, {"KC3MARSH"
                                "\x00\x00\x00\x00\x00\x00\x00\x00"
                                "\x00\x00\x00\x00\x00\x00\x00\x00"
                                "\x04\x00\x00\x00\x00\x00\x00\x00"
                                "\x32\x79\x06\x00"}};
  TEST_STR_EQ(str, expected);
  str_clean(&str);
}
TEST_CASE_END(marshall_to_str)

TEST_CASE(marshall_u8)
{
  MARSHALL_TEST_BUF_U8(0x00, "\x00");
  MARSHALL_TEST_BUF_U8(0xFF, "\xFF");
  MARSHALL_TEST_HEAP_U8(0x00, "\x00");
  MARSHALL_TEST_HEAP_U8(0xFF, "\xFF");
}
TEST_CASE_END(marshall_u8)

TEST_CASE(marshall_u16)
{
  MARSHALL_TEST_BUF_U16(0xFFFF, "\xFF\xFF");
  MARSHALL_TEST_HEAP_U16(0xFFFF, "\xFF\xFF");
}
TEST_CASE_END(marshall_u16)

TEST_CASE(marshall_u32)
{
  MARSHALL_TEST_BUF_U32(0xFFFFFFFF, "\xFF\xFF\xFF\xFF");
  MARSHALL_TEST_HEAP_U32(0xFFFFFFFF, "\xFF\xFF\xFF\xFF");
}
TEST_CASE_END(marshall_u32)

TEST_CASE(marshall_u64)
{
  MARSHALL_TEST_BUF_U64(0xFFFFFFFFFFFFFFFF,
    "\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF");
  MARSHALL_TEST_HEAP_U64(0xFFFFFFFFFFFFFFFF,
    "\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF");
}
TEST_CASE_END(marshall_u64)

TEST_CASE(marshall_uw)
{
  MARSHALL_TEST_BUF_UW(0, "\0\0\0\0\0\0\0\0");
  MARSHALL_TEST_BUF_UW(~0, "\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF");
  MARSHALL_TEST_HEAP_UW(0, "\0\0\0\0\0\0\0\0");
  MARSHALL_TEST_HEAP_UW(~0, "\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF");
}
TEST_CASE_END(marshall_uw)

TEST_CASE(marshall_plist)
{
  s_marshall m = {0};
  s_list *list_test;
  s_str str = {0};
  const s_str expected =
    {{0}, 51, {"KC3MARSH\x02\0\0\0\0\0\0\0"
               "\x13\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
               "\0\0\0\0\0\0\0\0\x13\0\x18\0\0\0\0\0"
               "\0\0\0"}};

  TEST_ASSERT(marshall_init(&m));
  list_test = list_new_1("[0, 1]");
  TEST_ASSERT(list_test);
  TEST_ASSERT(marshall_plist(&m, true, &list_test));
  marshall_to_str(&m, &str);
  TEST_STR_EQ(str, expected);
}
TEST_CASE_END(marshall_plist)

TEST_CASE(marshall_tag)
{
  MARSHALL_TEST_TAG_BUF_AND_HEAP("0");
  MARSHALL_TEST_TAG_BUF_AND_HEAP("1");
  MARSHALL_TEST_TAG_BUF_AND_HEAP("2");
  MARSHALL_TEST_TAG_BUF_AND_HEAP("256");
  MARSHALL_TEST_TAG_BUF_AND_HEAP("-2147483648");
  MARSHALL_TEST_TAG_BUF_AND_HEAP("92233720368547");
  MARSHALL_TEST_TAG_BUF_AND_HEAP("10000000000000000000000000000000000");
  MARSHALL_TEST_TAG_BUF_AND_HEAP("[1, 2]");
  MARSHALL_TEST_TAG_BUF_AND_HEAP("defmodule Test do end");
  MARSHALL_TEST_TAG_BUF_AND_HEAP("%{a: 1, b: 2}");
  MARSHALL_TEST_TAG_BUF_AND_HEAP("List.reverse([])");
  MARSHALL_TEST_TAG_BUF_AND_HEAP("a = 1");
}
TEST_CASE_END(marshall_tag)
