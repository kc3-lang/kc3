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

static const char * g_gl_camera_vertex_shader_src =
  "#version 330 core\n"
  "layout (location = 0) in vec3 iPos;\n"
  "layout (location = 1) in vec3 iNormal;\n"
  "layout (location = 2) in vec2 iTexCoord;\n"
  "out vec3 ioFragNormal;\n"
  "out vec2 ioTexCoord;\n"
  "uniform mat4 uProjectionMatrix;\n"
  "uniform mat4 uViewMatrix;\n"
  "uniform mat4 uModelMatrix;\n"
  "void main() {\n"
  "  gl_Position = vec4(uProjectionMatrix * uViewMatrix *\n"
  "                     uModelMatrix * vec4(iPos, 1.0));\n"
  "  ioFragNormal = vec3(mat3(transpose(inverse(uModelMatrix))) *\n"
  "                      iNormal);\n"
  "  ioTexCoord = iTexCoord;\n"
  "}\n";

static const char * g_gl_camera_fragment_shader_src =
  "#version 330 core\n"
  "in vec3 ioFragNormal;\n"
  "in vec2 ioTexCoord;\n"
  "out vec4 oFragColor;\n"
  "uniform bool uEnableTex2D;\n"
  "uniform sampler2D uTex2D;\n"
  "uniform vec4 uColor;\n"
  "void main() {\n"
  "  vec4 texColor = texture(uTex2D, ioTexCoord);\n"
  "  if (uEnableTex2D) {\n"
  "    oFragColor = texColor * uColor;\n"
  "  }\n"
  "  else\n"
  "    oFragColor = uColor;\n"
  "}\n";

void gl_camera_bind_texture (s_gl_camera *camera, GLuint texture)
{
  assert(camera);
  assert(glGetError() == GL_NO_ERROR);
  if (! texture) {
    glActiveTexture(GL_TEXTURE0);
    assert(glGetError() == GL_NO_ERROR);
    glBindTexture(GL_TEXTURE_2D, 0);
    assert(glGetError() == GL_NO_ERROR);
    glUniform1i(camera->gl_enable_tex2d_loc, 0);
    assert(glGetError() == GL_NO_ERROR);
    glUniform1i(camera->gl_tex2d_loc, 0);
    assert(glGetError() == GL_NO_ERROR);
    return;
  }
  glActiveTexture(GL_TEXTURE0);
  assert(glGetError() == GL_NO_ERROR);
  glBindTexture(GL_TEXTURE_2D, texture);
  assert(glGetError() == GL_NO_ERROR);
  glUniform1i(camera->gl_enable_tex2d_loc, 1);
  assert(glGetError() == GL_NO_ERROR);
  glUniform1i(camera->gl_tex2d_loc, 0);
  assert(glGetError() == GL_NO_ERROR);
}

void gl_camera_clean (s_gl_camera *camera)
{
  assert(camera);
  glDeleteProgram(camera->gl_shader_program);
}

void gl_camera_color (s_gl_camera *camera, f32 r, f32 g, f32 b, f32 a)
{
  s_rgba color = {r, g, b, a};
  assert(camera);
  assert(glGetError() == GL_NO_ERROR);
  glUniform4fv(camera->gl_color_loc, 1, &color.r);    
  assert(glGetError() == GL_NO_ERROR);
}

void gl_camera_delete (s_gl_camera *camera)
{
  gl_camera_clean(camera);
  free(camera);
}

s_gl_camera * gl_camera_init (s_gl_camera *camera, uw w, uw h)
{
  GLuint fragment_shader;
  GLint success;
  GLuint vertex_shader;
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
  fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragment_shader, 1, &g_gl_camera_fragment_shader_src,
                 NULL);
  glCompileShader(fragment_shader);
  glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
  if (! success) {
    char info_log[512];
    glGetShaderInfoLog(fragment_shader, sizeof(info_log), NULL, info_log);
    err_write_1("gl_camera_init: shader compilation failed: ");
    err_puts(info_log);
  }
  camera->gl_shader_program = glCreateProgram();
  glAttachShader(camera->gl_shader_program, vertex_shader);
  glAttachShader(camera->gl_shader_program, fragment_shader);
  glLinkProgram(camera->gl_shader_program);
  glDeleteShader(vertex_shader);
  glDeleteShader(fragment_shader);
  camera->gl_projection_matrix_loc =
    glGetUniformLocation(camera->gl_shader_program,
                         "uProjectionMatrix");
  camera->gl_view_matrix_loc =
    glGetUniformLocation(camera->gl_shader_program,
                         "uViewMatrix");
  camera->gl_model_matrix_loc =
    glGetUniformLocation(camera->gl_shader_program,
                         "uModelMatrix");
  camera->gl_enable_tex2d_loc =
    glGetUniformLocation(camera->gl_shader_program,
                         "uEnableTex2D");
  camera->gl_tex2d_loc =
    glGetUniformLocation(camera->gl_shader_program,
                         "uTex2D");
  camera->gl_color_loc =
    glGetUniformLocation(camera->gl_shader_program,
                         "uColor");
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
  const s_rgba color = {1, 1, 1, 1};
  assert(camera);
  assert(glGetError() == GL_NO_ERROR);
  gl_matrix_4f_init_identity(&camera->projection_matrix);
  gl_matrix_4f_perspective(&camera->projection_matrix, camera->fov_y,
                           camera->aspect_ratio, camera->clip_z_near,
                           camera->clip_z_far);
  gl_matrix_4f_init_identity(&camera->view_matrix);
  gl_matrix_4f_translate(&camera->view_matrix, camera->position.x,
                         camera->position.y, camera->position.z);
  gl_matrix_4f_rotate_axis(&camera->view_matrix, camera->rotation.x,
                           &(s_gl_point_3f) { 1.0f, 0.0f, 0.0f });
  gl_matrix_4f_rotate_axis(&camera->view_matrix, camera->rotation.y,
                           &(s_gl_point_3f) { 0.0f, 1.0f, 0.0f });
  gl_matrix_4f_rotate_axis(&camera->view_matrix, camera->rotation.z,
                           &(s_gl_point_3f) { 0.0f, 0.0f, 1.0f });
  gl_matrix_4f_init_identity(&camera->model_matrix);
  glUseProgram(camera->gl_shader_program);
  assert(glGetError() == GL_NO_ERROR);
  glUniformMatrix4fv(camera->gl_projection_matrix_loc, 1, GL_FALSE,
                     &camera->projection_matrix.xx);
  assert(glGetError() == GL_NO_ERROR);
  glUniformMatrix4fv(camera->gl_view_matrix_loc, 1, GL_FALSE,
                     &camera->view_matrix.xx);
  assert(glGetError() == GL_NO_ERROR);
  glUniformMatrix4fv(camera->gl_model_matrix_loc, 1, GL_FALSE,
                     &camera->model_matrix.xx);
  assert(glGetError() == GL_NO_ERROR);
  glUniform1i(camera->gl_enable_tex2d_loc, 0);
  assert(glGetError() == GL_NO_ERROR);
  glUniform1i(camera->gl_tex2d_loc, 0);
  assert(glGetError() == GL_NO_ERROR);
  glUniform4fv(camera->gl_color_loc, 1, &color.r);
  assert(glGetError() == GL_NO_ERROR);
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
