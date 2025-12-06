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
#include <assert.h>
#include <stdlib.h>
#include "../../libkc3/io.h"
#include "../../libkc3/tag.h"
#include "../../gl/gl.h"
#include "../window.h"
#include "window_egl.h"

void kc3_window_egl_clean (void)
{
}

void kc3_window_egl_init (void)
{
}

void window_egl_clean (s_window_egl *window)
{
  assert(window);
  if (window->egl_context != EGL_NO_CONTEXT)
    eglDestroyContext(window->egl_display, window->egl_context);
  if (window->egl_surface != EGL_NO_SURFACE)
    eglDestroySurface(window->egl_display, window->egl_surface);
  if (window->egl_display != EGL_NO_DISPLAY)
    eglTerminate(window->egl_display);
  window_clean((s_window *) window);
}

s_window_egl * window_egl_init (s_window_egl *window,
                                s64 x, s64 y, u64 w, u64 h,
                                const char *title,
                                u64 sequence_count)
{
  s_window_egl tmp = {0};
  assert(window);
  title = title ? title : "KC3.Window.EGL";
  window_init((s_window *) &tmp, x, y, w, h, title, sequence_count);
  tmp.button = window_egl_button_default;
  tmp.key    = window_egl_key_default;
  tmp.load   = window_egl_load_default;
  tmp.motion = window_egl_motion_default;
  tmp.render = window_egl_render_default;
  tmp.resize = window_egl_resize_default;
  tmp.pixel_w = w;
  tmp.pixel_h = h;
  tmp.egl_display = EGL_NO_DISPLAY;
  tmp.egl_config = NULL;
  tmp.egl_context = EGL_NO_CONTEXT;
  tmp.egl_surface = EGL_NO_SURFACE;
  *window = tmp;
  return window;
}

bool window_egl_button_default (s_window_egl *window, u8 button,
                                s64 x, s64 y)
{
  assert(window);
  (void) window;
  err_write_1("window_egl_button_default: ");
  err_inspect_u8(button);
  err_write_1(" (");
  err_inspect_s64_decimal(x);
  err_write_1(", ");
  err_inspect_s64_decimal(y);
  err_write_1("\n");
  return true;
}

bool window_egl_key_default (s_window_egl *window, u32 keysym)
{
  // char keysym_name[64];
  assert(window);
  (void) window;
  // xkb_keysym_get_name(keysym, keysym_name, sizeof(keysym_name));
  err_write_1("window_egl_key_default: ");
  err_inspect_u32_decimal(keysym);
  err_write_1("\n");
  // err_puts(keysym_name);
  return true;
}

bool window_egl_load_default (s_window_egl *window)
{
  assert(window);
  (void) window;
  err_puts("window_egl_load_default");
  return true;
}

bool window_egl_motion_default (s_window_egl *window, s64 x, s64 y)
{
  assert(window);
  (void) window;
  (void) x;
  (void) y;
  return true;
}

bool window_egl_render_default (s_window_egl *window)
{
  assert(window);
  glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);
  err_puts("window_egl_render_default");
  return true;
}

bool window_egl_resize_default (s_window_egl *window, u64 w, u64 h)
{
  assert(window);
  (void) window;
  glViewport(0, 0, w, h);
  err_write_1("window_egl_resize_default: ");
  err_inspect_u64(w);
  err_write_1(" x ");
  err_inspect_u64(h);
  err_write_1("\n");
  return true;
}
