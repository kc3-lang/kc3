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
 *  @brief Module C3.Window
 *
 *  Base struct for all GUI window operations.
 */
#ifndef LIBC3_WINDOW_TYPES_H
#define LIBC3_WINDOW_TYPES_H

#include <libc3/types.h>

typedef struct window s_window;

/* return false to break event loop */
typedef bool (*f_window_button) (s_window *window, u8 button,
                                 uw x, uw y);

/* return false to break event loop */
typedef bool (*f_window_key) (s_window *window, uw key);

/* return false to break event loop */
typedef bool (*f_window_load) (s_window *window);

/* return false to break event loop */
typedef bool (*f_window_motion) (s_window *window, sw x, sw y);

/* return false to break event loop */
typedef bool (*f_window_render) (s_window *window,
                                 void *render_context);

/* return false to break event loop */
typedef bool (*f_window_resize) (s_window *window,
                                 uw w, uw h);

/* To subtype make a copy of this struct while replacing
 * pointer types (including pointer to function types).
 * E.g. see libc3/window/cairo/types.h
 */
struct window {
  sw              x;
  sw              y;
  uw              w;
  uw              h;
  f_window_button button;
  f_window_key    key;
  f_window_load   load;
  f_window_motion motion;
  f_window_render render;
  void           *render_context;
  f_window_resize resize;
  s_sequence     *sequence;
  uw              sequence_count;
  uw              sequence_pos;
  const s8       *title;
};

#endif /* LIBC3_WINDOW_TYPES_H */
