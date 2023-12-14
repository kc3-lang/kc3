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

#define EARTH_CAMERA_ROTATION_Z_SPEED 0.1
#define EARTH_SEGMENTS_U 200
#define EARTH_SEGMENTS_V 100

s_sdl2_sprite g_sprite_earth = {0};

bool earth_load (s_sequence *seq,
                 s_window_sdl2 *window)
{
  s_map *map;
  s_gl_camera *camera;
  s_gl_sphere *sphere;
  (void) window;
  camera = gl_camera_new(window->w, window->h);
  if (! camera)
    return false;
  sphere = gl_sphere_new(EARTH_SEGMENTS_U, EARTH_SEGMENTS_V);
  if (! sphere)
    return false;
  if (! tag_map(&seq->tag, 3))
    return false;
  map = &seq->tag.data.map;
  tag_init_sym_1(             map->key + 0, "camera");
  tag_init_ptr_free(        map->value + 0, camera);
  tag_init_sym_1(             map->key + 1, "camera_rot_x_speed");
  tag_init_f64(             map->value + 1, 0.01);
  tag_init_sym_1(             map->key + 2, "sphere");
  tag_init_struct_with_data(map->value + 2, sym_1("GL.Sphere"),
                            true, sphere);
  return true;
}

bool earth_render (s_sequence *seq, s_window_sdl2 *window,
                   void *context)
{
  s_gl_camera *camera;
  f64         *camera_rot_x_speed;
  s_map *map;
  s_gl_sphere *sphere;
  f64 sphere_radius;
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
      map->value[1].type != TAG_F64 ||
      map->value[2].type != TAG_PTR) {
    warnx("earth_render: invalid map");
    return false;
  }
  camera             =  map->value[0].data.ptr.p;
  camera_rot_x_speed = &map->value[1].data.f64;
  sphere             =  map->value[2].data.ptr.p;
  gl_camera_set_aspect_ratio(camera, window->w, window->h);
  camera->rotation.x += seq->dt * (*camera_rot_x_speed) *
    360.0;
  if (camera->rotation.x > 180 || camera->rotation.x < 0)
    *camera_rot_x_speed *= -1.0;
  camera->rotation.z += seq->dt * EARTH_CAMERA_ROTATION_Z_SPEED *
    360.0;
  gl_camera_render(camera);
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
  f32 ambiant[4] = {0.1f, 0.1f, 0.1f, 1.0f};
  glLightfv(GL_LIGHT0, GL_AMBIENT, ambiant);
  f32 diffuse[4] = {1.0f, 0.92f, 0.83f, 1.0f};
  glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
  glLightfv(GL_LIGHT0, GL_SPECULAR, diffuse);
  f32 position[4] = {-1000.0, 0.0, 0.0, 1.0};
  glLightfv(GL_LIGHT0, GL_POSITION, position);
  glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, 0.1f);
  glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, 0.0f);
  glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 0.0f);
  glEnable(GL_DEPTH_TEST);
  glPushMatrix(); {
    sphere_radius = 5.0;
    glScalef(sphere_radius, sphere_radius, sphere_radius);
    glEnable(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                    GL_LINEAR_MIPMAP_LINEAR);
    sdl2_sprite_bind(&g_sprite_earth, 0);
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
    gl_sphere_render(sphere);
    /*
      glDisable(GL_TEXTURE_2D);
      glColor4f(0.0f, 0.0f, 0.0f, 1.0f);
      gl_sphere_render_wireframe(sphere);
    */
  } glPopMatrix();
  glDisable(GL_TEXTURE_2D);
  glDisable(GL_DEPTH_TEST);
  glDisable(GL_LIGHT0);
  glDisable(GL_LIGHTING);
  return true;
}
