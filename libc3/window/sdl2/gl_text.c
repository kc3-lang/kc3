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
  str_clean(&text->str);
  glDeleteTextures(1, &text->texture);
}

s_gl_text * gl_text_init (s_gl_text *text, const s_gl_font *font)
{
  s_gl_text tmp = {0};
  assert(glGetError() == GL_NO_ERROR);
  tmp.font = font;
  glGenTextures(1, &tmp.texture);
  assert(glGetError() == GL_NO_ERROR);
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
      total_width += delta.x >> 6;
    }
    if (FT_Load_Glyph(font->ft_face, glyph_index, FT_LOAD_RENDER)) {
      err_write_1("gl_font_render_to_texture: failed to load glyph: ");
      err_inspect_character(&c);
      err_write_1("\n");
      continue;
    }
    glyph = font->ft_face->glyph;
    total_width += glyph->bitmap.width;
    if (glyph->bitmap.rows + glyph->bitmap_top > max_height)
      max_height = glyph->bitmap.rows + glyph->bitmap_top;
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
      x += delta.x >> 6;
    }
    if (FT_Load_Glyph(font->ft_face, glyph_index, FT_LOAD_RENDER)) {
      continue;
    }
    glyph = font->ft_face->glyph;
    i = 0;
    while (i < glyph->bitmap.rows) {
      data_y = i + glyph->bitmap_top;
      //printf("\n");
      j = 0;
      while (j < glyph->bitmap.width) {
        data_x = x + j;
        data_pixel = data + (data_y * data_w + data_x) * 4;
        u8 value = glyph->bitmap.buffer[i * glyph->bitmap.width + j];
        data_pixel[0] = 255;
        data_pixel[1] = 255;
        data_pixel[2] = 255;
        data_pixel[3] = (u8) value;
        //printf("%s", (const char *[]) {" ", ".", ":", "#", "░", "▒", "▓", "█"}[value / 32]);
        j++;
      }
      i++;
    }
    x += glyph->bitmap.width;
    prev_glyph_index = glyph_index;
  }
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, data_w, data_h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
  assert(glGetError() == GL_NO_ERROR);
  glGenerateMipmap(GL_TEXTURE_2D);
  assert(glGetError() == GL_NO_ERROR);
  free(data);
  glBindTexture(GL_TEXTURE_2D, 0);
  text->pix_w = data_w;
  text->pix_h = data_h;
  text->pt_w = text->pix_w * text->font->point_per_pixel;
  text->pt_h = text->pix_h * text->font->point_per_pixel;
  //printf("\n");
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
  return gl_text_render_to_texture(text);
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
