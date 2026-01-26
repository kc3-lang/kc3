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
#include <math.h>
#include "../../libkc3/kc3.h"
#include "../../gl/gl.h"
#include "demo.h"
#include "matrix.h"

#define              MATRIX_FONT_SIZE 20
#define              MATRIX_TIME 0.1

static s_gl_font   g_matrix_font = {0};
static s_gl_sprite g_matrix_shade = {0};
static f64         g_matrix_time;

void matrix_column_clean (s_tag *tag);
u8 matrix_column_init (s_sequence *seq, s_tag *tag);
u8 matrix_column_render (s_sequence *seq, s_tag *tag);
void matrix_screen_clean (s_tag *tag);
u8 matrix_screen_init (s_tag *tag);
u8 matrix_screen_render (s_sequence *seq, s_tag *tag);
void matrix_text_clean (s_tag *tag);
u8 matrix_text_init (s_tag *tag, f32 y);
u8 matrix_text_render (s_sequence *seq, const s_tag *tag, f32 **py);
u8 matrix_update (s_sequence *seq);

void matrix_column_clean (s_tag *tag)
{
  s_list *list;
  if (tag->type != TAG_PLIST) {
    err_puts("matrix_column_clean: invalid tag");
    assert(! "matrix_column_clean: invalid tag");
    return;
  }
  list = tag->data.plist;
  while (list) {
    matrix_text_clean(&list->tag);
    list = list_next(list);
  }
}

u8 matrix_column_init (s_sequence *seq, s_tag *tag)
{
  s_list *list;
  s_window *window;
  assert(seq);
  window = seq->window;
  assert(window);
  if (! (list = list_new(NULL)))
    return false;
  if (! matrix_text_init(&list->tag, window->h)) {
    list_delete_all(list);
    return false;
  }
  tag_init_plist(tag, list);
  return true;
}

u8 matrix_column_render (s_sequence *seq, s_tag *tag)
{
  s_list **list;
  f32 *py;
  s_window *window;
  f32 y;
  assert(seq);
  assert(glGetError() == GL_NO_ERROR);
  window = seq->window;
  assert(window);
  if (!tag ||
      tag->type != TAG_PLIST) {
    err_puts("matrix_column_render: invalid tag");
    assert(! "matrix_column_render: invalid tag");
    return false;
  }
  list = &tag->data.plist;
  y = (*list)->tag.data.map.value[2].data.f32;
  if (y < window->h) {
    *list = list_new(*list);
    if (! matrix_text_init(&(*list)->tag, window->h))
      return false;
  }
  while (*list) {
    if (! matrix_text_render(seq, &(*list)->tag, &py))
      return false;
    if (*py < 0) {
      matrix_text_clean(&(*list)->tag);
      *list = list_delete(*list);
    }
    else
      list = &(*list)->next.data.plist;
  }
  return true;
}

u8 matrix_load (s_sequence *seq)
{
  f32 point_per_pixel;
  s_window *window;
  assert(seq);
  assert(glGetError() == GL_NO_ERROR);
  window = seq->window;
  assert(window);
  point_per_pixel = (f32) window->w / window->pixel_w;
  if (! gl_font_init(&g_matrix_font,
                     "fonts/Noto Sans/NotoSans-Regular.ttf",
                     point_per_pixel))
    return false;
  gl_font_set_size(&g_matrix_font, MATRIX_FONT_SIZE);
  if (! matrix_screen_init(&seq->tag))
    return false;
  if (! gl_sprite_init(&g_matrix_shade,
                       "img/matrix_shade.png",
                       1, 1, 1, 1))
    return false;
  g_matrix_time = seq->t;
  return true;
}

u8 matrix_render (s_sequence *seq)
{
  assert(seq);
  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
  glClear(GL_COLOR_BUFFER_BIT);
  assert(glGetError() == GL_NO_ERROR);
  matrix_screen_render(seq, &seq->tag);
  assert(glGetError() == GL_NO_ERROR);
  if (seq->t - g_matrix_time > MATRIX_TIME)
    g_matrix_time = seq->t;
  return true;
}

void matrix_screen_clean (s_tag *tag)
{
  s_list *list;
  if (! tag ||
      tag->type != TAG_PLIST) {
    err_puts("matrix_screen_clean: invalid tag");
    assert(! "matrix_screen_clean: invalid tag");
    return;
  }
  list = tag->data.plist;
  while (list) {
    matrix_column_clean(&list->tag);
    list = list_next(list);
  }
}

u8 matrix_screen_init (s_tag *tag)
{
  tag_init_plist(tag, NULL);
  return true;
}

