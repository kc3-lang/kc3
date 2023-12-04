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
#include <assert.h>
#include <err.h>
#include <libc3/c3.h>
#include "sdl2_font.h"

void sdl2_font_clean (s_sdl2_font *font)
{
  assert(font);
  ftglDestroyFont(font->ftgl_font);
  str_clean(&font->path);
  str_clean(&font->real_path);
}

s_sdl2_font * sdl2_font_init (s_sdl2_font *font, const s8 *path)
{
  assert(font);
  assert(path);
  str_init_copy_1(&font->path, path);
  if (! file_search(&font->path, sym_1("r"), &font->real_path)) {
    warnx("sdl2_font_init(%s): file not found", path);
    str_clean(&font->path);
    return NULL;
  }
  font->ftgl_font = ftglCreateTextureFont(font->real_path.ptr.ps8);
  if (! font->ftgl_font) {
    warnx("sdl2_font_init(%s): Error loading font",
          font->real_path.ptr.ps8);
    str_clean(&font->path);
    str_clean(&font->real_path);
    return NULL;
  }
  font->size = 0;
  font->dpi = 72;
  return font;
}

void sdl2_font_set_size (s_sdl2_font *font, u32 size, u32 dpi)
{
  assert(font);
  assert(size);
  ftglSetFontFaceSize(font->ftgl_font, size, dpi);
  font->size = size;
  font->dpi = dpi;
}

void sdl2_font_render_text (const s_sdl2_font *font, const s8 *p)
{
  ftglRenderFont(font->ftgl_font, p, FTGL_RENDER_ALL);
}
