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
#include <stdio.h>
#include <time.h>
#include <libc3/c3.h>
#include <cairo/cairo.h>
#include "../window.h"
#include "c3_window_cairo_demo.h"
#include "types.h"

bool c3_window_cairo_demo_load (s_window_cairo *window)
{
  assert(window->sequence_count ==
         LIBC3_WINDOW_CAIRO_DEMO_SEQUENCE_COUNT);
  sequence_init(window->sequence, 8.0, "01. Background rectangles");
  window_set_sequence_pos((s_window *) window, 0);
  return true;
}

bool c3_window_cairo_demo_render (s_window_cairo *window,
                                  cairo_t *cr)
{
  s_sequence *seq;
  assert(window);
  assert(cr);
  if (! window_animate((s_window *) window))
    return false;
  seq = window->sequence + window->sequence_pos;
  switch (window->sequence_pos) {
  case 0:
    if (seq->t < 1.0)
      cairo_set_source_rgb(cr, 1, 1, 1);
    else if (seq->t < 2.0)
      cairo_set_source_rgb(cr, 1, 0, 0);
    else if (seq->t < 3.0)
      cairo_set_source_rgb(cr, 1, 1, 0);
    else if (seq->t < 4.0)
      cairo_set_source_rgb(cr, 0, 1, 0);
    else if (seq->t < 5.0)
      cairo_set_source_rgb(cr, 0, 1, 1);
    else if (seq->t < 6.0)
      cairo_set_source_rgb(cr, 0, 0, 1);
    else if (seq->t < 7.0)
      cairo_set_source_rgb(cr, 1, 0, 1);
    else
      cairo_set_source_rgb(cr, 0, 0, 0);
    break;
  default:
    cairo_set_source_rgb(cr, 1, 1, 1);
  }
  cairo_rectangle(cr, 0, 0, window->w, window->h);
  cairo_fill(cr);
  return true;
}

bool c3_window_cairo_demo_resize (s_window_cairo *window,
                                  uw w, uw h)
{
  assert(window);
  (void) window;
  (void) w;
  (void) h;
  return true;
}
