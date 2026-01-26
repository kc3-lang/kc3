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
#include "assert.h"
#include "abs.h"
#include "ceiling.h"
#include "sign.h"

#define DEF_CEILING_S(bits)                                            \
  s ## bits ceiling_s ## bits (s ## bits a, s ## bits b)               \
  {                                                                    \
    s ## bits result;                                                  \
    s ## bits sign;                                                    \
    sign = sign_s ## bits (a) * sign_s ## bits (b);                    \
    a = abs_s ## bits (a);                                             \
    b = abs_s ## bits (b);                                             \
    result = sign * (a / b + ((a % b) ? 1 : 0));                       \
    return result;                                                     \
  }

#define DEF_CEILING_U(bits)                                            \
  u ## bits ceiling_u ## bits (u ## bits a, u ## bits b)               \
  {                                                                    \
    u ## bits result;                                                  \
    if (b == 0) {                                                      \
      err_puts("ceiling_u" # bits ": division by zero");               \
      assert(! "ceiling_u" # bits ": division by zero");               \
      return 0;                                                        \
    }                                                                  \
    result = a / b + ((a % b) ? 1 : 0);                                \
    return result;                                                     \
  }

DEF_CEILING_S(8)
DEF_CEILING_S(16)
DEF_CEILING_S(32)
DEF_CEILING_S(64)
DEF_CEILING_S(w)
DEF_CEILING_U(8)
DEF_CEILING_U(16)
DEF_CEILING_U(32)
DEF_CEILING_U(64)
DEF_CEILING_U(w)
