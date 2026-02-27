/* kc3
 * Copyright from 2022 to 2026 kmx.io <contact@kmx.io>
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
#include "../../../libkc3/kc3.h"
#include "lightspeed.h"

static void star_init (s_tag *star)
{
  f64 x;
  f64 y;
  f64_random(&x);
  f64_random(&y);
  tag_map(star, 3);
  tag_init_psym(star->data.td_map.key   + 0, sym_1("speed"));
  tag_init_f64( star->data.td_map.value + 0, 0.0);
  tag_init_psym(star->data.td_map.key   + 1, sym_1("x"));
  tag_init_f64( star->data.td_map.value + 1, 2.0 * x - 1.0);
  tag_init_psym(star->data.td_map.key   + 2, sym_1("y"));
  tag_init_f64( star->data.td_map.value + 2, 2.0 * y - 1.0);
}

static void star_render (s_tag *star, s_window_cairo *window,
                         cairo_t *cr, s_sequence *seq)
{
  f64 q;
  f64 *speed;
  f64 *x;
  f64 *y;
  (void) window;
  speed = &star->data.td_map.value[0].data.td_f64;
  x = &star->data.td_map.value[1].data.td_f64;
  y = &star->data.td_map.value[2].data.td_f64;
  cairo_set_line_width(cr, 0.004);
  cairo_set_source_rgb(cr, 1, 1, 1);
  cairo_move_to(cr, *x, *y);
  q = (1 + *speed / 20);
  cairo_line_to(cr, *x * q, *y * q);
  cairo_stroke(cr);
  q = (1 + *speed / 100);
  *x = *x * q;
  *y = *y * q;
  *speed += seq->dt;
  if (*x < -1.0 || *x > 1.0 || *y < -1.0 || *y > 1.0)
    star_init(star);
}

bool lightspeed_load (s_sequence *seq)
{
  uw i;
  tag_ptuple(&seq->tag, LIGHTSPEED_STARS);
  i = 0;
  while (i < LIGHTSPEED_STARS) {
    star_init(seq->tag.data.td_ptuple->tag + i);
    i++;
  }
  return true;
}

bool lightspeed_render (s_sequence *seq)
{
  cairo_t *cr;
  uw i;
  cairo_matrix_t matrix;
  s_window_cairo *window;
  window = seq->window;
  cr = window->cr;
  cairo_get_matrix(cr, &matrix);
  cairo_scale(cr, window->w / 2.0, window->h / 2.0);
  cairo_translate(cr, 1.0, 1.0);
  cairo_set_source_rgb(cr, 0, 0, 0);
  cairo_rectangle(cr, -1, -1, 2, 2);
  cairo_fill(cr);
  i = 0;
  while (i < LIGHTSPEED_STARS) {
    star_render(seq->tag.data.td_ptuple->tag + i, window, cr, seq);
    i++;
  }
  cairo_set_matrix(cr, &matrix);
  return true;
}

bool lightspeed_unload (s_sequence *seq)
{
  (void) seq;
  return true;
}
