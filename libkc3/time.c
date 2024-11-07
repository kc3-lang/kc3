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
#include "str.h"
#include "sw.h"
#include "sym.h"
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
  const s_sym *sym_Sw = &g_sym_Sw;
  s_time tmp = {0};
  if (src->tag) {
    if (! sw_init_cast(&tmp.tv_sec, &sym_Sw, src->tag))
      return NULL;
    if (! sw_init_cast(&tmp.tv_nsec, &sym_Sw, src->tag + 1))
      return NULL;
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

s_time * time_init_str (s_time *time, const s_str *src)
{
  struct tm t = {0};
  s_time tmp = {0};
  sscanf(src->ptr.pchar, "%d-%d-%d %d:%d:%d",
         &t.tm_year, &t.tm_mon, &t.tm_mday,
         &t.tm_hour, &t.tm_min, &t.tm_sec);
  t.tm_year -= 1900;
  t.tm_mon -= 1;
  tmp.tv_sec = timegm(&t);
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

s_str * time_to_str (const s_time *time, s_str *dest)
{
  char a[64] = {0};
  time_t t;
  const struct tm *tm;
  t = time->tv_sec;
  if (! (tm = gmtime(&t))) {
    err_puts("time_to_str: gmtime");
    assert(! "time_to_str: gmtime");
    return NULL;
  }
  if (! strftime(a, sizeof(a), "%F %T", tm)) {
    err_puts("time_to_str: strftime");
    assert(! "time_to_str: strftime");
    return NULL;
  }
  return str_init_copy_1(dest, a);
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
