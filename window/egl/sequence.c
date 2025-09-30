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
#include <libkc3/kc3.h>
#include "sequence.h"

void sequence_egl_clean (s_sequence_egl *seq)
{
  assert(seq);
  sequence_clean((s_sequence *) seq);
}

s_sequence_egl * sequence_egl_init (s_sequence_egl *sequence,
                                    f64 duration,
                                    const char *title,
                                    f_sequence load,
                                    f_sequence render,
                                    f_sequence unload,
                                    s_window_egl *window)
{
  s_sequence_egl tmp = {0};
  assert(sequence);
  sequence_init((s_sequence *) &tmp, duration, title,
                load, render, unload, (s_window *) window);
  *sequence = tmp;
  return sequence;
}
