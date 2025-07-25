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
#include "timespec.h"

s_timespec * timespec_sub (const s_timespec *a, const s_timespec *b,
                           s_timespec *dest)
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

f64 * timespec_to_f64 (const s_timespec *time, f64 *dest)
{
  *dest = (f64) time->tv_sec + (f64) time->tv_nsec * 0.000000001;
  return dest;
}
