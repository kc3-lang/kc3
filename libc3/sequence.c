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
#include <assert.h>
#include "sequence.h"

s_sequence * sequence_init (s_sequence *sequence, f64 duration,
                            const s8 *title, f_sequence_load load,
                            f_sequence_render render)
{
  assert(sequence);
  sequence->t = 0.0;
  sequence->duration = duration;
  sequence->title = title;
  sequence->load = load;
  sequence->render = render;
  return sequence;
}
