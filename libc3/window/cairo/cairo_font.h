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
#ifndef LIBC3_WINDOW_CAIRO_CAIRO_FONT_H
#define LIBC3_WINDOW_CAIRO_CAIRO_FONT_H

#include <assert.h>
#include <err.h>
#include "types.h"

/* Stack-allocation compatible functions, call cairo_font_clean after
   use. */
void           cairo_font_clean (s_cairo_font *font);
s_cairo_font * cairo_font_init (s_cairo_font *font, const s8 *path);

/* Observers */
void cairo_set_font (cairo_t *cr, const s_cairo_font *font);

#endif /* LIBC3_WINDOW_CAIRO_CAIRO_FONT_H */
