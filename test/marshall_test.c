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
#include <string.h>
#include "../libkc3/io.h"
#include "../libkc3/marshall.h"
#include "test.h"
#include <limits.h>

void marshal_test (void);
TEST_CASE_PROTOTYPE(marshall_u32);

void marshall_test (void)
{
  TEST_CASE_RUN(marshall_u32);
}


#define MARSHALL_TEST(type, test, expected)                           \
  do {                                                                \
    uw i = 0;                                                         \
    const char *e = (expected);                                       \
    s_serialize s = {0};                                              \
    type val = (test);                                                \
    const s_str str = {.free = {0}, .size = sizeof(type),             \
      .ptr = {e}};                                                    \
    TEST_ASSERT(sizeof(type) == sizeof(expected) - 1);                \
    TEST_ASSERT(serialize_init(&s));                                  \
    TEST_ASSERT(serialize_ ## type(&s, val));                         \
    while (i < str.size) {                                            \
      TEST_EQ(s.buf.ptr.pchar[i], str.ptr.pchar[i]);                  \
      i++;                                                            \
    }                                                                 \
  } while (0)


#define MARSHALL_TEST_U8(test, expected) \
  MARSHALL_TEST(u8, test, expected)

#define MARSHALL_TEST_U16(test, expected) \
  MARSHALL_TEST(u16, test, expected)

#define MARSHALL_TEST_U32(test, expected) \
  MARSHALL_TEST(u32, test, expected)

#define MARSHALL_TEST_U64(test, expected) \
  MARSHALL_TEST(u64, test, expected)

TEST_CASE(marshall_u32)
{
  MARSHALL_TEST_U8(0xFF,        "\xFF\0\0\0");
  MARSHALL_TEST_U16(0xFFFF,     "\xFF\xFF\0\0");
  MARSHALL_TEST_U32(0xFFFFFFFF, "\xFF\xFF\xFF\xFF");
  MARSHALL_TEST_U64(0xFFFFFFFFFFFFFFFF,
    "\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF");
}
TEST_CASE_END(marshall_u32)
