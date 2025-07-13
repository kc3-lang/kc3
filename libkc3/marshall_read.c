/* kc3
 * Copyright from 2022 to 2025 kmx.io <contact@kmx.io>
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

#include "types.h"
#include "assert.h"
#include "marshall.h"
#include "marshall_read.h"
#include "buf.h"

#define DEF_MARSHALL_READ(name, type)                                  \
  sw marshall_read_ ## name (s_marshall_read *mr, type *dest)          \
  {                                                                    \
    if (buf_read_ ## type (&mr->buf, dest) < 0)                        \
      return -1;                                                       \
    return 0;                                                          \
  }

DEF_MARSHALL_READ(s8, s8)
DEF_MARSHALL_READ(s16, s16)
DEF_MARSHALL_READ(s32, s32)
DEF_MARSHALL_READ(s64, s64)
DEF_MARSHALL_READ(sw, sw)
