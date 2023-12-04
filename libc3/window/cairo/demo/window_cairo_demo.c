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
#include "../cairo_sprite.h"
#include "window_cairo_demo.h"
#include "bg_rect.h"
#include "lightspeed.h"
#include "toasters.h"
#include "flies.h"

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
  assert(window);
  if (window->sequence_count != WINDOW_CAIRO_DEMO_SEQUENCE_COUNT) {
    fprintf(stderr, "window_cairo_demo_load: "
            "window->sequence_count = %lu\n", window->sequence_count);
    assert(window->sequence_count == WINDOW_CAIRO_DEMO_SEQUENCE_COUNT);
    return false;
  }
  window_cairo_sequence_init(window->sequence, 8.0,
                             "01. Background rectangles",
                             bg_rect_load, bg_rect_render);
  window_cairo_sequence_init(window->sequence + 1, 20.0,
                             "02. Lightspeed",
                             lightspeed_load, lightspeed_render);
  if (! cairo_sprite_init(&g_toaster_sprite, "img/flaps.png",
                          4, 1, 4))
    return false;
  if (! cairo_sprite_init(&g_toast_sprite, "img/toast.png",
                          1, 1, 1))
    return false;
  window_cairo_sequence_init(window->sequence + 2, 60.0,
                             "03. Toasters",
                             toasters_load, toasters_render);
  if (! cairo_sprite_init(&g_fly_sprite, "img/fly-noto.png",
                          1, 1, 1))
    return false;
  if (! cairo_sprite_init(&g_dead_fly_sprite, "img/fly-dead.png",
                          1, 1, 1))
    return false;
  window_cairo_sequence_init(window->sequence + 3, 60.0,
                             "04. Flies",
                             flies_load, flies_render);
  window_set_sequence_pos((s_window *) window, 0);
  return true;
}

static void render_text (cairo_t *cr, double x, double y,
                         const s8 *p)
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
  /* text */
  cairo_set_source_rgb(cr, 0.0, 0.0, 0.0);
  cairo_move_to(cr, x, y);
  cairo_show_text(cr, p);
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
  if (! seq->render(seq, window, cr))
    return false;
  /* text */
  cairo_set_font_size(cr, 20);
  cairo_select_font_face(cr, "Courier New",
                         CAIRO_FONT_SLANT_NORMAL,
                         CAIRO_FONT_WEIGHT_NORMAL);
  cairo_text_extents(cr, seq->title, &te);
  render_text(cr, 20.0, window->h - te.height - te.y_bearing - 20,
              seq->title);
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
  /* fps */
  s8 fps[32];
  snprintf(fps, sizeof(fps), "%f", (f64) seq->frame / seq->t);
  cairo_text_extents(cr, fps, &te);
  render_text(cr, 20.0, 20.0 + te.height, fps);
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
