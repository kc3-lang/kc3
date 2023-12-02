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
#include <assert.h>
#include <err.h>
#include <stdlib.h>
#include <xkbcommon/xkbcommon.h>
#include <libc3/tag.h>
#include "window_sdl2.h"

static bool g_window_sdl2_initialized = false;

s_window_sdl2 * window_sdl2_init (s_window_sdl2 *window,
                                    sw x, sw y, uw w, uw h,
                                    const s8 *title,
                                    uw sequence_count)
{
  assert(window);
  window->x = x;
  window->y = y;
  window->w = w;
  window->h = h;
  window->button = window_sdl2_button_default;
  window->key = window_sdl2_key_default;
  window->load = window_sdl2_load_default;
  window->motion = window_sdl2_motion_default;
  window->render = window_sdl2_render_default;
  window->resize = window_sdl2_resize_default;
  window->context = NULL;
  window->sequence = calloc(sequence_count, sizeof(s_sequence));
  window->sequence_count = sequence_count;
  window->sequence_pos = 0;
  window->title = title ? title : "C3.Window.Sdl2";
  return window;
}

bool window_sdl2_button_default (s_window_sdl2 *window, u8 button,
                                  sw x, sw y)
{
  assert(window);
  (void) window;
  (void) button;
  (void) x;
  (void) y;
  printf("window_sdl2_button_default: %d (%ld, %ld)\n",
         (int) button, x, y);
  return true;
}

bool window_sdl2_key_default (s_window_sdl2 *window, SDL_Keysym *keysym)
{
  assert(window);
  assert(keysym);
  (void) window;
  (void) keysym;
  printf("window_sdl2_key_default: %d\n", keysym->sym);
  return true;
}

bool window_sdl2_load_default (s_window_sdl2 *window)
{
  assert(window);
  (void) window;
  printf("window_sdl2_load_default\n");
  return true;
}

bool window_sdl2_motion_default (s_window_sdl2 *window, sw x, sw y)
{
  assert(window);
  (void) window;
  (void) x;
  (void) y;
  /*
  printf("window_sdl2_motion_default (%ld, %ld)\n", x, y);
  */
  return true;
}

bool window_sdl2_render_default (s_window_sdl2 *window,
                                 SDL_GLContext context)
{
  (void) window;
  (void) context;
  assert(window);
  printf("window_sdl2_render_default\n");
  return true;
}

bool window_sdl2_resize_default (s_window_sdl2 *window, uw w, uw h)
{
  assert(window);
  (void) window;
  (void) w;
  (void) h;
  printf("window_sdl2_resize_default: %lu x %lu\n", w, h);
  return true;
}

bool window_sdl2_run (s_window_sdl2 *window)
{
  int quit = 0;
  SDL_Event sdl_event;
  assert(window);
  if (! g_window_sdl2_initialized) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
      warnx("window_sdl2_run: SDL initialization failed: %s",
            SDL_GetError());
      return false;
    }
    g_window_sdl2_initialized = true;
  }
  window->sdl_window = SDL_CreateWindow(window->title,
                                        window->x, window->y,
                                        window->w, window->h,
                                        SDL_WINDOW_OPENGL |
                                        SDL_WINDOW_RESIZABLE);
  if (! window->sdl_window) {
    warnx("window_sdl2_run: failed to create window: %s",
          SDL_GetError());
    return false;
  }
  window->context = SDL_GL_CreateContext(window->sdl_window);
  if (! window->context) {
    warnx("window_sdl2_run: failed to create OpenGL context: %s",
          SDL_GetError());
    return false;
  }
  SDL_GL_SetSwapInterval(1);
  while (! quit) {
    while (SDL_PollEvent(&sdl_event) != 0) {
      switch (sdl_event.type) {
      case SDL_QUIT:
        warnx("window_sdl2_run: SDL_QUIT");
        quit = 1;
        break;
      case SDL_KEYDOWN:
        if (! window->key(window, &sdl_event.key.keysym)) {
          warnx("window_sdl2_run: window->key -> false");
          quit = 1;
        }
        break;
      case SDL_MOUSEBUTTONDOWN:
        if (! window->button(window, sdl_event.button.button,
                             sdl_event.button.x, sdl_event.button.y)) {
          warnx("window_sdl2_run: window->button -> false");
          quit = 1;
        }
        break;
      case SDL_MOUSEMOTION:
        //int relativeX = sdl_event.motion.xrel;
        //int relativeY = sdl_event.motion.yrel;
        if (! window->motion(window, sdl_event.motion.x,
                             sdl_event.motion.y)) {
          warnx("window_sdl2_run: window->motion -> false");
          quit = 1;
        }
        break;
      case SDL_WINDOWEVENT:
        if (sdl_event.window.event == SDL_WINDOWEVENT_RESIZED) {
          if (! window->resize(window, sdl_event.window.data1,
                               sdl_event.window.data2)) {
            warnx("window_sdl2_run: window->resize -> false");
            quit = 1;
          }
          window->w = sdl_event.window.data1;
          window->h = sdl_event.window.data2;
        }
        break;
      default:
        break;
      }
    }
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    SDL_GL_SwapWindow(window->sdl_window);
  }
  SDL_GL_DeleteContext(window->context);
  SDL_DestroyWindow(window->sdl_window);
  return true;
}

s_sequence * window_sdl2_sequence_init
(s_sequence *seq, f64 duration, const s8 *title,
 f_window_sdl2_sequence_load load,
 f_window_sdl2_sequence_render render)
{
  assert(seq);
  seq->t = 0.0;
  seq->duration = duration;
  seq->title = title;
  seq->load = (f_sequence_load) load;
  seq->render = (f_sequence_render) render;
  tag_init_void(&seq->tag);
  return seq;
}
