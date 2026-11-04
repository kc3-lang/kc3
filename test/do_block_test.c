/* kc3
 * Copyright from 2022 to 2026 kmx.io <contact@kmx.io>
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
#include "../libkc3/alloc.h"
#include "../libkc3/do_block.h"
#include "test.h"

static char do_block_init_size_overflow_check (void)
{
  s_do_block block = {0};
  uw count = UW_MAX / sizeof(s_tag) + 2;
  if (do_block_init(&block, count)) {
    alloc_free(block.tag);
    return 1;
  }
  return 0;
}

TEST_CASE(do_block_init_size_overflow)
{
  TEST_EQ(do_block_init_size_overflow_check(), 0);
}
TEST_CASE_END(do_block_init_size_overflow)

void do_block_test (void)
{
  TEST_CASE_RUN(do_block_init_size_overflow);
}
