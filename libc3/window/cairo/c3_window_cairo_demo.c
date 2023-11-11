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
#include <stdio.h>
#include <libc3/c3.h>
#include <cairo/cairo.h>
#include "c3_window_cairo_demo.h"
#include "types.h"

bool c3_window_cairo_demo_render (s_window_cairo *window,
                                  cairo_t *cr)
{
  assert(window);
  assert(cr);
  (void) window;
  cairo_set_source_rgb(cr, 1, 1, 1);
  cairo_rectangle(cr, 0, 0, window->w, window->h);
  cairo_fill(cr);
  return true;
}

bool c3_window_cairo_demo_resize (s_window_cairo *window,
                                  uw w, uw h)
{
  assert(window);
  window->w = w;
  window->h = h;
  return true;
}
