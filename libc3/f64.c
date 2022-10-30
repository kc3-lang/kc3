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
#include <float.h>
#include <math.h>
#include <string.h>
#include "f64.h"

s8 f64_compare (f64 a, f64 b)
{
  if (fabs(a - b) <= DBL_EPSILON)
    return 0;
  if (a < b)
    return -1;
  if (a > b)
    return 1;
  return memcmp(&a, &b, sizeof(f64));
}

t_hash_context * f64_hash_update (t_hash_context *context, f64 x)
{
  return hash_update(context, &x, sizeof(x));
}
