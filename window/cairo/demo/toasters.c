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
#include <math.h>
#include "../../../libkc3/kc3.h"
#include "../cairo_sprite.h"
#include "toasters.h"

static const f64 g_speed_x =  80.0;
static const f64 g_speed_y = -40.0;
s_cairo_sprite   g_sprite_toast = {0};
s_cairo_sprite   g_sprite_toaster = {0};

static bool toasters_render_toasters (p_list *toasters,
                                      s_window_cairo *window,
                                      cairo_t *cr,
                                      s_sequence *seq);
static bool toasters_render_toasts (p_list *toasts,
                                    s_window_cairo *window,
                                    cairo_t *cr,
                                    s_sequence *seq);

static s_tag * toast_init (s_tag *toast, f64 x, f64 y)
{
  tag_init_map(toast, 2);
  tag_init_psym(toast->data.td_map.key   + 0, sym_1("x"));
  tag_init_f64( toast->data.td_map.value + 0, x);
  tag_init_psym(toast->data.td_map.key   + 1, sym_1("y"));
  tag_init_f64( toast->data.td_map.value + 1, y);
  return toast;
}

static void toast_render (s_tag *toast, s_window_cairo *window,
                          cairo_t *cr, s_sequence *seq)
{
  cairo_matrix_t matrix;
  f64 *x;
  f64 *y;
  if (toast->type == TAG_MAP) {
    x = &toast->data.td_map.value[0].data.td_f64;
    y = &toast->data.td_map.value[1].data.td_f64;
    *x -= seq->dt * g_speed_x;
    *y -= seq->dt * g_speed_y;
    if (*x < -100 || *y > window->h) {
      tag_clean(toast);
      toast->type = TAG_VOID;
      return;
    }
    cairo_get_matrix(cr, &matrix);
    cairo_translate(cr, *x, *y);
    cairo_sprite_blit(&g_sprite_toast, 0, cr, 0, 0);
    cairo_set_matrix(cr, &matrix);
  }
}

static s_tag * toaster_init (s_tag *toaster, f64 y)
{
  tag_init_map(toaster, 2);
  tag_init_psym(toaster->data.td_map.key   + 0, sym_1("x"));
  tag_init_f64( toaster->data.td_map.value + 0, -150);
  tag_init_psym(toaster->data.td_map.key   + 1, sym_1("y"));
  tag_init_f64( toaster->data.td_map.value + 1, y);
  return toaster;
}

static void toaster_render (s_tag *toaster, s_window_cairo *window,
                            cairo_t *cr, s_sequence *seq)
{
  cairo_matrix_t matrix;
  f64 *x;
  f64 *y;
  if (toaster->type == TAG_MAP) {
    x = &toaster->data.td_map.value[0].data.td_f64;
    y = &toaster->data.td_map.value[1].data.td_f64;
    *x += seq->dt * g_speed_x;
    *y += seq->dt * g_speed_y;
    if (*x > window->w || *y < -200) {
      tag_clean(toaster);
      toaster->type = TAG_VOID;
      return;
    }
    cairo_get_matrix(cr, &matrix);
    cairo_translate(cr, *x, *y);
    cairo_sprite_blit(&g_sprite_toaster,
                      fmod(seq->t * g_sprite_toaster.frame_count,
                           g_sprite_toaster.frame_count),
                      cr, 0, 0);
    cairo_set_matrix(cr, &matrix);
  }
}

bool toasters_load (s_sequence *seq)
{
  s_map *map;
  tag_map(&seq->tag, 2);
  map = &seq->tag.data.td_map;
  tag_init_psym( map->key   + 0, sym_1("toasters"));
  tag_init_plist(map->value + 0, NULL);
  tag_init_psym( map->key   + 1, sym_1("toasts"));
  tag_init_plist(map->value + 1, NULL);
  return true;
}

