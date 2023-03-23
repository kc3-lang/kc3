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
#ifndef COMPARE_TEST_H
#define COMPARE_TEST_H

#include "test.h"
#include "../libc3/compare.h"
#include "../libc3/str.h"

#define COMPARE_TEST_STR(a, b, expected)                               \
  do {                                                                 \
    const s_str *a_ = (a);                                                   \
    const s_str *b_ = (b);                                                   \
    sw tmp = compare_str(a_, b_);                                      \
    if (tmp == expected) {                                             \
      test_ok();                                                       \
    }                                                                  \
    else {                                                             \
      test_ko();                                                       \
      printf("\n%sAssertion failed in %s:%d %s\n"                      \
             "compare_str(%s, %s) == %s\n"                             \
             "Expected %s got %ld.%s\n",                               \
             TEST_COLOR_KO,                                            \
             __FILE__, __LINE__, __func__,                             \
             # a, # b, # expected, # expected, tmp,                    \
             TEST_COLOR_RESET);                                        \
    }                                                                  \
  } while (0)

#endif /* COMPARE_TEST_H */
