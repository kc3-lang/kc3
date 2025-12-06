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
#include "../libkc3/kc3.h"
#include "gles2.h"
#include "gl_deprecated.h"
#include "mat4.h"
#include "gl_ortho.h"
#include "gl_square.h"

static const char * g_gl_ortho_vertex_shader_src =
  "#version 300 es\n"
  "precision highp float;\n"
  "in vec3 iPos;\n"
  "in vec3 iNormal;\n"
  "in vec2 iTexCoord;\n"
  "out vec3 ioFragNormal;\n"
  "out vec2 ioTexCoord;\n"
  "uniform mat4 uProjectionMatrix;\n"
  "uniform mat4 uViewMatrix;\n"
  "uniform mat4 uModelMatrix;\n"
  "void main() {\n"
  "  gl_Position = vec4(uProjectionMatrix * uViewMatrix * \n"
  "                     uModelMatrix * vec4(iPos, 1.0));\n"
  "  ioTexCoord = iTexCoord;\n"
  "  ioFragNormal = vec3(mat3(uModelMatrix) * iNormal);\n"
  "}\n";

static const char * g_gl_ortho_fragment_shader_src =
  "#version 300 es\n"
  "precision highp float;\n"
  "in vec3 ioFragNormal;\n"
  "in vec2 ioTexCoord;\n"
  "out vec4 oFragColor;\n"
  "uniform vec4 uColor;\n"
  "uniform bool uEnableTex2D;\n"
  "uniform sampler2D uTex2D;\n"
  "void main() {\n"
  "  vec4 texColor = texture(uTex2D, ioTexCoord);\n"
  "  if (uEnableTex2D) {\n"
  "    oFragColor = vec4(texColor[0] * uColor[0],\n"
  "                      texColor[1] * uColor[1],\n"
  "                      texColor[2] * uColor[2],\n"
  "                      texColor[3] * uColor[3]);\n"
  "  }\n"
  "  else\n"
  "    oFragColor = uColor;\n"
  "}\n";

void gl_ortho_bind_texture (s_gl_ortho *ortho, GLuint texture)
{
  assert(ortho);
  assert(glGetError() == GL_NO_ERROR);
  glUseProgram(ortho->gl_shader_program);
  assert(glGetError() == GL_NO_ERROR);
  if (! texture) {
    glActiveTexture(GL_TEXTURE0);
    assert(glGetError() == GL_NO_ERROR);
    glBindTexture(GL_TEXTURE_2D, ortho->gl_dummy_texture);
    assert(glGetError() == GL_NO_ERROR);
    glUniform1i(ortho->gl_enable_tex2d_loc, 0);
    assert(glGetError() == GL_NO_ERROR);
    glUniform1i(ortho->gl_tex2d_loc, 0);
    assert(glGetError() == GL_NO_ERROR);
    return;
  }
  glActiveTexture(GL_TEXTURE0);
  assert(glGetError() == GL_NO_ERROR);
  glBindTexture(GL_TEXTURE_2D, texture);
  assert(glGetError() == GL_NO_ERROR);
  glUniform1i(ortho->gl_enable_tex2d_loc, 1);
  assert(glGetError() == GL_NO_ERROR);
  glUniform1i(ortho->gl_tex2d_loc, 0);
  assert(glGetError() == GL_NO_ERROR);
}

void gl_ortho_clean (s_gl_ortho *ortho)
{
  assert(ortho);
  assert(glGetError() == GL_NO_ERROR);
  glDeleteTextures(1, &ortho->gl_dummy_texture);
  assert(glGetError() == GL_NO_ERROR);
  glDeleteProgram(ortho->gl_shader_program);
  assert(glGetError() == GL_NO_ERROR);
  gl_square_clean(&ortho->square);
  assert(glGetError() == GL_NO_ERROR);
}

void gl_ortho_color (s_gl_ortho *ortho, f32 r, f32 g, f32 b, f32 a)
{
  s_rgba color = {r, g, b, a};
  assert(ortho);
  assert(glGetError() == GL_NO_ERROR);
  glUseProgram(ortho->gl_shader_program);
  assert(glGetError() == GL_NO_ERROR);
  glUniform4fv(ortho->gl_color_loc, 1, &color.r);
  assert(glGetError() == GL_NO_ERROR);
}

void gl_ortho_delete (s_gl_ortho *ortho)
{
  gl_ortho_clean(ortho);
  free(ortho);
}

