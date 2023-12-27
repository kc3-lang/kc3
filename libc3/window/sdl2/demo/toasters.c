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
#include <math.h>
#include <libc3/c3.h>
#include "../gl_sprite.h"
#include "toasters.h"

#define TOASTERS_SCALE_TOAST   0.52
#define TOASTERS_SCALE_TOASTER 0.4
#define TOASTERS_SPACING \
  (g_sprite_toaster.h * TOASTERS_SCALE_TOASTER * 2.0)

static const f64 g_speed_x =  80.0;
static const f64 g_speed_y = -40.0;
s_gl_sprite    g_sprite_toast = {0};
s_gl_sprite    g_sprite_toaster = {0};

static bool toasters_render_toasters (s_list **toasters,
                                      s_window_sdl2 *window,
                                      s_sequence *seq);
static bool toasters_render_toasts (s_list **toasts,
                                    s_window_sdl2 *window,
                                    s_sequence *seq);

static s_tag * toast_init (s_tag *toast, f64 x, f64 y)
{
  tag_init_map(toast, 2);
  tag_init_sym(toast->data.map.key   + 0, sym_1("x"));
  tag_init_f64(toast->data.map.value + 0, x);
  tag_init_sym(toast->data.map.key   + 1, sym_1("y"));
  tag_init_f64(toast->data.map.value + 1, y);
  return toast;
}


static void toast_render (s_tag *toast, s_window_sdl2 *window,
                          s_sequence *seq)
{
  f64 *x;
  f64 *y;
  if (toast->type == TAG_MAP) {
    x = &toast->data.map.value[0].data.f64;
    y = &toast->data.map.value[1].data.f64;
    *x -= seq->dt * g_speed_x;
    *y -= seq->dt * g_speed_y;
    if (*x < -100 || *y > window->h) {
      tag_clean(toast);
      toast->type = TAG_VOID;
      return;
    }
    glPushMatrix();
    glTranslated(*x, *y + g_sprite_toast.h, 0.0);
    glScalef(TOASTERS_SCALE_TOAST, -TOASTERS_SCALE_TOAST, 1);
    gl_sprite_render(&g_sprite_toast, 0);
    glPopMatrix();
  }
}

static s_tag * toaster_init (s_tag *toaster, f64 y)
{
  tag_init_map(toaster, 2);
  tag_init_sym_1(toaster->data.map.key + 0, "x");
  tag_init_f64(toaster->data.map.value + 0, -150);
  tag_init_sym_1(toaster->data.map.key + 1, "y");
  tag_init_f64(toaster->data.map.value + 1, y);
  return toaster;
}

static void toaster_render (s_tag *toaster, s_window_sdl2 *window,
                            s_sequence *seq)
{
  f64 *x;
  f64 *y;
  if (toaster->type == TAG_MAP) {
    x = &toaster->data.map.value[0].data.f64;
    y = &toaster->data.map.value[1].data.f64;
    *x += seq->dt * g_speed_x;
    *y += seq->dt * g_speed_y;
    if (*x > window->w || *y < -300) {
      tag_clean(toaster);
      toaster->type = TAG_VOID;
      return;
    }
    glPushMatrix();
    glTranslated(*x, *y + g_sprite_toaster.h, 0.0);
    glScalef(TOASTERS_SCALE_TOASTER, -TOASTERS_SCALE_TOASTER, 1);
    gl_sprite_render(&g_sprite_toaster,
                     fmod(seq->t * g_sprite_toaster.frame_count,
                          g_sprite_toaster.frame_count));
    glPopMatrix();
  }
}

bool toasters_load (s_sequence *seq,
                    s_window_sdl2 *window)
{
  s_map *map;
  (void) window;
  tag_map(&seq->tag, 2);
  map = &seq->tag.data.map;
  tag_init_sym_1( map->key + 0, "toasters");
  tag_init_list(map->value + 0, NULL);
  tag_init_sym_1( map->key + 1, "toasts");
  tag_init_list(map->value + 1, NULL);
  return true;
}

