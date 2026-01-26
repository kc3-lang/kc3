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
/**
 * @file sign.h
 * @brief Get the sign of a number
 *
 * Functions that return the sign of a number.
 */
#ifndef LIBKC3_SIGN_H
#define LIBKC3_SIGN_H

#include "types.h"

#define DEF_SIGN_PROTOTYPE(type)                                    \
  type sign_ ## type (type a)

DEF_SIGN_PROTOTYPE(s8);
DEF_SIGN_PROTOTYPE(s16);
DEF_SIGN_PROTOTYPE(s32);
DEF_SIGN_PROTOTYPE(s64);
DEF_SIGN_PROTOTYPE(sw);
DEF_SIGN_PROTOTYPE(u8);
DEF_SIGN_PROTOTYPE(u16);
DEF_SIGN_PROTOTYPE(u32);
DEF_SIGN_PROTOTYPE(u64);
DEF_SIGN_PROTOTYPE(uw);

#endif /* LIBKC3_SIGN_H */
