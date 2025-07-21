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

#include "../libkc3/marshall.h"
#include "test.h"

void marshal_test (void);
TEST_CASE_PROTOTYPE(marshall_serialize);

void marshall_test (void)
{
  TEST_CASE_RUN(marshall_serialize);
}

TEST_CASE(marshall_serialize)
{
  s_serialize *serializer = serialize_new();
  TEST_ASSERT(serializer);

  serialize_u32(serializer, 0xDEADBEEF);
}
TEST_CASE_END(marshall_serialize)
