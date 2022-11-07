/* c3
 * Copyright 2022 kmx.io <contact@kmx.io>
 *
 * Permission is hereby granted to use this software granted
 * the above copyright notice and this permission paragraph
 * are included in all copies and substantial portions of this
 * software.
 *
 * THIS SOFTWARE IS PROVIDED "AS-IS" WITHOUT ANY GUARANTEE OF
 * PURPOSE AND PERFORMANCE. IN NO EVENT WHATSOEVER SHALL THE
 * AUTHOR BE CONSIDERED LIABLE FOR THE USE AND PERFORMANCE OF
 * THIS SOFTWARE.
 */
#ifndef FACT_TEST_H
#define FACT_TEST_H

#include "../libc3/c3.h"

#define FACT_TEST_EQ(test, expected)                                   \
  do {                                                                 \
    s_fact *fact_expected;                                             \
    s_fact *fact_test;                                                 \
    fact_expected = (expected);                                        \
    fact_test = (test);                                                \
    if (fact_compare(fact_test, fact_expected) == 0) {                 \
      test_ok();                                                       \
    }                                                                  \
    else {                                                             \
      s_str str_expected;                                              \
      s_str str_test;                                                  \
      test_ko();                                                       \
      fact_inspect(fact_expected, &str_expected);                      \
      fact_inspect(fact_test, &str_test);                              \
      printf("\n%sAssertion failed in %s:%d %s\n"                      \
             "%s == %s\n"                                              \
             "Expected %s got %s.%s\n",                                \
             TEST_COLOR_KO,                                            \
             __FILE__, __LINE__, __func__,                             \
             str_test.ptr.ps8, str_expected.ptr.ps8,                   \
             str_expected.ptr.ps8, str_test.ptr.ps8,                   \
             TEST_COLOR_RESET);                                        \
      str_clean(&str_expected);                                        \
      str_clean(&str_test);                                            \
    }                                                                  \
  } while (0)

s_fact * fact_test_clean (s_fact *fact);
s_fact * fact_test_init_1 (s_fact *fact, const s8 *tag);
s_fact * fact_test_init_3 (s_fact *fact, const s8 *subject,
                           const s8 *predicate, const s8 *object);

#endif /* FACT_TEST_H */
