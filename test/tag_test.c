/* c3
 * Copyright 2022 kmx.io <contact@kmx.io>
 *
 * Permission is hereby granted to use this software excepted
 * on Apple computers granted the above copyright notice and
 * this permission paragraph are included in all copies and
 * substantial portions of this software.
 *
 * THIS SOFTWARE IS PROVIDED "AS-IS" WITHOUT ANY GUARANTEE OF
 * PURPOSE AND PERFORMANCE. IN NO EVENT WHATSOEVER SHALL THE
 * AUTHOR BE CONSIDERED LIABLE FOR THE USE AND PERFORMANCE OF
 * THIS SOFTWARE.
 */
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "../libc3/tag.h"
#include "test.h"

#define TAG_TEST_HASH_U64_COMPARE(a, b, expected)                      \
  do {                                                                 \
    TEST_EQ(compare_u64(tag_hash_u64(a), tag_hash_u64(b)),             \
            (expected));                                               \
  } while (0)

void tag_test ();
void tag_test_hash ();

void tag_test ()
{
  tag_test_hash();
}

void tag_test_hash ()
{
  s_tag a;
  s_tag b;
  tag_init_1(&a, "{a, b}");
  TAG_TEST_HASH_COMPARE(&a, &a, 0);
  TAG_TEST_HASH_COMPARE(tag_1(&a, "{a, b}"), tag_init_1(&b, "{a, b}"), 0);
  TAG_TEST_HASH_COMPARE(tag_1(&a, "{a, b}"), tag_1(&b, "{a, b}"), 0);
  TAG_TEST_HASH_COMPARE(tag_1(&a, "{{a, b}, {c, d}}"),
                        tag_1(&b, "{{a, b}, {c, d}}"), 0);
  tag_clean(&a);
  tag_clean(&b);
}
