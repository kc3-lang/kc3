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
#include "../libkc3/types.h"
#include "test.h"

void types_test (void);
TEST_CASE_PROTOTYPE(types_sizeof);

void types_test (void)
{
  TEST_CASE_RUN(types_sizeof);
}

TEST_CASE(types_sizeof)
{
  TEST_EQ(sizeof(f32), 4);
  TEST_EQ(sizeof(f64), 8);
#if HAVE_F80
  TEST_ASSERT(sizeof(f80) == 12 || sizeof(f80) == 16);
#endif
  TEST_EQ(sizeof(s8), 1);
  TEST_EQ(sizeof(s16), 2);
  TEST_EQ(sizeof(s32), 4);
  TEST_ASSERT(sizeof(sw) == 4 || sizeof(sw) == 8);
  TEST_EQ(sizeof(sw), sizeof(void *));
  TEST_EQ(sizeof(s64), 8);
  TEST_EQ(sizeof(u8), 1);
  TEST_EQ(sizeof(u16), 2);
  TEST_EQ(sizeof(u32), 4);
  TEST_ASSERT(sizeof(uw) == 4 || sizeof(uw) == 8);
  TEST_EQ(sizeof(uw), sizeof(void *));
  TEST_EQ(sizeof(u64), 8);
  TEST_EQ(sizeof(bool), sizeof(u8));
  TEST_EQ(sizeof(u_ptr), sizeof(void *));
  TEST_EQ(sizeof(u_ptr_w), sizeof(void *));
  TEST_EQ((uw) NULL, 0);
}
TEST_CASE_END(types_sizeof)
