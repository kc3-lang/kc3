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
#include "../window_sdl2.h"
#include "../sdl2_font.h"
#include "../sdl2_sprite.h"
#include "../gl_camera.h"
#include "../gl_sphere.h"
#include "earth.h"

#define EARTH_SEGMENTS 10

s_sdl2_sprite g_sprite_earth = {0};

bool earth_load (s_sequence *seq,
                 s_window_sdl2 *window)
{
  s_map *map;
  s_gl_camera *camera;
  s_gl_sphere *sphere;
  (void) window;
  // FIXME: leak
  camera = gl_camera_new(window->w, window->h);
  if (! camera)
    return false;
  sphere = gl_sphere_new(EARTH_SEGMENTS);
  if (! sphere)
    return false;
  if (! tag_map(&seq->tag, 2))
    return false;
  map = &seq->tag.data.map;
  tag_init_sym_1(map->key + 0, "camera");
  tag_init_ptr(map->value + 0, sym_1("GL.Camera"), camera);
  tag_init_sym_1(map->key + 1, "sphere");
  tag_init_ptr(map->value + 1, sym_1("GL.Sphere"), sphere);
  return true;
}

bool earth_render (s_sequence *seq, s_window_sdl2 *window,
                   void *context)
{
  s_gl_camera *camera;
  s_map *map;
  s_gl_sphere *sphere;
  assert(seq);
  assert(window);
  (void) context;
  if (! seq || seq->tag.type != TAG_MAP ||
      seq->tag.data.map.count < 2) {
    warnx("earth_render: invalid seq->tag");
    return false;
  }
  map = &seq->tag.data.map;
  if (map->value[0].type != TAG_PTR ||
      map->value[1].type != TAG_PTR) {
    warnx("earth_render: invalid map");
    return false;
  }
  camera = map->value[0].data.ptr.p;
  sphere = map->value[1].data.ptr.p;
  gl_camera_set_aspect_ratio(camera, window->w, window->h);
  gl_camera_render(camera);
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
  glDisable(GL_TEXTURE_2D);
  //sdl2_sprite_bind(&g_sprite_earth, 0);
  glPointSize(4.0);
  gl_sphere_render(sphere);
  return true;
}
