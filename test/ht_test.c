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
#include "../libkc3/ht.h"
#include "../libkc3/sym.h"
#include "test.h"

static char ht_iterator_empty_check (void)
{
  s_ht ht;
  s_ht_iterator iterator;
  s_tag *item = (s_tag *) 1;
  s_tag **result;
  if (! ht_init(&ht, &g_sym_U8, 2) ||
      ! ht_iterator_init(&iterator, &ht))
    return 1;
  result = ht_iterator_next(&iterator, &item);
  ht_clean(&ht);
  return result != NULL || item != NULL;
}

TEST_CASE(ht_iterator_empty)
{
  TEST_EQ(ht_iterator_empty_check(), 0);
}
TEST_CASE_END(ht_iterator_empty)

void ht_test (void)
{
  TEST_CASE_RUN(ht_iterator_empty);
}
