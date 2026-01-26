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
#ifndef LIBKC3_WINDOW_CAIRO_XCB_H
#define LIBKC3_WINDOW_CAIRO_XCB_H

#include <xcb/xcb.h>
#include "../window_cairo.h"

bool window_cairo_xcb_run (s_window_cairo *window);

xcb_visualtype_t * xcb_screen_visual_type (xcb_screen_t *screen);

#endif /* LIBKC3_WINDOW_CAIRO_XCB_H */
