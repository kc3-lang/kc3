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
#include <stdlib.h>
#include <cairo/cairo.h>
#include <libc3/c3.h>
#include "../../../window.h"
#include "../../c3_window_cairo_demo.h"
#include "../window_cairo_xcb.h"

static bool on_button (s_window_cairo *window, u8 button,
                       sw x, sw y);
static bool on_key (s_window_cairo *window, uw key);

int main ()
{
  s_window_cairo window;
  window_cairo_init(&window, 0, 0, 800, 600,
                    "C3.Window.Cairo.XCB demo",
                    LIBC3_WINDOW_CAIRO_DEMO_SEQUENCE_COUNT);
  window.button = on_button;
  window.key    = on_key;
  window.load   = c3_window_cairo_demo_load;
  window.render = c3_window_cairo_demo_render;
  window.resize = c3_window_cairo_demo_resize;
  if (! window_cairo_xcb_run(&window))
    return g_c3_exit_code;
  return 0;
}

static bool on_button (s_window_cairo *window, u8 button, sw x, sw y)
{
  assert(window);
  (void) window;
  (void) button;
  (void) x;
  (void) y;
  printf("on_button: %lu (%ld, %ld)\n", (uw) button, x, y);
  return true;
}

static bool on_key (s_window_cairo *window, uw key)
{
  assert(window);
  (void) window;
  switch (key) {
  case 9:
  case 38:
    g_c3_exit_code = 0;
    return false;
  case 100:
    if (! window_set_sequence_pos((s_window *) window,
                                  (window->sequence_pos +
                                   window->sequence_count - 1) %
                                  window->sequence_count))
      return false;
    break;
  case 102:
    if (! window_set_sequence_pos((s_window *) window,
                                  (window->sequence_pos + 1) %
                                  window->sequence_count))
      return false;
    break;
  default:
    printf("on_key: %lu\n", key);
  }
  return true;
}
