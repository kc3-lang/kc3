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
#include <stdio.h>
#include <stdlib.h>
#include <libc3/c3.h>
#include "../../window.h"
#include "../gl_font.h"
#include "../gl_lines.h"
#include "../gl_matrix_4f.h"
#include "../gl_ortho.h"
#include "../gl_square.h"
#include "../gl_text.h"
#include "../gl_sprite.h"
#include "../window_sdl2.h"
#include "bg_rect.h"
#include "lightspeed.h"
#include "toasters.h"
#include "flies.h"
#include "earth.h"

#define WINDOW_SDL2_DEMO_SEQUENCE_COUNT 5

//s_gl_font   g_font_computer_modern = {0};
s_gl_font   g_font_courier_new = {0};
s_gl_ortho  g_ortho = {0};
s_gl_square g_square = {0};
s_gl_text   g_text_fps = {0};
s_gl_text   g_text_seq_title = {0};

static bool window_sdl2_demo_button (s_window_sdl2 *window, u8 button,
                                     sw x, sw y);
static bool window_sdl2_demo_key (s_window_sdl2 *window,
                                  SDL_Keysym *keysym);
static bool window_sdl2_demo_load (s_window_sdl2 *window);
static bool window_sdl2_demo_render (s_window_sdl2 *window);
static bool window_sdl2_demo_resize (s_window_sdl2 *window,
                                     uw w, uw h);
static void window_sdl2_demo_unload (s_window_sdl2 *window);

int main (int argc, char **argv)
{
  s_window_sdl2 window;
  if (FT_Init_FreeType(&g_ft)) {
    err_puts("main: failed to initialize FreeType");
    return 1;
  }
  if (! c3_init(NULL, argc, argv)) {
    err_puts("c3_init");
    return 1;
  }
  window_sdl2_init(&window, 50, 50, 800, 600,
                   "C3.Window.SDL2 demo",
                   WINDOW_SDL2_DEMO_SEQUENCE_COUNT);
  window.button = window_sdl2_demo_button;
  window.key    = window_sdl2_demo_key;
  window.load   = window_sdl2_demo_load;
  window.render = window_sdl2_demo_render;
  window.resize = window_sdl2_demo_resize;
  window.unload = window_sdl2_demo_unload;
  if (! window_sdl2_run(&window)) {
    err_puts("window_sdl2_run -> false");
    window_sdl2_clean(&window);
    c3_clean(NULL);
    SDL_Quit();
    return g_c3_exit_code;
  }
  window_sdl2_clean(&window);
  c3_clean(NULL);
  SDL_Quit();
  FT_Done_FreeType(g_ft);
  return 0;
}

bool window_sdl2_demo_button (s_window_sdl2 *window, u8 button,
                              sw x, sw y)
{
  assert(window);
  (void) window;
  printf("c3_window_sdl2_demo_button: %lu (%ld, %ld)\n", (uw) button, x, y);
  return true;
}

bool window_sdl2_demo_key (s_window_sdl2 *window, SDL_Keysym *keysym)
{
  assert(window);
  assert(keysym);
  (void) window;
  switch (keysym->sym) {
  case SDLK_f:
    if (! window->fullscreen) {
      if (SDL_SetWindowFullscreen(window->sdl_window,
                                  SDL_WINDOW_FULLSCREEN_DESKTOP)) {
        err_write_1("window_sdl2_demo_key:"
                    " SDL_SetWindowFullscreen(:desktop): ");
        err_puts(SDL_GetError());
        SDL_MaximizeWindow(window->sdl_window);
      }
    }
    else {
      if (SDL_SetWindowFullscreen(window->sdl_window, 0)) {
        err_write_1("window_sdl2_demo_key:"
                    " SDL_SetWindowFullscreen(0): ");
        err_puts(SDL_GetError());
        SDL_RestoreWindow(window->sdl_window);
      }
    }
    window->fullscreen = ! window->fullscreen;
    return true;
  case SDLK_ESCAPE:
  case SDLK_q:
    g_c3_exit_code = 0;
    return false;
  case SDLK_LEFT:
    if (! window_set_sequence_pos((s_window *) window,
                                  (window->sequence_pos +
                                   window->sequence_count - 1) %
                                  window->sequence_count))
      return false;
    break;
  case SDLK_RIGHT:
    if (! window_set_sequence_pos((s_window *) window,
                                  (window->sequence_pos + 1) %
                                  window->sequence_count))
      return false;
    break;
  default:
    printf("c3_window_sdl2_demo_key: %d\n", keysym->sym);
  }
  return true;
}

