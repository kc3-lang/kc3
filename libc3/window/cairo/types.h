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
/** @file types.h
 *  @brief Module C3.Window.Cairo
 *
 *  Struct for all GUI window Cairo graphics operations.
 */
#ifndef LIBC3_WINDOW_CAIRO_TYPES_H
#define LIBC3_WINDOW_CAIRO_TYPES_H

#include <cairo/cairo.h>
#include <libc3/types.h>
#include "../types.h"

typedef struct window_cairo s_window_cairo;

/* return false to break event loop */
typedef bool (*f_window_cairo_render) (s_cairo_window *window,
                                       cairo_t *cr);

/* return false to break event loop */
typedef bool (*f_window_cairo_resize) (s_cairo_window *window,
                                       uw w, uw h);

struct window_cairo {
  /* compatible with s_window */
  sw x;
  sw y;
  uw w;
  uw h;
  f_cairo_window_render render;
  f_cairo_window_resize resize;
  cairo_t *cr;
};

#endif /* LIBC3_WINDOW_CAIRO_TYPES_H */
