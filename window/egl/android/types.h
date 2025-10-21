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
 *  @brief Module KC3.Window.EGL.Android
 *
 *  Struct for an Android window with EGL graphics operations.
 */
#ifndef LIBKC3_WINDOW_EGL_ANDROID_TYPES_H
#define LIBKC3_WINDOW_EGL_ANDROID_TYPES_H

#include <android/native_activity.h>
#include <android/native_window.h>
#include "native_app_glue/android_native_app_glue.h"
#include "../types.h"

/* Struct types. */
typedef struct window_egl_android   s_window_egl_android;

/* Pointer types. */
typedef struct android_app *p_android_app;

/* Function types. */
/* return false to break event loop */
typedef bool (*f_window_egl_android_button)
(s_window_egl_android *window, u8 button, s64 x, s64 y);
typedef bool (*f_window_egl_android_key) (s_window_egl_android *window,
                                  u32 keysym);
typedef bool (*f_window_egl_android_load) (s_window_egl_android *window);
typedef bool (*f_window_egl_android_motion) (s_window_egl_android *window, s64 x,
                                     s64 y);
typedef bool (*f_window_egl_android_render) (s_window_egl_android *window);
typedef bool (*f_window_egl_android_resize) (s_window_egl_android *window,
                                     u64 w, u64 h);
typedef void (*f_window_egl_android_unload) (s_window_egl_android *window);

/* Subtype of s_sequence. See libkc3/types.h */
struct sequence_egl_android {
  s_tag tag;
  f64 dt;
  f64 duration;
  u64 frame;
  f64 t;
  s_timespec t0;
  const char *title;
  s_window_egl_android *window;
  f_sequence load;
  f_sequence render;
  f_sequence unload;
  f_sequence_button button;
  f_sequence_key key;
};

/* Subtype of s_window. See libkc3/window/types.h */
struct window_egl_android {
  s64                         x;
  s64                         y;
  u64                         w;
  u64                         h;
  u64                         pixel_w;
  u64                         pixel_h;
  bool                        fullscreen;
  f_window_egl_android_button button;
  f_window_egl_android_key    key;
  f_window_egl_android_load   load;
  f_window_egl_android_motion motion;
  f_window_egl_android_render render;
  EGLContext                  egl_context;
  f_window_egl_android_resize resize;
  s_sequence                 *seq;
  s_sequence                 *sequence;
  u32                         sequence_count;
  u32                         sequence_pos;
  s_tag                       tag;
  const char                 *title;
  f_window_egl_android_unload unload;
  EGLDisplay                  egl_display;
  EGLConfig                   egl_config;
  EGLSurface                  egl_surface;
  p_android_app               app;
};

#endif /* LIBKC3_WINDOW_EGL_ANDROID_TYPES_H */
