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
#include "../libc3/array.h"
#include "test.h"

#define ARRAY_TEST_INIT_CLEAN(type, dimension, dimensions)             \
  do {                                                                 \
    s_array a;                                                         \
    TEST_EQ(array_init(&a, (type), (dimension), (dimensions)), &a);    \
    array_clean(&a);                                                   \
  } while(0)

void array_test ();
TEST_CASE_PROTOTYPE(array_init_clean);

void array_test ()
{
  TEST_CASE_RUN(array_init_clean);
}

TEST_CASE(array_init_clean)
{
  ARRAY_TEST_INIT_CLEAN(TAG_U8, 1, (uw []) {1});
  ARRAY_TEST_INIT_CLEAN(TAG_U8, 1, (uw []) {10});
  ARRAY_TEST_INIT_CLEAN(TAG_U8, 1, (uw []) {100});
  ARRAY_TEST_INIT_CLEAN(TAG_U8, 2, ((uw []) {1, 1}));
  ARRAY_TEST_INIT_CLEAN(TAG_U8, 2, ((uw []) {10, 10}));
  ARRAY_TEST_INIT_CLEAN(TAG_U8, 2, ((uw []) {100, 100}));
  ARRAY_TEST_INIT_CLEAN(TAG_U8, 3, ((uw []) {1, 1, 1}));
  ARRAY_TEST_INIT_CLEAN(TAG_U8, 3, ((uw []) {10, 10, 10}));
  ARRAY_TEST_INIT_CLEAN(TAG_U8, 3, ((uw []) {100, 100, 100}));
}
TEST_CASE_END(array_init_clean)
