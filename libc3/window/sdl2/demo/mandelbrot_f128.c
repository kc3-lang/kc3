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
#include "../gl_font.h"
#include "../mat4.h"
#include "../gl_ortho.h"
#include "../gl_text.h"
#include "mandelbrot_f128.h"
#include "window_sdl2_demo.h"

static s_gl_font g_mandelbrot_f128_font = {0};
static s_gl_text g_mandelbrot_f128_text = {0};
static GLuint g_mandelbrot_f128_texture = 0;

static bool mandelbrot_f128_resize (s_sequence *seq);
static bool mandelbrot_f128_update (s_sequence *seq);

bool mandelbrot_f128_button (s_sequence *seq, u8 button, sw x, sw y)
{
  s_map *map;
  f128 *next_x;
  f128 *next_y;
  f128 *next_z;
  s_window_sdl2 *win;
  assert(seq);
  win = seq->window;
  assert(win);
  assert(seq->tag.type == TAG_MAP);
  map = &seq->tag.data.map;
  assert(map->count == 9);
  assert(map->key[1].type == TAG_SYM);
  assert(map->key[1].data.sym == sym_1("next_x"));
  assert(map->value[1].type == TAG_F128);
  next_x = &map->value[1].data.f128;
  assert(map->key[2].type == TAG_SYM);
  assert(map->key[2].data.sym == sym_1("next_y"));
  assert(map->value[2].type == TAG_F128);
  next_y = &map->value[2].data.f128;
  assert(map->key[3].type == TAG_SYM);
  assert(map->key[3].data.sym == sym_1("next_z"));
  assert(map->value[3].type == TAG_F128);
  next_z = &map->value[3].data.f128;
  if (button == 1) {
    *next_x = *next_x + *next_z * (x - (f128) win->w / 2);
    *next_y = *next_y + *next_z * ((f128) win->h / 2 - y);
  }
  else if (button == 5) {
    *next_z = *next_z * exp2l(0.5);
  }
  else if (button == 4) {
    *next_z = *next_z * exp2l(-0.5);
  }
  return true;
}

bool mandelbrot_f128_load (s_sequence *seq)
{
  f32 point_per_pixel;
  s_map *map;
  s_window_sdl2 *window;
  assert(seq);
  window = seq->window;
  assert(window);
  point_per_pixel = (f32) window->w / window->gl_w;
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
  tag_init_array(map->value + 4, sym_1("U8[]"), 0, NULL);
  tag_init_sym(    map->key + 5, sym_1("w"));
  tag_init_uw(   map->value + 5, 0);
  tag_init_sym(    map->key + 6, sym_1("x"));
  tag_init_f128( map->value + 6, 0.0);
  tag_init_sym(    map->key + 7, sym_1("y"));
  tag_init_f128( map->value + 7, 0.0);
  tag_init_sym(    map->key + 8, sym_1("z"));
  tag_init_f128( map->value + 8, 0.0);
  if (! gl_font_init(&g_mandelbrot_f128_font,
                     "fonts/Courier New/Courier New.ttf",
                     point_per_pixel))
    return false;
  gl_font_set_size(&g_mandelbrot_f128_font, 20.0);
  if (! gl_text_init_1(&g_mandelbrot_f128_text, &g_mandelbrot_f128_font,
                       "x: 0.0\n"
                       "y: 0.0\n"
                       "z: 0.01"))
    return false;
  gl_text_update(&g_mandelbrot_f128_text);
  assert(glGetError() == GL_NO_ERROR);
  glGenTextures(1, &g_mandelbrot_f128_texture);
  assert(glGetError() == GL_NO_ERROR);
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
  glClearColor(0, 0, 0, 0);
  glClear(GL_COLOR_BUFFER_BIT);
  mat4_init_identity(&g_ortho.model_matrix);
  gl_ortho_bind_texture(&g_ortho, g_mandelbrot_f128_texture);
  gl_ortho_rect(&g_ortho, 0, 0, win->w, win->h);
  gl_ortho_text_render_outline(&g_ortho, &g_mandelbrot_f128_text,
                               20.0, 66.0);
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
  if (! array_init(pixels, sym_1("U8[]"), 3, dim))
    return false;
  if (! array_allocate(pixels))
    return false;
  printf("mandelbrot_f128_resize: %lux%lu\n", win->w, win->h);
  return true;
}

bool mandelbrot_f128_unload (s_sequence *seq)
{
  (void) seq;
  glDeleteTextures(1, &g_mandelbrot_f128_texture);
  gl_text_clean(&g_mandelbrot_f128_text);
  gl_font_clean(&g_mandelbrot_f128_font);
  return true;
}

static bool mandelbrot_f128_update (s_sequence *seq)
{
  f128 _2z_xz_y;
  char a[512];
  s_buf buf;
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
  s_array *pixels;
  u8 *pix;
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
    c_y = next_y + next_z * ((f128) i - win->h / 2);
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
      pix[3] = 255;
      pix += 4;
      j++;
    }
    i++;
  }
  assert(glGetError() == GL_NO_ERROR);
  glBindTexture(GL_TEXTURE_2D, g_mandelbrot_f128_texture);
  assert(glGetError() == GL_NO_ERROR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  assert(glGetError() == GL_NO_ERROR);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, win->w, win->h, 0,
               GL_RGBA, GL_UNSIGNED_BYTE, pixels->data);
  assert(glGetError() == GL_NO_ERROR);
  buf_init(&buf, false, sizeof(a), a);
  buf_write_1(&buf, "x: ");
  buf_inspect_f128(&buf, &next_x);
  buf_write_1(&buf, "\ny: ");
  buf_inspect_f128(&buf, &next_y);
  buf_write_1(&buf, "\nz: ");
  buf_inspect_f128(&buf, &next_z);
  gl_text_update_buf(&g_mandelbrot_f128_text, &buf);
  printf("mandelbrot_f128_update: %lux%lu\n", win->w, win->h);
  return true;
}