bool toasters_render (s_sequence *seq, s_window_sdl2 *window,
                      void *context)
{
  s_list **toasters;
  s_list **toasts;
  (void) context;
  glClearColor(0.7f, 0.95f, 1.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);
  glPushMatrix();
  glTranslated(0, window->h, 0);
  glScalef(1, -1, 1);
  /* io_inspect(&seq->tag); */
  if (seq->tag.type == TAG_MAP) {
    toasters = &seq->tag.data.map.value[0].data.list;
    toasts   = &seq->tag.data.map.value[1].data.list;
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                    GL_LINEAR_MIPMAP_LINEAR);
    toasters_render_toasts(toasts, window, seq);
    toasters_render_toasters(toasters, window, seq);
  }
  glPopMatrix();
  return true;
}

bool toasters_render_toasts (s_list **toasts, s_window_sdl2 *window,
                             s_sequence *seq)
{
  s_list *i;
  s_list *j;
  s_map *map;
  s_list **t = NULL;
  f64 x;
  f64 y;
  assert(toasts);
  assert(window);
  assert(seq);
  y = window->w * g_speed_y / g_speed_x -
    TOASTERS_SPACING * 3.0 / 2.0 +
    50;
  if (*toasts && (*toasts)->tag.type == TAG_MAP) {
    t = &(*toasts)->tag.data.map.value[0].data.list;
    y =  (*toasts)->tag.data.map.value[1].data.f64;
  }
  while (y < window->h - TOASTERS_SPACING / 2) {
    y += TOASTERS_SPACING;
    *toasts = list_new_map(2, *toasts);
    map = &(*toasts)->tag.data.map;
    tag_init_sym_1(map->key  + 0, "toasts");
    tag_init_list(map->value + 0, NULL);
    tag_init_sym_1(map->key  + 1, "y");
    tag_init_f64(map->value  + 1, y);
  }
  i = *toasts;
  while (i) {
    if (i->tag.type == TAG_MAP) {
      t = &i->tag.data.map.value[0].data.list;
      y =  i->tag.data.map.value[1].data.f64;
      x = 0.0;
      if (*t && (*t)->tag.type == TAG_MAP)
        x = (*t)->tag.data.map.value[0].data.f64;
      if (x < window->w - 160.0) {
        *t = list_new(*t);
        toast_init(&(*t)->tag, window->w, y);
      }
      list_remove_void(t);
      j = *t;
      while (j) {
        toast_render(&j->tag, window, seq);
        j = list_next(j);
      }
    }
    i = list_next(i);
  }
  return true;
}

bool toasters_render_toasters (s_list **toasters, s_window_sdl2 *window,
                               s_sequence *seq)
{
  s_list *i;
  s_list *j;
  s_map *map;
  s_list **t = NULL;
  f64 x;
  f64 y;
  assert(toasters);
  assert(window);
  assert(seq);
  /* io_inspect_list((const s_list **) toasters); */
  y = -TOASTERS_SPACING - 40;
  if (*toasters && (*toasters)->tag.type == TAG_MAP) {
    t = &(*toasters)->tag.data.map.value[0].data.list;
    y =  (*toasters)->tag.data.map.value[1].data.f64;
  }
  while (y < window->h - window->w * g_speed_y / g_speed_x +
         TOASTERS_SPACING) {
    y += TOASTERS_SPACING;
    *toasters = list_new_map(2, *toasters);
    map = &(*toasters)->tag.data.map;
    tag_init_sym_1(map->key  + 0, "toasters");
    tag_init_list(map->value + 0, NULL);
    tag_init_sym_1(map->key  + 1, "y");
    tag_init_f64(map->value  + 1, y);
  }
  i = *toasters;
  while (i) {
    if (i->tag.type == TAG_MAP) {
      t = &i->tag.data.map.value[0].data.list;
      y =  i->tag.data.map.value[1].data.f64;
      x = 1000.0;
      if (*t && (*t)->tag.type == TAG_MAP)
        x = (*t)->tag.data.map.value[0].data.f64;
      if (x > 60.0) {
        *t = list_new(*t);
        toaster_init(&(*t)->tag, y);
      }
      list_remove_void(t);
      j = *t;
      while (j) {
        toaster_render(&j->tag, window, seq);
        j = list_next(j);
      }
    }
    i = list_next(i);
  }
  return true;
}
