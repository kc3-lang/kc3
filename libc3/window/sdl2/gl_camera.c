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
#include "gl_camera.h"

s_gl_camera * gl_camera_init (s_gl_camera *camera, uw w, uw h)
{
  assert(camera);
  gl_camera_set_aspect_ratio(camera, w, h);
  camera->clip_z_far = 1000;
  camera->clip_z_near = 0.1;
  camera->fov_y = 90.0;
  camera->position.x = 0.0;
  camera->position.y = 0.0;
  camera->position.z = -10.0;
  camera->rotation.x = 90.0;
  camera->rotation.y = 0.0;
  camera->rotation.z = 0.0;
  return camera;
}

void gl_camera_delete (s_gl_camera *camera)
{
  free(camera);
}

s_gl_camera * gl_camera_new (uw w, uw h)
{
  s_gl_camera *camera;
  camera = calloc(1, sizeof(s_gl_camera));
  if (! camera) {
    warn("gl_camera_new: camera");
    return NULL;
  }
  if (! gl_camera_init(camera, w, h)) {
    free(camera);
    return NULL;
  }
  return camera;
}

void gl_camera_render (const s_gl_camera *camera)
{
  assert(camera);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(camera->fov_y, camera->aspect_ratio,
                 camera->clip_z_near, camera->clip_z_far);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glTranslated(camera->position.x, camera->position.y,
               camera->position.z);
  glRotated(camera->rotation.x, 1.0, 0.0, 0.0);
  glRotated(camera->rotation.y, 0.0, 1.0, 0.0);
  glRotated(camera->rotation.z, 0.0, 0.0, 1.0);
}

s_gl_camera * gl_camera_set_aspect_ratio (s_gl_camera *camera, uw w,
                                          uw h)
{
  assert(camera);
  camera->aspect_ratio = (f64) (w ? w : 1) / (h ? h : 1);
  return camera;
}
