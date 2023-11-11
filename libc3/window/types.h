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
typedef bool (*f_window_render) (s_window *window,
                                 void *render_context);

/* return false to break event loop */
typedef bool (*f_window_resize) (s_window *window,
                                 uw w, uw h);

struct window {
  sw x;
  sw y;
  uw w;
  uw h;
  const s8 *title;
  f_window_render render;
  f_window_resize resize;
  void *render_context;
};

#endif /* LIBC3_WINDOW_TYPES_H */
