/* kc3
 * Copyright 2022,2023,2024 kmx.io <contact@kmx.io>
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
#include <libkc3/kc3.h>
#include "../../../window.h"
#include "../../demo/window_cairo_demo.h"
#include "../window_cairo_quartz.h"

int main (int argc, char **argv)
{
  sw r = 0;
  s_window_cairo window;
  if (! c3_init(NULL, argc, argv))
    return 1;
  c3_window_cairo_init();
  window_cairo_init(&window, 0, 0, 800, 600,
                    "C3.Window.Cairo.Quartz demo",
                    WINDOW_CAIRO_DEMO_SEQUENCE_COUNT);
  window.button = window_cairo_demo_button;
  window.key    = window_cairo_demo_key;
  window.load   = window_cairo_demo_load;
  window.render = window_cairo_demo_render;
  window.resize = window_cairo_demo_resize;
  window.unload = window_cairo_demo_unload;
  if (! window_cairo_quartz_run(&window))
    r = g_c3_exit_code;
  window_cairo_clean(&window);
  c3_window_cairo_clean();
  c3_clean(NULL);
  return r;
}
