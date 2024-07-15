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
#include <assert.h>
#include "tag.h"
#include "time.h"

s_time * time_sub (const s_time *a, const s_time *b, s_time *dest)
{
  if ((a->tv_nsec - b->tv_nsec) < 0) {
    dest->tv_sec = a->tv_sec - b->tv_sec - 1;
    dest->tv_nsec = 1000000000 + a->tv_nsec - b->tv_nsec;
  } else {
    dest->tv_sec = a->tv_sec - b->tv_sec;
    dest->tv_nsec = a->tv_nsec - b->tv_nsec;
  }
  return dest;
}

f64 * time_to_f64 (const s_time *time, f64 *dest)
{
  *dest = (f64) time->tv_sec + (f64) time->tv_nsec * 0.000000001;
  return dest;
}

s_tag * time_to_tag (const s_time *time, s_tag *dest)
{
  s_tag tmp;
  assert(time);
  if (! tag_init_tuple(&tmp, 2))
    return NULL;
  tag_init_s64(&tmp.data.tuple.tag[0], time->tv_sec);
  tag_init_s64(&tmp.data.tuple.tag[1], time->tv_nsec);
  *dest = tmp;
  return dest;
}
