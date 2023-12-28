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
#include "tag.h"

void sequence_clean (s_sequence *seq)
{
  tag_clean(&seq->tag);
}

s_sequence * sequence_init (s_sequence *seq, f64 duration,
                            const s8 *title, f_sequence load,
                            f_sequence render, f_sequence unload)
{
  assert(seq);
  seq->dt = 0.0;
  seq->t = 0.0;
  seq->duration = duration;
  seq->title = title;
  seq->load = load;
  seq->render = render;
  seq->unload = unload;
  tag_init_void(&seq->tag);
  return seq;
}
