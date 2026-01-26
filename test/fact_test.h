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
#ifndef FACT_TEST_H
#define FACT_TEST_H

#include "../libkc3/kc3.h"

#define FACT_TEST_EQ(test, expected)                                   \
  do {                                                                 \
    const s_fact *fact_expected;                                       \
    const s_fact *fact_test;                                           \
    s_str str_expected;                                                \
    s_str str_test;                                                    \
    fact_expected = (expected);                                        \
    fact_test = (test);                                                \
    if (compare_fact(fact_test, fact_expected) == 0) {                 \
      g_test_assert_count++;                                           \
      g_test_assert_ok++;                                              \
    }                                                                  \
    else {                                                             \
      inspect_fact(fact_expected, &str_expected);                      \
      inspect_fact(fact_test, &str_test);                              \
      printf("\n%sAssertion failed in %s:%d %s\n"                      \
             "%s == %s\n"                                              \
             "Expected %s got %s.%s\n",                                \
             TEST_COLOR_KO,                                            \
             __FILE__, __LINE__, __func__,                             \
             str_test.ptr.pchar, str_expected.ptr.pchar,               \
             str_expected.ptr.pchar, str_test.ptr.pchar,               \
             TEST_COLOR_RESET);                                        \
      str_clean(&str_expected);                                        \
      str_clean(&str_test);                                            \
      test_ko();                                                       \
    }                                                                  \
  } while (0)

void     fact_test_clean_1 (s_fact *fact);
void     fact_test_clean_3 (s_fact *fact);
s_fact * fact_test_init_1 (s_fact *fact, const char *tag);
s_fact * fact_test_init_3 (s_fact *fact, const char *subject,
                           const char *predicate, const char *object);

#endif /* FACT_TEST_H */
