/* kc3
 * Copyright 2022,2023,2024 kmx.io <contact@kmx.io>
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
#include "window.h"

bool window_animate (s_window *window)
{
  s_timespec clock_monotonic;
  s_timespec delta;
  s_sequence *seq;
  f64 t;
  if (clock_gettime(CLOCK_MONOTONIC, &clock_monotonic)) {
    err_puts("window_animate: clock_gettime");
    return false;
  }
  if (window->sequence_pos >= window->sequence_count) {
    err_puts("window_animate: window->sequence_pos >="
             " window->sequence_count");
    return false;
  }
  seq = window->sequence + window->sequence_pos;
  time_sub(&clock_monotonic, &seq->t0, &delta);
  time_to_f64(&delta, &t);
  seq->dt = t - seq->t;
  seq->frame++;
  seq->t = t;
  /* printf("window_animate: %f\n", t); */
  if (t > seq->duration &&
      ! window_set_sequence_pos(window, (window->sequence_pos + 1) %
                                window->sequence_count))
    return false;
  return true;
}

void window_clean (s_window *window)
{
  uw i;
  assert(window);
  i = 0;
  if (window->seq && window->seq->unload)
    window->seq->unload(window->seq);
  while (i < window->sequence_count) {
    sequence_clean(window->sequence + i);
    i++;
  }
  free(window->sequence);
  tag_clean(&window->tag);
  window->unload(window);
}

s_window * window_init (s_window *window,
                        sw x, sw y, uw w, uw h,
                        const char *title,
                        uw sequence_count)
{
  s_window tmp = {0};
  assert(window);
  tmp.x = x;
  tmp.y = y;
  tmp.w = w;
  tmp.h = h;
  tmp.sequence = calloc(sequence_count, sizeof(s_sequence));
  tmp.sequence_count = sequence_count;
  tmp.sequence_pos = 0;
  tag_init_void(&tmp.tag);
  tmp.title = title ? title : "KC3.Window";
  *window = tmp;
  return window;
}

bool window_set_sequence_pos (s_window *window, uw sequence_pos)
{
  s_sequence *seq;
  assert(window);
  printf("window_set_sequence_pos %lu\n", sequence_pos);
  if (sequence_pos >= window->sequence_count)
    return false;
  seq = window->sequence + sequence_pos;
  seq->dt = 0.0;
  seq->frame = 0;
  seq->t = 0.0;
  if (clock_gettime(CLOCK_MONOTONIC, &seq->t0)) {
    err_puts("window_set_sequence_pos: clock_gettime");
    return false;
  }
  window->sequence_pos = sequence_pos;
  if (window->seq)
    if (! window->seq->unload(window->seq))
      return false;
  window->seq = seq;
  io_puts(seq->title);
  if (! seq->load(seq))
    return false;
  return true;
}
