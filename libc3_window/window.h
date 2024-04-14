/* c3
 * Copyright 2022-2024 kmx.io <contact@kmx.io>
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
#ifndef LIBC3_WINDOW_H
#define LIBC3_WINDOW_H

#include <libc3/types.h>
#include "types.h"

/* Stack-allocation compatible functions, call window_clean
   after use. */
void       window_clean (s_window *window);
s_window * window_init (s_window *window,
                        sw x, sw y, uw w, uw h,
                        const char *title,
                        uw sequence_count);

/* Operators. */
bool window_animate (s_window *window);
bool window_set_sequence_pos (s_window *window, uw sequence_pos);

#endif /* LIBC3_WINDOW_H */