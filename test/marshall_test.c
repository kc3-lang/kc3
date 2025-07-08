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

void marshal_test (void);
TEST_CASE_PROTOTYPE(marshall_u32);

void marshall_test (void)
{
  TEST_CASE_RUN(marshall_u32);
}

TEST_CASE(marshall_u32)
{
  s_serialize s = {0};
  intptr_t value = 0xDEADBEEF;
  const s_str str = {.free = {0}, .size = 4,
    .ptr = {"\xEF\xBE\xAD\xDE"}};

  TEST_ASSERT(serialize_init(&s));
  TEST_ASSERT(serialize_u32(&s, value));

  err_inspect_buf(&s.buf);
  err_write_1("\n");

  TEST_EQ(s.buf.ptr.pchar[0], str.ptr.pchar[0]);
  TEST_EQ(s.buf.ptr.pchar[1], str.ptr.pchar[1]);
  TEST_EQ(s.buf.ptr.pchar[2], str.ptr.pchar[2]);
  TEST_EQ(s.buf.ptr.pchar[3], str.ptr.pchar[3]);
}
TEST_CASE_END(marshall_u32)
