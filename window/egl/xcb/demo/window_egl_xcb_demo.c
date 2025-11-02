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
#include <stdio.h>
#include <stdlib.h>
#include "../../../../libkc3/kc3.h"
#include "../../../../gl/gl_font.h"
#include "../../../../gl/gl_lines.h"
#include "../../../../gl/gl_ortho.h"
#include "../../../../gl/gl_sprite.h"
#include "../../../../gl/gl_text.h"
#include "../../../../gl/mat4.h"
#include "../../../demo/bg_rect.h"
#include "../../../demo/lightspeed.h"
#include "../../../demo/toasters.h"
#include "../../../demo/flies.h"
#include "../../../demo/earth.h"
#include "../../../demo/mandelbrot_f80.h"
#include "../../../demo/matrix.h"
#include "../../../window.h"
#include "../../window_egl.h"
#include "../window_egl_xcb.h"

#define WINDOW_EGL_DEMO_SEQUENCE_COUNT 7

const char *g_env_argv0_default = PROG;
const char *g_env_argv0_dir_default = PREFIX;
s_gl_font   g_font_courier_new = {0};
s_gl_ortho  g_ortho = {0};
s_gl_text   g_text_fps = {0};
s_gl_text   g_text_seq_title = {0};

static bool window_egl_demo_button (s_window_egl *window, u8 button,
                                    s64 x, s64 y);
static bool window_egl_demo_key (s_window_egl *window, u32 keysym);
static bool window_egl_demo_load (s_window_egl *window);
static bool window_egl_demo_render (s_window_egl *window);
static bool window_egl_demo_resize (s_window_egl *window, u64 w, u64 h);
static void window_egl_demo_unload (s_window_egl *window);

int main (int argc, char **argv)
{
  s_window_egl window;
  if (FT_Init_FreeType(&g_ft)) {
    err_puts("main: failed to initialize FreeType");
    return 1;
  }
  if (! kc3_init(NULL, &argc, &argv)) {
    err_puts("kc3_init");
    return 1;
  }
  window_egl_init(&window, 50, 50, 800, 600,
                  "KC3.Window.EGL demo",
                  WINDOW_EGL_DEMO_SEQUENCE_COUNT);
  window.button = window_egl_demo_button;
  window.key    = window_egl_demo_key;
  window.load   = window_egl_demo_load;
  window.render = window_egl_demo_render;
  window.resize = window_egl_demo_resize;
  window.unload = window_egl_demo_unload;
  if (! window_egl_xcb_run(&window)) {
    err_puts("window_egl_xcb_run -> false");
    window_egl_clean(&window);
    kc3_clean(NULL);
    return g_kc3_exit_code;
  }
  window_egl_clean(&window);
  kc3_clean(NULL);
  FT_Done_FreeType(g_ft);
  return 0;
}

bool window_egl_demo_button (s_window_egl *window, u8 button,
                             s64 x, s64 y)
{
  assert(window);
  (void) window;
  err_write_1("window_egl_demo_button: ");
  err_inspect_u8(button);
  err_write_1(" (");
  err_inspect_s64(x);
  err_write_1(", ");
  err_inspect_s64(y);
  err_puts(")");
  if (window->seq && window->seq->button &&
      ! window->seq->button((s_sequence *) window->seq, button, x, y))
    return false;
  return true;
}

bool window_egl_demo_key (s_window_egl *window, u32 keysym)
{
  assert(window);
  (void) window;
  err_write_1("window_egl_demo_key: ");
  err_inspect_u32(keysym);
  err_write_1("\n");
  switch (keysym) {
  case 0xff1b: /* Escape */
  case 'q':
    g_kc3_exit_code = 0;
    return false;
  case 0xff51: /* Left */
    if (! window_set_sequence_pos((s_window *) window,
                                  (window->sequence_pos +
                                   window->sequence_count - 1) %
                                  window->sequence_count))
      return false;
    break;
  case 0xff53: /* Right */
    if (! window_set_sequence_pos((s_window *) window,
                                  (window->sequence_pos + 1) %
                                  window->sequence_count))
      return false;
    break;
  }
  if (window->seq && window->seq->key &&
      ! window->seq->key((s_sequence *) window->seq, keysym))
    return false;
  return true;
}

