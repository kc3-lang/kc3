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
#include <GL/gl.h>
#include <libkc3/kc3.h>
#include "../../../window.h"
#include "../../window_egl.h"
#include "../../sequence.h"
#include "../window_egl_xcb.h"
#include "bg_rect.h"

#define WINDOW_EGL_DEMO_SEQUENCE_COUNT 1

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
      ! window->seq->button(window->seq, button, x, y))
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
  case 0xff1b:
    return false;
  }
  if (window->seq && window->seq->key &&
      ! window->seq->key(window->seq, keysym))
    return false;
  return true;
}

bool window_egl_demo_load (s_window_egl *window)
{
  assert(window);
  sequence_egl_init(window->sequence, 8.0, "01. Background rect",
                    bg_rect_load, bg_rect_render, bg_rect_unload, window);
  window_set_sequence_pos((s_window *) window, 0);
  return true;
}

bool window_egl_demo_render (s_window_egl *window)
{
  s_sequence_egl *seq;
  assert(window);
  if (! window_animate((s_window *) window)) {
    err_puts("window_egl_demo_render: window_animate");
    return false;
  }
  seq = window->seq;
  if (seq && seq->render && ! seq->render(seq)) {
    err_puts("window_egl_demo_render: seq->render");
    return false;
  }
  return true;
}

bool window_egl_demo_resize (s_window_egl *window, u64 w, u64 h)
{
  assert(window);
  (void) window;
  err_write_1("window_egl_demo_resize: ");
  err_inspect_u64(w);
  err_write_1(" x ");
  err_inspect_u64(h);
  err_write_1("\n");
  glViewport(0, 0, w, h);
  return true;
}

void window_egl_demo_unload (s_window_egl *window)
{
  assert(window);
  (void) window;
  err_puts("window_egl_demo_unload");
}
