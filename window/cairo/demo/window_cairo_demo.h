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
#ifndef LIBKC3_WINDOW_CAIRO_DEMO_H
#define LIBKC3_WINDOW_CAIRO_DEMO_H

#include "../types.h"

#define WINDOW_CAIRO_DEMO_SEQUENCE_COUNT 5

extern s_cairo_font g_font_computer_modern;
extern s_cairo_font g_font_courier_new;

bool window_cairo_demo_button (s_window_cairo *window, u8 button,
                               s64 x, s64 y);
bool window_cairo_demo_key (s_window_cairo *window, u32 keysym);
bool window_cairo_demo_load (s_window_cairo *window);
bool window_cairo_demo_render (s_window_cairo *window);
bool window_cairo_demo_resize (s_window_cairo *window, u64 w, u64 h);
void window_cairo_demo_unload (s_window_cairo *window);

#endif /* LIBKC3_WINDOW_CAIRO_DEMO_H */
