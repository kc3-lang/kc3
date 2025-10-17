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
#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <xkbcommon/xkbcommon.h>
#include <EGL/egl.h>
#include "../../libkc3/kc3.h"
#include "../window.h"
#include "../../gl/gl_deprecated.h"
#include "window_sdl2.h"

const char *g_gray_3_bits_utf8[] = {
  " ", ".", ":", "#", "░", "▒", "▓", "█"
};

static bool g_window_sdl2_initialized = false;

/* Debug callback not used in GLES 2.0
static void gl_debug (GLenum source, GLenum type, GLuint id,
                      GLenum severity, GLsizei length,
                      const GLchar* message, const void* user_param)
{
  s_env *env;
  (void) source;
  (void) type;
  (void) id;
  (void) severity;
  (void) length;
  (void) user_param;
  env = env_global();
  if (env && env->err) {
    err_write_1("gl_debug_callback: ");
    err_puts(message);
  }
  else
    fprintf(stderr, "gl_debug_callback: %s\n", message);
}
*/

void window_sdl2_clean (s_window_sdl2 *window)
{
  window_clean((s_window *) window);
}

bool window_sdl2_default_button_cb (s_window_sdl2 *window, u8 button,
                                    s64 x, s64 y)
{
  assert(window);
  (void) window;
  (void) button;
  (void) x;
  (void) y;
  err_write_1("window_sdl2_default_button_cb: ");
  err_inspect_u8(button);
  err_write_1(" (");
  err_inspect_s64(x);
  err_write_1(", ");
  err_inspect_s64(y);
  err_puts(")");
  return true;
}

bool window_sdl2_default_key_cb (s_window_sdl2 *window,
                                 SDL_Keysym *keysym)
{
  s32 k;
  assert(window);
  assert(keysym);
  (void) window;
  k = keysym->sym;
  err_write_1("window_sdl2_default_key_cb: ");
  err_inspect_s32_decimal(k);
  err_write_1("\n");
  return true;
}

bool window_sdl2_default_load_cb (s_window_sdl2 *window)
{
  assert(window);
  (void) window;
  err_puts("window_sdl2_default_load_cb");
  return true;
}

bool window_sdl2_default_motion_cb (s_window_sdl2 *window, s64 x, s64 y)
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

bool window_sdl2_default_render_cb (s_window_sdl2 *window)
{
  assert(window);
  (void) window;
  err_puts("window_sdl2_default_render_cb");
  return true;
}

bool window_sdl2_default_resize_cb (s_window_sdl2 *window, u64 w, u64 h)
{
  assert(window);
  (void) window;
  (void) w;
  (void) h;
  err_write_1("window_sdl2_default_resize_cb: ");
  err_inspect_u64_decimal(w);
  err_write_1(" x ");
  err_inspect_u64_decimal(h);
  err_write_1("\n");
  return true;
}

void window_sdl2_default_unload_cb (s_window_sdl2 *window)
{
  assert(window);
  (void) window;
  err_puts("window_sdl2_default_unload_cb");
}

