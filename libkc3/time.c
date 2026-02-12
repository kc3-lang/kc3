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
#include <stdlib.h>
#include <time.h>
#include "alloc.h"
#include "assert.h"
#include "buf.h"
#include "buf_inspect.h"
#include "str.h"
#include "sw.h"
#include "sym.h"
#include "tag.h"
#include "time.h"

s_tag * time_access (s_time *time, s_list *key, s_tag *dest)
{
  s_tag *k;
  assert(time);
  assert(key);
  assert(dest);
  if (! key)
    return NULL;
  k = &key->tag;
  if (k->type != TAG_PSYM)
    return NULL;
  if (k->data.psym == &g_sym_tv_sec)
    return tag_init_uw(dest, time->tv_sec);
  if (k->data.psym == &g_sym_tv_nsec)
    return tag_init_uw(dest, time->tv_nsec);
  return NULL;
}

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

s_str * time_diff_to_str (const s_time *time, s_str *dest)
{
  char a[128] = {0};
  s_buf buf = {0};
  uw days;
  uw hours;
  uw milliseconds;
  uw minutes;
  uw months;
  uw seconds;
  uw total_seconds;
  uw years;
  assert(time);
  assert(dest);
  if (time->tv_nsec < 0 || time->tv_nsec > 1000 * 1000 * 1000) {
    err_puts("time_diff_to_str: invalid time");
    assert(! "time_diff_to_str: invalid time");
    return NULL;
  }
  buf_init(&buf, false, sizeof(a) - 1, a);
  if (time->tv_sec >= 0) {
    total_seconds = time->tv_sec;
    milliseconds = time->tv_nsec / (1000 * 1000);
  }
  else {
    buf_write_1(&buf, "-");
    total_seconds = -time->tv_sec;
    if (time->tv_nsec > 0) {
      total_seconds--;
      milliseconds = (1000000000 - time->tv_nsec) / (1000 * 1000);
    }
    else
      milliseconds = time->tv_nsec / (1000 * 1000);
  }
  years = total_seconds / TIME_SECONDS_PER_YEAR;
  total_seconds        %= TIME_SECONDS_PER_YEAR;
  months = total_seconds / TIME_SECONDS_PER_MONTH;
  total_seconds         %= TIME_SECONDS_PER_MONTH;
  days = total_seconds / TIME_SECONDS_PER_DAY;
  total_seconds       %= TIME_SECONDS_PER_DAY;
  hours = total_seconds / TIME_SECONDS_PER_HOUR;
  total_seconds        %= TIME_SECONDS_PER_HOUR;
  minutes = total_seconds / TIME_SECONDS_PER_MINUTE;
  seconds = total_seconds % TIME_SECONDS_PER_MINUTE;
  if (years) {
    buf_inspect_uw_decimal(&buf, years);
    buf_write_1(&buf, "y ");
  }
  if (months) {
    buf_inspect_uw_decimal_pad(&buf, 2, '0', months);
    buf_write_1(&buf, "mo ");
  }
  if (days) {
    buf_inspect_uw_decimal_pad(&buf, 2, '0', days);
    buf_write_1(&buf, "d ");
  }
  if (hours) {
    buf_inspect_uw_decimal_pad(&buf, 2, '0', hours);
    buf_write_1(&buf, ":");
  }
  if (hours || minutes) {
    buf_inspect_uw_decimal_pad(&buf, 2, '0', minutes);
    buf_write_1(&buf, ":");
  }
  buf_inspect_uw_decimal_pad(&buf, 2, '0', seconds);
  buf_write_1(&buf, ".");
  buf_inspect_uw_decimal_pad(&buf, 3, '0', milliseconds);
  if (buf_read_to_str(&buf, dest) <= 0) {
    buf_clean(&buf);
    return NULL;
  }
  buf_clean(&buf);
  return dest;
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

s_time * time_init_cast (s_time *time, const s_sym * const *type,
                         const s_tag *src)
{
  assert(time);
  assert(type);
  assert(*type);
  assert(src);
  (void) type;
  if (src->type == TAG_TIME)
    return time_init_copy(time, &src->data.time);
  err_puts("time_init_cast: cannot cast to Time");
  assert(! "time_init_cast: cannot cast to Time");
  return NULL;
}

s_time * time_init_copy (s_time *time, const s_time *src)
{
  s_time tmp = {0};
  tmp.tv_sec = src->tv_sec;
  tmp.tv_nsec = src->tv_nsec;
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

s_time * time_sub (const s_time *a, const s_time *b, s_time *dest)
{
  s_time tmp = {0};
  assert(a);
  assert(b);
  assert(dest);
  tmp.tv_sec = a->tv_sec - b->tv_sec;
  tmp.tv_nsec = a->tv_nsec - b->tv_nsec;
  while (tmp.tv_nsec < 0) {
    tmp.tv_sec -= 1;
    tmp.tv_nsec += 1000 * 1000 * 1000;
  }
  *dest = tmp;
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
  if (! strftime(a, sizeof(a), "%Y-%m-%d %H:%M:%S", tm)) {
    err_puts("time_to_str: strftime");
    assert(! "time_to_str: strftime");
    return NULL;
  }
  return str_init_copy_1(dest, a);
}
