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
#include <assert.h>
#include "sequence.h"
#include "tag.h"

void sequence_clean (s_sequence *seq)
{
  tag_clean(&seq->tag);
}

s_sequence * sequence_init (s_sequence *seq, f64 duration,
                            const char *title, f_sequence load,
                            f_sequence render, f_sequence unload,
                            void *window)
{
  s_sequence tmp = {0};
  assert(seq);
  tmp.dt = 0.0;
  tmp.t = 0.0;
  tmp.duration = duration;
  tag_init_void(&tmp.tag);
  tmp.title = title;
  tmp.load = load;
  tmp.render = render;
  tmp.unload = unload;
  tmp.window = window;
  *seq = tmp;
  return seq;
}
