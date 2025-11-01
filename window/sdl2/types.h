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
 *  @brief Module KC3.Window.SDL2
 *
 *  Struct for all GUI window SDL2 graphics operations.
 */
#ifndef LIBKC3_WINDOW_SDL2_TYPES_H
#define LIBKC3_WINDOW_SDL2_TYPES_H

#if defined(WIN32) || defined(WIN64)
# include <windef.h>
#endif

#include <SDL.h>
#include <png.h>
#include "../../gl/types.h"
#include "../types.h"

typedef struct window_sdl2 s_window_sdl2;

/* return false to break event loop */
typedef bool (*f_window_sdl2_button) (s_window_sdl2 *window,
                                      u8 button, s64 x, s64 y);

/* return false to break event loop */
typedef bool (*f_window_sdl2_key) (s_window_sdl2 *window,
                                   SDL_Keysym *keysym);

/* return false to break event loop */
typedef bool (*f_window_sdl2_load) (s_window_sdl2 *window);

/* return false to break event loop */
typedef bool (*f_window_sdl2_motion) (s_window_sdl2 *window, s64 x,
                                      s64 y);

/* return false to break event loop */
typedef bool (*f_window_sdl2_render) (s_window_sdl2 *window);

/* return false to break event loop */
typedef bool (*f_window_sdl2_resize) (s_window_sdl2 *window,
                                      u64 w, u64 h);

typedef bool (*f_window_sdl2_sequence_load) (s_sequence *seq,
                                             s_window_sdl2 *window);

typedef bool (*f_window_sdl2_sequence_render) (s_sequence *seq,
                                               s_window_sdl2 *window,
                                               void *context);

typedef void (*f_window_sdl2_unload) (s_window_sdl2 *window);

/* Subtype of s_window, see window/types.h . */
struct window_sdl2 {
  s64                  x;
  s64                  y;
  u64                  w;
  u64                  h;
  u64                  pixel_w;
  u64                  pixel_h;
  bool                 fullscreen;
  f_window_sdl2_button button;
  f_window_sdl2_key    key;
  f_window_sdl2_load   load;
  f_window_sdl2_motion motion;
  f_window_sdl2_render render;
  SDL_Window          *sdl_window;
  f_window_sdl2_resize resize;
  s_sequence          *seq;
  s_sequence          *sequence;
  u32                  sequence_count;
  u32                  sequence_pos;
  s_tag                tag; // TODO: move sequence to tag
  const char          *title;
  f_window_sdl2_unload unload;
  float dpi;
  float dpi_w;
  float dpi_h;
  SDL_GLContext       *context;
};

/* 2 */

struct sdl2_sprite {
  s_gl_object object;
  s_str path;
  s_str real_path;
  u64 total_w;
  u64 total_h;
  u64 dim_x;
  u64 dim_y;
  u64 frame_count;
  u64 w;
  u64 h;
  u64 tex_w;
  u64 tex_h;
  GLuint *texture;
};

#endif /* LIBKC3_WINDOW_SDL2_TYPES_H */
