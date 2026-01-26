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
#ifndef TAG_TEST_H
#define TAG_TEST_H

#include "../libkc3/kc3.h"

#define TAG_TEST_EQ(test, expected)                                    \
  do {                                                                 \
    const s_tag *TAG_TEST_EQ_expected;                                 \
    const s_tag *TAG_TEST_EQ_test;                                     \
    TAG_TEST_EQ_expected = (expected);                                 \
    TAG_TEST_EQ_test = (test);                                         \
    if (compare_tag(TAG_TEST_EQ_test, TAG_TEST_EQ_expected) == 0) {    \
      test_ok();                                                       \
    }                                                                  \
    else {                                                             \
      s_str str_expected;                                              \
      s_str str_test;                                                  \
      test_ko();                                                       \
      inspect_tag(TAG_TEST_EQ_expected, &str_expected);                \
      inspect_tag(TAG_TEST_EQ_test, &str_test);                        \
      printf("\n%sAssertion failed in %s:%d %s\n"                      \
             "%s == %s\n"                                              \
             "Expected %s got %s.%s\n",                                \
             TEST_COLOR_KO,                                            \
             __FILE__, __LINE__, __func__,                             \
             str_expected.ptr.ps8, str_test.ptr.ps8,                   \
             str_test.ptr.ps8, str_expected.ptr.ps8,                   \
             TEST_COLOR_RESET);                                        \
      str_clean(&str_expected);                                        \
      str_clean(&str_test);                                            \
    }                                                                  \
  } while (0)

#endif /* TAG_TEST_H */
