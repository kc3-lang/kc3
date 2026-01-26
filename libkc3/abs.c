/* kc3
 * Copyright from 2022 to 2026 kmx.io <contact@kmx.io>
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
#include "abs.h"

#define DEF_ABS_S(bits)                                                \
  s ## bits abs_s ## bits (s ## bits a)                                \
  {                                                                    \
    if (a < 0)                                                         \
      a = -a;                                                          \
    return a;                                                          \
  }

#define DEF_ABS_U(bits)                                                \
  u ## bits abs_u ## bits (u ## bits a)                                \
  {                                                                    \
    return a;                                                          \
  }

DEF_ABS_S(8)
DEF_ABS_S(16)
DEF_ABS_S(32)
DEF_ABS_S(64)
DEF_ABS_S(w)
DEF_ABS_U(8)
DEF_ABS_U(16)
DEF_ABS_U(32)
DEF_ABS_U(64)
DEF_ABS_U(w)
