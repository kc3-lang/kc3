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
#ifndef LIBKC3_WINDOW_H
#define LIBKC3_WINDOW_H

#include "types.h"

/* Stack-allocation compatible functions, call window_clean
   after use. */
void       window_clean (s_window *window);
s_window * window_init (s_window *window,
                        s64 x, s64 y, u64 w, u64 h,
                        const char *title,
                        u64 sequence_count);

/* Operators. */
bool window_animate (s_window *window);
bool window_set_sequence_pos (s_window *window, u32 sequence_pos);

#endif /* LIBKC3_WINDOW_H */
