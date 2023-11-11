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
#ifndef WINDOW_CAIRO_XCB_H
#define WINDOW_CAIRO_XCB_H

#include <libc3/types.h>
#include <xcb/xcb.h>
#include "../window_cairo.h"

bool window_cairo_xcb (sw x, sw y, sw w, sw h,
                       const s8 *title,
                       f_window_cairo_render render);

xcb_visualtype_t * xcb_screen_visual_type (xcb_screen_t *screen);

#endif /* ndef WINDOW_CAIRO_XCB_H */
