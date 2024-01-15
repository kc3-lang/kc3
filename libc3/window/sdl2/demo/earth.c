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
#include "../gl_sprite.h"
#include "../gl_camera.h"
#include "../gl_sphere.h"
#include "earth.h"

#define EARTH_CAMERA_ROTATION_Z_SPEED 0.1
#define EARTH_SEGMENTS_U 200
#define EARTH_SEGMENTS_V 100

s_gl_sprite g_sprite_earth = {0};

bool earth_load (s_sequence *seq)
{
  s_map *map;
  s_gl_camera *camera;
  s_gl_sphere *sphere;
  s_window_sdl2 *window;
  assert(seq);
  window = seq->window;
  assert(window);
  camera = gl_camera_new(window->w, window->h);
  if (! camera)
    return false;
  sphere = gl_sphere_new(EARTH_SEGMENTS_U, EARTH_SEGMENTS_V);
  if (! sphere)
    return false;
  if (! tag_map(&seq->tag, 3))
    return false;
  map = &seq->tag.data.map;
  tag_init_sym(               map->key + 0, sym_1("camera"));
  tag_init_ptr_free(        map->value + 0, camera);
  tag_init_sym(               map->key + 1, sym_1("camera_rot_x_speed"));
  tag_init_f64(             map->value + 1, 0.01);
  tag_init_sym(               map->key + 2, sym_1("sphere"));
  tag_init_struct_with_data(map->value + 2, sym_1("GL.Sphere"),
                            true, sphere);
  return true;
}

bool earth_render (s_sequence *seq)
{
  s_gl_camera *camera;
  f64         *camera_rot_x_speed;
  s_map *map;
  s_gl_sphere *sphere;
  //f64 sphere_radius;
  s_window_sdl2 *window;
  assert(seq);
  window = seq->window;
  assert(window);
  if (! seq || seq->tag.type != TAG_MAP ||
      seq->tag.data.map.count != 3) {
    err_puts("earth_render: invalid seq->tag");
    return false;
  }
  map = &seq->tag.data.map;
  if (map->value[0].type != TAG_PTR_FREE ||
      map->value[1].type != TAG_F64 ||
      map->value[2].type != TAG_STRUCT) {
    err_puts("earth_render: invalid map");
    return false;
  }
  camera             =  map->value[0].data.ptr_free.p;
  camera_rot_x_speed = &map->value[1].data.f64;
  sphere             =  map->value[2].data.struct_.data;
  gl_camera_set_aspect_ratio(camera, window->w, window->h);
  camera->rotation.x += seq->dt * (*camera_rot_x_speed) *
    M_PI * 2.0f;
  if (camera->rotation.x > 180 || camera->rotation.x < 0)
    *camera_rot_x_speed *= -1.0;
  camera->rotation.z += seq->dt * EARTH_CAMERA_ROTATION_Z_SPEED *
    M_PI * 2.0f;
  assert(glGetError() == GL_NO_ERROR);
  gl_camera_render(camera);
  assert(glGetError() == GL_NO_ERROR);
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  assert(glGetError() == GL_NO_ERROR);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  assert(glGetError() == GL_NO_ERROR);
  //glEnable(GL_LIGHTING);
  assert(glGetError() == GL_NO_ERROR);
  //glEnable(GL_LIGHT0);
  assert(glGetError() == GL_NO_ERROR);
  //f32 ambiant[4] = {0.1f, 0.1f, 0.1f, 1.0f};
  //glLightfv(GL_LIGHT0, GL_AMBIENT, ambiant);
  //f32 diffuse[4] = {1.0f, 0.92f, 0.83f, 1.0f};
  //glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
  //glLightfv(GL_LIGHT0, GL_SPECULAR, diffuse);
  //f32 position[4] = {-1000.0, 0.0, 0.0, 1.0};
  //glLightfv(GL_LIGHT0, GL_POSITION, position);
  //glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, 0.1f);
  //glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, 0.0f);
  //glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 0.0f);
  assert(glGetError() == GL_NO_ERROR);
  glEnable(GL_DEPTH_TEST);
  assert(glGetError() == GL_NO_ERROR);
  //glPushMatrix();
  {
    //sphere_radius = 5.0;
    assert(glGetError() == GL_NO_ERROR);
    //glScalef(sphere_radius, sphere_radius, sphere_radius);
    assert(glGetError() == GL_NO_ERROR);
    //glEnable(GL_TEXTURE_2D);
    assert(glGetError() == GL_NO_ERROR);
    gl_sprite_bind(&g_sprite_earth, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                    GL_LINEAR_MIPMAP_LINEAR);
    assert(glGetError() == GL_NO_ERROR);
    glBlendColor(1.0f, 1.0f, 1.0f, 1.0f);
    gl_sphere_render(sphere);
    /*
    glDisable(GL_TEXTURE_2D);
    glColor4f(0.0f, 0.0f, 0.0f, 1.0f);
    gl_sphere_render_wireframe(sphere);
    */
  } // glPopMatrix();
  //glDisable(GL_TEXTURE_2D);
  glDisable(GL_DEPTH_TEST);
  //glDisable(GL_LIGHT0);
  //glDisable(GL_LIGHTING);
  return true;
}

bool earth_unload (s_sequence *seq)
{
  (void) seq;
  return true;
}
