/* kc3
 * Copyright 2022,2023,2024 kmx.io <contact@kmx.io>
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
#import <Cocoa/Cocoa.h>

#include <cairo-quartz.h>
#include "../../libkc3/file.h"
#include "../../libkc3/io.h"
#include "../../libkc3/str.h"
#include "../../libkc3/sym.h"
#include "cairo_font.h"

s_cairo_font * cairo_font_init (s_cairo_font *font, const char *path)
{
  CGFontRef cg_font_ref;
  CGDataProviderRef cg_data_provider;
  str_init_copy_1(&font->path, path);
  if (! file_search(&font->path, &g_sym_r, &font->real_path)) {
    err_write_1("cairo_font_init: file not found: ");
    err_puts(path);
    str_clean(&font->path);
    return NULL;
  }
  if (! (cg_data_provider =
         CGDataProviderCreateWithFilename(font->real_path.ptr.pchar)))
    return NULL;
  cg_font_ref = CGFontCreateWithDataProvider(cg_data_provider);
  font->cairo_font_face =
    cairo_quartz_font_face_create_for_cgfont(cg_font_ref);
  CGFontRelease(cg_font_ref);
  CGDataProviderRelease(cg_data_provider);
  return font->cairo_font_face ? font : NULL;
}
