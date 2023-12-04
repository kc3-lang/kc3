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
#ifndef LIBC3_WINDOW_CAIRO_DEMO_H
#define LIBC3_WINDOW_CAIRO_DEMO_H

#include "../types.h"

#define WINDOW_CAIRO_DEMO_SEQUENCE_COUNT 4

bool window_cairo_demo_button (s_window_cairo *window, u8 button,
                               sw x, sw y);
bool window_cairo_demo_key (s_window_cairo *window, uw keysym);
bool window_cairo_demo_load (s_window_cairo *window);
bool window_cairo_demo_render (s_window_cairo *window, cairo_t *cr);
bool window_cairo_demo_resize (s_window_cairo *window, uw w, uw h);

#endif /* LIBC3_WINDOW_CAIRO_DEMO_H */
