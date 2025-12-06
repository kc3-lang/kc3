/* kc3
 * Copyright from 2022 to 2025 kmx.io <contact@kmx.io>
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
#include "../libkc3/kc3.h"
#include "freetype2.h"
#include "gles2.h"
#include "gl_vtext.h"

void gl_vtext_clean (s_gl_text *text)
{
  assert(text);
  str_clean(&text->str);
  glDeleteTextures(1, &text->texture);
}

void gl_vtext_delete (s_gl_text *text)
{
  gl_vtext_clean(text);
  free(text);
}

s_gl_text * gl_vtext_init (s_gl_text *text, const s_gl_font *font)
{
  s_gl_text tmp = {0};
  assert(glGetError() == GL_NO_ERROR);
  tmp.font = font;
  glGenTextures(1, &tmp.texture);
  assert(glGetError() == GL_NO_ERROR);
  *text = tmp;  
  return text;
}

s_gl_text * gl_vtext_init_1 (s_gl_text *text, const s_gl_font *font,
                             const char *p)
{
  s_str str;
  str_init_1(&str, NULL, p);
  return gl_vtext_init_str(text, font, &str);
}

s_gl_text * gl_vtext_init_str (s_gl_text *text, const s_gl_font *font,
                               const s_str *str)
{
  s_gl_text tmp = {0};
  if (! gl_vtext_init(&tmp, font))
    return NULL;
  if (! str_init_copy(&tmp.str, str))
    return NULL;
  *text = tmp;
  return text;
}

s_gl_text * gl_vtext_new (const s_gl_font *font)
{
  s_gl_text *text;
  text = calloc(1, sizeof(s_gl_text));
  if (! text) {
    err_puts("gl_vtext_new: failed to allocate memory");
    assert(! "gl_vtext_new: failed to allocate memory");
    return NULL;
  }
  if (! gl_vtext_init(text, font)) {
    free(text);
    return NULL;
  }
  return text;
}

s_gl_text * gl_vtext_new_1 (const s_gl_font *font, const char *p)
{
  s_gl_text *text;
  text = calloc(1, sizeof(s_gl_text));
  if (! text) {
    err_puts("gl_vtext_new_1: failed to allocate memory");
    assert(! "gl_vtext_new_1: failed to allocate memory");
    return NULL;
  }
  if (! gl_vtext_init_1(text, font, p)) {
    free(text);
    return NULL;
  }
  return text;
}

s_gl_text * gl_vtext_new_str (const s_gl_font *font, const s_str *str)
{
  s_gl_text *text;
  text = calloc(1, sizeof(s_gl_text));
  if (! text) {
    err_puts("gl_vtext_new_1: failed to allocate memory");
    assert(! "gl_vtext_new_1: failed to allocate memory");
    return NULL;
  }
  if (! gl_vtext_init_str(text, font, str)) {
    free(text);
    return NULL;
  }
  return text;
}

bool gl_vtext_render_to_texture (s_gl_text *text)
{
  character c;
  u8 *data;
  u32  data_w;
  u32  data_h;
  u8 *data_pixel;
  u32  data_size;
  u32  data_x;
  u32  data_y;
  FT_Face face;
  const s_gl_font *font;
  FT_GlyphSlot glyph;
  FT_UInt glyph_index;
  u32 i;
  u32 j;
  u32 line_height;
  u32 max_ascent;
  u32 max_descent;
  s_str s;
  f32 scale_y;
  u32 x;
  u32 y;
  assert(text);
  assert(text->font);
  assert(text->texture);
  assert(glGetError() == GL_NO_ERROR);
  if (! text->str.size)
    return true;
  glBindTexture(GL_TEXTURE_2D, text->texture);
  assert(glGetError() == GL_NO_ERROR);
  font = text->font;
  face = font->ft_face;
  s = text->str;
  scale_y = face->size->metrics.y_scale / 65536.0;
  max_ascent = (u32) (face->ascender * scale_y) >> 6;
  max_descent = (u32) abs((int) (face->descender * scale_y)) >> 6;
  line_height = max_ascent + max_descent;
  data_w = 0;
  data_h = 0;
  x = 0;
  while (str_read_character_utf8(&s, &c) > 0) {
    glyph_index = FT_Get_Char_Index(face, c);
    if (FT_Load_Glyph(face, glyph_index, FT_LOAD_RENDER)) {
      err_write_1("gl_font_render_to_texture: failed to load glyph: ");
      err_inspect_character(c);
      err_write_1("\n");
      continue;
    }
    glyph = face->glyph;
    x = glyph->metrics.horiAdvance >> 6;
    if (x > data_w)
      data_w = x;
    data_h += line_height;
  }
  data_size = data_w * data_h * 4;
  data = calloc(1, data_size);
  x = 0;
  y = 0;
  s = text->str;
  while (str_read_character_utf8(&s, &c) > 0) {
    glyph_index = FT_Get_Char_Index(face, c);
    if (FT_Load_Glyph(face, glyph_index, FT_LOAD_RENDER))
      continue;
    glyph = face->glyph;
    x = (data_w - (glyph->metrics.width >> 6)) / 2;
    i = 0;
    while (i < glyph->bitmap.rows) {
      data_y = data_h - line_height - 1 -
        (y + i + max_ascent - glyph->bitmap_top);
      //printf("\n");
      j = 0;
      while (j < glyph->bitmap.width) {
        data_x = x + j;
        data_pixel = data + (data_y * data_w + data_x) * 4;
        u8 value = glyph->bitmap.buffer[i * glyph->bitmap.width + j];
        data_pixel[0] = 255;
        data_pixel[1] = 255;
        data_pixel[2] = 255;
        data_pixel[3] = value;
        //printf("%s", g_gray_3_bits_utf8[value / 32]);
        j++;
      }
      i++;
    }
    y += line_height;
  }
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, data_w, data_h, 0, GL_RGBA,
               GL_UNSIGNED_BYTE, data);
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

bool gl_vtext_render_to_texture_random (s_gl_text *text, u32 len)
{
  FT_UInt *glyphs;
  u8 *data;
  sw  data_w;
  sw  data_h;
  u8 *data_pixel;
  sw  data_size;
  sw  data_x;
  sw  data_y;
  FT_Face face;
  const s_gl_font *font;
  FT_GlyphSlot glyph;
  FT_UInt glyph_index;
  sw i;
  u32 j;
  u32 k;
  sw line_height;
  sw max_ascent;
  sw max_descent;
  f32 scale_y;
  sw x;
  sw y;
  assert(text);
  assert(text->font);
  assert(text->texture);
  assert(glGetError() == GL_NO_ERROR);
  glBindTexture(GL_TEXTURE_2D, text->texture);
  assert(glGetError() == GL_NO_ERROR);
  font = text->font;
  face = font->ft_face;
  glyphs = calloc(len, sizeof(FT_UInt));
  if (! glyphs) {
    err_puts("gl_vtext_render_to_texture_random:"
             " failed to allocate memory (glyphs)");
    assert(! "gl_vtext_render_to_texture_random:"
             " failed to allocate memory (glyphs)");
    return false;
  }
  for (i = 0; i < (sw) len; i++) {
    glyph = NULL;
    do {
      u32_random_uniform(glyphs + i, face->num_glyphs - 2);
      glyphs[i]++;
      if (FT_Load_Glyph(face, glyphs[i], FT_LOAD_RENDER))
        continue;
      glyph = face->glyph;
      //printf("width %ld\n", glyph->metrics.width >> 6);
    } while (! glyph || ! (glyph->metrics.width >> 6));
  }
  scale_y = face->size->metrics.y_scale / 65536.0;
  max_ascent = (u32) (face->ascender * scale_y) >> 6;
  max_descent = (u32) abs((int) (face->descender * scale_y)) >> 6;
  line_height = max_ascent + max_descent;
  data_w = 0;
  data_h = 0;
  x = 0;
  for (i = 0; i < (sw) len; i++) {
    glyph_index = glyphs[i];
    if (FT_Load_Glyph(face, glyph_index, FT_LOAD_RENDER)) {
      err_write_1("gl_vtext_render_to_texture_random:"
                  " failed to load glyph: ");
      err_inspect_u32(glyph_index);
      err_write_1("\n");
      continue;
    }
    glyph = face->glyph;
    x = glyph->metrics.horiAdvance >> 6;
    if (x > data_w)
      data_w = x;
    data_h += line_height;
  }
  data_w++;
  data_h += line_height * 2;
  data_size = data_w * data_h * 4;
  data = calloc(1, data_size);
  if (! data) {
    err_puts("gl_vtext_render_to_texture_random:"
             " failed to allocate memory (data)");
    assert(! "gl_vtext_render_to_texture_random:"
             " failed to allocate memory (data)");
    free(glyphs);
    return false;
  }
  x = 0;
  y = 0;
  for (i = 0; i < (sw) len; i++) {
    glyph_index = glyphs[i];
    if (FT_Load_Glyph(face, glyph_index, FT_LOAD_RENDER))
      continue;
    glyph = face->glyph;
    x = (data_w - (glyph->metrics.width >> 6)) / 2;
    j = 0;
    while (j < glyph->bitmap.rows) {
      data_y = data_h - line_height - 1 -
        ((sw) y + j + max_ascent - glyph->bitmap_top);
      //printf("\n");
      k = 0;
      while (k < glyph->bitmap.width) {
        data_x = x + k;
        data_pixel = data + (data_y * data_w + data_x) * 4;
        u8 value = glyph->bitmap.buffer[j * glyph->bitmap.width + k];
        data_pixel[0] = 255;
        data_pixel[1] = 255;
        data_pixel[2] = 255;
        data_pixel[3] = value;
        //printf("%s", g_gray_3_bits_utf8[value / 32]);
        k++;
      }
      j++;
    }
    y += line_height;
  }
  free(glyphs);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, data_w, data_h, 0, GL_RGBA,
               GL_UNSIGNED_BYTE, data);
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

bool gl_vtext_set_font (s_gl_text *text, const s_gl_font *font)
{
  assert(text);
  assert(font);
  if (text->font != font) {
    text->font = font;
    return true;
  }
  return false;
}

bool gl_vtext_set_text (s_gl_text *text, const s_str *str)
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

bool gl_vtext_set_text_1 (s_gl_text *text, const char *p)
{
  s_str str;
  assert(text);
  assert(p);
  str_init_1(&str, NULL, p);
  return gl_vtext_set_text(text, &str);
}

bool gl_vtext_set_text_buf (s_gl_text *text, s_buf *buf)
{
  bool result;
  s_str str;
  assert(text);
  assert(buf);
  buf_peek_to_str(buf, &str);
  result = gl_vtext_set_text(text, &str);
  str_clean(&str);
  return result;
}

bool gl_vtext_update (s_gl_text *text)
{
  return gl_vtext_render_to_texture(text);
}

bool gl_vtext_update_1 (s_gl_text *text, const char *p)
{
  if (gl_vtext_set_text_1(text, p)) {
    gl_vtext_update(text);
    return true;
  }
  return false;
}

bool gl_vtext_update_buf (s_gl_text *text, s_buf *buf)
{
  if (gl_vtext_set_text_buf(text, buf)) {
    gl_vtext_update(text);
    return true;
  }
  return false;
}
