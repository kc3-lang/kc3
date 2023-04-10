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
#include "abs.h"
#include "ceiling.h"
#include "sign.h"

#define DEF_CEILING_S(bits, bits2)                                     \
  s ## bits ceiling_s ## bits (s ## bits a, s ## bits b)               \
  {                                                                    \
    s ## bits sign;                                                    \
    sign = sign_s ## bits (a) * sign_s ## bits (b);                    \
    a = abs_s ## bits (a);                                             \
    b = abs_s ## bits (b);                                             \
    return (s ## bits) (sign * ((s ## bits2) a + (b - 1)) / b);        \
  }

#define DEF_CEILING_U(bits, bits2)                                     \
  u ## bits ceiling_u ## bits (u ## bits a, u ## bits b)               \
  {                                                                    \
    return (u ## bits) (((u ## bits2) a + (b - 1)) / b);               \
  }

DEF_CEILING_S(8, 16)
DEF_CEILING_S(16, 32)
DEF_CEILING_S(32, 64)
DEF_CEILING_U(8, 16)
DEF_CEILING_U(16, 32)
DEF_CEILING_U(32, 64)
