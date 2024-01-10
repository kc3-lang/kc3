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
#include "gl_matrix_4f.h"

static const char * g_gl_camera_vertex_shader_src = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"uniform mat4 matrix;\n"
"\n"
"void main() {\n"
"  gl_Position = vec4(matrix * vec4(aPos, 1.0));\n"
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
  GLint success;
  u32 vertex_shader;
  assert(camera);
  gl_camera_set_aspect_ratio(camera, w, h);
  camera->clip_z_far = 1000.0f;
  camera->clip_z_near = 0.1f;
  camera->fov_y = 90.0f;
  camera->position.x = 0.0f;
  camera->position.y = 0.0f;
  camera->position.z = -10.0f;
  camera->rotation.x = 90.0f;
  camera->rotation.y = 0.0f;
  camera->rotation.z = 0.0f;
  vertex_shader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertex_shader, 1, &g_gl_camera_vertex_shader_src,
                 NULL);
  glCompileShader(vertex_shader);
  glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
  if (! success) {
    char info_log[512];
    glGetShaderInfoLog(vertex_shader, sizeof(info_log), NULL, info_log);
    err_write_1("gl_camera_init: shader compilation failed: ");
    err_puts(info_log);
  }
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
  gl_matrix_4f_init_identity(&camera->matrix);
  gl_matrix_4f_perspective(&camera->matrix, camera->fov_y,
                           camera->aspect_ratio, camera->clip_z_near,
                           camera->clip_z_far);
  gl_matrix_4f_translate(&camera->matrix, camera->position.x,
                         camera->position.y, camera->position.z);
  gl_matrix_4f_rotate_axis(&camera->matrix, camera->rotation.x,
                           &(s_gl_point_3f) { 1.0f, 0.0f, 0.0f });
  gl_matrix_4f_rotate_axis(&camera->matrix, camera->rotation.y,
                           &(s_gl_point_3f) { 0.0f, 1.0f, 0.0f });
  gl_matrix_4f_rotate_axis(&camera->matrix, camera->rotation.z,
                           &(s_gl_point_3f) { 0.0f, 0.0f, 1.0f });
  glUseProgram(camera->gl_shader_program);
  glUniformMatrix4fv(camera->gl_matrix_loc, 1, GL_FALSE,
                     &camera->matrix.xx);
}

void gl_camera_render_end (s_gl_camera *camera)
{
  assert(camera);
  (void) camera;
  glUseProgram(0);
}

s_gl_camera * gl_camera_set_aspect_ratio (s_gl_camera *camera, uw w,
                                          uw h)
{
  assert(camera);
  camera->aspect_ratio = (f64) (w ? w : 1) / (h ? h : 1);
  return camera;
}
