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
#include <math.h>
#include <stdlib.h>
#include <libc3/c3.h>
#include <xkbcommon/xkbcommon.h>
#include "../../window.h"
#include "../cairo_font.h"
#include "../cairo_sprite.h"
#include "../window_cairo.h"
#include "window_cairo_demo.h"
#include "bg_rect.h"
#include "lightspeed.h"
#include "toasters.h"
#include "flies.h"
#include "mandelbrot_f128.h"

s_cairo_font g_font_computer_modern = {0};
s_cairo_font g_font_courier_new = {0};

bool window_cairo_demo_button (s_window_cairo *window, u8 button,
                               sw x, sw y)
{
  assert(window);
  (void) window;
  io_write_1("c3_window_cairo_demo_button: ");
  io_inspect_u8(&button);
  io_write_1(" (");
  io_inspect_sw(&x);
  io_write_1(", ");
  io_inspect_sw(&y);
  io_puts(")");
  return true;
}

bool window_cairo_demo_key (s_window_cairo *window, uw keysym)
{
  char keysym_name[64];
  assert(window);
  (void) window;
  switch (keysym) {
  case XKB_KEY_Escape:
  case XKB_KEY_q:
    g_c3_exit_code = 0;
    return false;
  case XKB_KEY_Left:
    if (! window_set_sequence_pos((s_window *) window,
                                  (window->sequence_pos +
                                   window->sequence_count - 1) %
                                  window->sequence_count))
      return false;
    break;
  case XKB_KEY_Right:
    if (! window_set_sequence_pos((s_window *) window,
                                  (window->sequence_pos + 1) %
                                  window->sequence_count))
      return false;
    break;
  default:
    xkb_keysym_get_name(keysym, keysym_name, sizeof(keysym_name));
    io_write_1("c3_window_cairo_demo_key: ");
    io_inspect_uw(&keysym);
    io_write_1(" ");
    io_puts(keysym_name);
  }
  return true;
}

bool window_cairo_demo_load (s_window_cairo *window)
{
  assert(window);
  if (window->sequence_count != WINDOW_CAIRO_DEMO_SEQUENCE_COUNT) {
    err_write_1("window_cairo_demo_load: window->sequence_count = ");
    err_inspect_uw(&window->sequence_count);
    err_write_1("\n");
    assert(window->sequence_count == WINDOW_CAIRO_DEMO_SEQUENCE_COUNT);
    return false;
  }
  if (! cairo_font_init(&g_font_courier_new,
                        "fonts/Courier New/Courier New.ttf"))
    return false;
  sequence_init(window->sequence, 8.0, "01. Background rectangles",
                bg_rect_load, bg_rect_render, bg_rect_unload, window);
  sequence_init(window->sequence + 1, 20.0, "02. Lightspeed",
                lightspeed_load, lightspeed_render, lightspeed_unload,
                window);
  if (! cairo_sprite_init(&g_sprite_toaster, "img/flaps.png",
                          4, 1, 4))
    return false;
  if (! cairo_sprite_init(&g_sprite_toast, "img/toast.png",
                          1, 1, 1))
    return false;
  sequence_init(window->sequence + 2, 60.0, "03. Toasters",
                toasters_load, toasters_render, toasters_unload,
                window);
  if (! cairo_sprite_init(&g_sprite_fly, "img/fly-noto.png",
                          1, 1, 1))
    return false;
  if (! cairo_sprite_init(&g_sprite_dead_fly, "img/fly-dead.png",
                          1, 1, 1))
    return false;
  sequence_init(window->sequence + 3, 60.0, "04. Flies",
                flies_load, flies_render, flies_unload, window);
  sequence_init(window->sequence + 4, 3600.0, "05. Mandelbrot (f128)",
                mandelbrot_f128_load, mandelbrot_f128_render,
                mandelbrot_f128_unload, window);
  window_set_sequence_pos((s_window *) window, 0);
  return true;
}

static void render_text (cairo_t *cr, double x, double y,
                         const char *p)
{
  cairo_set_source_rgb(cr, 1.0, 1.0, 1.0);
  cairo_move_to(cr, x - 1.0, y - 1.0);
  cairo_show_text(cr, p);
  cairo_move_to(cr, x - 1.0, y);
  cairo_show_text(cr, p);
  cairo_move_to(cr, x - 1.0, y + 1.0);
  cairo_show_text(cr, p);
  cairo_move_to(cr, x, y - 1.0);
  cairo_show_text(cr, p);
  cairo_move_to(cr, x, y + 1.0);
  cairo_show_text(cr, p);
  cairo_move_to(cr, x + 1.0, y - 1.0);
  cairo_show_text(cr, p);
  cairo_move_to(cr, x + 1.0, y);
  cairo_show_text(cr, p);
  cairo_move_to(cr, x + 1.0, y + 1.0);
  cairo_show_text(cr, p);
  /* text */
  cairo_set_source_rgb(cr, 0.0, 0.0, 0.0);
  cairo_move_to(cr, x, y);
  cairo_show_text(cr, p);
}

bool window_cairo_demo_render (s_window_cairo *window)
{
  cairo_t *cr;
  s_sequence *seq;
  cairo_text_extents_t te;
  assert(window);
  cr = window->cr;
  assert(cr);
  if (! window_animate((s_window *) window))
    return false;
  seq = window->sequence + window->sequence_pos;
  if (! seq->render(seq))
    return false;
  /* text */
  cairo_identity_matrix(cr);
  cairo_set_font_size(cr, 20);
  cairo_set_font(cr, &g_font_courier_new);
  cairo_text_extents(cr, seq->title, &te);
  render_text(cr, 20.0, window->h - te.height - te.y_bearing - 20,
              seq->title);
  /* progress bar */
  cairo_set_source_rgb(cr, 1.0, 1.0, 1.0);
  cairo_rectangle(cr, 19, window->h - 12,
                  (window->w - 40.0) * seq->t / seq->duration + 2,
                  4);
  cairo_fill(cr);
  cairo_set_source_rgb(cr, 0.0, 0.0, 0.0);
  cairo_rectangle(cr, 20, window->h - 11,
                  (window->w - 40.0) * seq->t / seq->duration,
                  2);
  cairo_fill(cr);
  /* fps */
  char fps[32];
  snprintf(fps, sizeof(fps), "%f", (f64) seq->frame / seq->t);
  cairo_text_extents(cr, fps, &te);
  render_text(cr, 20.0, 20.0 + te.height, fps);
  return true;
}

bool window_cairo_demo_resize (s_window_cairo *window,
                                  uw w, uw h)
{
  assert(window);
  (void) window;
  (void) w;
  (void) h;
  return true;
}

void window_cairo_demo_unload (s_window_cairo *window)
{
  assert(window);
  (void) window;
  cairo_font_clean(&g_font_courier_new);
  cairo_sprite_clean(&g_sprite_toaster);
  cairo_sprite_clean(&g_sprite_toast);
  cairo_sprite_clean(&g_sprite_fly);
  cairo_sprite_clean(&g_sprite_dead_fly);
}
