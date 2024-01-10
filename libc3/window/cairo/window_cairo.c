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
#include <libc3/tag.h>
#include "../window.h"
#include "window_cairo.h"

void c3_window_cairo_clean (void)
{
}

void c3_window_cairo_init (void)
{
}

void window_cairo_clean (s_window_cairo *window)
{
  window_clean((s_window *) window);
}

s_window_cairo * window_cairo_init (s_window_cairo *window,
                                    sw x, sw y, uw w, uw h,
                                    const char *title,
                                    uw sequence_count)
{
  s_window_cairo tmp = {0};
  assert(window);
  title = title ? title : "C3.Window.Cairo";
  window_init((s_window *) &tmp, x, y, w, h, title, sequence_count);
  tmp.button = window_cairo_button_default;
  tmp.key    = window_cairo_key_default;
  tmp.load   = window_cairo_load_default;
  tmp.motion = window_cairo_motion_default;
  tmp.render = window_cairo_render_default;
  tmp.resize = window_cairo_resize_default;
  *window = tmp;
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

bool window_cairo_render_default (s_window_cairo *window)
{
  cairo_t *cr;
  assert(window);
  cr = window->cr;
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
