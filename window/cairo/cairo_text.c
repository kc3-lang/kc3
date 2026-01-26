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
#include "../../libkc3/kc3.h"
#include "cairo_text.h"

void cairo_text_outline (cairo_t *cr, double x, double y,
                         const char *p)
{
  cairo_set_source_rgb(cr, 1.0, 1.0, 1.0);
  cairo_move_to(cr, x - 1.0, y - 1.0);
  cairo_show_text(cr, p);
  cairo_move_to(cr, x - 1.0, y);
  cairo_show_text(cr, p);
  cairo_move_to(cr, x - 1.0, y + 1.0);
  cairo_show_text(cr, p);
  cairo_move_to(cr, x, y - 1.0);
  cairo_show_text(cr, p);
  cairo_move_to(cr, x, y + 1.0);
  cairo_show_text(cr, p);
  cairo_move_to(cr, x + 1.0, y - 1.0);
  cairo_show_text(cr, p);
  cairo_move_to(cr, x + 1.0, y);
  cairo_show_text(cr, p);
  cairo_move_to(cr, x + 1.0, y + 1.0);
  cairo_show_text(cr, p);
  cairo_set_source_rgb(cr, 0.0, 0.0, 0.0);
  cairo_move_to(cr, x, y);
  cairo_show_text(cr, p);
}
