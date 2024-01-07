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

static const s8 * g_gl_ortho_vertex_shader_src = "#version 330 core\n"
  "layout (location = 0) in vec3 aPos;\n"
  "layout (location = 1) in vec3 aNorm;\n"
  "layout (location = 2) in vec2 aTexCoord;\n"
  "out vec3 FragNormal;\n"
  "out vec2 TexCoord;\n"
  "uniform mat4 projection_matrix;\n"
  "uniform mat4 view_matrix;\n"
  "uniform mat4 model_matrix;\n"
  "\n"
  "void main() {\n"
  "  gl_Position = vec4(projection_matrix * view_matrix * \n"
  "                     model_matrix * vec4(aPos, 1.0));\n"
  "  FragNormal = vec3(mat3(transpose(inverse(model_matrix))) *\n"
  "                    aNorm);\n"
  "  TexCoord = vec2(aTexCoord);\n"
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

s_gl_ortho * gl_ortho_init (s_gl_ortho *ortho)
{
  GLint success;
  u32 vertex_shader;
  assert(ortho);
  gl_matrix_4d_init_identity(&ortho->projection_matrix);
  gl_matrix_4d_ortho(&ortho->projection_matrix, -1, 1, -1, 1, -1, 1);
  ortho->position.x = 0.0;
  ortho->position.y = 0.0;
  ortho->position.z = 0.0;
  ortho->rotation.x = 0.0;
  ortho->rotation.y = 0.0;
  ortho->rotation.z = 0.0;
  gl_matrix_4d_init_identity(&ortho->view_matrix);
  gl_matrix_4d_init_identity(&ortho->model_matrix);
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
  assert(glGetError() == GL_NO_ERROR);
  glAttachShader(ortho->gl_shader_program, vertex_shader);
  assert(glGetError() == GL_NO_ERROR);
  glLinkProgram(ortho->gl_shader_program);
  assert(glGetError() == GL_NO_ERROR);
  glDeleteShader(vertex_shader);
  assert(glGetError() == GL_NO_ERROR);
  ortho->gl_projection_matrix_loc =
    glGetUniformLocation(ortho->gl_shader_program, "projection_matrix");
  assert(glGetError() == GL_NO_ERROR);
  ortho->gl_view_matrix_loc =
    glGetUniformLocation(ortho->gl_shader_program, "view_matrix");
  assert(glGetError() == GL_NO_ERROR);
  ortho->gl_model_matrix_loc =
    glGetUniformLocation(ortho->gl_shader_program, "model_matrix");
  assert(glGetError() == GL_NO_ERROR);
  return ortho;
}

s_gl_ortho * gl_ortho_new (void)
{
  s_gl_ortho *ortho;
  ortho = calloc(1, sizeof(s_gl_ortho));
  if (! ortho) {
    err_puts("gl_ortho_new: failed to allocate memory");
    return NULL;
  }
  if (! gl_ortho_init(ortho)) {
    free(ortho);
    return NULL;
  }
  return ortho;
}

void gl_ortho_render (s_gl_ortho *ortho)
{
  GLenum error;
  assert(ortho);
  assert(glGetError() == GL_NO_ERROR);
  glUseProgram(ortho->gl_shader_program);
  assert(glGetError() == GL_NO_ERROR);
  glUniformMatrix4fv(ortho->gl_projection_matrix_loc, 1, GL_FALSE,
                     &ortho->projection_matrix.xx);
  if ((error = glGetError()) != GL_NO_ERROR) {
    err_write_1("gl_ortho_render: glUniformMatrix4fv: ");
    err_puts((const s8 *) glewGetErrorString(error));
    assert(! "gl_ortho_render: glUniformMatrix4fv");
  }
  assert(glGetError() == GL_NO_ERROR);
  glUniformMatrix4fv(ortho->gl_view_matrix_loc, 1, GL_FALSE,
                     &ortho->view_matrix.xx);
  assert(glGetError() == GL_NO_ERROR);
  glUniformMatrix4fv(ortho->gl_model_matrix_loc, 1, GL_FALSE,
                     &ortho->model_matrix.xx);
  assert(glGetError() == GL_NO_ERROR);
}

void gl_ortho_resize (s_gl_ortho *ortho, f32 x1, f32 x2, f32 y1, f32 y2,
                      f32 clip_z_near, f32 clip_z_far)
{
  assert(ortho);
  gl_matrix_4d_init_identity(&ortho->projection_matrix);
  gl_matrix_4d_ortho(&ortho->projection_matrix, x1, x2, y1, y2,
                     clip_z_near, clip_z_far);
}

void gl_ortho_render_end (s_gl_ortho *ortho)
{
  assert(ortho);
  (void) ortho;
  assert(glGetError() == GL_NO_ERROR);
  glUseProgram(0);
  assert(glGetError() == GL_NO_ERROR);
}

void gl_ortho_update_model_matrix (s_gl_ortho *ortho)
{
  assert(ortho);
  assert(glGetError() == GL_NO_ERROR);
  glUniformMatrix4fv(ortho->gl_model_matrix_loc, 1, GL_FALSE,
                     &ortho->model_matrix.xx);
  assert(glGetError() == GL_NO_ERROR);
}

void gl_ortho_update_view_matrix (s_gl_ortho *ortho)
{
  assert(ortho);
  assert(glGetError() == GL_NO_ERROR);
  gl_matrix_4d_init_identity(&ortho->view_matrix);
  gl_matrix_4d_translate(&ortho->view_matrix, ortho->position.x,
                         ortho->position.y, ortho->position.z);
  gl_matrix_4d_rotate_axis(&ortho->view_matrix, ortho->rotation.x,
                           &(s_gl_point_3d) { 1.0, 0.0, 0.0 });
  gl_matrix_4d_rotate_axis(&ortho->view_matrix, ortho->rotation.y,
                           &(s_gl_point_3d) { 0.0, 1.0, 0.0 });
  gl_matrix_4d_rotate_axis(&ortho->view_matrix, ortho->rotation.z,
                           &(s_gl_point_3d) { 0.0, 0.0, 1.0 });
  glUniformMatrix4fv(ortho->gl_view_matrix_loc, 1, GL_FALSE,
                     &ortho->view_matrix.xx);
  assert(glGetError() == GL_NO_ERROR);
}
