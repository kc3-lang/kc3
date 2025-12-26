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
#include "player.h"

u8 player_load (s_sequence *seq)
{
  (void) seq;
  return 0;
  /*
  s_gl_camera *camera;
  s_map *map;
  s_mat4 matrix;
  s_gl_sphere *sphere;
  const f32    sphere_radius = 5.0;
  s_window *window;
  assert(seq);
  window = seq->window;
  assert(window);
  camera = gl_camera_new(window->w, window->h);
  if (! camera)
    return false;
  if (! tag_map(&seq->tag, 4))
    return false;
  return true;
  */
}

u8 player_render (s_sequence *seq)
{
  (void) seq;
  return true;
}

u8 player_unload (s_sequence *seq)
{
  (void) seq;
  return true;
}