s_gl_ortho * gl_ortho_init (s_gl_ortho *ortho)
{
  GLuint fragment_shader;
  GLint success;
  GLuint vertex_shader;
  GLubyte white_pixel[4] = {255, 255, 255, 255};
  assert(ortho);
  if (! gl_square_init(&ortho->square, 2, 2))
    return NULL;
  mat4_init_identity(&ortho->projection_matrix);
  mat4_ortho(&ortho->projection_matrix, -1, 1, -1, 1, 0, 1);
  ortho->position.x = 0.0f;
  ortho->position.y = 0.0f;
  ortho->position.z = 0.0f;
  ortho->rotation.x = 0.0f;
  ortho->rotation.y = 0.0f;
  ortho->rotation.z = 0.0f;
  ortho->scale.x = 1.0f;
  ortho->scale.y = 1.0f;
  ortho->scale.z = 1.0f;
  mat4_init_identity(&ortho->view_matrix);
  mat4_init_identity(&ortho->model_matrix);
  glGenTextures(1, &ortho->gl_dummy_texture);
  glBindTexture(GL_TEXTURE_2D, ortho->gl_dummy_texture);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA,
               GL_UNSIGNED_BYTE, white_pixel);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glBindTexture(GL_TEXTURE_2D, 0);
  vertex_shader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertex_shader, 1, &g_gl_ortho_vertex_shader_src,
                 NULL);
  glCompileShader(vertex_shader);
  glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
  if (! success) {
    char info_log[512];
    glGetShaderInfoLog(vertex_shader, sizeof(info_log), NULL, info_log);
    err_write_1("gl_ortho_init: shader compilation failed: ");
    err_puts(info_log);
  }
  fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragment_shader, 1, &g_gl_ortho_fragment_shader_src,
                 NULL);
  glCompileShader(fragment_shader);
  glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
  if (! success) {
    char info_log[512];
    glGetShaderInfoLog(fragment_shader, sizeof(info_log), NULL,
                       info_log);
    err_write_1("gl_ortho_init: shader compilation failed: ");
    err_puts(info_log);
  }
  ortho->gl_shader_program = glCreateProgram();
  assert(glGetError() == GL_NO_ERROR);
  glAttachShader(ortho->gl_shader_program, vertex_shader);
  assert(glGetError() == GL_NO_ERROR);
  glAttachShader(ortho->gl_shader_program, fragment_shader);
  assert(glGetError() == GL_NO_ERROR);
  glBindAttribLocation(ortho->gl_shader_program, 0, "iPos");
  glBindAttribLocation(ortho->gl_shader_program, 1, "iNormal");
  glBindAttribLocation(ortho->gl_shader_program, 2, "iTexCoord");
  glLinkProgram(ortho->gl_shader_program);
  glGetProgramiv(ortho->gl_shader_program, GL_LINK_STATUS, &success);
  if (! success) {
    char info_log[512];
    glGetProgramInfoLog(ortho->gl_shader_program, sizeof(info_log),
                        NULL, info_log);
    err_write_1("gl_ortho_init: program linking failed: ");
    err_puts(info_log);
  }
  assert(glGetError() == GL_NO_ERROR);
  glDeleteShader(vertex_shader);
  assert(glGetError() == GL_NO_ERROR);
  glDeleteShader(fragment_shader);
  assert(glGetError() == GL_NO_ERROR);
  glUseProgram(ortho->gl_shader_program);
  assert(glGetError() == GL_NO_ERROR);
  ortho->gl_projection_matrix_loc =
    glGetUniformLocation(ortho->gl_shader_program, "uProjectionMatrix");
  assert(glGetError() == GL_NO_ERROR);
  ortho->gl_view_matrix_loc =
    glGetUniformLocation(ortho->gl_shader_program, "uViewMatrix");
  assert(glGetError() == GL_NO_ERROR);
  ortho->gl_model_matrix_loc =
    glGetUniformLocation(ortho->gl_shader_program, "uModelMatrix");
  assert(glGetError() == GL_NO_ERROR);
  ortho->gl_enable_tex2d_loc =
    glGetUniformLocation(ortho->gl_shader_program, "uEnableTex2D");
  assert(glGetError() == GL_NO_ERROR);
  ortho->gl_tex2d_loc =
    glGetUniformLocation(ortho->gl_shader_program, "uTex2D");
  assert(glGetError() == GL_NO_ERROR);
  ortho->gl_color_loc =
    glGetUniformLocation(ortho->gl_shader_program, "uColor");
  assert(glGetError() == GL_NO_ERROR);
  glUseProgram(0);
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

