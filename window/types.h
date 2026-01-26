/* kc3
 * Copyright from 2022 to 2026 kmx.io <contact@kmx.io>
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
 *  @brief Module C3.Window
 *
 *  Base struct for all GUI window operations.
 */
#ifndef LIBKC3_WINDOW_TYPES_H
#define LIBKC3_WINDOW_TYPES_H

#include "../libkc3/types.h"

typedef struct window s_window;

/* return false to break event loop */
typedef bool (*f_window_button) (s_window *window, u8 button, s64 x,
                                 s64 y);
typedef bool (*f_window_key) (s_window *window, u32 keysym);
typedef bool (*f_window_load) (s_window *window);
typedef bool (*f_window_motion) (s_window *window, s64 x, s64 y);
typedef bool (*f_window_render) (s_window *window);
typedef bool (*f_window_resize) (s_window *window, u64 w, u64 h);

typedef void (*f_window_unload) (s_window *window);

/* To subtype make a copy of this struct while replacing
 * pointer types (including pointer to function types).
 * E.g. see window/egl/types.h */
struct window {
  s64             x;
  s64             y;
  u64             w;
  u64             h;
  u64             pixel_w;
  u64             pixel_h;
  bool            fullscreen;
  f_window_button button;
  f_window_key    key;
  f_window_load   load;
  f_window_motion motion;
  f_window_render render;
  void           *context;
  f_window_resize resize;
  s_sequence     *seq;
  s_sequence     *sequence;
  u32             sequence_count;
  u32             sequence_pos;
  s_tag           tag;
  const char     *title;
  f_window_unload unload;
};

#endif /* LIBKC3_WINDOW_TYPES_H */
