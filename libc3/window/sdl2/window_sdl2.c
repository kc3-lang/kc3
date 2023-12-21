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
#include <err.h>
#include <stdlib.h>
#include <xkbcommon/xkbcommon.h>
#include <libc3/c3.h>
#include "../window.h"
#include "window_sdl2.h"

static bool g_window_sdl2_initialized = false;

void window_sdl2_clean (s_window_sdl2 *window)
{
  window_clean((s_window *) window);
}

bool window_sdl2_default_button_cb (s_window_sdl2 *window, u8 button,
                                    sw x, sw y)
{
  assert(window);
  (void) window;
  (void) button;
  (void) x;
  (void) y;
  printf("window_sdl2_default_button_cb: %d (%ld, %ld)\n",
         (int) button, x, y);
  return true;
}

bool window_sdl2_default_key_cb (s_window_sdl2 *window,
                                 SDL_Keysym *keysym)
{
  assert(window);
  assert(keysym);
  (void) window;
  (void) keysym;
  printf("window_sdl2_default_key_cb: %d\n", keysym->sym);
  return true;
}

bool window_sdl2_default_load_cb (s_window_sdl2 *window)
{
  assert(window);
  (void) window;
  printf("window_sdl2_default_load_cb\n");
  return true;
}

bool window_sdl2_default_motion_cb (s_window_sdl2 *window, sw x, sw y)
{
  assert(window);
  (void) window;
  (void) x;
  (void) y;
  /*
  printf("window_sdl2_default_motion_cb (%ld, %ld)\n", x, y);
  */
  return true;
}

bool window_sdl2_default_render_cb (s_window_sdl2 *window,
                                    void *sdl_window)
{
  (void) window;
  (void) sdl_window;
  assert(window);
  printf("window_sdl2_default_render_cb\n");
  return true;
}

bool window_sdl2_default_resize_cb (s_window_sdl2 *window, uw w, uw h)
{
  assert(window);
  (void) window;
  (void) w;
  (void) h;
  printf("window_sdl2_default_resize_cb: %lu x %lu\n", w, h);
  return true;
}

void window_sdl2_default_unload_cb (s_window_sdl2 *window)
{
  assert(window);
  (void) window;
  printf("window_sdl2_default_unload_cb\n");
}

s_window_sdl2 * window_sdl2_init (s_window_sdl2 *window,
                                  sw x, sw y, uw w, uw h,
                                  const s8 *title,
                                  uw sequence_count)
{
  s_window_sdl2 tmp = {0};
  assert(window);
  title = title ? title : "C3.Window.Sdl2";
  window_init((s_window *) &tmp, x, y, w, h, title, sequence_count);
  tmp.button = window_sdl2_default_button_cb;
  tmp.key    = window_sdl2_default_key_cb;
  tmp.load   = window_sdl2_default_load_cb;
  tmp.motion = window_sdl2_default_motion_cb;
  tmp.render = window_sdl2_default_render_cb;
  tmp.resize = window_sdl2_default_resize_cb;
  tmp.unload = window_sdl2_default_unload_cb;
  *window = tmp;
  return window;
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
  sdl_window = SDL_CreateWindow(window->title,
                                window->x, window->y,
                                window->w, window->h,
                                SDL_WINDOW_ALLOW_HIGHDPI |
                                SDL_WINDOW_OPENGL |
                                SDL_WINDOW_RESIZABLE |
				SDL_WINDOW_SHOWN);
  if (! sdl_window) {
    warnx("window_sdl2_run: failed to create window: %s",
          SDL_GetError());
    return false;
  }
  SDL_SetWindowBordered(sdl_window, SDL_TRUE);
  window->sdl_window = sdl_window;
  SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,
                      SDL_GL_CONTEXT_PROFILE_CORE);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
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
  if (glewInit() != GLEW_OK) {
    warnx("window_sdl2_run: failed to initialize GLEW");
    goto ko;
  }
  const s8 * version = (const s8 *) glGetString(GL_VERSION);
  if (version) {
    err_write_1("window_sdl2_run: OpenGL Version: ");
    err_puts(version);
  } else {
    err_puts("window_sdl2_run: failed to retrieve OpenGL version");
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
    err_write_1("window_sdl2_run: failed to get display index: ");
    err_puts(SDL_GetError());
    goto ko;
  }
  if (SDL_GetDisplayDPI(display_index, &window->dpi, &window->dpi_w,
                        &window->dpi_h) != 0) {
    err_write_1("window_sdl2_run: failed to get display DPI: ");
    err_puts(SDL_GetError());
    goto ko;
  }
  fprintf(stderr, "window_sdl2_run: dpi=%.2f, dpi_w=%.2f, dpi_h=%.2f\n",
          window->dpi, window->dpi_w, window->dpi_h);
  SDL_GL_SetSwapInterval(1);
  if (! window->load(window)) {
    err_puts("window_sdl2_run: window->load => false");
    goto ko;
  }
  while (! quit) {
    while (SDL_PollEvent(&sdl_event) != 0) {
      switch (sdl_event.type) {
      case SDL_QUIT:
        err_puts("window_sdl2_run: SDL_QUIT");
        quit = 1;
        break;
      case SDL_KEYDOWN:
        if (! window->key(window, &sdl_event.key.keysym)) {
          err_puts("window_sdl2_run: window->key -> false");
          quit = 1;
        }
        break;
      case SDL_MOUSEBUTTONDOWN:
        if (! window->button(window, sdl_event.button.button,
                             sdl_event.button.x, sdl_event.button.y)) {
          err_puts("window_sdl2_run: window->button -> false");
          quit = 1;
        }
        break;
      case SDL_MOUSEMOTION:
        //int relativeX = sdl_event.motion.xrel;
        //int relativeY = sdl_event.motion.yrel;
        if (! window->motion(window, sdl_event.motion.x,
                             sdl_event.motion.y)) {
          err_puts("window_sdl2_run: window->motion -> false");
          quit = 1;
        }
        break;
      case SDL_WINDOWEVENT:
        if (sdl_event.window.event == SDL_WINDOWEVENT_RESIZED) {
          if (! window->resize(window, sdl_event.window.data1,
                               sdl_event.window.data2)) {
            err_puts("window_sdl2_run: window->resize -> false");
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
      err_puts("window_sdl2_run: window->render -> false");
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