void gl_ortho_rect (s_gl_ortho *ortho, f32 x, f32 y, f32 w, f32 h)
{
  s_mat4 matrix;
  assert(ortho);
  assert(glGetError() == GL_NO_ERROR);
  matrix = ortho->model_matrix;
  mat4_translate(&ortho->model_matrix, x, y, 0.0f);
  mat4_scale(&ortho->model_matrix, w, h, 1.0f);
  assert(glGetError() == GL_NO_ERROR);
  glUseProgram(ortho->gl_shader_program);
  gl_ortho_update_model_matrix(ortho);
  assert(glGetError() == GL_NO_ERROR);
  gl_square_render(&ortho->square);
  assert(glGetError() == GL_NO_ERROR);
  ortho->model_matrix = matrix;
  gl_ortho_update_model_matrix(ortho);
}

void gl_ortho_render (s_gl_ortho *ortho)
{
  GLenum error;
  assert(ortho);
  assert(glGetError() == GL_NO_ERROR);
  glUseProgram(ortho->gl_shader_program);
  assert(glGetError() == GL_NO_ERROR);
  if (ortho->gl_projection_matrix_loc < 0) {
    err_write_1("gl_ortho_render: invalid projection_matrix_loc: ");
    err_inspect_s32(ortho->gl_projection_matrix_loc);
    err_write_1("\n");
  }
  glUniformMatrix4fv(ortho->gl_projection_matrix_loc, 1, GL_FALSE,
                     &ortho->projection_matrix.xx);
  if ((error = glGetError()) != GL_NO_ERROR) {
    err_write_1("gl_ortho_render: glUniformMatrix4fv(projection): ");
    err_puts(gl_error_string(error));
    assert(! "gl_ortho_render: glUniformMatrix4fv(projection)");
  }
  assert(glGetError() == GL_NO_ERROR);
  glUniformMatrix4fv(ortho->gl_view_matrix_loc, 1, GL_FALSE,
                     &ortho->view_matrix.xx);
  assert(glGetError() == GL_NO_ERROR);
  glUniformMatrix4fv(ortho->gl_model_matrix_loc, 1, GL_FALSE,
                     &ortho->model_matrix.xx);
  assert(glGetError() == GL_NO_ERROR);
  glUniform1i(ortho->gl_enable_tex2d_loc, 0);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, ortho->gl_dummy_texture);
  assert(glGetError() == GL_NO_ERROR);
  glUniform1i(ortho->gl_tex2d_loc, 0);
  assert(glGetError() == GL_NO_ERROR);
  glUniform4f(ortho->gl_color_loc, 1.0f, 1.0f, 1.0f, 1.0f);
  assert(glGetError() == GL_NO_ERROR);
  // glDepthRangef(ortho->clip_z_near, ortho->clip_z_far);
  glDepthRangef(0.0f, 1.0f);
  assert(glGetError() == GL_NO_ERROR);
}

void gl_ortho_render_end (s_gl_ortho *ortho)
{
  assert(ortho);
  (void) ortho;
  assert(glGetError() == GL_NO_ERROR);
  glUseProgram(0);
  assert(glGetError() == GL_NO_ERROR);
}

void gl_ortho_resize (s_gl_ortho *ortho, f32 x1, f32 x2, f32 y1, f32 y2,
                      f32 clip_z_near, f32 clip_z_far)
{
  assert(ortho);
  mat4_init_identity(&ortho->projection_matrix);
  mat4_ortho(&ortho->projection_matrix, x1, x2, y1, y2,
             clip_z_near, clip_z_far);
}

void gl_ortho_text_render (s_gl_ortho *ortho, const s_gl_text *text)
{
  assert(ortho);
  assert(text);
  gl_ortho_bind_texture(ortho, text->texture);
  gl_ortho_rect(ortho, 0, 0, text->pt_w, text->pt_h);
  gl_ortho_bind_texture(ortho, 0);
}

