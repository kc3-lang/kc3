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
#include "../../libkc3/kc3.h"
#include "../../gl/gl.h"
#include "../window.h"
#include "lightspeed.h"

s_gl_lines g_lines_stars = {0};
//s_gl_ortho g_ortho_lightspeed = {0};

static void star_init (s_tag *star)
{
  f64 x;
  f64 y;
  f64_random(&x);
  f64_random(&y);
  if (star->type != TAG_MAP || star->data.map.count != 3) {
    tag_map(star, 3);
    tag_init_psym(star->data.map.key + 0, sym_1("speed"));
    tag_init_psym(star->data.map.key + 1, sym_1("x"));
    tag_init_psym(star->data.map.key + 2, sym_1("y"));
  }
  tag_init_f64(star->data.map.value + 0, 0.0);
  tag_init_f64(star->data.map.value + 1, 2.0 * x - 1.0);
  tag_init_f64(star->data.map.value + 2, 2.0 * y - 1.0);
}

static void star_render (s_tag *star, s_sequence *seq,
                         s_gl_vertex *v)
{
  f64 q;
  f64 *speed;
  f64 *x;
  f64 *y;
  if (star->type != TAG_MAP || star->data.map.count < 3)
    star_init(star);
  speed = &star->data.map.value[0].data.f64;
  x = &star->data.map.value[1].data.f64;
  y = &star->data.map.value[2].data.f64;
  v[0].pos_x = *x;
  v[0].pos_y = *y;
  v[0].pos_z = 0.0;
  q = (1 + *speed / 20);
  v[1].pos_x = *x * q;
  v[1].pos_y = *y * q;
  v[1].pos_z = 0.0;
  q = (1 + *speed / 100);
  *x = *x * q;
  *y = *y * q;
  *speed += seq->dt;
  if ((*x == 0.0 && *y == 0.0) ||
      *x < -1.0 || *x > 1.0 ||
      *y < -1.0 || *y > 1.0)
    star_init(star);
}

u8 lightspeed_load (s_sequence *seq)
{
  uw i;
  uw star_count;
  s_window *window;
  assert(seq);
  window = seq->window;
  assert(window);
  tag_tuple(&seq->tag, LIGHTSPEED_STAR_MAX);
  star_count = window->w * window->h * LIGHTSPEED_STAR_PROBABILITY;
  if (star_count > LIGHTSPEED_STAR_MAX)
    star_count = LIGHTSPEED_STAR_MAX;
  i = 0;
  while (i < star_count) {
    star_init(seq->tag.data.tuple.tag + i);
    i++;
  }
  return true;
}

u8 lightspeed_render (s_sequence *seq)
{
  uw i;
  uw star_count;
  s_gl_vertex *v;
  s_window *window;
  assert(seq);
  window = seq->window;
  assert(window);
  assert(glGetError() == GL_NO_ERROR);
  mat4_init_identity(&g_ortho.model_matrix);
  mat4_scale(&g_ortho.model_matrix, (f32) window->w / 2.0f,
                     (f32) window->h / 2.0f, 1.0f);
  mat4_translate(&g_ortho.model_matrix, 1, 1, 0);
  glUseProgram(g_ortho.gl_shader_program);
  assert(glGetError() == GL_NO_ERROR);
  gl_ortho_update_model_matrix(&g_ortho);
  assert(glGetError() == GL_NO_ERROR);
  star_count = window->w * window->h * LIGHTSPEED_STAR_PROBABILITY;
  if (star_count > LIGHTSPEED_STAR_MAX)
    star_count = LIGHTSPEED_STAR_MAX;
  v = g_lines_stars.vertex.data;
  i = 0;
  while (i < star_count) {
    star_render(seq->tag.data.tuple.tag + i, seq, v);
    v += 2;
    i++;
  }
  assert(glGetError() == GL_NO_ERROR);
  gl_lines_update(&g_lines_stars, star_count);
  assert(glGetError() == GL_NO_ERROR);
  glDisable(GL_DEPTH_TEST);
  assert(glGetError() == GL_NO_ERROR);
  glClearColor(0, 0, 0, 1);
  assert(glGetError() == GL_NO_ERROR);
  glClear(GL_COLOR_BUFFER_BIT);
  assert(glGetError() == GL_NO_ERROR);
  glBlendColor(1, 1, 1, 0.7f);
  assert(glGetError() == GL_NO_ERROR);
  glEnable(GL_BLEND);
  assert(glGetError() == GL_NO_ERROR);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  assert(glGetError() == GL_NO_ERROR);
  /* glEnable(GL_LINE_SMOOTH); */ /* Not available in OpenGL ES */
  assert(glGetError() == GL_NO_ERROR);
  gl_ortho_render(&g_ortho);
  assert(glGetError() == GL_NO_ERROR);
  gl_ortho_color(&g_ortho, 1.0f, 1.0f, 1.0f, 0.7f);
  assert(glGetError() == GL_NO_ERROR);
  gl_lines_render(&g_lines_stars, star_count);
  assert(glGetError() == GL_NO_ERROR);
  gl_ortho_render_end(&g_ortho);
  assert(glGetError() == GL_NO_ERROR);
  return true;
}

u8 lightspeed_unload (s_sequence *seq)
{
  (void) seq;
  return true;
}
