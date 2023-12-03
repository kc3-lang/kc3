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
/** @file types.h
 *  @brief Module C3.Window.Sdl2
 *
 *  Struct for all GUI window Sdl2 graphics operations.
 */
#ifndef LIBC3_WINDOW_SDL2_TYPES_H
#define LIBC3_WINDOW_SDL2_TYPES_H

#include <SDL.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <FTGL/ftgl.h>
#include <libc3/types.h>
#include "../types.h"

typedef struct sdl2_font   s_sdl2_font;
typedef struct sdl2_sprite s_sdl2_sprite;
typedef struct rgb          s_rgb;
typedef struct rgba         s_rgba;
typedef struct window_sdl2 s_window_sdl2;

/* return false to break event loop */
typedef bool (*f_window_sdl2_button) (s_window_sdl2 *window,
                                       u8 button, sw x, sw y);

/* return false to break event loop */
typedef bool (*f_window_sdl2_key) (s_window_sdl2 *window,
                                   SDL_Keysym *keysym);

/* return false to break event loop */
typedef bool (*f_window_sdl2_load) (s_window_sdl2 *window);

/* return false to break event loop */
typedef bool (*f_window_sdl2_motion) (s_window_sdl2 *window, sw x,
                                       sw y);

/* return false to break event loop */
typedef bool (*f_window_sdl2_render) (s_window_sdl2 *window,
                                      void *context);

/* return false to break event loop */
typedef bool (*f_window_sdl2_resize) (s_window_sdl2 *window,
                                       uw w, uw h);

typedef bool (*f_window_sdl2_sequence_load) (s_sequence *seq,
                                             s_window_sdl2 *window);

typedef bool (*f_window_sdl2_sequence_render) (s_sequence *seq,
                                               s_window_sdl2 *window,
                                               void *context);

struct sdl2_font {
  FTGLfont *ftgl_font;
  u32 size;
  u32 resolution;
  s_str path;
  s_str real_path;
};

struct rgb {
  double r;
  double g;
  double b;
};

struct rgba {
  double r;
  double g;
  double b;
  double a;
};

/* Subtype of s_window. See libc3/window/types.h */
struct window_sdl2 {
  sw                   x;
  sw                   y;
  uw                   w;
  uw                   h;
  bool                 fullscreen;
  f_window_sdl2_button button;
  f_window_sdl2_key    key;
  f_window_sdl2_load   load;
  f_window_sdl2_motion motion;
  f_window_sdl2_render render;
  SDL_Window          *sdl_window;
  f_window_sdl2_resize resize;
  s_sequence          *sequence;
  uw                   sequence_count;
  uw                   sequence_pos;
  const s8            *title;
  uw gl_w;
  uw gl_h;
  float dpi;
  float dpi_w;
  float dpi_h;
};

#endif /* LIBC3_WINDOW_SDL2_TYPES_H */
