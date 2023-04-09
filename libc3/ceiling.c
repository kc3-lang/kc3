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
#include "ceiling.h"

#define DEF_CEILING(type)                                              \
  type ceiling_ ## type (type a, type b)                               \
  {                                                                    \
    return (a + (b - 1)) / b;                                          \
  }                                                                    \

DEF_CEILING(s8)
DEF_CEILING(s16)
DEF_CEILING(s32)
DEF_CEILING(s64)
DEF_CEILING(sw)
DEF_CEILING(u8)
DEF_CEILING(u16)
DEF_CEILING(u32)
DEF_CEILING(u64)
DEF_CEILING(uw)
