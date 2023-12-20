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
#include "gl_camera.h"
#include "gl_matrix_4d.h"

static const s8 * g_gl_camera_vertex_shader_src = "#version 460 core\n"
"layout (location = 0) in dvec3 aPos;\n"
"uniform dmat4 matrix;\n"
"\n"
"void main() {\n"
"  gl_Position = matrix * dvec4(aPos, 1.0);\n"
"}\n";

void gl_camera_clean (s_gl_camera *camera)
{
  assert(camera);
  glDeleteProgram(camera->gl_shader_program);
}

void gl_camera_delete (s_gl_camera *camera)
{
  gl_camera_clean(camera);
  free(camera);
}

s_gl_camera * gl_camera_init (s_gl_camera *camera, uw w, uw h)
{
  u32 vertex_shader;
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
  vertex_shader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertex_shader, 1, &g_gl_camera_vertex_shader_src,
                 NULL);
  glCompileShader(vertex_shader);
  camera->gl_shader_program = glCreateProgram();
  glAttachShader(camera->gl_shader_program, vertex_shader);
  glLinkProgram(camera->gl_shader_program);
  glDeleteShader(vertex_shader);
  camera->gl_matrix_loc =
    glGetUniformLocation(camera->gl_shader_program, "matrix");
  return camera;
}

s_gl_camera * gl_camera_new (uw w, uw h)
{
  s_gl_camera *camera;
  camera = calloc(1, sizeof(s_gl_camera));
  if (! camera) {
    err_puts("gl_camera_new: failed to allocate memory");
    return NULL;
  }
  if (! gl_camera_init(camera, w, h)) {
    free(camera);
    return NULL;
  }
  return camera;
}

void gl_camera_render (s_gl_camera *camera)
{
  assert(camera);
  gl_matrix_4d_init_identity(&camera->matrix);
  gl_matrix_4d_perspective(&camera->matrix, camera->fov_y,
                           camera->aspect_ratio, camera->clip_z_near,
                           camera->clip_z_far);
  gl_matrix_4d_translate(&camera->matrix, camera->position.x,
                         camera->position.y, camera->position.z);
  gl_matrix_4d_rotate_axis(&camera->matrix, camera->rotation.x,
                           &(s_gl_point_3d) { 1.0, 0.0, 0.0 });
  gl_matrix_4d_rotate_axis(&camera->matrix, camera->rotation.y,
                           &(s_gl_point_3d) { 0.0, 1.0, 0.0 });
  gl_matrix_4d_rotate_axis(&camera->matrix, camera->rotation.z,
                           &(s_gl_point_3d) { 0.0, 0.0, 1.0 });
  glUseProgram(camera->gl_shader_program);
  glUniformMatrix4dv(camera->gl_matrix_loc, 1, GL_FALSE,
                     &camera->matrix.xx);
}

s_gl_camera * gl_camera_set_aspect_ratio (s_gl_camera *camera, uw w,
                                          uw h)
{
  assert(camera);
  camera->aspect_ratio = (f64) (w ? w : 1) / (h ? h : 1);
  return camera;
}
