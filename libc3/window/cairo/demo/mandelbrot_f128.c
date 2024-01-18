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
#include <libc3/c3.h>
#include "mandelbrot_f128.h"

static cairo_surface_t *g_mandelbrot_f128_surface = NULL;

static bool mandelbrot_f128_resize (s_sequence *seq);
static bool mandelbrot_f128_update (s_sequence *seq);

bool mandelbrot_f128_load (s_sequence *seq)
{
  s_map *map;
  assert(seq);
  if (! tag_map(&seq->tag, 9))
    return false;
  map = &seq->tag.data.map;
  tag_init_sym(    map->key + 0, sym_1("h"));
  tag_init_uw(   map->value + 0, 0);
  tag_init_sym(    map->key + 1, sym_1("next_x"));
  tag_init_f128( map->value + 1, 0.0);
  tag_init_sym(    map->key + 2, sym_1("next_y"));
  tag_init_f128( map->value + 2, 0.0);
  tag_init_sym(    map->key + 3, sym_1("next_z"));
  tag_init_f128( map->value + 3, 0.01);
  tag_init_sym(    map->key + 4, sym_1("pixels"));
  tag_init_array(map->value + 4, sym_1("U8"), 0, NULL);
  tag_init_sym(    map->key + 5, sym_1("w"));
  tag_init_uw(   map->value + 5, 0);
  tag_init_sym(    map->key + 6, sym_1("x"));
  tag_init_f128( map->value + 6, 0.0);
  tag_init_sym(    map->key + 7, sym_1("y"));
  tag_init_f128( map->value + 7, 0.0);
  tag_init_sym(    map->key + 8, sym_1("z"));
  tag_init_f128( map->value + 8, 0.0);
  return true;
}

bool mandelbrot_f128_render (s_sequence *seq)
{
  cairo_t *cr;
  uw *h;
  s_map *map;
  f128 next_x;
  f128 next_y;
  f128 next_z;
  uw *w;
  s_window_cairo *win;
  f128 *x;
  f128 *y;
  f128 *z;
  assert(seq);
  win = seq->window;
  assert(win);
  cr = win->cr;
  assert(seq->tag.type == TAG_MAP);
  map = &seq->tag.data.map;
  assert(map->count == 9);
  assert(map->key[0].type == TAG_SYM);
  assert(map->key[0].data.sym == sym_1("h"));
  assert(map->value[0].type == TAG_UW);
  h = &map->value[0].data.uw;
  assert(map->key[1].type == TAG_SYM);
  assert(map->key[1].data.sym == sym_1("next_x"));
  assert(map->value[1].type == TAG_F128);
  next_x = map->value[1].data.f128;
  assert(map->key[2].type == TAG_SYM);
  assert(map->key[2].data.sym == sym_1("next_y"));
  assert(map->value[2].type == TAG_F128);
  next_y = map->value[2].data.f128;
  assert(map->key[3].type == TAG_SYM);
  assert(map->key[3].data.sym == sym_1("next_z"));
  assert(map->value[3].type == TAG_F128);
  next_z = map->value[3].data.f128;
  assert(map->key[5].type == TAG_SYM);
  assert(map->key[5].data.sym == sym_1("w"));
  assert(map->value[5].type == TAG_UW);
  w = &map->value[5].data.uw;
  assert(map->key[6].type == TAG_SYM);
  assert(map->key[6].data.sym == sym_1("x"));
  assert(map->value[6].type == TAG_F128);
  x = &map->value[6].data.f128;
  assert(map->key[7].type == TAG_SYM);
  assert(map->key[7].data.sym == sym_1("y"));
  assert(map->value[7].type == TAG_F128);
  y = &map->value[7].data.f128;
  assert(map->key[8].type == TAG_SYM);
  assert(map->key[8].data.sym == sym_1("z"));
  assert(map->value[8].type == TAG_F128);
  z = &map->value[8].data.f128;
  if (! g_mandelbrot_f128_surface ||
      *w != win->w || *h != win->h)
    if (! mandelbrot_f128_resize(seq))
      return false;
  if (*w != win->w || *h != win->h ||
      *x != next_x || *y != next_y || *z != next_z) {
    mandelbrot_f128_update(seq);
    *w = win->w;
    *h = win->h;
    *x = next_x;
    *y = next_y;
    *z = next_z;
  }
  cairo_identity_matrix(cr);
  cairo_set_source_surface(cr, g_mandelbrot_f128_surface, 0, 0);
  cairo_rectangle(cr, 0, 0, win->w, win->h);
  cairo_fill(cr);
  return true;
}

