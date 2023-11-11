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
#include <libc3/c3.h>
#include <cairo/cairo.h>
#include "c3_window_cairo_demo.h"

bool c3_window_cairo_demo_render (cairo_t *cr)
{
  cairo_set_source_rgb(cr, 0, 0, 1);
  cairo_rectangle(cr, 20, 20, 100, 100);
  cairo_fill(cr);
  return true;
}
