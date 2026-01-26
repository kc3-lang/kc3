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
 * @file ceiling.h
 * @brief Mathematical ceiling
 *
 * Functions that return the ceiling of a number divided by another
 * number.
 */
#ifndef LIBKC3_CEILING_H
#define LIBKC3_CEILING_H

#include "types.h"

#define DEF_CEILING_PROTOTYPE(type)                                    \
  type ceiling_ ## type (type a, type b)

DEF_CEILING_PROTOTYPE(s8);
DEF_CEILING_PROTOTYPE(s16);
DEF_CEILING_PROTOTYPE(s32);
DEF_CEILING_PROTOTYPE(s64);
DEF_CEILING_PROTOTYPE(sw);
DEF_CEILING_PROTOTYPE(u8);
DEF_CEILING_PROTOTYPE(u16);
DEF_CEILING_PROTOTYPE(u32);
DEF_CEILING_PROTOTYPE(u64);
DEF_CEILING_PROTOTYPE(uw);

#endif /* LIBKC3_CEILING_H */