void gl_ortho_text_render_outline (s_gl_ortho *ortho, s_gl_text *text,
                                   f64 x, f64 y,
                                   const s_rgb *outline_color,
                                   const s_rgb *color)
{
  s_mat4 matrix;
  assert(glGetError() == GL_NO_ERROR);
  glEnable(GL_BLEND);
  glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA,
                      GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
  assert(glGetError() == GL_NO_ERROR);
  gl_ortho_bind_texture(ortho, text->texture);
  assert(glGetError() == GL_NO_ERROR);
  gl_ortho_color(ortho, outline_color->r, outline_color->g,
                 outline_color->b, 1.0f);
  assert(glGetError() == GL_NO_ERROR);
  matrix = ortho->model_matrix;
  gl_ortho_rect(ortho, x - 1.0, y - 1.0, text->pt_w, text->pt_h);
  gl_ortho_rect(ortho, x,       y - 1.0, text->pt_w, text->pt_h);
  gl_ortho_rect(ortho, x + 1.0, y - 1.0, text->pt_w, text->pt_h);
  gl_ortho_rect(ortho, x - 1.0, y,       text->pt_w, text->pt_h);
  //gl_ortho_rect(ortho, x,       y,       text->pt_w, text->pt_h);
  gl_ortho_rect(ortho, x + 1.0, y,       text->pt_w, text->pt_h);
  gl_ortho_rect(ortho, x - 1.0, y + 1.0, text->pt_w, text->pt_h);
  gl_ortho_rect(ortho, x,       y + 1.0, text->pt_w, text->pt_h);
  gl_ortho_rect(ortho, x + 1.0, y + 1.0, text->pt_w, text->pt_h);
  gl_ortho_color(ortho, color->r, color->g, color->b, 1.0f);
  assert(glGetError() == GL_NO_ERROR);
  gl_ortho_rect(ortho, x,       y,       text->pt_w, text->pt_h);
  assert(glGetError() == GL_NO_ERROR);
  ortho->model_matrix = matrix;
  gl_ortho_update_model_matrix(ortho);
  assert(glGetError() == GL_NO_ERROR);
}

void gl_ortho_update_model_matrix (s_gl_ortho *ortho)
{
  assert(ortho);
  assert(glGetError() == GL_NO_ERROR);
  glUseProgram(ortho->gl_shader_program);
  assert(glGetError() == GL_NO_ERROR);
  glUniformMatrix4fv(ortho->gl_model_matrix_loc, 1, GL_FALSE,
                     &ortho->model_matrix.xx);
  assert(glGetError() == GL_NO_ERROR);
  /*
  err_puts("gl_ortho_update_model_matrix projection matrix");
  mat4_buf_inspect(&g_c3_env.err, &ortho->projection_matrix);
  err_puts("view matrix");
  mat4_buf_inspect(&g_c3_env.err, &ortho->view_matrix);
  err_puts("model matrix");
  mat4_buf_inspect(&g_c3_env.err, &ortho->model_matrix);
  buf_flush(&g_c3_env.err);
  */
}

void gl_ortho_update_view_matrix (s_gl_ortho *ortho)
{
  assert(ortho);
  assert(glGetError() == GL_NO_ERROR);
  mat4_init_identity(&ortho->view_matrix);
  mat4_translate(&ortho->view_matrix, ortho->position.x,
                 ortho->position.y, ortho->position.z);
  mat4_rotate_axis(&ortho->view_matrix, ortho->rotation.x,
                   &(s_vec3) { 1.0f, 0.0f, 0.0f });
  mat4_rotate_axis(&ortho->view_matrix, ortho->rotation.y,
                   &(s_vec3) { 0.0f, 1.0f, 0.0f });
  mat4_rotate_axis(&ortho->view_matrix, ortho->rotation.z,
                   &(s_vec3) { 0.0f, 0.0f, 1.0f });
  mat4_scale(&ortho->view_matrix, ortho->scale.x,
             ortho->scale.y, ortho->scale.z);
  glUseProgram(ortho->gl_shader_program);
  assert(glGetError() == GL_NO_ERROR);
  glUniformMatrix4fv(ortho->gl_view_matrix_loc, 1, GL_FALSE,
                     &ortho->view_matrix.xx);
  assert(glGetError() == GL_NO_ERROR);
}

void gl_ortho_vtext_render (s_gl_ortho *ortho, const s_gl_text *text)
{
  assert(ortho);
  assert(text);
  gl_ortho_bind_texture(ortho, text->texture);
  gl_ortho_rect(ortho, 0, -text->pt_h, text->pt_w, text->pt_h);
  gl_ortho_bind_texture(ortho, 0);
}
