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
#include "../libc3/f64.h"
#include "test.h"

void f64_test_compare ();

void f64_test()
{
  f64_test_compare();
}

void f64_test_compare ()
{
  TEST_EQ(f64_compare(0.0, 0.0), 0);
  TEST_EQ(f64_compare(0.0, 1.0), -1);
  TEST_EQ(f64_compare(1.0, 0.0), 1);
  TEST_EQ(f64_compare(1.0, 1.0), 0);
  TEST_EQ(f64_compare(10000000000000000000000.0,
                      10000000000000000000000.0), 0);
  TEST_EQ(f64_compare(3.40282346638528859811704183484516925440e+38,
                      3.40282346638528859811704183484516925440e+38), 0);
  TEST_EQ(f64_compare(1.797693134862315708145274237317043567981e+308,
                      1.797693134862315708145274237317043567981e+308), 0);
  TEST_EQ(f64_compare(1.597693134862315708145274237317043567981e+308,
                      1.797693134862315708145274237317043567980e+308), -1);
  TEST_EQ(f64_compare(1.797693134862315708145274237317043567980e+308,
                      1.597693134862315708145274237317043567981e+308), 1);

 }

