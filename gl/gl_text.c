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
#include "../libkc3/kc3.h"
#include "freetype2.h"
#include "gles2.h"
#include "gl_text.h"

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
  if (glGetError() != GL_NO_ERROR || ! tmp.texture) {
    if (tmp.texture)
      glDeleteTextures(1, &tmp.texture);
    return NULL;
  }
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
  if (! str_init_copy(&tmp.str, str)) {
    gl_text_clean(&tmp);
    return NULL;
  }
  *text = tmp;
  return text;
}

typedef struct gl_text_bounds {
  s64 x_min;
  s64 y_min;
  s64 x_max;
  s64 y_max;
} s_gl_text_bounds;

static s64 gl_text_floor (FT_Pos value)
{
  return value / 64 - (value % 64 < 0);
}

static s64 gl_text_ceil (FT_Pos value)
{
  return value / 64 + (value % 64 > 0);
}

static bool gl_text_bounds_add (s_gl_text_bounds *bounds, s64 left,
                                s64 top, s64 right, s64 bottom)
{
  if (left < -S32_MAX || top < -S32_MAX ||
      right > S32_MAX || bottom > S32_MAX ||
      left > right || top > bottom)
    return false;
  if (left < bounds->x_min)
    bounds->x_min = left;
  if (top < bounds->y_min)
    bounds->y_min = top;
  if (right > bounds->x_max)
    bounds->x_max = right;
  if (bottom > bounds->y_max)
    bounds->y_max = bottom;
  return true;
}

static bool gl_text_bitmap_valid (const FT_Bitmap *bitmap)
{
  u64 pitch;
  u64 row_size;
  if (! bitmap->width || ! bitmap->rows)
    return true;
  if (! bitmap->buffer)
    return false;
  pitch = bitmap->pitch < 0 ? - (s64) bitmap->pitch : bitmap->pitch;
  switch (bitmap->pixel_mode) {
  case FT_PIXEL_MODE_GRAY:
    if (bitmap->num_grays < 2 || bitmap->num_grays > 256)
      return false;
    row_size = bitmap->width;
    break;
  case FT_PIXEL_MODE_MONO:
    row_size = ((u64) bitmap->width + 7) / 8;
    break;
  default:
    return false;
  }
  return pitch >= row_size && pitch <= (u64) SW_MAX / bitmap->rows;
}

static bool gl_text_bitmap_copy (const FT_Bitmap *bitmap, s64 x, s64 y,
                                 uw width, uw height, u8 *data)
{
  uw i;
  uw j;
  uw pitch;
  u8 *pixel;
  const u8 *row;
  uw row_index;
  u32 value;
  if (! bitmap->width || ! bitmap->rows)
    return true;
  if (x < 0 || y < 0 || (u64) x > width || (u64) y > height ||
      bitmap->width > width - (uw) x ||
      bitmap->rows > height - (uw) y)
    return false;
  pitch = bitmap->pitch < 0 ? - (s64) bitmap->pitch : bitmap->pitch;
  for (i = 0; i < bitmap->rows; i++) {
    row_index = bitmap->pitch < 0 ? bitmap->rows - 1 - i : i;
    row = bitmap->buffer + row_index * pitch;
    pixel = data + ((height - 1 - ((uw) y + i)) * width + (uw) x) * 4;
    for (j = 0; j < bitmap->width; j++) {
      if (bitmap->pixel_mode == FT_PIXEL_MODE_MONO)
        value = row[j / 8] & (0x80 >> (j % 8)) ? 255 : 0;
      else {
        value = row[j];
        if (value >= bitmap->num_grays)
          value = bitmap->num_grays - 1;
        value = value * 255 / (bitmap->num_grays - 1);
      }
      pixel[0] = 255;
      pixel[1] = 255;
      pixel[2] = 255;
      pixel[3] = value + (pixel[3] * (255 - value) + 127) / 255;
      pixel += 4;
    }
  }
  return true;
}

