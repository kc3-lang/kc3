/* kc3
 * Copyright 2022,2023,2024 kmx.io <contact@kmx.io>
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
#include <string.h>
#include "../libkc3/bool.h"
#include "../libkc3/str.h"
#include "test.h"

void bool_test (void);
TEST_CASE_PROTOTYPE(bool_compat);

void bool_test (void)
{
  TEST_CASE_RUN(bool_compat);
}

TEST_CASE(bool_compat)
{
  TEST_EQ(true, 1);
  TEST_EQ(false, 0);
  TEST_ASSERT(true);
  TEST_ASSERT(! false);
  TEST_ASSERT((bool) true);
  TEST_ASSERT(! (bool) false);
}
TEST_CASE_END(bool_compat)
