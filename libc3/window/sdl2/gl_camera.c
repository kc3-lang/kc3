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
#include "mat4.h"

static const char * g_gl_camera_vertex_shader_src =
  "#version 330 core\n"
  "layout (location = 0) in vec3 iPos;\n"
  "layout (location = 1) in vec3 iNormal;\n"
  "layout (location = 2) in vec2 iTexCoord;\n"
  "out vec3 ioNormal;\n"
  "out vec3 ioPos;\n"
  "out vec2 ioTexCoord;\n"
  "uniform mat4 uProjectionMatrix;\n"
  "uniform mat4 uViewMatrix;\n"
  "uniform mat4 uModelMatrix;\n"
  "\n"
  "void main() {\n"
  "  gl_Position = vec4(uProjectionMatrix * uViewMatrix *\n"
  "                     uModelMatrix * vec4(iPos, 1.0));\n"
  "  ioNormal = vec3(mat3(transpose(inverse(uModelMatrix))) *\n"
  "                  iNormal);\n"
  "  ioPos = (uViewMatrix * uModelMatrix * vec4(iPos, 1.0)).xyz;\n"
  "  ioTexCoord = iTexCoord;\n"
  "}\n";

static const char * g_gl_camera_fragment_shader_src =
  "#version 330 core\n"
  "const float PI = 3.141592653;\n"
  "in vec3 ioNormal;\n"
  "in vec3 ioPos;\n"
  "in vec2 ioTexCoord;\n"
  "out vec4 oColor;\n"
  "uniform bool uEnableTex2D;\n"
  "uniform sampler2D uTex2D;\n"
  "uniform int uLightCount;\n"
  "uniform vec3 uLightPos[16];       // Light position in cam. coords.\n"
  "uniform vec3 uLightColor[16];\n"
  "uniform struct MaterialInfo {\n"
  "  float Rough;     // Roughness\n"
  "  bool Metal;      // Metallic (true) or dielectric (false)\n"
  "  vec4 Color;      // Diffuse color for dielectrics, f0 for metallic\n"
  "} uMaterial;\n"
  "float ggxDistribution (float nDotH) {\n"
  "  float alpha2 = uMaterial.Rough * uMaterial.Rough *\n"
  "                 uMaterial.Rough * uMaterial.Rough;\n"
  "  float d = (nDotH * nDotH) * (alpha2 - 1) + 1;\n"
  "  return alpha2 / (PI * d * d);\n"
  "}\n"
  "float geomSmith( float dotProd ) {\n"
  "  float k = (uMaterial.Rough + 1.0) * (uMaterial.Rough + 1.0) / 8.0;\n"
  "  float denom = dotProd * (1 - k) + k;\n"
  "  return 1.0 / denom;\n"
  "}\n"
  "vec4 schlickFresnel( float lDotH ) {\n"
  "  vec4 f0 = vec4(0.04);\n"
  "  if( uMaterial.Metal ) {\n"
  "    f0 = uMaterial.Color;\n"
  "  }\n"
  "  return f0 + (1 - f0) * pow(1.0 - lDotH, 5);\n"
  "}\n"
  "vec4 microfacetModel (int lightIdx, vec3 pos, vec3 n, vec4 color) {\n"
  "  vec4 diffuseBrdf = vec4(0.0, 0.0, 0.0, 1.0);  // Metallic\n"
  "  if (! uMaterial.Metal) {\n"
  "    diffuseBrdf = color;\n"
  "  }\n"
  "  vec3 l = vec3(0.0);\n"
  "  vec3 lightI = uLightColor[lightIdx];\n"
  "  l = uLightPos[lightIdx] - pos;\n"
  "  float dist = length(l);\n"
  "  l = normalize(l);\n"
  "  lightI /= (dist * dist);\n"
  "  vec3 v = normalize(-pos);\n"
  "  vec3 h = normalize(v + l);\n"
  "  float nDotH = dot(n, h);\n"
  "  float lDotH = dot(l, h);\n"
  "  float nDotL = max(dot(n, l), 0.0);\n"
  "  float nDotV = dot(n, v);\n"
  "  vec4 specBrdf = 0.25 * ggxDistribution(nDotH) *\n"
  "    schlickFresnel(lDotH) * geomSmith(nDotL) * geomSmith(nDotV);\n"
  "  return (diffuseBrdf + PI * specBrdf) * vec4(lightI, 1.0) * nDotL;\n"
  "}\n"
  "void main() {\n"
  "  vec4 texColor = texture(uTex2D, ioTexCoord);\n"
  "  if (uEnableTex2D) {\n"
  "    oColor = texColor * uMaterial.Color;\n"
  "  }\n"
  "  else\n"
  "    oColor = uMaterial.Color;\n"
  "  vec4 sum = vec4(0);\n"
  "  vec3 n = normalize(ioNormal);\n"
  "  for (int i = 0; i < 3; i++) {\n"
  "    sum += microfacetModel(i, ioPos, n, oColor);\n"
  "  }\n"
  "  sum = pow(sum, vec4(1.0 / 2.2));\n"
  "  oColor += sum;\n"
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

