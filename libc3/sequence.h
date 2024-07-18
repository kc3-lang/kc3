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
#ifndef LIBC3_SEQUENCE_H
#define LIBC3_SEQUENCE_H

#include "types.h"

/* Stack-allocation compatible functions, call sequence_clean
   after use. */
void         sequence_clean (s_sequence *seq);
s_sequence * sequence_init (s_sequence *sequence, f64 duration,
                            const s8 *title, f_sequence load,
                            f_sequence render, f_sequence unload,
                            void *window);

#endif /* LIBC3_SEQUENCE_H */
