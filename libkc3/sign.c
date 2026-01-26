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
#include "sign.h"

#define DEF_SIGN_S(bits)                                               \
  s ## bits sign_s ## bits (s ## bits a)                               \
  {                                                                    \
    if (a < 0)                                                         \
      return -1;                                                       \
    if (a == 0)                                                        \
      return 0;                                                        \
    return 1;                                                          \
  }

DEF_SIGN_S(8)
DEF_SIGN_S(16)
DEF_SIGN_S(32)
DEF_SIGN_S(64)
DEF_SIGN_S(w)
