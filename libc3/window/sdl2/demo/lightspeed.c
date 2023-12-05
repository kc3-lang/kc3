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
#include "lightspeed.h"

static void star_init (s_tag *star)
{
  f64 x;
  f64 y;
  f64_random(&x);
  f64_random(&y);
  tag_init_map(star, 3);
  tag_init_sym(star->data.map.keys, sym_1("speed"));
  tag_init_f64(star->data.map.values, 0.0);
  tag_init_sym(star->data.map.keys + 1, sym_1("x"));
  tag_init_f64(star->data.map.values + 1, 2.0 * x - 1.0);
  tag_init_sym(star->data.map.keys + 2, sym_1("y"));
  tag_init_f64(star->data.map.values + 2, 2.0 * y - 1.0);
}

static void star_render (s_tag *star, s_sequence *seq)
{
  f64 q;
  f64 *speed;
  f64 *x;
  f64 *y;
  speed = &star->data.map.values[0].data.f64;
  x = &star->data.map.values[1].data.f64;
  y = &star->data.map.values[2].data.f64;
  glVertex2d(*x, *y);
  q = (1 + *speed / 20);
  glVertex2d(*x * q, *y * q);
  q = (1 + *speed / 100);
  *x = *x * q;
  *y = *y * q;
  *speed += seq->dt;
  if (*x < -1.0 || *x > 1.0 || *y < -1.0 || *y > 1.0)
    star_init(star);
}

bool lightspeed_load (s_sequence *seq, s_window_sdl2 *window)
{
  uw i;
  (void) window;
  tag_tuple(&seq->tag, LIGHTSPEED_STARS);
  i = 0;
  while (i < LIGHTSPEED_STARS) {
    star_init(seq->tag.data.tuple.tag + i);
    i++;
  }
  return true;
}

bool lightspeed_render (s_sequence *seq, s_window_sdl2 *window,
                        void *context)
{
  uw i;
  (void) window;
  (void) context;
  glDisable(GL_TEXTURE_2D);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  //glScale2d(window->w / 2.0, window->h / 2.0);
  //glTranslatef(1.0f, 1.0f, 0.0f);
  glClearColor(0, 0, 0, 1);
  glClear(GL_COLOR_BUFFER_BIT);
  glLineWidth(2);
  glColor4f(1, 1, 1, 0.7f);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_LINE_SMOOTH);
  glBegin(GL_LINES);
  i = 0;
  while (i < LIGHTSPEED_STARS) {
    star_render(seq->tag.data.tuple.tag + i, seq);
    i++;
  }
  glEnd();
  return true;
}
