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
#include "../gl_deprecated.h"
#include "../gl_matrix_4f.h"
#include "../gl_ortho.h"
#include "mandelbrot_f128.h"
#include "window_sdl2_demo.h"

// FIXME: move g_mandelbrot_f128_texture to seq->tag
static GLuint g_mandelbrot_f128_texture = 0;

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
  tag_init_f128( map->value + 8, 0.01);
  glGenTextures(1, &g_mandelbrot_f128_texture);
  return true;
}

bool mandelbrot_f128_render (s_sequence *seq)
{
  uw *h;
  s_map *map;
  f128 next_x;
  f128 next_y;
  f128 next_z;
  uw *w;
  s_window_sdl2 *win;
  f128 *x;
  f128 *y;
  f128 *z;
  assert(seq);
  assert(glGetError() == GL_NO_ERROR);
  assert(seq->window);
  win = seq->window;
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
  if (*w != win->w || *h != win->h)
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
  gl_matrix_4f_init_identity(&g_ortho.model_matrix);
  gl_ortho_bind_texture(&g_ortho, g_mandelbrot_f128_texture);
  gl_ortho_rect(&g_ortho, 0, 0, win->w, win->h);
  return true;
}

static bool mandelbrot_f128_resize (s_sequence *seq)
{
  uw dim[3];
  s_map *map;
  s_array *pixels;
  s_window_sdl2 *win;
  assert(seq);
  win = seq->window;
  assert(win);
  assert(seq->tag.type == TAG_MAP);
  map = &seq->tag.data.map;
  assert(map->key[4].type == TAG_SYM);
  assert(map->key[4].data.sym == sym_1("pixels"));
  assert(map->value[4].type == TAG_ARRAY);
  pixels = &map->value[4].data.array;
  array_free(pixels);
  dim[0] = win->h;
  dim[1] = win->w;
  dim[2] = 4;
  if (! array_init(pixels, &g_sym_U8, 3, dim))
    return false;
  if (! array_allocate(pixels))
    return false;
  return true;
}

bool mandelbrot_f128_unload (s_sequence *seq)
{
  (void) seq;
  glDeleteTextures(1, &g_mandelbrot_f128_texture);
  return true;
}

static bool mandelbrot_f128_update (s_sequence *seq)
{
  f128 _2z_xz_y;
  f128 c_x;
  f128 c_y;
  uw i;
  uw j;
  u8 k;
  u8 level;
  s_map *map;
  f128 next_x;
  f128 next_y;
  f128 next_z;
  u8 *pix;
  s_array *pixels;
  s_window_sdl2 *win;
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
  assert(map->key[4].type == TAG_SYM);
  assert(map->key[4].data.sym == sym_1("pixels"));
  assert(map->value[4].type == TAG_ARRAY);
  pixels = &map->value[4].data.array;
  pix = pixels->data;
  assert(pix);
  i = 0;
  while (i < win->h) {
    c_y = next_y + next_z * (i - win->h / 2);
    j = 0;
    while (j < win->w) {
      c_x = next_x + next_z * (j - win->w / 2);
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
      level = (f32) k / 255.0f;
      pix[0] = level;
      pix[1] = level;
      pix[2] = level;
      pix[3] = 255;
      pix += 4;
      j++;
    }
    i++;
  }
  glBindTexture(GL_TEXTURE_2D, g_mandelbrot_f128_texture);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, win->w, win->h, 0,
               GL_RGBA, GL_UNSIGNED_BYTE, pixels->data);
  return true;
}
