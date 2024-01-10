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
#include "gl_font.h"

FT_Library g_ft = {0};

void gl_font_clean (s_gl_font *font)
{
  assert(font);
  FT_Done_Face(font->ft_face);
  str_clean(&font->path);
  str_clean(&font->real_path);
}

s_gl_font * gl_font_init (s_gl_font *font, const char *path)
{
  assert(font);
  assert(path);
  str_init_copy_1(&font->path, path);
  if (! file_search(&font->path, sym_1("r"), &font->real_path)) {
    err_write_1("gl_font_init: file not found: ");
    err_puts(path);
    str_clean(&font->path);
    return NULL;
  }
  assert(glGetError() == GL_NO_ERROR);
  if (FT_New_Face(g_ft, font->real_path.ptr.pchar, 0, &font->ft_face)) {
    err_write_1("gl_font_init: error loading font: ");
    err_puts(font->real_path.ptr.pchar);
    str_clean(&font->path);
    str_clean(&font->real_path);
    return NULL;
  }
  return font;
}

void gl_font_set_size (s_gl_font *font, f64 point_size,
                       f64 pixel_per_point)
{
  f64 size;
  size = point_size * pixel_per_point;
  FT_Set_Pixel_Sizes(font->ft_face, 0, size);
}
