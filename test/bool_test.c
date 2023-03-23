/* c3
 * Copyright 2022,2023 kmx.io <contact@kmx.io>
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
#include "../libc3/bool.h"
#include "../libc3/str.h"
#include "test.h"

#define BOOL_TEST_INSPECT(test, expected)                              \
  do {                                                                 \
    s_str str;                                                         \
    test_context("bool_inspect(" # test ") -> " # expected);           \
    TEST_EQ(bool_inspect(test, &str), &str);                           \
    TEST_EQ(str.size, strlen(expected));                               \
    if (g_test_last_ok)                                                \
      TEST_STRNCMP(str.ptr.p, (expected), str.size);                   \
    str_clean(&str);						       \
  } while (0)

void bool_test ();
void bool_test_compat ();
void bool_test_inspect ();

void bool_test ()
{
  bool_test_compat();
  bool_test_inspect();
}

void bool_test_compat ()
{
  TEST_EQ(true, 1);
  TEST_EQ(false, 0);
  TEST_ASSERT(true);
  TEST_ASSERT(! false);
  TEST_ASSERT((bool) true);
  TEST_ASSERT(! (bool) false);
}

void bool_test_inspect ()
{
  BOOL_TEST_INSPECT(true, "true");
  BOOL_TEST_INSPECT(false, "false");
}
