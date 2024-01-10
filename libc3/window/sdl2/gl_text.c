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
#include <libc3/c3.h>
#include "gl_object.h"
#include "gl_point_2f.h"
#include "gl_point_3f.h"
#include "gl_text.h"
#include "gl_triangle.h"

void gl_text_clean (s_gl_text *text)
{
  gl_object_clean(&text->object);
  str_clean(&text->str);
  glDeleteTextures(1, &text->texture);
}

s_gl_text * gl_text_init (s_gl_text *text, const s_gl_font *font)
{
  uw dimension;
  s_gl_text tmp = {0};
  assert(glGetError() == GL_NO_ERROR);
  tmp.font = font;
  glGenTextures(1, &tmp.texture);
  assert(glGetError() == GL_NO_ERROR);
  gl_object_init(&tmp.object);
  dimension = 4;
  array_init(&tmp.object.vertex, sym_1("GL.Vertex"), 1,
             &dimension);
  array_allocate(&tmp.object.vertex);
  dimension = 2;
  array_init(&tmp.object.triangle, sym_1("GL.Triangle"), 1,
             &dimension);
  array_allocate(&tmp.object.triangle);
  *text = tmp;  
  return text;
}

s_gl_text * gl_text_init_1 (s_gl_text *text, const s_gl_font *font,
                            const char *p)
{
  s_str str;
  str_init_1(&str, NULL, p);
  return gl_text_init_str(text, font, &str);
}

s_gl_text * gl_text_init_str (s_gl_text *text, const s_gl_font *font,
                              const s_str *str)
{
  s_gl_text tmp = {0};
  if (! gl_text_init(&tmp, font))
    return NULL;
  if (! str_init_copy(&tmp.str, str))
    return NULL;
  *text = tmp;
  return text;
}

void gl_text_render (const s_gl_text *text)
{
  assert(text);
  assert(glGetError() == GL_NO_ERROR);
  gl_object_render(&text->object);
}

bool gl_text_render_to_texture (s_gl_text *text)
{
  character c;
  u8 *data;
  uw  data_w;
  uw  data_h;
  u8 *data_pixel;
  uw  data_size;
  uw  data_x;
  uw  data_y;
  FT_Vector delta;
  const s_gl_font *font;
  FT_GlyphSlot glyph;
  FT_UInt glyph_index;
  uw i;
  uw j;
  f32 max_height = 0.0;
  FT_UInt prev_glyph_index = 0;
  s_str s;
  f32 total_width = 0.0;
  assert(text);
  assert(text->font);
  assert(text->texture);
  assert(glGetError() == GL_NO_ERROR);
  if (! text->str.size)
    return true;
  glBindTexture(GL_TEXTURE_2D, text->texture);
  assert(glGetError() == GL_NO_ERROR);
  font = text->font;
  s = text->str;
  while (str_read_character_utf8(&s, &c) > 0) {
    glyph_index = FT_Get_Char_Index(font->ft_face, c);
    if (prev_glyph_index && glyph_index) {
      FT_Get_Kerning(font->ft_face, prev_glyph_index, glyph_index,
                     FT_KERNING_DEFAULT, &delta);
      total_width += ceil((f32) delta.x / (1 << 6));
    }
    if (FT_Load_Glyph(font->ft_face, glyph_index, FT_LOAD_RENDER)) {
      err_write_1("gl_font_render_to_texture: failed to load glyph: ");
      err_inspect_character(&c);
      err_write_1("\n");
      continue;
    }
    glyph = font->ft_face->glyph;
    total_width += glyph->bitmap.width;
    max_height =
      (glyph->bitmap.rows + glyph->bitmap_top > max_height) ?
      (glyph->bitmap.rows + glyph->bitmap_top) : max_height;
    prev_glyph_index = glyph_index;
  }
  data_w = ceil(total_width);
  data_h = ceil(max_height);
  data_size = data_w * data_h * 4;
  data = calloc(1, data_size);
  f32 x = 0;
  prev_glyph_index = 0;
  s = text->str;
  while (str_read_character_utf8(&s, &c) > 0) {
    glyph_index = FT_Get_Char_Index(font->ft_face, c);

    // Apply kerning (adjust spacing between characters)
    if (prev_glyph_index && glyph_index) {
      FT_Vector delta;
      FT_Get_Kerning(font->ft_face, prev_glyph_index, glyph_index,
                     FT_KERNING_DEFAULT, &delta);
      x += (f32) delta.x / (1 << 6);
    }
    if (FT_Load_Glyph(font->ft_face, glyph_index, FT_LOAD_RENDER)) {
      continue;
    }
    glyph = font->ft_face->glyph;
    i = 0;
    while (i < glyph->bitmap.width) {
      j = 0;
      while (j < glyph->bitmap.rows) {
        data_x = round(x) + i;
        data_y = j + glyph->bitmap_top;
        data_pixel = data + (data_y * data_w + data_x) * 4;
        u8 value = glyph->bitmap.buffer[j * glyph->bitmap.width + i];
        data_pixel[0] = value;
        data_pixel[1] = value;
        data_pixel[2] = value;
        data_pixel[3] = value;
        j++;
      }
      i++;
    }
    x += glyph->bitmap.width;
    prev_glyph_index = glyph_index;
  }
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, data_w, data_h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
  assert(glGetError() == GL_NO_ERROR);
  free(data);
  glBindTexture(GL_TEXTURE_2D, 0);
  text->w = data_w;
  text->h = data_h;
  return true;
}

