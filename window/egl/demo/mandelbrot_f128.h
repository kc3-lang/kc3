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
#ifndef MANDELBROT_F128_H
#define MANDELBROT_F128_H

#include "../types.h"

u8 mandelbrot_f128_button (s_sequence_egl *seq, u8 button, s64 x,
                           s64 y);
u8 mandelbrot_f128_load (s_sequence_egl *seq);
u8 mandelbrot_f128_render (s_sequence_egl *seq);
u8 mandelbrot_f128_unload (s_sequence_egl *seq);

#endif /* MANDELBROT_F128_H */