u8 matrix_screen_render (s_sequence *seq, s_tag *tag)
{
  s_list **l;
  s_list *list;
  s_mat4 matrix;
  s_window *window;
  f32 x;
  assert(seq);
  window = seq->window;
  assert(window);
  if (! tag ||
      tag->type != TAG_PLIST) {
    err_puts("matrix_screen_render: invalid tag");
    assert(! "matrix_screen_render: invalid tag");
    return false;
  }
  x = 0;
  l = &tag->data.plist;
  while (*l) {
    if (x > window->w) {
      matrix_column_clean(&(*l)->tag);
      *l = list_delete(*l);
    }
    else {
      x += MATRIX_FONT_SIZE;
      l = &(*l)->next.data.plist;
    }
  }
  while (x < window->w) {
    *l = list_new(NULL);
    if (! matrix_column_init(seq, &(*l)->tag))
      return false;
    x += MATRIX_FONT_SIZE;
    l = &(*l)->next.data.plist;
  }
  matrix = g_ortho.model_matrix; {
    list = tag->data.plist;
    while (list) {
      if (! matrix_column_render(seq, &list->tag))
        return false;
      mat4_translate(&g_ortho.model_matrix, MATRIX_FONT_SIZE, 0, 0);
      list = list_next(list);
    }
  } g_ortho.model_matrix = matrix;
  return true;
}

u8 matrix_text_init (s_tag *tag, f32 y)
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
  spacing = i * MATRIX_FONT_SIZE;
  u8_random_uniform(&len, 40);
  len += 10;
  text = gl_vtext_new(&g_matrix_font);
  if (! text)
    return false;
  if (! gl_vtext_render_to_texture_random(text, len)) {
    gl_vtext_delete(text);
    return false;
  }
  map = &tag->data.map;
  tag_init_psym(map->key   + 0, sym_1("spacing"));
  tag_init_f32( map->value + 0, spacing);
  tag_init_psym(map->key   + 1, sym_1("text"));
  tag_init_ptr( map->value + 1, text);
  tag_init_psym(map->key   + 2, sym_1("y"));
  tag_init_f32( map->value + 2, y + text->pt_h + spacing);
  return true;
}

u8 matrix_text_render (s_sequence *seq, const s_tag *tag, f32 **py)
{
  const s_map *map;
  s_mat4 matrix;
  const s_gl_text *text;
  f32 *y;
  assert(seq);
  assert(glGetError() == GL_NO_ERROR);
  assert(tag);
  assert(tag->type == TAG_MAP);
  map = &tag->data.map;
  assert(map->count == 3);
  /*
  assert(      map->key[0].type == TAG_PSYM);
  assert(      map->key[0].data.psym == sym_1("spacing"));
  assert(    map->value[0].type == TAG_F32);
  spacing = &map->value[0].data.f32;
  */
  assert(  map->key[1].type == TAG_PSYM);
  assert(  map->key[1].data.psym == sym_1("text"));
  assert(map->value[1].type == TAG_PTR);
  text = map->value[1].data.ptr.p;
  assert(  map->key[2].type == TAG_PSYM);
  assert(  map->key[2].data.psym == sym_1("y"));
  assert(map->value[2].type == TAG_F32);
  y =   &map->value[2].data.f32;
  if (seq->t - g_matrix_time > MATRIX_TIME)
    *y -= MATRIX_FONT_SIZE;
  matrix = g_ortho.model_matrix; {
    //printf("y %f\n", *y);
    mat4_translate(&g_ortho.model_matrix,
                   (MATRIX_FONT_SIZE - text->pt_w) / 2, *y, 0);
    gl_ortho_update_model_matrix(&g_ortho);
    assert(glGetError() == GL_NO_ERROR);
    gl_ortho_color(&g_ortho, 0, 1, 0, 1);
    assert(glGetError() == GL_NO_ERROR);
    glEnable(GL_BLEND);
    glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA,
                        GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    gl_ortho_vtext_render(&g_ortho, text);
    assert(glGetError() == GL_NO_ERROR);
    glDisable(GL_DEPTH_TEST);
    gl_ortho_color(&g_ortho, 1, 1, 1, 1);
    gl_ortho_bind_texture(&g_ortho,
                          gl_sprite_texture(&g_matrix_shade, 0));
    assert(glGetError() == GL_NO_ERROR);
    gl_ortho_rect(&g_ortho, 0, MATRIX_FONT_SIZE - text->pt_h,
                  text->pt_w, text->pt_h - MATRIX_FONT_SIZE);
  } g_ortho.model_matrix = matrix;
  assert(glGetError() == GL_NO_ERROR);
  glDisable(GL_BLEND);
  assert(glGetError() == GL_NO_ERROR);
  *py = y;
  return true;
}

void matrix_text_clean (s_tag *tag)
{
  s_map *map;
  s_gl_text *text;
  if (! (tag->type == TAG_MAP &&
         (map = &tag->data.map) &&
         map->count == 3 &&
         map->key[1].type == TAG_PSYM &&
         map->key[1].data.psym == sym_1("text") &&
         map->value[1].type == TAG_PTR &&
         (text = map->value[1].data.ptr.p))) {
    err_puts("matrix_text_clean: invalid tag");
    assert(! "matrix_text_clean: invalid tag");
    return;
  }
  gl_vtext_delete(text);
}

u8 matrix_unload (s_sequence *seq)
{
  assert(seq);
  matrix_screen_clean(&seq->tag);
  tag_void(&seq->tag);
  gl_font_clean(&g_matrix_font);
  gl_sprite_clean(&g_matrix_shade);
  return true;
}
