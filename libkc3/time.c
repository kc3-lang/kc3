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
#include <stdlib.h>
#include "alloc.h"
#include "assert.h"
#include "tag.h"
#include "time.h"

s_time * time_allocate (s_time *time)
{
  time->tag = alloc(2 * sizeof(s_tag));
  if (! time->tag)
    return NULL;
  return time;
}

void time_clean (s_time *time)
{
  if (time->tag) {
    tag_clean(time->tag);
    tag_clean(time->tag + 1);
    free(time->tag);
  }
}

s_time * time_init (s_time *time)
{
  s_time tmp = {0};
  *time = tmp;
  return time;
}

s_time * time_init_add (s_time *time, const s_time *a, const s_time *b)
{
  s_time tmp = {0};
  assert(time);
  assert(a);
  assert(b);
  assert(! a->tag);
  assert(! b->tag);
  tmp.tv_sec = a->tv_sec + b->tv_sec;
  tmp.tv_nsec = a->tv_nsec + b->tv_nsec;
  while (tmp.tv_nsec < 0) {
    tmp.tv_sec--;
    tmp.tv_nsec += 1000000000;
  }
  while (tmp.tv_nsec > 1000000000) {
    tmp.tv_sec++;
    tmp.tv_nsec -= 1000000000;
  }
  *time = tmp;
  return time;
}

s_time * time_init_copy (s_time *time, const s_time *src)
{
  s_time tmp = {0};
  if (src->tag) {
    if (! time_allocate(&tmp))
      return NULL;
    if (! tag_init_copy(tmp.tag, src->tag)) {
      free(tmp.tag);
      return NULL;
    }
    if (! tag_init_copy(tmp.tag + 1, src->tag + 1)) {
      tag_clean(tmp.tag);
      free(tmp.tag);
      return NULL;
    }
    *time = tmp;
    return time;
  }
  else {
    tmp.tv_sec = src->tv_sec;
    tmp.tv_nsec = src->tv_nsec;
  }
  *time = tmp;
  return time;
}

s_time * time_init_now (s_time *time)
{
  s_timespec timespec;
  s_time tmp = {0};
  clock_gettime(CLOCK_REALTIME, &timespec);
  tmp.tv_sec = timespec.tv_sec;
  tmp.tv_nsec = timespec.tv_nsec;
  *time = tmp;
  return time;
}

s_timespec * time_sub (const s_timespec *a, const s_timespec *b, s_timespec *dest)
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

f64 * time_to_f64 (const s_timespec *time, f64 *dest)
{
  *dest = (f64) time->tv_sec + (f64) time->tv_nsec * 0.000000001;
  return dest;
}

s_tag * time_to_tag (const s_timespec *time, s_tag *dest)
{
  s_tag tmp = {0};
  assert(time);
  if (! tag_init_tuple(&tmp, 2))
    return NULL;
  tag_init_s64(&tmp.data.tuple.tag[0], time->tv_sec);
  tag_init_s64(&tmp.data.tuple.tag[1], time->tv_nsec);
  *dest = tmp;
  return dest;
}