static bool mandelbrot_f128_resize (s_sequence *seq)
{
  s_window_cairo *win;
  assert(seq);
  win = seq->window;
  assert(win);
  cairo_surface_destroy(g_mandelbrot_f128_surface);
  g_mandelbrot_f128_surface =
    cairo_image_surface_create(CAIRO_FORMAT_RGB24,
                               win->w, win->h);
  return true;
}

bool mandelbrot_f128_unload (s_sequence *seq)
{
  (void) seq;
  cairo_surface_destroy(g_mandelbrot_f128_surface);
  g_mandelbrot_f128_surface = NULL;
  return true;
}

static bool mandelbrot_f128_update (s_sequence *seq)
{
  f128 _2z_xz_y;
  f128 c_x;
  f128 c_y;
  u8 *data;
  uw i;
  uw j;
  u8 k;
  u8 level;
  s_map *map;
  f128 next_x;
  f128 next_y;
  f128 next_z;
  u8 *pix;
  uw stride;
  s_window_cairo *win;
  f128 z_x;
  f128 z_y;
  f128 z_x2;
  f128 z_y2;
  assert(seq);
  assert(seq->window);
  win = seq->window;
  assert(seq->tag.type == TAG_MAP);
  map = &seq->tag.data.map;
  assert(map->count == 9);
  assert(map->key[1].type == TAG_SYM);
  assert(map->key[1].data.sym == sym_1("next_x"));
  assert(map->value[1].type == TAG_F128);
  next_x = map->value[1].data.f128;
  assert(map->key[2].type == TAG_SYM);
  assert(map->key[2].data.sym == sym_1("next_y"));
  assert(map->value[2].type == TAG_F128);
  next_y = map->value[2].data.f128;
  assert(map->key[3].type == TAG_SYM);
  assert(map->key[3].data.sym == sym_1("next_z"));
  assert(map->value[3].type == TAG_F128);
  next_z = map->value[3].data.f128;
  cairo_surface_flush(g_mandelbrot_f128_surface);
  data = cairo_image_surface_get_data(g_mandelbrot_f128_surface);
  assert(data);
  stride = cairo_image_surface_get_stride(g_mandelbrot_f128_surface);
  i = 0;
  while (i < win->h) {
    c_y = next_y + next_z * ((f128) i - win->h / 2);
    pix = data + i * stride;
    j = 0;
    while (j < win->w) {
      c_x = next_x + next_z * ((f128) j - win->w / 2);
      z_x = c_x;
      z_y = c_y;
      k = 0;
      z_x2 = z_x * z_x;
      z_y2 = z_y * z_y;
      while (k < 255 && z_x2 + z_y2 < 4) {
        _2z_xz_y = 2 * z_x * z_y;
        z_x = c_x + z_x2 - z_y2;
        z_y = c_y + _2z_xz_y;
        z_x2 = z_x * z_x;
        z_y2 = z_y * z_y;
        k++;
      }
      level = 255 - k;
      /*if (k)
        printf("x %lu, y %lu, k %d, level %d", j, i, k, level);*/
      pix[0] = level;
      pix[1] = level;
      pix[2] = level;
      pix += 4;
      j++;
    }
    i++;
  }
  cairo_surface_mark_dirty(g_mandelbrot_f128_surface);
  return true;
}