s_window_sdl2 * window_sdl2_init (s_window_sdl2 *window,
                                  s64 x, s64 y, u64 w, u64 h,
                                  const char *title,
                                  u32 sequence_count)
{
  s_window_sdl2 tmp = {0};
  assert(window);
  title = title ? title : "KC3.Window.Sdl2";
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
  int mouse_x, mouse_y;
  int quit = 0;
  SDL_Event sdl_event;
  assert(window);
  if (! g_window_sdl2_initialized) {
    //SDL_SetHint(SDL_HINT_VIDEO_HIGHDPI_DISABLED, "1");
    if (SDL_Init(SDL_INIT_VIDEO)) {
      err_write_1("window_sdl2_run: SDL initialization failed: ");
      err_puts(SDL_GetError());
      return false;
    }
    g_window_sdl2_initialized = true;
  }
  if (SDL_VideoInit(NULL)) {
    err_write_1("window_sdl2_run: SDL_VideoInit failed: ");
    err_puts(SDL_GetError());
    return false;
  }
  SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,
                      SDL_GL_CONTEXT_PROFILE_ES);
  window->sdl_window = SDL_CreateWindow(window->title,
                                window->x, window->y,
                                window->w, window->h,
                                SDL_WINDOW_ALLOW_HIGHDPI |
                                SDL_WINDOW_OPENGL |
                                SDL_WINDOW_RESIZABLE |
				SDL_WINDOW_SHOWN);
  if (! window->sdl_window) {
    warnx("window_sdl2_run: failed to create window: %s",
          SDL_GetError());
    return false;
  }
  SDL_SetWindowBordered(window->sdl_window, SDL_TRUE);
  window->context = SDL_GL_CreateContext(window->sdl_window);
  if (! window->context) {
    fprintf(stderr, "window_sdl2_run:"
            " failed to create OpenGL context: %s\n",
            SDL_GetError());
    return false;
  }
  if (SDL_GL_MakeCurrent(window->sdl_window, window->context)) {
    err_write_1("window_sdl2_run: failed to make OpenGL context"
                " current: ");
    err_write_1(SDL_GetError());
    goto ko;
  }
  GLenum gl_error = glGetError();
  if (gl_error != GL_NO_ERROR) {
    fprintf(stderr, "OpenGL initialization error: %s\n",
          gl_error_string(gl_error));
    goto ko;
  }
  const char * version = (const char *) glGetString(GL_VERSION);
  if (version) {
    err_write_1("window_sdl2_run: OpenGL Version: ");
    err_puts(version);
  } else {
    err_puts("window_sdl2_run: failed to retrieve OpenGL version");
    version = "unknown";
  }
  /* Debug output not available in GLES 2.0
  glEnable(GL_DEBUG_OUTPUT);
  glDebugMessageCallback((GLDEBUGPROC) gl_debug, NULL);
  */
  int gl_w = window->w;
  int gl_h = window->h;
  SDL_GL_GetDrawableSize(window->sdl_window, &gl_w, &gl_h);
  window->pixel_w = gl_w;
  window->pixel_h = gl_h;
  int display_index;
  display_index = SDL_GetWindowDisplayIndex(window->sdl_window);
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
  EGLDisplay egl_display = eglGetCurrentDisplay();
  if (egl_display != EGL_NO_DISPLAY)
    eglSwapInterval(egl_display, 1);
  assert(glGetError() == GL_NO_ERROR);
  if (! window->load(window)) {
    err_puts("window_sdl2_run: window->load => false");
    goto ko;
  }
  assert(glGetError() == GL_NO_ERROR);
  while (! quit) {
    assert(glGetError() == GL_NO_ERROR);
    while (SDL_PollEvent(&sdl_event) != 0) {
      assert(glGetError() == GL_NO_ERROR);
      switch (sdl_event.type) {
      case SDL_QUIT:
        err_puts("window_sdl2_run: SDL_QUIT");
        quit = 1;
        break;
      case SDL_KEYDOWN:
        if (! window->key(window, &sdl_event.key.keysym)) {
          err_puts("window_sdl2_run: window->key => false");
          quit = 1;
        }
        break;
      case SDL_MOUSEBUTTONDOWN:
        if (! window->button(window, sdl_event.button.button,
                             sdl_event.button.x,
                             window->h - sdl_event.button.y)) {
          err_puts("window_sdl2_run: window->button => false");
          quit = 1;
        }
        break;
      case SDL_MOUSEWHEEL:
        SDL_GetMouseState(&mouse_x, &mouse_y);
        if (! window->button(window, sdl_event.wheel.y > 0 ? 4 : 5,
                             mouse_x, window->h - mouse_y)) {
          err_puts("window_sdl2_run: window->button => false");
          quit = 1;
        }
        break;
      case SDL_MOUSEMOTION:
        //int relativeX = sdl_event.motion.xrel;
        //int relativeY = sdl_event.motion.yrel;
        if (! window->motion(window, sdl_event.motion.x,
                             window->h - sdl_event.motion.y)) {
          err_puts("window_sdl2_run: window->motion => false");
          quit = 1;
        }
        break;
      case SDL_WINDOWEVENT:
        if (sdl_event.window.event == SDL_WINDOWEVENT_RESIZED) {
          if (! window->resize(window, sdl_event.window.data1,
                               sdl_event.window.data2)) {
            err_puts("window_sdl2_run: window->resize => false");
            quit = 1;
          }
          assert(glGetError() == GL_NO_ERROR);
          window->w = sdl_event.window.data1;
          window->h = sdl_event.window.data2;
          SDL_GL_GetDrawableSize(window->sdl_window, &gl_w, &gl_h);
          assert(glGetError() == GL_NO_ERROR);
          window->pixel_w = gl_w;
          window->pixel_h = gl_h;
          glViewport(0, 0, gl_w, gl_h);
        }
        break;
      default:
        break;
      }
    }
    assert(glGetError() == GL_NO_ERROR);
    //glDrawBuffer(GL_BACK);
    if (SDL_GL_MakeCurrent(window->sdl_window, window->context)) {
      fprintf(stderr, "window_sdl2_run:"
              " failed to make OpenGL context current: %s\n",
              SDL_GetError());
      return false;
    }
    if (! window->render(window)) {
      err_puts("window_sdl2_run: window->render => false");
      quit = 1;
    }
    glFinish();
    SDL_GL_SwapWindow(window->sdl_window);
  }
  SDL_GL_DeleteContext(window->context);
  SDL_DestroyWindow(window->sdl_window);
  return true;
 ko:
  SDL_GL_DeleteContext(window->context);
  SDL_DestroyWindow(window->sdl_window);
  return false;
}
