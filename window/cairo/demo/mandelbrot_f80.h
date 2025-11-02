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
#ifndef MANDELBROT_F80_H
#define MANDELBROT_F80_H

#include "../types.h"

u8 mandelbrot_f80_button (s_sequence *seq, u8 button, s64 x, s64 y);
u8 mandelbrot_f80_load (s_sequence *seq);
u8 mandelbrot_f80_render (s_sequence *seq);
u8 mandelbrot_f80_unload (s_sequence *seq);

#endif /* MANDELBROT_F80_H */
