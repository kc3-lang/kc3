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
#include "cairo_font.h"

static FT_Library *g_cairo_font_ft = NULL;

static FT_Library * cairo_font_ft (void);

void cairo_font_clean (s_cairo_font *font)
{
  assert(font);
  cairo_font_face_destroy(font->cairo_font_face);
  FT_Done_Face(font->ft_face);
  str_clean(&font->path);
}

static FT_Library * cairo_font_ft (void)
{
  if (! g_cairo_font_ft) {
    if (! (g_cairo_font_ft = malloc(sizeof(FT_Library)))) {
      warn("cairo_font_ft");
      return NULL;
    }
    if (FT_Init_FreeType(g_cairo_font_ft)) {
      warnx("cairo_font_ft: error initializing FreeType");
      free(g_cairo_font_ft);
      g_cairo_font_ft = NULL;
      return NULL;
    }
  }
  return g_cairo_font_ft;
}

s_cairo_font * cairo_font_init (s_cairo_font *font, s_str *path)
{
  FT_Library *ft;
  assert(font);
  assert(path);
  if (! (ft = cairo_font_ft()))
    return NULL;
  str_init_copy(&font->path, path);
  if (FT_New_Face(*ft, path->ptr.ps8, 0, &font->ft_face)) {
    warnx("cairo_font_init(%s): Error loading font", path->ptr.ps8);
    return NULL;
  }
  font->cairo_font_face = cairo_ft_font_face_create_for_ft_face
    (font->ft_face, 0);
  return font;
}

void cairo_font_set (const s_cairo_font *font, cairo_t *cr)
{
  cairo_set_font_face(cr, font->cairo_font_face);
}
