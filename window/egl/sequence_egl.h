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
#ifndef LIBKC3_WINDOW_EGL_SEQUENCE_EGL_H
#define LIBKC3_WINDOW_EGL_SEQUENCE_EGL_H

#include "types.h"

/* Stack-allocation compatible functions, call sequence_egl_clean
   after use. */
void             sequence_egl_clean (s_sequence_egl *seq);
s_sequence_egl * sequence_egl_init (s_sequence_egl *sequence,
                                    f64 duration, const char *title,
                                    f_sequence_egl load,
                                    f_sequence_egl render,
                                    f_sequence_egl unload,
                                    s_window_egl *window);

#endif /* LIBKC3_WINDOW_EGL_SEQUENCE_EGL_H */
