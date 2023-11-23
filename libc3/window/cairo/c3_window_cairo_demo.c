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
#include <stdlib.h>
#include <time.h>
#include <libc3/c3.h>
#include <cairo/cairo.h>
#include <xkbcommon/xkbcommon.h>
#include "../window.h"
#include "window_cairo.h"
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
  assert(window->sequence_count == C3_WINDOW_CAIRO_DEMO_SEQUENCE_COUNT);
  window_cairo_sequence_init(window->sequence, 8.0,
                             "01. Background rectangles",
                             c3_window_cairo_demo_render_bg_rect);
  window_cairo_sequence_init(window->sequence + 1, 30.0,
                             "02. Lightspeed",
                             c3_window_cairo_demo_render_lightspeed);
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
  seq->render(window, cr, seq);
  /* text */
  cairo_set_font_size(cr, 20);
  cairo_select_font_face(cr, "Courier New",
                          CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
  cairo_text_extents(cr, seq->title, &te);
  /* text bg */
  cairo_set_source_rgb(cr, 1.0, 1.0, 1.0);
  cairo_move_to(cr, 19, window->h - te.height - te.y_bearing - 21);
  cairo_show_text(cr, seq->title);
  cairo_move_to(cr, 19, window->h - te.height - te.y_bearing - 20);
  cairo_show_text(cr, seq->title);
  cairo_move_to(cr, 19, window->h - te.height - te.y_bearing - 19);
  cairo_show_text(cr, seq->title);
  cairo_move_to(cr, 20, window->h - te.height - te.y_bearing - 21);
  cairo_show_text(cr, seq->title);
  cairo_move_to(cr, 20, window->h - te.height - te.y_bearing - 19);
  cairo_show_text(cr, seq->title);
  cairo_move_to(cr, 21, window->h - te.height - te.y_bearing - 19);
  cairo_show_text(cr, seq->title);
  cairo_move_to(cr, 21, window->h - te.height - te.y_bearing - 20);
  cairo_show_text(cr, seq->title);
  cairo_move_to(cr, 21, window->h - te.height - te.y_bearing - 21);
  cairo_show_text(cr, seq->title);
  /* text */
  cairo_set_source_rgb(cr, 0.0, 0.0, 0.0);
  cairo_move_to(cr, 20, window->h - te.height - te.y_bearing - 20);
  cairo_show_text(cr, seq->title);
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

f64 frandom (void)
{
  return (double) random() / (((sw) 1 << 31) - 1);
}

static void star_init (s_tag *star)
{
  tag_init_map(star, 3);
  tag_init_sym(star->data.map.keys, sym_1("speed"));
  tag_init_f64(star->data.map.values, 0.0);
  tag_init_sym(star->data.map.keys + 1, sym_1("x"));
  tag_init_f64(star->data.map.values + 1, 2.0 * frandom() - 1.0);
  tag_init_sym(star->data.map.keys + 2, sym_1("y"));
  tag_init_f64(star->data.map.values + 2, 2.0 * frandom() - 1.0);
}

static void star_render (s_tag *star, s_window_cairo *window,
                         cairo_t *cr, s_sequence *seq)
{
  f64 q;
  f64 *speed;
  f64 *x;
  f64 *y;
  (void) window;
  speed = &star->data.map.values[0].data.f64;
  x = &star->data.map.values[1].data.f64;
  y = &star->data.map.values[2].data.f64;
  cairo_set_line_width(cr, 0.01);
  cairo_set_source_rgb(cr, 1, 1, 1);
  cairo_move_to(cr, *x, *y);
  q = (1 + *speed / 10);
  cairo_line_to(cr, *x * q, *y * q);
  cairo_stroke(cr);
  q = (1 + *speed / 100);
  *x = *x * q;
  *y = *y * q;
  *speed += seq->dt;
  if (*x < -1.0 || *x > 1.0 || *y < -1.0 || *y > 1.0)
    star_init(star);
}

void c3_window_cairo_demo_render_lightspeed (s_window_cairo *window,
                                             cairo_t *cr,
                                             s_sequence *seq)
{
  uw i;
  cairo_matrix_t matrix;
  cairo_get_matrix(cr, &matrix);
  cairo_scale(cr, window->w / 2.0, window->h / 2.0);
  cairo_translate(cr, 1.0, 1.0);
  cairo_set_source_rgb(cr, 0, 0, 0);
  cairo_rectangle(cr, -1, -1, 2, 2);
  cairo_fill(cr);
  if (seq->data.type == TAG_VOID) {
    tag_init_tuple(&seq->data, C3_WINDOW_CAIRO_DEMO_LIGHTSPEED_STARS);
    i = 0;
    while (i < C3_WINDOW_CAIRO_DEMO_LIGHTSPEED_STARS) {
      star_init(seq->data.data.tuple.tag + i);
      i++;
    }
  }
  i = 0;
  while (i < C3_WINDOW_CAIRO_DEMO_LIGHTSPEED_STARS) {
    star_render(seq->data.data.tuple.tag + i, window, cr, seq);
    i++;
  }
  cairo_set_matrix(cr, &matrix);
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