bool gl_text_set_font (s_gl_text *text, const s_gl_font *font)
{
  assert(text);
  assert(font);
  if (text->font != font) {
    text->font = font;
    return true;
  }
  return false;
}

bool gl_text_set_text (s_gl_text *text, const s_str *str)
{
  assert(text);
  assert(str);
  if (compare_str(&text->str, str)) {
    str_clean(&text->str);
    str_init_copy(&text->str, str);
    return true;
  }
  return false;
}

bool gl_text_set_text_1 (s_gl_text *text, const char *p)
{
  s_str str;
  assert(text);
  assert(p);
  str_init_1(&str, NULL, p);
  return gl_text_set_text(text, &str);
}

bool gl_text_set_text_buf (s_gl_text *text, s_buf *buf)
{
  bool result;
  s_str str;
  assert(text);
  assert(buf);
  buf_peek_to_str(buf, &str);
  result = gl_text_set_text(text, &str);
  str_clean(&str);
  return result;
}

bool gl_text_update (s_gl_text *text)
{
  s_gl_vertex *vertex;
  s_gl_triangle *triangle;
  if (! gl_text_render_to_texture(text))
    return false;
  vertex = text->object.vertex.data;
  gl_point_3f_init(&vertex[0].position, 0.0, text->h, 0.0);
  gl_point_3f_init(&vertex[0].normal, 0.0, 0.0, -1.0);
  gl_point_2f_init(&vertex[0].tex_coord, 0.0, 1.0);
  gl_point_3f_init(&vertex[1].position, 0.0, 0.0, 0.0);
  gl_point_3f_init(&vertex[1].normal, 0.0, 0.0, -1.0);
  gl_point_2f_init(&vertex[1].tex_coord, 0.0, 0.0);
  gl_point_3f_init(&vertex[2].position, text->w, text->h, 0.0);
  gl_point_3f_init(&vertex[2].normal, 0.0, 0.0, -1.0);
  gl_point_2f_init(&vertex[2].tex_coord, 1.0, 1.0);
  gl_point_3f_init(&vertex[3].position, text->w, 0.0, 0.0);
  gl_point_3f_init(&vertex[3].normal, 0.0, 0.0, -1.0);
  gl_point_2f_init(&vertex[3].tex_coord, 1.0, 0.0);
  triangle = text->object.triangle.data;
  gl_triangle_init(triangle + 0, 0, 1, 2);
  gl_triangle_init(triangle + 1, 1, 3, 2);
  gl_object_update(&text->object);
  return true;
}

bool gl_text_update_1 (s_gl_text *text, const char *p)
{
  if (gl_text_set_text_1(text, p)) {
    gl_text_update(text);
    return true;
  }
  return false;
}

bool gl_text_update_buf (s_gl_text *text, s_buf *buf)
{
  if (gl_text_set_text_buf(text, buf)) {
    gl_text_update(text);
    return true;
  }
  return false;
}
