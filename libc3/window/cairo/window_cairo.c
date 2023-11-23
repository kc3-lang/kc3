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
#include <stdlib.h>
#include <xkbcommon/xkbcommon.h>
#include "window_cairo.h"

s_window_cairo * window_cairo_init (s_window_cairo *window,
                                    sw x, sw y, uw w, uw h,
                                    const s8 *title,
                                    uw sequence_count)
{
  assert(window);
  window->x = x;
  window->y = y;
  window->w = w;
  window->h = h;
  window->button = window_cairo_button_default;
  window->key = window_cairo_key_default;
  window->load = window_cairo_load_default;
  window->motion = window_cairo_motion_default;
  window->render = window_cairo_render_default;
  window->resize = window_cairo_resize_default;
  window->cr = NULL;
  window->sequence = calloc(sequence_count, sizeof(s_sequence));
  window->sequence_count = sequence_count;
  window->sequence_pos = 0;
  window->title = title ? title : "C3.Window.Cairo";
  return window;
}

bool window_cairo_button_default (s_window_cairo *window, u8 button,
                                  sw x, sw y)
{
  assert(window);
  (void) window;
  (void) button;
  (void) x;
  (void) y;
  printf("window_cairo_button_default: %d (%ld, %ld)\n",
         (int) button, x, y);
  return true;
}

bool window_cairo_key_default (s_window_cairo *window, uw keysym)
{
  char keysym_name[64];
  assert(window);
  (void) window;
  xkb_keysym_get_name(keysym, keysym_name, sizeof(keysym_name));
  printf("window_cairo_key_default: %lu %s\n", keysym, keysym_name);
  return true;
}

bool window_cairo_load_default (s_window_cairo *window)
{
  assert(window);
  (void) window;
  printf("window_cairo_load_default\n");
  return true;
}

bool window_cairo_motion_default (s_window_cairo *window, sw x, sw y)
{
  assert(window);
  (void) window;
  (void) x;
  (void) y;
  /*
  printf("window_cairo_motion_default (%ld, %ld)\n", x, y);
  */
  return true;
}

bool window_cairo_render_default (s_window_cairo *window, cairo_t *cr)
{
  assert(window);
  assert(cr);
  cairo_set_source_rgb(cr, 1, 1, 1);
  cairo_rectangle(cr, 0, 0, window->w, window->h);
  cairo_fill(cr);
  printf("window_cairo_render_default\n");
  return true;
}

bool window_cairo_resize_default (s_window_cairo *window, uw w, uw h)
{
  assert(window);
  (void) window;
  (void) w;
  (void) h;
  printf("window_cairo_resize_default: %lu x %lu\n", w, h);
  return true;
}

s_sequence * window_cairo_sequence_init
(s_sequence *sequence, f64 duration, const s8 *title,
 f_window_cairo_render_sequence render)
{
  assert(sequence);
  sequence->t = 0.0;
  sequence->duration = duration;
  sequence->title = title;
  sequence->render = (f_sequence_render) render;
  return sequence;
}
