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
#ifndef LIBC3_WINDOW_CAIRO_H
#define LIBC3_WINDOW_CAIRO_H

#include "types.h"

/* Library functions, call c3_window_cairo_init at program start,
   and c3_window_cairo_clean at program end. */
void c3_window_cairo_clean (void);
void c3_window_cairo_init (void);

/* Stack-allocation compatible functions, call window_cairo_clean
   after use. */
void             window_cairo_clean (s_window_cairo *window);
s_window_cairo * window_cairo_init (s_window_cairo *window,
                                    sw x, sw y, uw w, uw h,
                                    const char *title,
                                    uw sequence_count);
bool             window_cairo_run (s_window_cairo *window);

/* Callbacks. */
bool window_cairo_button_default (s_window_cairo *window, u8 button,
                                  sw x, sw y);
bool window_cairo_key_default (s_window_cairo *window, uw keysym);
bool window_cairo_load_default (s_window_cairo *window);
bool window_cairo_motion_default (s_window_cairo *window, sw x, sw y);
bool window_cairo_render_default (s_window_cairo *window);
bool window_cairo_resize_default (s_window_cairo *window, uw w, uw h);

#endif /* LIBC3_WINDOW_CAIRO_H */