bool window_egl_demo_load (s_window_egl *window)
{
  f32 point_per_pixel;
  assert(window);
  assert(glGetError() == GL_NO_ERROR);
  if (! module_load(sym_1("GL.Vertex")) ||
      ! module_load(sym_1("GL.Triangle")) ||
      ! module_load(sym_1("GL.Object")) ||
      ! module_load(sym_1("GL.Sphere")))
    return false;
  point_per_pixel = (f32) window->w / window->pixel_w;
  err_write_1("point_per_pixel: ");
  err_inspect_f32(point_per_pixel);
  err_write_1("\n");
  if (window->sequence_count != WINDOW_EGL_DEMO_SEQUENCE_COUNT) {
    err_write_1("window_egl_demo_load: window->sequence_count = ");
    err_inspect_u32_decimal(window->sequence_count);
    err_write_1("\n");
    assert(window->sequence_count == WINDOW_EGL_DEMO_SEQUENCE_COUNT);
    return false;
  }
  if (! gl_ortho_init(&g_ortho))
    return false;
  gl_ortho_resize(&g_ortho, 0, window->w, 0, window->h, 0, 1);
  if (! gl_font_init(&g_font_courier_new,
                     "fonts/Courier New/Courier New.ttf",
                     point_per_pixel)) {
    err_puts("window_egl_demo_load: gl_font_init");
    return false;
  }
  gl_font_set_size(&g_font_courier_new, 20);
  if (! gl_text_init_1(&g_text_fps, &g_font_courier_new, "0.00")) {
    err_puts("window_egl_demo_load: gl_text_init g_text_fps");
    return false;
  }
  if (! gl_text_init_1(&g_text_seq_title, &g_font_courier_new, "")) {
    err_puts("window_egl_demo_load: gl_text_init g_text_seq_title");
    return false;
  }
  sequence_init((s_sequence *) window->sequence, 8.0,
                "01. Background rectangles",
                bg_rect_load,
                bg_rect_render,
                bg_rect_unload,
                window);
  if (! gl_lines_init(&g_lines_stars) ||
      ! gl_lines_allocate(&g_lines_stars, LIGHTSPEED_STAR_MAX))
    return false;
  sequence_init((s_sequence *) window->sequence + 1, 20.0,
                "02. Lightspeed",
                lightspeed_load,
                lightspeed_render,
                lightspeed_unload,
                window);
  if (! gl_sprite_init(&g_sprite_toaster, "img/flaps.256.png",
                       4, 1, 4, 1))
    return false;
  if (! gl_sprite_init(&g_sprite_toast, "img/toast.128.png",
                       1, 1, 1, 1))
    return false;
  sequence_init((s_sequence *) window->sequence + 2, 60.0,
                "03. Toasters",
                toasters_load,
                toasters_render,
                toasters_unload,
                window);
  if (! gl_font_init(&g_font_flies,
                     "fonts/Courier New/Courier New.ttf",
                     point_per_pixel))
    return false;
  if (! gl_sprite_init(&g_sprite_fly, "img/fly-noto.png",
                       1, 1, 1, point_per_pixel))
    return false;
  if (! gl_sprite_init(&g_sprite_dead_fly, "img/fly-dead.png",
                       1, 1, 1, point_per_pixel))
    return false;
  sequence_init((s_sequence *) window->sequence + 3, 60.0,
                "04. Flies",
                flies_load,
                flies_render,
                flies_unload,
                window);
  if (! gl_sprite_init(&g_sprite_earth, "img/earth.png",
                       1, 1, 1, point_per_pixel))
    return false;
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  assert(glGetError() == GL_NO_ERROR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  assert(glGetError() == GL_NO_ERROR);
  sequence_init((s_sequence *) window->sequence + 4, 120.0,
                "05. Earth",
                earth_load,
                earth_render,
                earth_unload,
                window);
  sequence_init((s_sequence *) window->sequence + 5, 3600.0,
                "06. Mandelbrot (f80)",
                mandelbrot_f80_load,
                mandelbrot_f80_render,
                mandelbrot_f80_unload,
                window);
  window->sequence[5].button =
    (f_sequence_button) mandelbrot_f80_button;
  sequence_init((s_sequence *) window->sequence + 6, 3600.0,
                "07. Matrix",
                matrix_load,
                matrix_render,
                matrix_unload,
                window);
  window_set_sequence_pos((s_window *) window, 0);
  return true;
}

bool window_egl_demo_render (s_window_egl *window)
{
  s_sequence_egl *seq;
  const s_rgb text_color[2] = {
    {1.0f, 1.0f, 1.0f},
    {0.0f, 0.0f, 0.0f}
  };
  assert(window);
  if (! window_animate((s_window *) window)) {
    err_puts("window_egl_demo_render: window_animate");
    return false;
  }
  seq = window->seq;
  if (seq && seq->render && ! seq->render((s_sequence *) seq)) {
    err_puts("window_egl_demo_render: seq->render");
    return false;
  }
  mat4_init_identity(&g_ortho.model_matrix);
  gl_ortho_render(&g_ortho);
  glDisable(GL_DEPTH_TEST);
  glEnable(GL_BLEND);
  glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA,
                      GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
  gl_font_set_size(&g_font_courier_new, 20);
  gl_text_update_1(&g_text_seq_title, seq->title);
  mat4_init_identity(&g_ortho.model_matrix);
  gl_ortho_text_render_outline(&g_ortho, &g_text_seq_title,
                               20.0f, 30.0f, text_color,
                               text_color + 1);
  mat4_init_identity(&g_ortho.model_matrix);
  glDisable(GL_BLEND);
  gl_ortho_bind_texture(&g_ortho, 0);
  gl_ortho_color(&g_ortho, 1.0f, 1.0f, 1.0f, 1.0f);
  gl_ortho_rect(&g_ortho, 19, 11,
                (window->w - 40.0) * seq->t / seq->duration + 2,
                4);
  gl_ortho_color(&g_ortho, 0.0f, 0.0f, 0.0f, 1.0f);
  gl_ortho_rect(&g_ortho, 20, 12,
                (window->w - 40.0) * seq->t / seq->duration,
                2);
  char fps[32];
  snprintf(fps, sizeof(fps), "%.1f", (f64) seq->frame / seq->t);
  mat4_init_identity(&g_ortho.model_matrix);
  gl_text_update_1(&g_text_fps, fps);
  glEnable(GL_BLEND);
  gl_ortho_text_render_outline(&g_ortho, &g_text_fps,
                               20, window->h - 30, text_color,
                               text_color + 1);
  gl_ortho_render_end(&g_ortho);
  return true;
}

bool window_egl_demo_resize (s_window_egl *window, u64 w, u64 h)
{
  assert(window);
  err_write_1("window_egl_demo_resize: ");
  err_inspect_u64(w);
  err_write_1(" x ");
  err_inspect_u64(h);
  err_write_1("\n");
  glViewport(0, 0, w, h);
  gl_ortho_resize(&g_ortho, 0, window->w, 0, window->h, 0, 1);
  return true;
}

void window_egl_demo_unload (s_window_egl *window)
{
  assert(window);
  (void) window;
  gl_text_clean(&g_text_fps);
  gl_text_clean(&g_text_seq_title);
  gl_font_clean(&g_font_courier_new);
  gl_lines_clean(&g_lines_stars);
  gl_sprite_clean(&g_sprite_toaster);
  gl_sprite_clean(&g_sprite_toast);
  gl_font_clean(&g_font_flies);
  gl_sprite_clean(&g_sprite_fly);
  gl_sprite_clean(&g_sprite_dead_fly);
  gl_sprite_clean(&g_sprite_earth);
  gl_ortho_clean(&g_ortho);
  err_puts("window_egl_demo_unload");
}
