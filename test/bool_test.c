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
#include <string.h>
#include "../libkc3/bool.h"
#include "../libkc3/str.h"
#include "../libkc3/sym.h"
#include "test.h"

void bool_test (void);
TEST_CASE_PROTOTYPE(bool_compat);
TEST_CASE_PROTOTYPE(bool_init_cast_pstruct);

void bool_test (void)
{
  TEST_CASE_RUN(bool_compat);
  TEST_CASE_RUN(bool_init_cast_pstruct);
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

TEST_CASE(bool_init_cast_pstruct)
{
  bool b;
  s_tag pstruct_tag = {0};
  const s_sym *bool_type = &g_sym_Bool;
  
  pstruct_tag.type = TAG_PSTRUCT;
  pstruct_tag.data.pstruct = NULL; // Minimal pstruct for testing
  
  TEST_ASSERT(bool_init_cast(&b, &bool_type, &pstruct_tag));
  TEST_EQ(b, true);
}
TEST_CASE_END(bool_init_cast_pstruct)
