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
#include "gl_font.h"

void *g_ft = NULL;

void gl_font_clean (s_gl_font *font)
{
  assert(font);
  FT_Done_Face(font->ft_face);
  str_clean(&font->path);
  str_clean(&font->real_path);
}

s_gl_font * gl_font_init (s_gl_font *font, const char *path,
                          f32 point_per_pixel)
{
  s_gl_font tmp = {0};
  assert(font);
  assert(path);
  str_init_copy_1(&tmp.path, path);
  if (! file_search(&tmp.path, sym_1("r"), &tmp.real_path)) {
    err_write_1("gl_font_init: file not found: ");
    err_puts(path);
    str_clean(&tmp.path);
    return NULL;
  }
  assert(glGetError() == GL_NO_ERROR);
  if (FT_New_Face((FT_Library) g_ft, tmp.real_path.ptr.p_pchar, 0,
                  (FT_Face *) &tmp.ft_face)) {
    err_write_1("gl_font_init: error loading font: ");
    err_puts(tmp.real_path.ptr.p_pchar);
    str_clean(&tmp.path);
    str_clean(&tmp.real_path);
    return NULL;
  }
  tmp.point_per_pixel = point_per_pixel;
  *font = tmp;
  return font;
}

void gl_font_set_size (s_gl_font *font, f32 point_size)
{
  f32 size;
  size = point_size / font->point_per_pixel;
  FT_Set_Pixel_Sizes(font->ft_face, 0, size);
}
