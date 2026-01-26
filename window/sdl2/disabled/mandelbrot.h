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
#ifndef MANDELBROT_H
#define MANDELBROT_H

#include "../types.h"

bool mandelbrot_load (s_sequence *seq, s_window_sdl2 *window);
bool mandelbrot_render (s_sequence *seq, s_window_sdl2 *window,
                        void *context);

#endif /* MANDELBROT_H */
