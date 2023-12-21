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
#include "gl_ortho.h"
#include "gl_matrix_4d.h"

static const s8 * g_gl_ortho_vertex_shader_src = "#version 460 core\n"
"layout (location = 0) in dvec3 aPos;\n"
"uniform dmat4 matrix;\n"
"\n"
"void main() {\n"
"  gl_Position = vec4(matrix * dvec4(aPos, 1.0));\n"
"}\n";

void gl_ortho_clean (s_gl_ortho *ortho)
{
  assert(ortho);
  glDeleteProgram(ortho->gl_shader_program);
}

void gl_ortho_delete (s_gl_ortho *ortho)
{
  gl_ortho_clean(ortho);
  free(ortho);
}

s_gl_ortho * gl_ortho_init (s_gl_ortho *ortho, uw w, uw h)
{
  GLint success;
  u32 vertex_shader;
  assert(ortho);
  ortho->clip_z_far = 1.0;
  ortho->clip_z_near = -1.0;
  ortho->fov_y = 90.0;
  ortho->position.x = 0.0;
  ortho->position.y = 0.0;
  ortho->position.z = 0.0;
  ortho->rotation = 0.0;
  vertex_shader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertex_shader, 1, &g_gl_ortho_vertex_shader_src,
                 NULL);
  glCompileShader(vertex_shader);
  glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
  if (! success) {
    s8 info_log[512];
    glGetShaderInfoLog(vertex_shader, sizeof(info_log), NULL, info_log);
    err_write_1("gl_ortho_init: shader compilation failed: ");
    err_puts(info_log);
  }
  ortho->gl_shader_program = glCreateProgram();
  glAttachShader(ortho->gl_shader_program, vertex_shader);
  glLinkProgram(ortho->gl_shader_program);
  glDeleteShader(vertex_shader);
  ortho->gl_matrix_loc =
    glGetUniformLocation(ortho->gl_shader_program, "matrix");
  return ortho;
}

s_gl_ortho * gl_ortho_new (uw w, uw h)
{
  s_gl_ortho *ortho;
  ortho = calloc(1, sizeof(s_gl_ortho));
  if (! ortho) {
    err_puts("gl_ortho_new: failed to allocate memory");
    return NULL;
  }
  if (! gl_ortho_init(ortho, w, h)) {
    free(ortho);
    return NULL;
  }
  return ortho;
}

void gl_ortho_render (s_gl_ortho *ortho)
{
  assert(ortho);
  gl_matrix_4d_init_identity(&ortho->matrix);
  gl_matrix_4d_ortho(&ortho->matrix, ortho->x1, ortho->x2,
                     ortho->y1, ortho->y2,
                     ortho->clip_z_near, ortho->clip_z_far);
  gl_matrix_4d_translate(&ortho->matrix, ortho->position.x,
                         ortho->position.y, ortho->position.z);
  gl_matrix_4d_rotate_axis(&ortho->matrix, ortho->rotation.x,
                           &(s_gl_point_3d) { 1.0, 0.0, 0.0 });
  gl_matrix_4d_rotate_axis(&ortho->matrix, ortho->rotation.y,
                           &(s_gl_point_3d) { 0.0, 1.0, 0.0 });
  gl_matrix_4d_rotate_axis(&ortho->matrix, ortho->rotation.z,
                           &(s_gl_point_3d) { 0.0, 0.0, 1.0 });
  glUseProgram(ortho->gl_shader_program);
  glUniformMatrix4dv(ortho->gl_matrix_loc, 1, GL_FALSE,
                     &ortho->matrix.xx);
}

void gl_ortho_render_end (s_gl_ortho *ortho)
{
  assert(ortho);
  (void) ortho;
  glUseProgram(0);
}

s_gl_ortho * gl_ortho_set_aspect_ratio (s_gl_ortho *ortho, uw w,
                                          uw h)
{
  assert(ortho);
  ortho->aspect_ratio = (f64) (w ? w : 1) / (h ? h : 1);
  return ortho;
}
