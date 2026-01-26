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
#ifndef LIBKC3_WINDOW_EGL_H
#define LIBKC3_WINDOW_EGL_H

#include "types.h"

/* Library functions, call kc3_window_egl_init at program start,
   and kc3_window_egl_clean at program end. */
void kc3_window_egl_clean (void);
void kc3_window_egl_init (void);

/* Stack-allocation compatible functions, call window_egl_clean
   after use. */
void           window_egl_clean (s_window_egl *window);
s_window_egl * window_egl_init (s_window_egl *window,
                                s64 x, s64 y, u64 w, u64 h,
                                const char *title,
                                u64 sequence_count);
bool           window_egl_run (s_window_egl *window);

/* Callbacks. */
bool window_egl_button_default (s_window_egl *window, u8 button,
                                s64 x, s64 y);
bool window_egl_key_default (s_window_egl *window, u32 keysym);
bool window_egl_load_default (s_window_egl *window);
bool window_egl_motion_default (s_window_egl *window, s64 x, s64 y);
bool window_egl_render_default (s_window_egl *window);
bool window_egl_resize_default (s_window_egl *window, u64 w, u64 h);

#endif /* LIBKC3_WINDOW_EGL_H */
