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
#ifndef LIBKC3_WINDOW_CAIRO_CAIRO_TEXT_H
#define LIBKC3_WINDOW_CAIRO_CAIRO_TEXT_H

#include "types.h"

/* Stack-allocation compatible functions, call cairo_font_clean
   after use. */
void           cairo_font_clean (s_cairo_font *font);
s_cairo_font * cairo_font_init (s_cairo_font *font, const char *path);

/* Observers */
void cairo_text_outline (cairo_t *cr, double x, double y,
                         const char *p);

#endif /* LIBKC3_WINDOW_CAIRO_CAIRO_TEXT_H */
