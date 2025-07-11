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
#include "../libkc3/marshall.h"
#include "test.h"

#define MARSHALL_TEST(type, test, expected)                            \
  {                                                                    \
    s_marshall m = {0};                                                \
    TEST_ASSERT(marshall_init(&m));                                    \
    TEST_ASSERT(marshall_ ## type (&m, (type) (test)));                \
    TEST_MEM_EQ(m.buf.ptr.pu8, sizeof(type),                           \
      (expected), sizeof(expected) - 1);                               \
  }

#define MARSHALL_TEST_U8(test, expected)        \
  MARSHALL_TEST(u8, test, expected)

#define MARSHALL_TEST_U16(test, expected)       \
  MARSHALL_TEST(u16, test, expected)

#define MARSHALL_TEST_U32(test, expected)       \
  MARSHALL_TEST(u32, test, expected)

#define MARSHALL_TEST_U64(test, expected)       \
  MARSHALL_TEST(u64, test, expected)

#define MARSHALL_TEST_UW(test, expected)        \
  MARSHALL_TEST(uw, test, expected)

#define MARSHALL_TEST_S8(test, expected)        \
  MARSHALL_TEST(s8, test, expected)

#define MARSHALL_TEST_S16(test, expected)       \
  MARSHALL_TEST(s16, test, expected)

#define MARSHALL_TEST_S32(test, expected)       \
  MARSHALL_TEST(s32, test, expected)

#define MARSHALL_TEST_S64(test, expected)       \
  MARSHALL_TEST(s64, test, expected)

#define MARSHALL_TEST_SW(test, expected)        \
  MARSHALL_TEST(sw, test, expected)

void marshal_test (void);

TEST_CASE_PROTOTYPE(marshall_u8);
TEST_CASE_PROTOTYPE(marshall_u16);
TEST_CASE_PROTOTYPE(marshall_u32);
TEST_CASE_PROTOTYPE(marshall_u64);
TEST_CASE_PROTOTYPE(marshall_s8);
TEST_CASE_PROTOTYPE(marshall_s16);
TEST_CASE_PROTOTYPE(marshall_s32);
TEST_CASE_PROTOTYPE(marshall_s64);
TEST_CASE_PROTOTYPE(marshall_to_buf);
TEST_CASE_PROTOTYPE(marshall_save_to_file);
TEST_CASE_PROTOTYPE(marshall_read_from_file);

void marshall_test (void)
{
  TEST_CASE_RUN(marshall_u8);
  TEST_CASE_RUN(marshall_u16);
  TEST_CASE_RUN(marshall_u32);
  TEST_CASE_RUN(marshall_u64);
  TEST_CASE_RUN(marshall_s8);
  TEST_CASE_RUN(marshall_s16);
  TEST_CASE_RUN(marshall_s32);
  TEST_CASE_RUN(marshall_s64);
  TEST_CASE_RUN(marshall_to_buf);
  TEST_CASE_RUN(marshall_save_to_file);
  TEST_CASE_RUN(marshall_read_from_file);
}

TEST_CASE(marshall_s8)
{
  MARSHALL_TEST_S8(0, "\0");
  MARSHALL_TEST_S8(0xAB, "\xAB");
}
TEST_CASE_END(marshall_s8)

TEST_CASE(marshall_s16)
{
  MARSHALL_TEST_S16(0, "\0\0");
  MARSHALL_TEST_S16(-1, "\xFF\xFF");
}
TEST_CASE_END(marshall_s16)

TEST_CASE(marshall_s32)
{
  MARSHALL_TEST_S32(0, "\0\0\0\0");
  MARSHALL_TEST_S32(~0, "\xFF\xFF\xFF\xFF");
  MARSHALL_TEST_S32(0xDEADBEEF, "\xEF\xBE\xAD\xDE");
  MARSHALL_TEST_S32(0xDEADCAFE, "\xFE\xCA\xAD\xDE");
  MARSHALL_TEST_S32(0xFEEDBABE, "\xBE\xBA\xED\xFE");
}
TEST_CASE_END(marshall_s32)

TEST_CASE(marshall_s64)
{
  MARSHALL_TEST_S64(0, "\0\0\0\0\0\0\0\0");
  MARSHALL_TEST_S64(~0, "\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF");
}
TEST_CASE_END(marshall_s64)

TEST_CASE(marshall_sw)
{
  MARSHALL_TEST_SW(0, "\0\0\0\0\0\0\0\0");
  MARSHALL_TEST_SW(~0, "\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF");
}
TEST_CASE_END(marshall_sw)


TEST_CASE(marshall_to_buf)
{
  s_marshall s = {0};
  char b[] = {0};
  s_buf buf = {0};
  buf_init(&buf, false, sizeof(b), b);
  TEST_ASSERT(marshall_init(&s) != NULL);
  TEST_EQ(marshall_to_buf(&s, &buf), 0);
  TEST_MEM_EQ(buf.ptr.pu8, buf.wpos, s.buf.ptr.pu8, s.buf.wpos);
}
TEST_CASE_END(marshall_to_buf)


TEST_CASE(marshall_u8)
{
  MARSHALL_TEST_U8(0x00, "\x00");
  MARSHALL_TEST_U8(0xFF, "\xFF");
}
TEST_CASE_END(marshall_u8)

TEST_CASE(marshall_u16)
{
  MARSHALL_TEST_U16(0xFFFF, "\xFF\xFF");
}
TEST_CASE_END(marshall_u16)

TEST_CASE(marshall_u32)
{
  MARSHALL_TEST_U32(0xFFFFFFFF, "\xFF\xFF\xFF\xFF");
}
TEST_CASE_END(marshall_u32)

TEST_CASE(marshall_u64)
{
  MARSHALL_TEST_U64(0xFFFFFFFFFFFFFFFF,
    "\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF");
}
TEST_CASE_END(marshall_u64)

TEST_CASE(marshall_uw)
{
  MARSHALL_TEST_UW(0, "\0\0\0\0\0\0\0\0");
  MARSHALL_TEST_UW(~0, "\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF");
}
TEST_CASE_END(marshall_uw)

TEST_CASE(marshall_save_to_file)
{
  u32 value = 42;
  s_marshall m = {0};
  TEST_ASSERT(marshall_init(&m));
  TEST_ASSERT(marshall_u32(&m, value));
  
  TEST_EQ(marshall_save_to_file(&m, "./file_test.msh"), 0);
}
TEST_CASE_END(marshall_write_to_file)

TEST_CASE(marshall_read_from_file)
{
  u32 value = 0;
  s_marshall *m;

  TEST_ASSERT(
    (m = marshall_read_from_file("./file_test.msh")) != NULL);
  MARSHALL_READ(m, u32, value);
  TEST_EQ(value, 42);
  marshall_delete(m);
}
TEST_CASE_END(marshall_read_from_file)
