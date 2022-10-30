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
#include "u8.h"

s8 u8_compare (u8 a, u8 b)
{
  if (a < b)
    return -1;
  if (a > b)
    return 1;
  return 0;
}

t_hash_context * u8_hash_update (t_hash_context *context, u8 x)
{
  return hash_update(context, &x, sizeof(x));
}
