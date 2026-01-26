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
#ifndef LIBKC3_CAST_H
#define LIBKC3_CAST_H

#include "types.h"

bool cast_needed_s8 (s8 s);
bool cast_needed_s16 (s16 s);
bool cast_needed_s32 (s32 s);
bool cast_needed_s64 (s64 s);
bool cast_needed_sw (sw s);
bool cast_needed_u8 (u8 s);
bool cast_needed_u16 (u16 s);
bool cast_needed_u32 (u32 s);
bool cast_needed_u64 (u64 s);
bool cast_needed_uw (uw s);

#endif /* LIBKC3_CAST_H */
