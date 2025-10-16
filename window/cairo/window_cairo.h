/* kc3
 * Copyright from 2022 to 2025 kmx.io <contact@kmx.io>
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
#ifndef LIBKC3_WINDOW_CAIRO_H
#define LIBKC3_WINDOW_CAIRO_H

#include "types.h"

/* Library functions, call kc3_window_cairo_init at program start,
   and kc3_window_cairo_clean at program end. */
void kc3_window_cairo_clean (void);
void kc3_window_cairo_init (void);

/* Stack-allocation compatible functions, call window_cairo_clean
   after use. */
void             window_cairo_clean (s_window_cairo *window);
s_window_cairo * window_cairo_init (s_window_cairo *window,
                                    s64 x, s64 y, u64 w, u64 h,
                                    const char *title,
                                    u64 sequence_count);
bool             window_cairo_run (s_window_cairo *window);

/* Callbacks. */
bool window_cairo_button_default (s_window_cairo *window, u8 button,
                                  s64 x, s64 y);
bool window_cairo_key_default (s_window_cairo *window, u32 keysym);
bool window_cairo_load_default (s_window_cairo *window);
bool window_cairo_motion_default (s_window_cairo *window, s64 x, s64 y);
bool window_cairo_render_default (s_window_cairo *window);
bool window_cairo_resize_default (s_window_cairo *window, u64 w, u64 h);
void window_cairo_unload_default (s_window_cairo *window);

#endif /* LIBKC3_WINDOW_CAIRO_H */
