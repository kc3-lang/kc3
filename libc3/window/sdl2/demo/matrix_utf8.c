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
#include "../gl_font.h"
#include "../gl_ortho.h"
#include "../gl_sprite.h"
#include "../gl_vtext.h"
#include "../mat4.h"
#include "window_sdl2_demo.h"
#include "matrix_utf8.h"

#define            G_MATRIX_UTF8_FONT_SIZE 20

static s_gl_font   g_matrix_utf8_font = {0};
static s_gl_sprite g_matrix_utf8_shade = {0};
static f64         g_matrix_utf8_time;

void matrix_utf8_column_clean (s_tag *tag);
bool matrix_utf8_column_init (s_sequence *seq, s_tag *tag, f32 x);
bool matrix_utf8_column_render (s_sequence *seq, const s_tag *tag);
void matrix_utf8_text_clean (s_tag *tag);
bool matrix_utf8_text_init (s_tag *tag, f32 y);
bool matrix_utf8_text_render (s_sequence *seq, const s_tag *tag);
bool matrix_utf8_update (s_sequence *seq);

void matrix_utf8_column_clean (s_tag *tag)
{
  matrix_utf8_text_clean(tag);
}

bool matrix_utf8_column_init (s_sequence *seq, s_tag *tag, f32 x)
{
  s_window_sdl2 *window;
  (void) x;
  assert(seq);
  window = seq->window;
  assert(window);
  return matrix_utf8_text_init(tag, window->h);
}

bool matrix_utf8_column_render (s_sequence *seq, const s_tag *tag)
{
  return matrix_utf8_text_render(seq, tag);
}

bool matrix_utf8_load (s_sequence *seq)
{
  f32 point_per_pixel;
  s_window_sdl2 *window;
  assert(seq);
  assert(glGetError() == GL_NO_ERROR);
  window = seq->window;
  assert(window);
  point_per_pixel = (f32) window->w / window->gl_w;
  if (! gl_font_init(&g_matrix_utf8_font,
                     "fonts/NotoSans-Regular.ttf",
                     point_per_pixel))
    return false;
  gl_font_set_size(&g_matrix_utf8_font, G_MATRIX_UTF8_FONT_SIZE);
  if (! matrix_utf8_column_init(seq, &seq->tag, 0))
    return false;
  if (! gl_sprite_init(&g_matrix_utf8_shade,
                       "img/matrix_shade.png",
                       1, 1, 1, 1))
    return false;
  g_matrix_utf8_time = seq->t;
  return true;
}

bool matrix_utf8_render (s_sequence *seq)
{
  assert(seq);
  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
  glClear(GL_COLOR_BUFFER_BIT);
  assert(glGetError() == GL_NO_ERROR);
  matrix_utf8_text_render(seq, &seq->tag);
  assert(glGetError() == GL_NO_ERROR);
  return true;
}

bool matrix_utf8_text_init (s_tag *tag, f32 y)
{
  char a[1024];
  s_buf buf;
  u8 i;
  u8 len;
  s_map *map;
  f32 spacing;
  s_gl_text *text;
  if (! tag_map(tag, 3))
    return false;
  buf_init(&buf, false, sizeof(a), a);
  u8_random_uniform(&i, 10);
  spacing = i * G_MATRIX_UTF8_FONT_SIZE;
  u8_random_uniform(&len, 10);
  len += 10;
  text = gl_vtext_new(&g_matrix_utf8_font);
  if (! text ||
      ! gl_vtext_render_to_texture_random(text, len))
    return false;
  map = &tag->data.map;
  tag_init_sym(  map->key + 0, sym_1("spacing"));
  tag_init_f32(map->value + 0, spacing);
  tag_init_sym(  map->key + 1, sym_1("text"));
  tag_init_ptr(map->value + 1, text);
  tag_init_sym(  map->key + 2, sym_1("y"));
  tag_init_f32(map->value + 2, y + text->pt_h);
  return true;
}

bool matrix_utf8_text_render (s_sequence *seq, const s_tag *tag)
{
  u8 i;
  const s_map *map;
  f32 *spacing;
  const s_gl_text *text;
  s_window_sdl2 *window;
  f32 *y;
  assert(seq);
  assert(glGetError() == GL_NO_ERROR);
  window = seq->window;
  assert(window);
  assert(tag);
  assert(tag->type == TAG_MAP);
  map = &tag->data.map;
  assert(map->count == 3);
  assert(      map->key[0].type == TAG_SYM);
  assert(      map->key[0].data.sym == sym_1("spacing"));
  assert(    map->value[0].type == TAG_F32);
  spacing = &map->value[0].data.f32;
  assert(  map->key[1].type == TAG_SYM);
  assert(  map->key[1].data.sym == sym_1("text"));
  assert(map->value[1].type == TAG_PTR);
  text = map->value[1].data.ptr.p;
  assert(  map->key[2].type == TAG_SYM);
  assert(  map->key[2].data.sym == sym_1("y"));
  assert(map->value[2].type == TAG_F32);
  y =   &map->value[2].data.f32;
  if (seq->t - g_matrix_utf8_time > 0.2) {
    *y -= G_MATRIX_UTF8_FONT_SIZE;
    if (*y < 0) {
      u8_random_uniform(&i, 15);
      *spacing = i * G_MATRIX_UTF8_FONT_SIZE;
      *y = window->h + text->pt_h;
    }
    g_matrix_utf8_time = seq->t;
  }
  mat4_init_identity(&g_ortho.model_matrix);
  mat4_translate(&g_ortho.model_matrix, 0, *y, 0);
  gl_ortho_update_model_matrix(&g_ortho);
  assert(glGetError() == GL_NO_ERROR);
  gl_ortho_color(&g_ortho, 0, 1, 0, 1);
  assert(glGetError() == GL_NO_ERROR);
  gl_ortho_vtext_render(&g_ortho, text);
  assert(glGetError() == GL_NO_ERROR);
  glDisable(GL_DEPTH_TEST);
  glEnable(GL_BLEND);
  glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA,
                      GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
  gl_ortho_color(&g_ortho, 1, 1, 1, 1);
  gl_ortho_bind_texture(&g_ortho,
                        gl_sprite_texture(&g_matrix_utf8_shade, 0));
  assert(glGetError() == GL_NO_ERROR);
  gl_ortho_rect(&g_ortho, 0, G_MATRIX_UTF8_FONT_SIZE - text->pt_h,
                text->pt_w, text->pt_h - G_MATRIX_UTF8_FONT_SIZE);
  assert(glGetError() == GL_NO_ERROR);
  glDisable(GL_BLEND);
  assert(glGetError() == GL_NO_ERROR);
  return true;
}

void matrix_utf8_text_clean (s_tag *tag)
{
  s_map *map;
  s_gl_text *text;
  if (tag->type == TAG_MAP &&
      (map = &tag->data.map) &&
      map->count == 3 &&
      map->key[1].type == TAG_SYM &&
      map->key[1].data.sym == sym_1("text") &&
      map->value[1].type == TAG_PTR &&
      (text = map->value[1].data.ptr.p)) {
    gl_vtext_clean(text);
  }
}

bool matrix_utf8_unload (s_sequence *seq)
{
  assert(seq);
  matrix_utf8_text_clean(&seq->tag);
  tag_void(&seq->tag);
  gl_font_clean(&g_matrix_utf8_font);
  return true;
}

bool matrix_utf8_update (s_sequence *seq)
{
  (void) seq;
  return true;
}
