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
#include <err.h>
#include <libc3/c3.h>
#include "window.h"

bool window_animate (s_window *window)
{
  s_time clock_monotonic;
  s_time delta;
  s_sequence *seq;
  f64 t;
  if (clock_gettime(CLOCK_MONOTONIC, &clock_monotonic)) {
    warn("window_animate");
    return false;
  }
  if (window->sequence_pos >= window->sequence_count) {
    warnx("window_animate: window->sequence_pos >="
          " window->sequence_count");
    return false;
  }
  seq = window->sequence + window->sequence_pos;
  time_sub(&clock_monotonic, &seq->t0, &delta);
  time_to_f64(&delta, &t);
  seq->dt = t - seq->t;
  seq->t = t;
  /* printf("window_animate: %f\n", t); */
  if (t > seq->duration &&
      ! window_set_sequence_pos(window, (window->sequence_pos + 1) %
                                window->sequence_count))
    return false;
  return true;
}

bool window_set_sequence_pos (s_window *window, uw sequence_pos)
{
  s_sequence *seq;
  assert(window);
  printf("window_set_sequence_pos %lu\n", sequence_pos);
  if (sequence_pos >= window->sequence_count)
    return false;
  seq = window->sequence + sequence_pos;
  seq->t = 0.0;
  if (clock_gettime(CLOCK_MONOTONIC, &seq->t0)) {
    warn("window_set_sequence_pos");
    return false;
  }
  window->sequence_pos = sequence_pos;
  printf("%s\n", seq->title);
  if (! seq->load(seq))
    return false;
  return true;
}