bool window_sdl2_demo_load (s_window_sdl2 *window)
{
  assert(window);
  assert(glGetError() == GL_NO_ERROR);
  if (window->sequence_count != WINDOW_SDL2_DEMO_SEQUENCE_COUNT) {
    fprintf(stderr, "window_sdl2_demo_load: "
            "window->sequence_count = %lu\n", window->sequence_count);
    assert(window->sequence_count == WINDOW_SDL2_DEMO_SEQUENCE_COUNT);
    return false;
  }
  if (! gl_ortho_init(&g_ortho))
    return false;
  gl_ortho_resize(&g_ortho, 0, window->w, 0, window->h, 0, 1);
  if (! gl_font_init(&g_font_courier_new,
                     "fonts/Courier New/Courier New.ttf"))
    return false;
  if (! gl_text_init_1(&g_text_seq_title, &g_font_courier_new, ""))
    return false;
  if (! gl_text_init_1(&g_text_fps, &g_font_courier_new, "0.00"))
    return false;
  sequence_init(window->sequence, 8.0, "01. Background rectangles",
                bg_rect_load, bg_rect_render, bg_rect_unload, window);
  if (! gl_lines_init(&g_lines_stars) ||
      ! gl_lines_allocate(&g_lines_stars, LIGHTSPEED_STAR_MAX))
    return false;
  sequence_init(window->sequence + 1, 20.0, "02. Lightspeed",
                lightspeed_load, lightspeed_render, lightspeed_unload,
                window);
  if (! gl_sprite_init(&g_sprite_toaster, "img/flaps.256.png",
                       4, 1, 4))
    return false;
  if (! gl_sprite_init(&g_sprite_toast, "img/toast.128.png",
                       1, 1, 1))
    return false;
  sequence_init(window->sequence + 2, 60.0, "03. Toasters",
                toasters_load, toasters_render, toasters_unload,
                window);
  if (! gl_font_init(&g_font_flies,
                     "fonts/Courier New/Courier New.ttf"))
    return false;
  if (! gl_sprite_init(&g_sprite_fly, "img/fly-noto.png",
                       1, 1, 1))
    return false;
  if (! gl_sprite_init(&g_sprite_dead_fly, "img/fly-dead.png",
                       1, 1, 1))
    return false;
  sequence_init(window->sequence + 3, 60.0, "04. Flies",
                flies_load, flies_render, flies_unload, window);
  if (! gl_sprite_init(&g_sprite_earth, "img/earth.png",
                       1, 1, 1))
    return false;
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  assert(glGetError() == GL_NO_ERROR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  assert(glGetError() == GL_NO_ERROR);
  sequence_init(window->sequence + 4, 120.0, "05. Earth",
                earth_load, earth_render, earth_unload, window);
  assert(glGetError() == GL_NO_ERROR);
  window_set_sequence_pos((s_window *) window, 0);
  assert(glGetError() == GL_NO_ERROR);
  return true;
}

static void render_text (s_gl_text *text, f64 x, f64 y)
{
  s_gl_matrix_4f matrix;
  assert(glGetError() == GL_NO_ERROR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                  GL_LINEAR);
  glEnable(GL_BLEND);
  glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA,
                      GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
  assert(glGetError() == GL_NO_ERROR);
  matrix = g_ortho.model_matrix;
  gl_matrix_4f_translate(&g_ortho.model_matrix, x - 1.0, y - 1.0, 0.0);
  gl_ortho_update_model_matrix(&g_ortho);
  glBlendColor(1.0f, 1.0f, 1.0f, 1.0f);
  assert(glGetError() == GL_NO_ERROR);
  gl_ortho_text_render(&g_ortho, text);
  assert(glGetError() == GL_NO_ERROR);
  gl_matrix_4f_translate(&g_ortho.model_matrix, 1.0, 0.0, 0.0);
  gl_ortho_update_model_matrix(&g_ortho);
  gl_ortho_text_render(&g_ortho, text);
  gl_matrix_4f_translate(&g_ortho.model_matrix, 1.0, 0.0, 0.0);
  gl_ortho_update_model_matrix(&g_ortho);
  gl_ortho_text_render(&g_ortho, text);
  gl_matrix_4f_translate(&g_ortho.model_matrix, 0.0, 1.0, 0.0);
  gl_ortho_update_model_matrix(&g_ortho);
  gl_ortho_text_render(&g_ortho, text);
  gl_matrix_4f_translate(&g_ortho.model_matrix, -1.0, 0.0, 0.0);
  gl_ortho_update_model_matrix(&g_ortho);
  gl_ortho_text_render(&g_ortho, text);
  gl_matrix_4f_translate(&g_ortho.model_matrix, -1.0, 0.0, 0.0);
  gl_ortho_update_model_matrix(&g_ortho);
  gl_ortho_text_render(&g_ortho, text);
  gl_matrix_4f_translate(&g_ortho.model_matrix, 0.0, 1.0, 0.0);
  gl_ortho_update_model_matrix(&g_ortho);
  gl_ortho_text_render(&g_ortho, text);
  gl_matrix_4f_translate(&g_ortho.model_matrix, 1.0, 0.0, 0.0);
  gl_ortho_update_model_matrix(&g_ortho);
  gl_ortho_text_render(&g_ortho, text);
  assert(glGetError() == GL_NO_ERROR);
  gl_matrix_4f_translate(&g_ortho.model_matrix, 1.0, 0.0, 0.0);
  gl_ortho_update_model_matrix(&g_ortho);
  gl_ortho_text_render(&g_ortho, text);
  assert(glGetError() == GL_NO_ERROR);
  glBlendColor(0.0f, 0.0f, 0.0f, 1.0f);
  assert(glGetError() == GL_NO_ERROR);
  gl_matrix_4f_translate(&g_ortho.model_matrix, -1.0, -1.0, 0.0);
  gl_ortho_update_model_matrix(&g_ortho);
  assert(glGetError() == GL_NO_ERROR);
  gl_ortho_text_render(&g_ortho, text);
  assert(glGetError() == GL_NO_ERROR);
  g_ortho.model_matrix = matrix;
}

bool window_sdl2_demo_render (s_window_sdl2 *window)
{
  s_sequence *seq;
  assert(window);
  assert(glGetError() == GL_NO_ERROR);
  if (! window_animate((s_window *) window))
    return false;
  seq = window->seq;
  gl_matrix_4f_init_identity(&g_ortho.model_matrix);
  gl_ortho_render(&g_ortho);
  glEnable(GL_BLEND);
  glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA,
                      GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
  assert(glGetError() == GL_NO_ERROR);
  if (! seq->render(seq))
    return false;
  assert(glGetError() == GL_NO_ERROR);
  /* 2D */
  gl_ortho_render(&g_ortho);
  glDisable(GL_DEPTH_TEST);
  glEnable(GL_BLEND);
  glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA,
                      GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
  assert(glGetError() == GL_NO_ERROR);
  gl_font_set_size(&g_font_courier_new, 20,
                   (f64) window->gl_h / window->h);
  gl_text_update_1(&g_text_seq_title, seq->title);
  gl_matrix_4f_init_identity(&g_ortho.model_matrix);
  render_text(&g_text_seq_title, 20.0f, 30.0f);
  /* progress bar */
  gl_matrix_4f_init_identity(&g_ortho.model_matrix);
  glDisable(GL_BLEND);
  assert(glGetError() == GL_NO_ERROR);
  gl_ortho_bind_texture(&g_ortho, 0);
  assert(glGetError() == GL_NO_ERROR);
  glBlendColor(1.0f, 1.0f, 1.0f, 1.0f);
  gl_ortho_rect(&g_ortho, 19, 11,
                (window->w - 40.0) * seq->t / seq->duration + 2,
                4);
  glBlendColor(0.0f, 0.0f, 0.0f, 1.0f);
  gl_ortho_rect(&g_ortho, 20, 12,
                (window->w - 40.0) * seq->t / seq->duration,
                2);
  /* fps */
  char fps[32];
  snprintf(fps, sizeof(fps), "%.1f", (f64) seq->frame / seq->t);
  gl_matrix_4f_init_identity(&g_ortho.model_matrix);
  gl_text_update_1(&g_text_fps, fps);
  glEnable(GL_BLEND);
  render_text(&g_text_fps, 20, window->h - 30);
  gl_ortho_render_end(&g_ortho);
  return true;
}

bool window_sdl2_demo_resize (s_window_sdl2 *window,
                              uw w, uw h)
{
  assert(window);
  (void) window;
  assert(glGetError() == GL_NO_ERROR);
  gl_ortho_resize(&g_ortho, 0, w, 0, h, 0, 1);
  assert(glGetError() == GL_NO_ERROR);
  return true;
}

void window_sdl2_demo_unload (s_window_sdl2 *window)
{
  assert(window);
  (void) window;
  if (g_ortho.gl_shader_program) {
    gl_ortho_clean(&g_ortho);
    gl_font_clean(&g_font_courier_new);
    gl_sprite_clean(&g_sprite_toaster);
    gl_sprite_clean(&g_sprite_toast);
    gl_font_clean(&g_font_flies);
    gl_sprite_clean(&g_sprite_fly);
    gl_sprite_clean(&g_sprite_dead_fly);
    gl_sprite_clean(&g_sprite_earth);
  }
}
