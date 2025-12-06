/* kc3
 * Copyright from 2022 to 2025 kmx.io <contact@kmx.io>
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
#include "../../libkc3/kc3.h"
#include "../../gl/gl.h"
#include "../window.h"
#include "bg_rect.h"

bool bg_rect_load (s_sequence *seq)
{
  (void) seq;
  return true;
}

bool bg_rect_render (s_sequence *seq)
{
#define BG_RECT_COLOR_MAX 8
  const s_rgb color[BG_RECT_COLOR_MAX] = {
    {0, 0, 0},
    {1, 0, 0},
    {1, 1, 0},
    {0, 1, 0},
    {0, 1, 1},
    {0, 0, 1},
    {1, 0, 1},
    {1, 1, 1}
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
  assert(glGetError() == GL_NO_ERROR);
  glClearColor(rgb.r, rgb.g, rgb.b, 1.0);
  assert(glGetError() == GL_NO_ERROR);
  glClear(GL_COLOR_BUFFER_BIT);
  assert(glGetError() == GL_NO_ERROR);
  return true;
}

bool bg_rect_unload (s_sequence *seq)
{
  (void) seq;
  return true;
}
