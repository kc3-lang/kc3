/* kc3
 * Copyright from 2022 to 2025 kmx.io <contact@kmx.io>
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
#include "../../libkc3/io.h"
#include "../../libkc3/tag.h"
#include "../window.h"
#include "cairo_font.h"
#include "window_cairo.h"

void kc3_window_cairo_clean (void)
{
  kc3_window_cairo_font_clean();
}

void kc3_window_cairo_init (void)
{
}

void window_cairo_clean (s_window_cairo *window)
{
  window_clean((s_window *) window);
}

s_window_cairo * window_cairo_init (s_window_cairo *window,
                                    s64 x, s64 y, u64 w, u64 h,
                                    const char *title,
                                    u64 sequence_count)
{
  s_window_cairo tmp = {0};
  assert(window);
  title = title ? title : "KC3.Window.Cairo";
  window_init((s_window *) &tmp, x, y, w, h, title, sequence_count);
  tmp.button = window_cairo_button_default;
  tmp.key    = window_cairo_key_default;
  tmp.load   = window_cairo_load_default;
  tmp.motion = window_cairo_motion_default;
  tmp.render = window_cairo_render_default;
  tmp.resize = window_cairo_resize_default;
  tmp.unload = window_cairo_unload_default;
  *window = tmp;
  return window;
}

bool window_cairo_button_default (s_window_cairo *window, u8 button,
                                  s64 x, s64 y)
{
  assert(window);
  (void) window;
  err_write_1("window_cairo_button_default: ");
  err_inspect_u8(button);
  err_write_1(" (");
  err_inspect_s64_decimal(x);
  err_write_1(", ");
  err_inspect_s64_decimal(y);
  err_write_1("\n");
  return true;
}

bool window_cairo_key_default (s_window_cairo *window, u32 keysym)
{
  char keysym_name[64];
  assert(window);
  (void) window;
  xkb_keysym_get_name(keysym, keysym_name, sizeof(keysym_name));
  err_write_1("window_cairo_key_default: ");
  err_inspect_u32_decimal(keysym);
  err_write_1(" ");
  err_puts(keysym_name);
  return true;
}

bool window_cairo_load_default (s_window_cairo *window)
{
  assert(window);
  (void) window;
  err_puts("window_cairo_load_default");
  return true;
}

bool window_cairo_motion_default (s_window_cairo *window, s64 x, s64 y)
{
  assert(window);
  (void) window;
  (void) x;
  (void) y;
  /*
  printf("window_cairo_motion_default (%lld, %lld)\n", x, y);
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
  err_puts("window_cairo_render_default");
  return true;
}

bool window_cairo_resize_default (s_window_cairo *window, u64 w, u64 h)
{
  assert(window);
  (void) window;
  (void) w;
  (void) h;
  err_write_1("window_cairo_resize_default: ");
  err_inspect_u64(w);
  err_write_1(" x ");
  err_inspect_u64(h);
  err_write_1("\n");
  return true;
}

void window_cairo_unload_default (s_window_cairo *window)
{
  assert(window);
  (void) window;
}
