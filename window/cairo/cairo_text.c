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
#include <libkc3/kc3.h>
#include "cairo_text.h"

void cairo_text_outline (cairo_t *cr, double x, double y,
                         const char *p)
{
  printf("1 %s\n", cairo_status_to_string(cairo_status(cr)));
  if (cairo_status(cr))
    abort();
  cairo_set_source_rgba(cr, 1.0, 1.0, 1.0, 1.0);
  printf("2 %s\n", cairo_status_to_string(cairo_status(cr)));
  if (cairo_status(cr))
    abort();
  cairo_move_to(cr, x - 1.0, y - 1.0);
  printf("3 %s\n", cairo_status_to_string(cairo_status(cr)));
  if (cairo_status(cr))
    abort();
  cairo_show_text(cr, p);
  printf("4 %d %s\n", cairo_status(cr),
         cairo_status_to_string(cairo_status(cr)));
  if (cairo_status(cr))
    abort();
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
  cairo_set_source_rgba(cr, 0.0, 0.0, 0.0, 1.0);
  cairo_move_to(cr, x, y);
  cairo_show_text(cr, p);
}
