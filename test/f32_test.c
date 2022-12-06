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
#include <float.h>
#include <math.h>
#include <string.h>
#include "test.h"
#include "../libc3/f32.h"

void f32_test_compare ();

void f32_test ()
{
    f32_test_compare();
}

void f32_test_compare ()
{
  TEST_EQ(f32_compare('0', '0'), 0);
  TEST_EQ(f32_compare('0', '1'), -1);
  TEST_EQ(f32_compare('1', '0'), 1);
  TEST_EQ(f32_compare(1.0, 1.0), 0);
  TEST_EQ(f32_compare(1.0, 2.0), -1);
  TEST_EQ(f32_compare(1.0, 0.0), 1);
  TEST_EQ(f32_compare(10000000000000000000000.0,
                      10000000000000000000000.0), 0);
  TEST_EQ(f32_compare(3.40282346638528859811704183484516925440e+38,
                      3.40282346638528859811704183484516925440e+38), 0);
  TEST_EQ(f32_compare(1.797693134862315708145274237317043567981e+38,
                      1.797693134862315708145274237317043567981e+38), 0);
  TEST_EQ(f32_compare(1.797693134862315708145274237317043567981e+38,
                      1.797693134862315708145274237317043567981e+38), 0);
  TEST_EQ(f32_compare(1.597693134862315708145274237317043567981e+38,
                      1.797693134862315708145274237317043567980e+38), -1);
  TEST_EQ(f32_compare(1.797693134862315708145274237317043567980e+38,
                      1.597693134862315708145274237317043567981e+38), 1);
}
