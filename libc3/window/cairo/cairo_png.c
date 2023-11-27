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
#include <unistd.h>
#include <cairo.h>
#include <libc3/c3.h>
#include "cairo_png.h"

cairo_surface_t * cairo_png_1 (const s8 *path)
{
  s_str found_str;
  s_str path_str;
  cairo_surface_t *surface = NULL;
  assert(path);
  str_init_1(&path_str, NULL, path);
  if (file_search(&path_str, sym_1("r"), &found_str)) {
    surface = cairo_image_surface_create_from_png(found_str.ptr.ps8);
    if (! surface)
      warnx("cairo_png_1: cairo_image_surface_create_from_png: %s",
            found_str.ptr.ps8);
    str_clean(&found_str);
  }
  else
    warnx("cairo_png_1: file not found: %s", path);
  return surface;
}
