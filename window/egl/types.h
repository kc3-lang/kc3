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
/** @file types.h
 *  @brief Module KC3.Window.EGL
 *
 *  Struct for all GUI window with EGL graphics operations.
 */
#ifndef LIBKC3_WINDOW_EGL_TYPES_H
#define LIBKC3_WINDOW_EGL_TYPES_H

#include <EGL/egl.h>
#include "../../gl/types.h"
#include "../types.h"

typedef struct sequence_egl s_sequence_egl;
typedef struct window_egl   s_window_egl;

/* return false to break event loop */
typedef bool (*f_sequence_egl) (s_sequence_egl *seq);
typedef bool (*f_window_egl_button) (s_window_egl *window,
                                     u8 button, s64 x, s64 y);
typedef bool (*f_window_egl_key) (s_window_egl *window, u32 keysym);
typedef bool (*f_window_egl_load) (s_window_egl *window);
typedef bool (*f_window_egl_motion) (s_window_egl *window, s64 x,
                                     s64 y);
typedef bool (*f_window_egl_render) (s_window_egl *window);
typedef bool (*f_window_egl_resize) (s_window_egl *window,
                                     u64 w, u64 h);
typedef void (*f_window_egl_unload) (s_window_egl *window);


/* Subtype of s_sequence. See libkc3/types.h */
struct sequence_egl {
  s_tag tag;
  f64 dt;
  f64 duration;
  u64 frame;
  f64 t;
  s_timespec t0;
  const char *title;
  s_window_egl *window;
  f_sequence load;
  f_sequence render;
  f_sequence unload;
  f_sequence_button button;
  f_sequence_key key;
};

/* Subtype of s_window. See libkc3/window/types.h */
struct window_egl {
  s64                 x;
  s64                 y;
  u64                 w;
  u64                 h;
  u64                 pixel_w;
  u64                 pixel_h;
  bool                fullscreen;
  f_window_egl_button button;
  f_window_egl_key    key;
  f_window_egl_load   load;
  f_window_egl_motion motion;
  f_window_egl_render render;
  EGLContext          egl_context;
  f_window_egl_resize resize;
  s_sequence_egl     *seq;
  s_sequence_egl     *sequence;
  u32                 sequence_count;
  u32                 sequence_pos;
  s_tag               tag; // TODO: move sequence to tag
  const char         *title;
  f_window_egl_unload unload;
  EGLDisplay          egl_display;
  EGLConfig           egl_config;
  EGLSurface          egl_surface;
};

#endif /* LIBKC3_WINDOW_EGL_TYPES_H */
