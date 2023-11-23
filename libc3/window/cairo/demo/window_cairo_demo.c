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
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <libc3/c3.h>
#include <xkbcommon/xkbcommon.h>
#include "../../window.h"
#include "../window_cairo.h"
#include "window_cairo_demo.h"
#include "../types.h"
#include "bg_rect.h"
#include "lightspeed.h"

bool window_cairo_demo_button (s_window_cairo *window, u8 button,
                                  sw x, sw y)
{
  assert(window);
  (void) window;
  printf("c3_window_cairo_demo_button: %lu (%ld, %ld)\n", (uw) button, x, y);
  return true;
}

bool window_cairo_demo_key (s_window_cairo *window, uw keysym)
{
  char keysym_name[64];
  assert(window);
  (void) window;
  switch (keysym) {
  case XKB_KEY_Escape:
  case XKB_KEY_q:
    g_c3_exit_code = 0;
    return false;
  case XKB_KEY_Left:
    if (! window_set_sequence_pos((s_window *) window,
                                  (window->sequence_pos +
                                   window->sequence_count - 1) %
                                  window->sequence_count))
      return false;
    break;
  case XKB_KEY_Right:
    if (! window_set_sequence_pos((s_window *) window,
                                  (window->sequence_pos + 1) %
                                  window->sequence_count))
      return false;
    break;
  default:
    xkb_keysym_get_name(keysym, keysym_name, sizeof(keysym_name));
    printf("c3_window_cairo_demo_key: %lu %s\n", keysym, keysym_name);
  }
  return true;
}

bool window_cairo_demo_load (s_window_cairo *window)
{
  assert(window->sequence_count == C3_WINDOW_CAIRO_DEMO_SEQUENCE_COUNT);
  window_cairo_sequence_init(window->sequence, 8.0,
                             "01. Background rectangles",
                             bg_rect_load, bg_rect_render);
  window_cairo_sequence_init(window->sequence + 1, 30.0,
                             "02. Lightspeed",
                             lightspeed_load, lightspeed_render);
  window_set_sequence_pos((s_window *) window, 0);
  return true;
}

bool window_cairo_demo_render (s_window_cairo *window,
                                  cairo_t *cr)
{
  s_sequence *seq;
  cairo_text_extents_t te;
  assert(window);
  assert(cr);
  if (! window_animate((s_window *) window))
    return false;
  seq = window->sequence + window->sequence_pos;
  seq->render(window, cr, seq);
  /* text */
  cairo_set_font_size(cr, 20);
  cairo_select_font_face(cr, "Courier New",
                          CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
  cairo_text_extents(cr, seq->title, &te);
  /* text bg */
  cairo_set_source_rgb(cr, 1.0, 1.0, 1.0);
  cairo_move_to(cr, 19, window->h - te.height - te.y_bearing - 21);
  cairo_show_text(cr, seq->title);
  cairo_move_to(cr, 19, window->h - te.height - te.y_bearing - 20);
  cairo_show_text(cr, seq->title);
  cairo_move_to(cr, 19, window->h - te.height - te.y_bearing - 19);
  cairo_show_text(cr, seq->title);
  cairo_move_to(cr, 20, window->h - te.height - te.y_bearing - 21);
  cairo_show_text(cr, seq->title);
  cairo_move_to(cr, 20, window->h - te.height - te.y_bearing - 19);
  cairo_show_text(cr, seq->title);
  cairo_move_to(cr, 21, window->h - te.height - te.y_bearing - 19);
  cairo_show_text(cr, seq->title);
  cairo_move_to(cr, 21, window->h - te.height - te.y_bearing - 20);
  cairo_show_text(cr, seq->title);
  cairo_move_to(cr, 21, window->h - te.height - te.y_bearing - 21);
  cairo_show_text(cr, seq->title);
  /* text */
  cairo_set_source_rgb(cr, 0.0, 0.0, 0.0);
  cairo_move_to(cr, 20, window->h - te.height - te.y_bearing - 20);
  cairo_show_text(cr, seq->title);
  /* progress bar */
  cairo_set_source_rgb(cr, 1.0, 1.0, 1.0);
  cairo_rectangle(cr, 19, window->h - 12,
                  (window->w - 40.0) * seq->t / seq->duration + 2,
                  4);
  cairo_fill(cr);
  cairo_set_source_rgb(cr, 0.0, 0.0, 0.0);
  cairo_rectangle(cr, 20, window->h - 11,
                  (window->w - 40.0) * seq->t / seq->duration,
                  2);
  cairo_fill(cr);
  return true;
}

bool window_cairo_demo_resize (s_window_cairo *window,
                                  uw w, uw h)
{
  assert(window);
  (void) window;
  (void) w;
  (void) h;
  return true;
}
