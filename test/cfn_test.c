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
#include "../libc3/cfn.h"
#include "test.h"

void cfn_test ();
void cfn_test_init_clean ();
void cfn_test_apply ();
void cfn_test_copy ();
void cfn_test_link ();
void cfn_test_set_type ();

void cfn_test ()
{
  cfn_test_init_clean();
  cfn_test_apply();
  cfn_test_copy();
  cfn_test_link();
  cfn_test_set_type();
}

void cfn_test_init_clean ()
{
  s_cfn tmp;
  TEST_EQ(cfn_init(&tmp), &tmp);
  cfn_clean(&tmp);
  test_ok();
}

void cfn_test_apply ()
{
  s_cfn tmp;
  cfn_init(&tmp);
  
  cfn_clean(&tmp);
}

void cfn_test_copy ()
{
  s_cfn a;
  s_cfn b;
  cfn_init(&a);
  cfn_clean(&a);
}

void cfn_test_link ()
{
  s_cfn tmp;
  cfn_init(&tmp);
  tmp.name
  cfn_clean(&tmp);
}

void cfn_test_set_type ()
{
}
