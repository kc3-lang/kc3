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
#include "../window_sdl2.h"
#include "../sdl2_font.h"
#include "bg_rect.h"

#define WINDOW_SDL2_DEMO_SEQUENCE_COUNT 1

//static s_sdl2_font g_font_computer_modern;
static s_sdl2_font g_font_courier_new;

static bool window_sdl2_demo_button (s_window_sdl2 *window, u8 button,
                                     sw x, sw y);
static bool window_sdl2_demo_key (s_window_sdl2 *window,
                                  SDL_Keysym *keysym);
static bool window_sdl2_demo_load (s_window_sdl2 *window);
static bool window_sdl2_demo_render (s_window_sdl2 *window,
                                     void *context);
static bool window_sdl2_demo_resize (s_window_sdl2 *window,
                                     uw w, uw h);

int main (int argc, char **argv)
{
  s_window_sdl2 window;
  if (! c3_init(NULL, argc, argv)) {
    err_puts("c3_init");
    return 1;
  }
  window_sdl2_init(&window, 0, 0, 800, 600,
                   "C3.Window.SDL2 demo",
                   WINDOW_SDL2_DEMO_SEQUENCE_COUNT);
  window.button = window_sdl2_demo_button;
  window.key    = window_sdl2_demo_key;
  window.load   = window_sdl2_demo_load;
  window.render = window_sdl2_demo_render;
  window.resize = window_sdl2_demo_resize;
  if (! window_sdl2_run(&window)) {
    err_puts("window_sdl2_run -> false");
    c3_clean(NULL);
    return g_c3_exit_code;
  }
  c3_clean(NULL);
  SDL_Quit();
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
        warnx("window_sdl2_demo_key: SDL_SetWindowFullscreen(:desktop): %s",
              SDL_GetError());
        SDL_MaximizeWindow(window->sdl_window);
      }
    }
    else {
      if (SDL_SetWindowFullscreen(window->sdl_window, 0)) {
        warnx("window_sdl2_demo_key: SDL_SetWindowFullscreen(0): %s",
              SDL_GetError());
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
  if (window->sequence_count != WINDOW_SDL2_DEMO_SEQUENCE_COUNT) {
    fprintf(stderr, "window_sdl2_demo_load: "
            "window->sequence_count = %lu\n", window->sequence_count);
    assert(window->sequence_count == WINDOW_SDL2_DEMO_SEQUENCE_COUNT);
    return false;
  }
  if (! sdl2_font_init(&g_font_courier_new,
                       "fonts/Courier New/Courier New.ttf"))
    return false;
  window_sdl2_sequence_init(window->sequence, 8.0,
                            "01. Background rectangles",
                            bg_rect_load, bg_rect_render);
  /*
  window_sdl2_sequence_init(window->sequence + 1, 20.0,
                            "02. Lightspeed",
                            lightspeed_load, lightspeed_render);
  if (! sdl2_sprite_init(&g_toaster_sprite,
                         sdl2_png_1("img/flaps.png"),
                         4, 1, 4))
    return false;
  if (! sdl2_sprite_init(&g_toast_sprite,
                          sdl2_png_1("img/toast.png"),
                          1, 1, 1))
    return false;
  window_sdl2_sequence_init(window->sequence + 2, 60.0,
                             "03. Toasters",
                             toasters_load, toasters_render);
  if (! sdl2_sprite_init(&g_fly_sprite,
                          sdl2_png_1("img/fly-noto.png"),
                          1, 1, 1))
    return false;
  if (! sdl2_sprite_init(&g_dead_fly_sprite,
                          sdl2_png_1("img/fly-dead.png"),
                          1, 1, 1))
    return false;
  window_sdl2_sequence_init(window->sequence + 3, 60.0,
                             "04. Flies",
                             flies_load, flies_render);
  */
  window_set_sequence_pos((s_window *) window, 0);
  return true;
}

static void render_text (s_sdl2_font *font, double x, double y,
                         const s8 *p)
{
  glPushMatrix();
  glTranslated(x, y, 0.0);
  glPushMatrix();
  glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
  glTranslatef(-1.0f, -1.0f, 0.0f);
  sdl2_font_render_text(font, p);
  glTranslatef( 1.0f,  0.0f, 0.0f);
  sdl2_font_render_text(font, p);
  glTranslatef( 1.0f,  0.0f, 0.0f);
  sdl2_font_render_text(font, p);
  glTranslatef( 0.0f,  1.0f, 0.0f);
  sdl2_font_render_text(font, p);
  glTranslatef(-1.0f,  0.0f, 0.0f);
  sdl2_font_render_text(font, p);
  glTranslatef(-1.0f,  0.0f, 0.0f);
  sdl2_font_render_text(font, p);
  glTranslatef( 0.0f,  1.0f, 0.0f);
  sdl2_font_render_text(font, p);
  glTranslatef( 1.0f,  0.0f, 0.0f);
  sdl2_font_render_text(font, p);
  glTranslatef( 1.0f,  0.0f, 0.0f);
  sdl2_font_render_text(font, p);
  glPopMatrix();
  glColor4f(0.0f, 0.0f, 0.0f, 1.0f);
  sdl2_font_render_text(font, p);
  glPopMatrix();
}

bool window_sdl2_demo_render (s_window_sdl2 *window, void *context)
{
  s_sequence *seq;
  assert(window);
  if (! window_animate((s_window *) window))
    return false;
  seq = window->sequence + window->sequence_pos;
  if (! seq->render(seq, window, context))
    return false;
  /* 2D */
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0, window->w, 0, window->h, -1, 1);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glDisable(GL_DEPTH_TEST);
  sdl2_font_set_size(&g_font_courier_new, 20, window->dpi);
  render_text(&g_font_courier_new, 20.0f, 30.0f, seq->title);
  /* progress bar */
  glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
  glRectd(19, 11,
          19 + (window->w - 40.0) * seq->t / seq->duration + 2,
          11 + 4);
  glColor4f(0.0f, 0.0f, 0.0f, 1.0f);
  glRectd(20, 12,
          20 + (window->w - 40.0) * seq->t / seq->duration,
          12 + 2);
  /* fps */
  s8 fps[32];
  snprintf(fps, sizeof(fps), "%f", (f64) seq->frame / seq->t);
  render_text(&g_font_courier_new, 20, window->h - 30, fps);
  return true;
}

bool window_sdl2_demo_resize (s_window_sdl2 *window,
                              uw w, uw h)
{
  assert(window);
  (void) window;
  (void) w;
  (void) h;
  return true;
}
