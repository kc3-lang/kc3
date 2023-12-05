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
  window->fullscreen = false;
  window->button = window_sdl2_button_default;
  window->key = window_sdl2_key_default;
  window->load = window_sdl2_load_default;
  window->motion = window_sdl2_motion_default;
  window->render = window_sdl2_render_default;
  window->resize = window_sdl2_resize_default;
  window->sdl_window = NULL;
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
                                 void *sdl_window)
{
  (void) window;
  (void) sdl_window;
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
  SDL_GLContext context;
  int quit = 0;
  SDL_Event sdl_event;
  SDL_Window *sdl_window;
  assert(window);
  if (! g_window_sdl2_initialized) {
    //SDL_SetHint(SDL_HINT_VIDEO_HIGHDPI_DISABLED, "1");
    if (SDL_Init(SDL_INIT_VIDEO)) {
      warnx("window_sdl2_run: SDL initialization failed: %s",
            SDL_GetError());
      return false;
    }
    g_window_sdl2_initialized = true;
  }
  if (SDL_VideoInit(NULL)) {
    warnx("window_sdl2_run: SDL_VideoInit failed: %s",
          SDL_GetError());
    return false;
  }
  SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
  /*
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,
                      SDL_GL_CONTEXT_PROFILE_CORE);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
  */
  sdl_window = SDL_CreateWindow(window->title,
                                window->x, window->y,
                                window->w, window->h,
                                SDL_WINDOW_ALLOW_HIGHDPI |
                                SDL_WINDOW_OPENGL |
                                SDL_WINDOW_RESIZABLE);
  if (! sdl_window) {
    warnx("window_sdl2_run: failed to create window: %s",
          SDL_GetError());
    return false;
  }
  window->sdl_window = sdl_window;
  context = SDL_GL_CreateContext(sdl_window);
  if (! context) {
    warnx("window_sdl2_run: failed to create OpenGL context: %s",
          SDL_GetError());
    return false;
  }
  GLenum gl_error = glGetError();
  if (gl_error != GL_NO_ERROR) {
    warnx("OpenGL initialization error: %s\n",
          gluErrorString(gl_error));
    goto ko;
  }
  if (SDL_GL_MakeCurrent(sdl_window, context) < 0) {
    warnx("window_sdl2_run: failed to make OpenGL context current: %s",
          SDL_GetError());
    goto ko;
  }
  int gl_w = window->w;
  int gl_h = window->h;
  SDL_GL_GetDrawableSize(sdl_window, &gl_w, &gl_h);
  window->gl_w = gl_w;
  window->gl_h = gl_h;
  int display_index;
  display_index = SDL_GetWindowDisplayIndex(sdl_window);
  if (display_index < 0) {
    fprintf(stderr, "window_sdl2_run: failed to get display DPI: %s\n",
            SDL_GetError());
    goto ko;
  }
  if (SDL_GetDisplayDPI(display_index, &window->dpi, &window->dpi_w,
                        &window->dpi_h) != 0) {
    fprintf(stderr, "window_sdl2_run: failed to get display DPI: %s\n",
            SDL_GetError());
    goto ko;
  }
  printf("Window DPI: dpi=%.2f, dpi_w=%.2f, dpi_h=%.2f\n", window->dpi,
         window->dpi_w, window->dpi_h);
  SDL_GL_SetSwapInterval(1);
  if (! window->load(window)) {
    warnx("window_sdl2_run: window->load => false");
    goto ko;
  }
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
          SDL_GL_GetDrawableSize(sdl_window, &gl_w, &gl_h);
          window->gl_w = gl_w;
          window->gl_h = gl_h;
          glViewport(0, 0, window->gl_w, window->gl_h);
        }
        break;
      default:
        break;
      }
    }
    //glDrawBuffer(GL_BACK);
    if (! window->render(window, NULL)) {
      warnx("window_sdl2_run: window->render -> false");
      quit = 1;
    }
    SDL_GL_SwapWindow(sdl_window);
  }
  SDL_GL_DeleteContext(context);
  SDL_DestroyWindow(sdl_window);
  return true;
 ko:
  SDL_GL_DeleteContext(context);
  SDL_DestroyWindow(sdl_window);
  return false;
}

s_sequence * window_sdl2_sequence_init
(s_sequence *seq, f64 duration, const s8 *title,
 f_window_sdl2_sequence_load load,
 f_window_sdl2_sequence_render render)
{
  assert(seq);
  seq->t = 0.0;
  seq->duration = duration;
  seq->frame = 0;
  seq->title = title;
  seq->load = (f_sequence_load) load;
  seq->render = (f_sequence_render) render;
  tag_init_void(&seq->tag);
  return seq;
}
