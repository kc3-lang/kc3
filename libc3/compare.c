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
#include "compare.h"

#define COMPARE_DEF(type)                       \
  s8 compare_##type (type a, type b)            \
  {                                             \
    if (a < b)                                  \
      return -1;                                \
    if (a > b)                                  \
      return 1;                                 \
    return 0;                                   \
  }                                             \

s8 compare_call (const s_call *a, const s_call *b)
{
  s8 r;
  if (a == b)
    return 0;
  if (!a)
    return -1;
  if (!b)
    return 1;
  if ((r = compare_ident(&a->ident, &b->ident)))
    return r;
  return compare_list(a->arguments, b->arguments);
}

COMPARE_DEF(f32);
COMPARE_DEF(f64);
COMPARE_DEF(s8);
COMPARE_DEF(s16);
COMPARE_DEF(s32);
COMPARE_DEF(s64);
COMPARE_DEF(u8);
COMPARE_DEF(u16);
COMPARE_DEF(u32);
COMPARE_DEF(u64);
