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
 * @file abs.h
 * @brief Mathematical absolute value
 *
 * Functions that return the absolute value of a number.
 */
#ifndef LIBKC3_ABS_H
#define LIBKC3_ABS_H

#include "types.h"

#define DEF_ABS_PROTOTYPE(type)                                    \
  type abs_ ## type (type a)

DEF_ABS_PROTOTYPE(s8);
DEF_ABS_PROTOTYPE(s16);
DEF_ABS_PROTOTYPE(s32);
DEF_ABS_PROTOTYPE(s64);
DEF_ABS_PROTOTYPE(sw);
DEF_ABS_PROTOTYPE(u8);
DEF_ABS_PROTOTYPE(u16);
DEF_ABS_PROTOTYPE(u32);
DEF_ABS_PROTOTYPE(u64);
DEF_ABS_PROTOTYPE(uw);

#endif /* LIBKC3_ABS_H */
