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
#include "../types.h"
#include "mandelbrot_f128.h"

bool mandelbrot_f128_load (s_sequence *seq)
{
  s_map *map;
  assert(seq);
  if (! tag_map(&seq->tag, 5))
    return false;
  map = &seq->tag.data.map;
  tag_init_sym(   map->key + 0, sym_1("h"));
  tag_init_uw(  map->value + 0, 0);
  tag_init_sym(   map->key + 1, sym_1("pixels"));
  tag_init_uw(  map->value + 1, 0);
  tag_init_sym(   map->key + 2, sym_1("w"));
  tag_init_uw(  map->value + 2, 0);
  tag_init_sym(   map->key + 3, sym_1("x"));
  tag_init_f128(map->value + 3, 0.0);
  tag_init_sym(   map->key + 4, sym_1("y"));
  tag_init_f128(map->value + 4, 0.0);
  tag_init_sym(   map->key + 5, sym_1("z"));
  tag_init_f128(map->value + 5, 0.01);
  return true;
}

bool mandelbrot_f128_render (s_sequence *seq)
{
  uw *h;
  s_map *map;
  uw *w;
  s_window_sdl2 *win;
  f128 *x;
  f128 *y;
  f128 *z;
  assert(seq);
  assert(seq->window);
  win = seq->window;
  assert(seq->tag.type == TAG_MAP);
  map = &seq->tag.data.map;
  assert(map->key[0].type == TAG_SYM);
  assert(map->key[0].data.sym == sym_1("h"));
  assert(map->value[0].type == TAG_UW);
  h = &map->value[0].data.uw;
  assert(map->key[1].type == TAG_SYM);
  assert(map->key[1].data.sym == sym_1("w"));
  assert(map->value[1].type == TAG_UW);
  w = &map->value[1].data.uw;
  assert(map->key[2].type == TAG_SYM);
  assert(map->key[2].data.sym == sym_1("x"));
  assert(map->value[2].type == TAG_F128);
  x = &map->value[2].data.f128;
  assert(map->key[3].type == TAG_SYM);
  assert(map->key[3].data.sym == sym_1("y"));
  assert(map->value[3].type == TAG_F128);
  y = &map->value[3].data.f128;
  assert(map->key[4].type == TAG_SYM);
  assert(map->key[4].data.sym == sym_1("z"));
  assert(map->value[4].type == TAG_F128);
  z = &map->value[4].data.f128;
  if (*w != win->w || *h != win->h)
    mandelbrot_f128_resize(seq);
  assert(glGetError() == GL_NO_ERROR);
  return true;
}

bool mandelbrot_f128_unload (s_sequence *seq)
{
  (void) seq;
  return true;
}
