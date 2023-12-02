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
#include <libc3/c3.h>
#include "../types.h"
#include "bg_rect.h"

bool bg_rect_load (s_sequence *seq, s_window_sdl2 *window)
{
  (void) seq;
  (void) window;
  return true;
}

bool bg_rect_render (s_sequence *seq, s_window_sdl2 *window,
                     SDL_GLContext context)
{
  (void) context;
#define BG_RECT_COLOR_MAX 8
  const s_rgb color[BG_RECT_COLOR_MAX] = {
    {1, 1, 1},
    {1, 0, 0},
    {1, 1, 0},
    {0, 1, 0},
    {0, 1, 1},
    {0, 0, 1},
    {1, 0, 1},
    {0, 0, 0}
  };
  u8 c1;
  u8 c2;
  double p;
  double q;
  s_rgb rgb;
  c1 = (u8) trunc(seq->t) % BG_RECT_COLOR_MAX;
  c2 = (u8) trunc(seq->t + 1.0) % BG_RECT_COLOR_MAX;
  p = fmod(seq->t, 1.0);
  q = 1.0 - p;
  rgb.r = color[c1].r * q + color[c2].r * p;
  rgb.g = color[c1].g * q + color[c2].g * p;
  rgb.b = color[c1].b * q + color[c2].b * p;
  glColor3d(rgb.r, rgb.g, rgb.b);
  glRectd(0, 0, window->w, window->h);
  return true;
}
