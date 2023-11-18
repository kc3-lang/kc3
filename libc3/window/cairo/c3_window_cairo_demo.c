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
#include <math.h>
#include <stdio.h>
#include <time.h>
#include <libc3/c3.h>
#include <cairo/cairo.h>
#include <xkbcommon/xkbcommon.h>
#include "../window.h"
#include "c3_window_cairo_demo.h"
#include "types.h"

bool c3_window_cairo_demo_button (s_window_cairo *window, u8 button,
                                  sw x, sw y)
{
  assert(window);
  (void) window;
  printf("c3_window_cairo_demo_button: %lu (%ld, %ld)\n", (uw) button, x, y);
  return true;
}

bool c3_window_cairo_demo_key (s_window_cairo *window, uw keysym)
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
    printf("c3_window_cairo_demo_key: %lu %s\n", keysym, keysym_name);
  }
  return true;
}

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
  cairo_text_extents_t te;
  assert(window);
  assert(cr);
  if (! window_animate((s_window *) window))
    return false;
  seq = window->sequence + window->sequence_pos;
  /* background */
  switch (window->sequence_pos) {
  case 0:
    c3_window_cairo_demo_render_bg_rect(window, cr, seq);
    break;
  default:
    cairo_set_source_rgb(cr, 1, 1, 1);
    cairo_rectangle(cr, 0, 0, window->w, window->h);
    cairo_fill(cr);
  }
  /* text */
  cairo_set_source_rgb (cr, 0.0, 0.0, 0.0);
  cairo_set_font_size (cr, 20);
  cairo_select_font_face (cr, "CourierNew",
                          CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
  cairo_text_extents (cr, seq->title, &te);
  cairo_move_to (cr, 20, window->h - te.height - te.y_bearing - 20);
  cairo_show_text (cr, seq->title);
  /* progress bar */
  cairo_rectangle(cr, 20, window->h - 11,
                  (window->w - 40.0) * seq->t / seq->duration,
                  2);
  cairo_fill(cr);
  return true;
}

void c3_window_cairo_demo_render_bg_rect (s_window_cairo *window,
                                          cairo_t *cr,
                                          s_sequence *seq)
{
#define C3_WINDOW_CAIRO_DEMO_BG_RECT_COLOR_MAX 8
  const s_rgb color[C3_WINDOW_CAIRO_DEMO_BG_RECT_COLOR_MAX] = {
    {1, 1, 1},
    {1, 0, 0},
    {1, 1, 0},
    {0, 1, 0},
    {0, 1, 1},
    {0, 0, 1},
    {1, 0, 1},
    {0, 0, 0}
  };
  u8 c1;
  u8 c2;
  double p;
  double q;
  s_rgb rgb;
  c1 = (u8) trunc(seq->t) % C3_WINDOW_CAIRO_DEMO_BG_RECT_COLOR_MAX;
  c2 = (u8) trunc(seq->t + 1.0) % C3_WINDOW_CAIRO_DEMO_BG_RECT_COLOR_MAX;
  p = fmod(seq->t, 1.0);
  q = 1.0 - p;
  rgb.r = color[c1].r * q + color[c2].r * p;
  rgb.g = color[c1].g * q + color[c2].g * p;
  rgb.b = color[c1].b * q + color[c2].b * p;
  cairo_set_source_rgb(cr, rgb.r, rgb.g, rgb.b);
  cairo_rectangle(cr, 0, 0, window->w, window->h);
  cairo_fill(cr);
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
