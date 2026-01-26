/* kc3
 * Copyright from 2022 to 2026 kmx.io <contact@kmx.io>
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
#include <libkc3/kc3.h>
#include "mandelbrot.h"

GLuint g_mandelbrotComputeProgram = 0;
GLuint g_mandelbrotComputeShader = 0;

bool mandelbrot_load (s_sequence *seq, s_window_sdl2 *window)
{
  uw i;
  uw star_count;
  s_map *map;
  (void) window;
  if (! tag_map(&seq->tag, 5))
    return false;
  map = &seq->tag.data.map;
  tag_init_sym_1(map->key + 0, "h");
  tag_init_uw( map->value + 0, 0);
  tag_init_sym_1(map->key + 1, "w");
  tag_init_uw( map->value + 1, 0);
  tag_init_sym_1(map->key + 2, "x");
  tag_init_f64(map->value + 2, 0.0);
  tag_init_sym_1(map->key + 3, "y");
  tag_init_f64(map->value + 3, 0.0);
  tag_init_sym_1(map->key + 4, "zoom");
  tag_init_f64(map->value + 4, 1.0);
  return true;
}

bool mandelbrot_render (s_sequence *seq, s_window_sdl2 *window,
                        void *context)
{
  uw    *h;
  s_map *map;
  uw    *w;
  f64   *x;
  f64   *y;
  f64   *zoom;
  assert(seq);
  assert(window);
  (void) context;
  if (seq->tag.type != TAG_MAP) {
    err_puts("mandelbrot_render: sequence tag is not a map");
    assert(! "mandelbrot_render: sequence tag is not a map");
    return false;
  }
  map = &seq->tag.data.map;
  if (map->count != 5 ||
      map->value[0].type != TAG_UW ||
      map->value[1].type != TAG_UW ||
      map->value[2].type != TAG_F64 ||
      map->value[3].type != TAG_F64 ||
      map->value[4].type != TAG_F64) {
    err_puts("mandelbrot_render: invalid map");
    assert(! "mandelbrot_render: invalid map");
    return false;
  }
  h    = &map->value[0].data.uw;
  w    = &map->value[1].data.uw;
  x    = &map->value[2].data.f64;
  y    = &map->value[3].data.f64;
  zoom = &map->value[4].data.f64;
  if (*w != window->w || *h != window->h) {
    *w = window->w;
    *h = window->h;
    mandelbrot_texture_update(*w, *h);
  }
  // fullscreen texture
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glColor4f(1, 1, 1, 1);
  glDisable(GL_BLEND);
  glDisable(GL_DEPTH_TEST);
  glDisable(GL_CULL_FACE);
  glEnable(GL_TEXTURE_2D);
  glBegin(GL_QUADS); {
    glTexCoord2f(0, 1);
    glVertex2f(-1, 1);
    glTexCoord2f(0, 0);
    glVertex2f(-1, -1);
    glTexCoord2f(1, 0);
    glVertex2f(1, -1);
    glTexCoord2f(1, 1);
    glVertex2f(1, 1);
  } glEnd();
  return true;
}

void mandelbrot_shader_init (void)
{
  const char* src =
    "#version 430 core\n"
    "layout (local_size_x = 16, local_size_y = 16) in;\n"
    "layout (rgba32f, binding = 0) uniform image2D mandelbrotImage;\n"
    "uniform int maxIterations;\n"
    "uniform double zoom;\n"
    "uniform dvec2 offset;\n"
    "void main() {\n"
    "   ivec2 texelCoord = ivec2(gl_GlobalInvocationID.xy);\n"
    "   dvec2 c = (dvec2(texelCoord) /\n"
    "              double(imageSize(mandelbrotImage).y) - 0.5) *\n"
    "             zoom + offset;\n"
    "   dvec2 z = c;\n"
    "   int iterations = 0;\n"
    "   for (int i = 0; i < maxIterations; ++i) {\n"
    "       if (dot(z, z) > 4.0) {\n"
    "           break;\n"
    "       }\n"
    "       z = dvec2(z.x * z.x - z.y * z.y, 2.0 * z.x * z.y) + c;\n"
    "       iterations++;\n"
    "   }\n"
    "   vec4 color = vec4(float(iterations) / float(maxIterations));\n"
    "   imageStore(mandelbrotImage, texelCoord, color);\n"
    "}\n";
  g_mandelbrot_shader = glCreateShader(GL_COMPUTE_SHADER);
  glShaderSource(g_mandelbrot_shader, 1, src, NULL);
  glCompileShader(g_mandelbrot_shader);
  g_mandelbrotShaderProgram = glCreateProgram();
  glAttachShader(mandelbrotShaderProgram, g_mandelbrot_shader);
  glLinkProgram(mandelbrotShaderProgram);
  glUseProgram(mandelbrotShaderProgram);
}

void mandelbrot_texture_delete (void)
{
  glDeleteTextures(1, &g_mandelbrot_texture);
}

void mandelbrot_texture_init (void)
{
  glGenTextures(1, &g_mandelbrot_texture);
}

void mandelbrot_update_texture () {
  glBindTexture(GL_TEXTURE_2D, mandelbrotImage);
  glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA32F, WIDTH, HEIGHT);
  glBindImageTexture(0, mandelbrotImage, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
}