bool toasters_render (s_sequence *seq)
{
  cairo_t *cr;
  p_list *toasters;
  p_list *toasts;
  s_window_cairo *window;
  window = seq->window;
  cr = window->cr;
  cairo_set_source_rgb(cr, 0.7, 0.95, 1.0);
  cairo_rectangle(cr, 0, 0, window->w, window->h);
  cairo_fill(cr);
  /* io_inspect(&seq->tag); */
  if (seq->tag.type == TAG_MAP) {
    toasters = &seq->tag.data.td_map.value[0].data.td_plist;
    toasts   = &seq->tag.data.td_map.value[1].data.td_plist;
    toasters_render_toasts(toasts, window, cr, seq);
    toasters_render_toasters(toasters, window, cr, seq);
  }
  return true;
}

bool toasters_render_toasts (p_list *toasts, s_window_cairo *window,
                             cairo_t *cr, s_sequence *seq)
{
  s_list *i;
  s_list *j;
  s_map *map;
  p_list *t = NULL;
  f64 x;
  f64 y;
  assert(toasts);
  assert(window);
  assert(cr);
  assert(seq);
  y = window->w * g_speed_y / g_speed_x - 210;
  if (*toasts && (*toasts)->tag.type == TAG_MAP) {
    t = &(*toasts)->tag.data.td_map.value[0].data.td_plist;
    y =  (*toasts)->tag.data.td_map.value[1].data.td_f64;
  }
  while (y < window->h - 100) {
    y += 170.0;
    *toasts = list_new_map(2, *toasts);
    map = &(*toasts)->tag.data.td_map;
    tag_init_psym( map->key   + 0, sym_1("toasts"));
    tag_init_plist(map->value + 0, NULL);
    tag_init_psym( map->key   + 1, sym_1("y"));
    tag_init_f64(  map->value + 1, y);
  }
  i = *toasts;
  while (i) {
    if (i->tag.type == TAG_MAP) {
      t = &i->tag.data.td_map.value[0].data.td_plist;
      y =  i->tag.data.td_map.value[1].data.td_f64;
      x = 0.0;
      if (*t && (*t)->tag.type == TAG_MAP)
        x = (*t)->tag.data.td_map.value[0].data.td_f64;
      if (x < window->w - 160.0) {
        *t = list_new(*t);
        toast_init(&(*t)->tag, window->w, y);
      }
      plist_remove_void(t);
      j = *t;
      while (j) {
        toast_render(&j->tag, window, cr, seq);
        j = list_next(j);
      }
    }
    i = list_next(i);
  }
  return true;
}

bool toasters_render_toasters (p_list *toasters,
                               s_window_cairo *window, cairo_t *cr,
                               s_sequence *seq)
{
  s_list *i;
  s_list *j;
  s_map *map;
  p_list *t = NULL;
  f64 x;
  f64 y;
  assert(toasters);
  assert(window);
  assert(cr);
  assert(seq);
  /* io_inspect_list((const p_list *) toasters); */
  y = -100.0;
  if (*toasters && (*toasters)->tag.type == TAG_MAP) {
    t = &(*toasters)->tag.data.td_map.value[0].data.td_plist;
    y =  (*toasters)->tag.data.td_map.value[1].data.td_f64;
  }
  while (y < window->h - window->w * g_speed_y / g_speed_x) {
    y += 170.0;
    *toasters = list_new_map(2, *toasters);
    map = &(*toasters)->tag.data.td_map;
    tag_init_psym( map->key   + 0, sym_1("toasters"));
    tag_init_plist(map->value + 0, NULL);
    tag_init_psym( map->key   + 1, sym_1("y"));
    tag_init_f64(  map->value + 1, y);
  }
  i = *toasters;
  while (i) {
    if (i->tag.type == TAG_MAP) {
      t = &i->tag.data.td_map.value[0].data.td_plist;
      y =  i->tag.data.td_map.value[1].data.td_f64;
      x = 1000.0;
      if (*t && (*t)->tag.type == TAG_MAP)
        x = (*t)->tag.data.td_map.value[0].data.td_f64;
      if (x > 60.0) {
        *t = list_new(*t);
        toaster_init(&(*t)->tag, y);
      }
      plist_remove_void(t);
      j = *t;
      while (j) {
        toaster_render(&j->tag, window, cr, seq);
        j = list_next(j);
      }
    }
    i = list_next(i);
  }
  return true;
}

bool toasters_unload (s_sequence *seq)
{
  (void) seq;
  return true;
}
