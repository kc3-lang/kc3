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
#ifndef COMPARE_TEST_H
#define COMPARE_TEST_H

#include "test.h"
#include "../libkc3/compare.h"
#include "../libkc3/str.h"

#define COMPARE_TEST_STR(a, b, expected)                               \
  do {                                                                 \
    const s_str *a_ = (a);                                             \
    const s_str *b_ = (b);                                             \
    s8 tmp = compare_str(a_, b_);                                      \
    if (tmp == expected) {                                             \
      g_test_assert_count++;                                           \
      g_test_assert_ok++;                                              \
    }                                                                  \
    else {                                                             \
      test_ko();                                                       \
      fprintf(stderr, "\n%sAssertion failed in %s:%d %s\n"             \
              "compare_str(%s, %s) == %s\n"                            \
              "Expected %s got %d.%s\n",                               \
              TEST_COLOR_KO,                                           \
              __FILE__, __LINE__, __func__,                            \
              # a, # b, # expected, # expected, tmp,                   \
              TEST_COLOR_RESET);                                       \
    }                                                                  \
  } while (0)

#endif /* COMPARE_TEST_H */
