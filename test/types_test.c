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
  TEST_EQ(sizeof(f128), 16);
  TEST_EQ(sizeof(s8), 1);
  TEST_EQ(sizeof(s16), 2);
  TEST_EQ(sizeof(s32), 4);
  TEST_ASSERT(sizeof(sw) == 4 || sizeof(sw) == 8);
  TEST_EQ(sizeof(s64), 8);
  TEST_EQ(sizeof(u8), 1);
  TEST_EQ(sizeof(u16), 2);
  TEST_EQ(sizeof(u32), 4);
  TEST_ASSERT(sizeof(uw) == 4 || sizeof(uw) == 8);
  TEST_EQ(sizeof(u64), 8);
  TEST_EQ(sizeof(bool), sizeof(u8));
  TEST_EQ(sizeof(u_ptr), sizeof(void *));
  TEST_EQ(sizeof(pthread_mutex_t), sizeof(uw));
}
TEST_CASE_END(types_sizeof)
