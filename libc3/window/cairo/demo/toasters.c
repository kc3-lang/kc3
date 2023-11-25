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
#include <libc3/c3.h>
#include "../cairo_sprite.h"
#include "toasters.h"

static const f64 g_speed_x = 10.0;
static const f64 g_speed_y = -2.0;
s_cairo_sprite   g_toast_sprite = {0};
s_cairo_sprite   g_toaster_sprite = {0};

static s_tag * toaster_init (s_tag *toaster, f64 y)
{
  tag_init_map(toaster, 4);
  tag_init_sym(toaster->data.map.keys, sym_1("size"));
  tag_init_f64(toaster->data.map.values, 0);
  tag_init_sym(toaster->data.map.keys + 1, sym_1("speed"));
  tag_init_f64(toaster->data.map.values + 1, 0);
  tag_init_sym(toaster->data.map.keys + 2, sym_1("x"));
  tag_init_f64(toaster->data.map.values + 2, 0);
  tag_init_sym(toaster->data.map.keys + 3, sym_1("y"));
  tag_init_f64(toaster->data.map.values + 3, y);
  return toaster;
}

static void toaster_render (s_tag *toaster, s_window_cairo *window,
                            cairo_t *cr, s_sequence *seq)
{
  f64 *size;
  f64 *speed;
  f64 *x;
  f64 *y;
  if (toaster->type == TAG_MAP) {
    size  = &toaster->data.map.values[0].data.f64;
    speed = &toaster->data.map.values[1].data.f64;
    x     = &toaster->data.map.values[2].data.f64;
    y     = &toaster->data.map.values[3].data.f64;
    *speed += seq->dt;
    *size += *speed * 10.0;
    *x += *speed * g_speed_x;
    *y += *speed * g_speed_y;
    if (*x > window->w || *y < 0.0) {
      tag_clean(toaster);
      toaster->type = TAG_VOID;
      return;
    }
    cairo_sprite_blit(&g_toaster_sprite,
                      ((uw) seq->t) % g_toaster_sprite.frame_count,
                      cr, *x, *y);
  }
}

bool toasters_load (s_sequence *seq,
                    s_window_cairo *window)
{
  f64 y = 0.0;
  tag_clean(&seq->tag);
  tag_init_list(&seq->tag, NULL);
  while (y < window->h - window->w * g_speed_y / g_speed_x) {
    tag_init_list(&seq->tag, list_new_f64(y, seq->tag.data.list));
    y += 100.0;
  }
  return true;
}

bool toasters_render (s_sequence *seq, s_window_cairo *window,
                      cairo_t *cr)
{
  s_list *first;
  s_list *i;
  s_list *j;
  f64 x;
  f64 y;
  cairo_set_source_rgb(cr, 0.7, 0.95, 1.0);
  cairo_rectangle(cr, 0, 0, window->w, window->h);
  cairo_fill(cr);
  io_inspect(&seq->tag);
  if (seq->tag.type == TAG_LIST) {
    i = seq->tag.data.list;
    while (i) {
      j = NULL;
      if (i->tag.type == TAG_LIST)
        j = i->tag.data.list;
      if (j && j->tag.type == TAG_F64) {
        y = i->tag.data.list->tag.data.f64;
        first = list_next(j);
        x = 1000.0;
        if (first && first->tag.type == TAG_MAP)
          x = j->tag.data.map.values[2].data.f64;
        if (x > 100) {
          i->tag.data.list = list_new(i->tag.data.list);
          toaster_init(&i->tag.data.list->tag, y);
        }
        j = list_next(j);
        while (j) {
          toaster_render(&j->tag, window, cr, seq);
          j = list_next(j);
        }
      }
      i = list_next(i);
    }
  }
  return true;
}
