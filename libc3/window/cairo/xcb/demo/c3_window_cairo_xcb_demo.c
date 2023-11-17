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

int main (void)
{
  s_window_cairo window;
  window_cairo_init(&window, 0, 0, 800, 600,
                    "C3.Window.Cairo.XCB demo",
                    LIBC3_WINDOW_CAIRO_DEMO_SEQUENCE_COUNT);
  window.button = c3_window_cairo_demo_button;
  window.key    = c3_window_cairo_demo_key;
  window.load   = c3_window_cairo_demo_load;
  window.render = c3_window_cairo_demo_render;
  window.resize = c3_window_cairo_demo_resize;
  if (! window_cairo_xcb_run(&window))
    return g_c3_exit_code;
  return 0;
}
