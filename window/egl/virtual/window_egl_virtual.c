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
#include <string.h>
#include "../../../libkc3/kc3.h"
#include "window_egl_virtual.h"

static void virtual_context_clean (s_virtual_context *ctx);
static bool virtual_context_init (s_virtual_context *ctx,
                                   s_window_egl *window);
static bool window_egl_virtual_setup (s_window_egl *window,
                                      s_virtual_context *ctx);

static void virtual_context_clean (s_virtual_context *ctx)
{
  free(ctx->buffer);
}

static s_virtual_context
virtual_context_init (s_virtual_context *ctx,
                      s_window_egl *window)
{
  size_t buffer_size;
  assert(ctx);
  assert(window);
  if (! (ctx->buffer = alloc(window->w * window->h * 4)))
    return NULL;
  return true;
}

void window_egl_virtual_clean (s_window_egl *window)
{
  assert(window);
  if (window->egl_context != EGL_NO_CONTEXT)
    eglDestroyContext(window->egl_display, window->egl_context);
  if (window->egl_surface != EGL_NO_SURFACE)
    eglDestroySurface(window->egl_display, window->egl_surface);
  if (window->egl_display != EGL_NO_DISPLAY)
    eglTerminate(window->egl_display);
  virtual_context_clean(&ctx);
}

bool window_egl_virtual_init (s_window_egl *window, uw w, uw h)
{
  s_virtual_context ctx = {0};
  assert(window);
  if (! virtual_context_init(&ctx, window)) {
    err_puts("window_egl_virtual_run: virtual_context_init");
    return false;
  }
  if (! window_egl_virtual_setup(window, &ctx)) {
    err_puts("window_egl_virtual_run: window_egl_virtual_setup");
    goto ko_ctx;
  }
  if (! window->load(window))
    goto ko_ctx;
}

bool window_egl_virtual_render (s_window_egl *window)
{
  if (! window->render(window))
    return false;
  glReadPixels(0, 0, window->w, window->h, GL_RGBA,
               GL_UNSIGNED_BYTE, ctx.buffer);
  eglSwapBuffers(window->egl_display, window->egl_surface);
}

static bool window_egl_virtual_setup (s_window_egl *window,
                                      s_virtual_context *ctx)
{
  EGLint major, minor;
  EGLConfig config;
  EGLint config_attribs[] = {
    EGL_SURFACE_TYPE, EGL_PBUFFER_BIT,
    EGL_RED_SIZE, 8,
    EGL_GREEN_SIZE, 8,
    EGL_BLUE_SIZE, 8,
    EGL_ALPHA_SIZE, 8,
    EGL_RENDERABLE_TYPE, EGL_OPENGL_ES3_BIT,
    EGL_NONE
  };
  EGLint context_attribs[] = {
    EGL_CONTEXT_MAJOR_VERSION, 3,
    EGL_CONTEXT_MINOR_VERSION, 0,
    EGL_NONE
  };
  EGLint pbuffer_attribs[] = {
    EGL_WIDTH, (EGLint) window->w,
    EGL_HEIGHT, (EGLint) window->h,
    EGL_NONE
  };
  EGLint num_configs;
  (void) ctx;
  window->egl_display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
  if (window->egl_display == EGL_NO_DISPLAY) {
    err_puts("window_egl_virtual_setup: eglGetDisplay");
    return false;
  }
  if (! eglInitialize(window->egl_display, &major, &minor)) {
    err_puts("window_egl_virtual_setup: eglInitialize");
    return false;
  }
  err_write_1("EGL Version: ");
  err_inspect_s32_decimal(major);
  err_write_1(".");
  err_inspect_s32_decimal(minor);
  err_write_1("\n");
  if (! eglBindAPI(EGL_OPENGL_ES_API)) {
    EGLint error = eglGetError();
    err_write_1("window_egl_virtual_setup: eglBindAPI: 0x");
    err_inspect_u32_hexadecimal((u32) error);
    err_write_1("\n");
    return false;
  }
  if (! eglChooseConfig(window->egl_display, config_attribs,
                       &config, 1, &num_configs)) {
    err_puts("window_egl_virtual_setup: eglChooseConfig");
    return false;
  }
  if (num_configs == 0) {
    err_puts("window_egl_virtual_setup: no EGL config");
    return false;
  }
  window->egl_config = config;
  window->egl_surface = eglCreatePbufferSurface
    (window->egl_display, config, pbuffer_attribs);
  if (window->egl_surface == EGL_NO_SURFACE) {
    EGLint error = eglGetError();
    err_write_1("window_egl_virtual_setup: "
                "eglCreatePbufferSurface: 0x");
    err_inspect_u32_hexadecimal((u32) error);
    err_write_1("\n");
    return false;
  }
  window->egl_context = eglCreateContext(window->egl_display,
                                         config,
                                         EGL_NO_CONTEXT,
                                         context_attribs);
  if (window->egl_context == EGL_NO_CONTEXT) {
    err_puts("window_egl_virtual_setup: eglCreateContext");
    return false;
  }
  if (!eglMakeCurrent(window->egl_display, window->egl_surface,
                      window->egl_surface, window->egl_context)) {
    err_puts("window_egl_virtual_setup: eglMakeCurrent");
    return false;
  }
  window->pixel_w = window->w;
  window->pixel_h = window->h;
  return true;
}
