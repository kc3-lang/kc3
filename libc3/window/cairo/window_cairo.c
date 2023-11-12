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
#include <assert.h>
#include "window_cairo.h"

s_window_cairo * window_cairo_init (s_window_cairo *window,
                                    sw x, sw y, uw w, uw h,
                                    const s8 *title)
{
  assert(window);
  window->x = x;
  window->y = y;
  window->w = w;
  window->h = h;
  window->title = title ? title : "C3.Window.Cairo";
  window->render = window_cairo_render_default;
  window->resize = window_cairo_resize_default;
  window->cr = NULL;
  return window;
}

bool window_cairo_render_default (s_window_cairo *window, cairo_t *cr)
{
  assert(window);
  assert(cr);
  cairo_set_source_rgb(cr, 1, 1, 1);
  cairo_rectangle(cr, 0, 0, window->w, window->h);
  cairo_fill(cr);
  return true;
}

bool window_cairo_resize_default (s_window_cairo *window, uw w, uw h)
{
  window->w = w;
  window->h = h;
  return true;
}