void gl_camera_material (s_gl_camera *camera, const s_gl_material *material)
{
  assert(camera);
  assert(material);
  assert(glGetError() == GL_NO_ERROR);
  glUniform1f(camera->gl_material_rough_loc, material->roughness);
  assert(glGetError() == GL_NO_ERROR);
  glUniform1i(camera->gl_material_metal_loc, material->metal);
  assert(glGetError() == GL_NO_ERROR);
  glUniform4fv(camera->gl_material_color_loc, 1, &material->color.r);
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
  camera->clip_z_far = 10.0f;
  camera->clip_z_near = 0.1f;
  camera->fov_y = 90.0f;
  camera->position.x = 0.0f;
  camera->position.y = 0.0f;
  camera->position.z = -10.0f;
  camera->rotation.x = M_PI / 2.0;
  camera->rotation.y = 0.0f;
  camera->rotation.z = 0.0f;
  camera->light_count = 1;
  camera->light_pos[0] = (s_vec3) {-100, 0, 0};
  camera->light_color[0] = (s_rgb) {1, 1, 1};
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
  camera->gl_light_pos_loc =
    glGetUniformLocation(camera->gl_shader_program,
                         "uLightPos");
  camera->gl_light_color_loc =
    glGetUniformLocation(camera->gl_shader_program,
                         "uLightColor");
  camera->gl_material_rough_loc =
    glGetUniformLocation(camera->gl_shader_program,
                         "uMaterial.Rough");
  camera->gl_material_metal_loc =
    glGetUniformLocation(camera->gl_shader_program,
                         "uMaterial.Metal");
  camera->gl_material_color_loc =
    glGetUniformLocation(camera->gl_shader_program,
                         "uMaterial.Color");
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
  s_mat4 matrix;
  const s_gl_material material = {0.1, false, {1, 1, 1, 1}};
  assert(camera);
  assert(glGetError() == GL_NO_ERROR);
  mat4_init_identity(&camera->projection_matrix);
  mat4_perspective(&camera->projection_matrix, camera->fov_y,
                   camera->aspect_ratio, camera->clip_z_near,
                   camera->clip_z_far);
  mat4_init_identity(&camera->view_matrix);
  mat4_translate(&camera->view_matrix, camera->position.x,
                 camera->position.y, camera->position.z);
  mat4_rotate_axis(&camera->view_matrix, camera->rotation.x,
                   &(s_vec3) { 1.0f, 0.0f, 0.0f });
  mat4_rotate_axis(&camera->view_matrix, camera->rotation.y,
                   &(s_vec3) { 0.0f, 1.0f, 0.0f });
  mat4_rotate_axis(&camera->view_matrix, camera->rotation.z,
                   &(s_vec3) { 0.0f, 0.0f, 1.0f });
  mat4_init_identity(&camera->model_matrix);
  mat4_init_copy(&matrix, &camera->view_matrix);
  mat4_mult_mat4(&camera->projection_matrix, &matrix, &matrix);
  for (int i = 0; i < camera->light_count; i++)
    mat4_mult_vec3(&matrix, camera->light_pos + i,
                   camera->light_pos_cam + i);
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
  glUniform3fv(camera->gl_light_pos_loc, camera->light_count,
               &camera->light_pos_cam[0].x);
  assert(glGetError() == GL_NO_ERROR);
  glUniform3fv(camera->gl_light_color_loc, camera->light_count,
               &camera->light_color[0].r);
  assert(glGetError() == GL_NO_ERROR);
  gl_camera_material(camera, &material);
  assert(glGetError() == GL_NO_ERROR);
  glDisable(GL_CULL_FACE);
  assert(glGetError() == GL_NO_ERROR);
  glDepthRange(camera->clip_z_near, camera->clip_z_far);
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