static bool gl_text_layout (const s_gl_text *text,
                            s_gl_text_bounds *bounds, u8 *data,
                            uw width, uw height)
{
  s64 ascent;
  character c;
  s_gl_text_bounds current = {0};
  FT_Vector delta;
  FT_Face face = text->font->ft_face;
  FT_GlyphSlot glyph;
  FT_UInt index;
  s64 left;
  s64 line_height;
  FT_UInt previous = 0;
  s_str str = text->str;
  s64 top;
  s64 x = 0;
  s64 y = 0;
  ascent = gl_text_ceil(face->size->metrics.ascender);
  line_height = ascent - gl_text_floor(face->size->metrics.descender);
  if (ascent < -S32_MAX || ascent > S32_MAX ||
      line_height <= 0 || line_height > S32_MAX)
    return false;
  if (str.size &&
      ! gl_text_bounds_add(&current, 0, 0, 0, line_height))
    return false;
  while (str.size) {
    if (str_read_character_utf8(&str, &c) <= 0)
      return false;
    if (c == '\n') {
      x = 0;
      y += line_height;
      if (! gl_text_bounds_add(&current, 0, y, 0, y + line_height))
        return false;
      previous = 0;
      continue;
    }
    index = FT_Get_Char_Index(face, c);
    if (previous && index) {
      if (FT_Get_Kerning(face, previous, index, FT_KERNING_DEFAULT,
                         &delta))
        return false;
      x += gl_text_floor(delta.x);
      if (! gl_text_bounds_add(&current, x, y, x, y))
        return false;
    }
    if (FT_Load_Glyph(face, index, FT_LOAD_RENDER))
      return false;
    glyph = face->glyph;
    if (! glyph || ! gl_text_bitmap_valid(&glyph->bitmap))
      return false;
    if (glyph->bitmap.width && glyph->bitmap.rows) {
      left = x + glyph->bitmap_left;
      top = y + ascent - glyph->bitmap_top;
      if (! gl_text_bounds_add(&current, left, top,
                               left + glyph->bitmap.width,
                               top + glyph->bitmap.rows))
        return false;
      if (data &&
          ! gl_text_bitmap_copy(&glyph->bitmap, left - bounds->x_min,
                                top - bounds->y_min, width, height,
                                data))
        return false;
    }
    x += gl_text_floor(glyph->advance.x);
    if (! gl_text_bounds_add(&current, x, y, x, y))
      return false;
    previous = index;
  }
  if (data)
    return current.x_min == bounds->x_min &&
      current.y_min == bounds->y_min &&
      current.x_max == bounds->x_max &&
      current.y_max == bounds->y_max;
  *bounds = current;
  return true;
}

bool gl_text_render_to_texture (s_gl_text *text)
{
  s_gl_text_bounds bounds = {0};
  u8 *data;
  uw data_h;
  uw data_w;
  FT_Face face;
  GLint max_size = 0;
  uw pix_h;
  uw pix_w;
  assert(text);
  assert(text->font);
  assert(text->texture);
  assert(glGetError() == GL_NO_ERROR);
  face = text->font->ft_face;
  if (! face || ! face->size ||
      ! gl_text_layout(text, &bounds, NULL, 0, 0))
    return false;
  if (bounds.x_max - bounds.x_min > S32_MAX ||
      bounds.y_max - bounds.y_min > S32_MAX)
    return false;
  pix_w = bounds.x_max - bounds.x_min;
  pix_h = bounds.y_max - bounds.y_min;
  data_w = pix_w ? pix_w : 1;
  data_h = pix_h ? pix_h : 1;
  glGetIntegerv(GL_MAX_TEXTURE_SIZE, &max_size);
  if (glGetError() != GL_NO_ERROR || max_size <= 0 ||
      data_w > (uw) max_size || data_h > (uw) max_size ||
      data_w > (uw) SW_MAX / 4 / data_h)
    return false;
  if (! (data = alloc(data_w * data_h * 4)))
    return false;
  if (! gl_text_layout(text, &bounds, data, data_w, data_h))
    goto ko;
  glBindTexture(GL_TEXTURE_2D, text->texture);
  if (glGetError() != GL_NO_ERROR)
    goto ko_gl;
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, data_w, data_h, 0, GL_RGBA,
               GL_UNSIGNED_BYTE, data);
  if (glGetError() != GL_NO_ERROR)
    goto ko_gl;
  glGenerateMipmap(GL_TEXTURE_2D);
  if (glGetError() != GL_NO_ERROR)
    goto ko_gl;
  glBindTexture(GL_TEXTURE_2D, 0);
  if (glGetError() != GL_NO_ERROR)
    goto ko;
  alloc_free(data);
  text->pix_w = pix_w;
  text->pix_h = pix_h;
  text->pt_w = pix_w * text->font->point_per_pixel;
  text->pt_h = pix_h * text->font->point_per_pixel;
  return true;
 ko_gl:
  glBindTexture(GL_TEXTURE_2D, 0);
 ko:
  alloc_free(data);
  return false;
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
  s_str tmp;
  assert(text);
  assert(str);
  if (compare_str(&text->str, str)) {
    if (! str_init_copy(&tmp, str))
      return false;
    str_clean(&text->str);
    text->str = tmp;
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
  if (gl_text_set_text_1(text, p))
    return gl_text_update(text);
  return false;
}

bool gl_text_update_buf (s_gl_text *text, s_buf *buf)
{
  if (gl_text_set_text_buf(text, buf))
    return gl_text_update(text);
  return false;
}
