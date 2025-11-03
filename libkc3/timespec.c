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
#include "assert.h"
#include "timespec.h"

#define DEF_TIMESPEC_INIT_F(type)                                     \
  s_timespec * timespec_init_ ## type (s_timespec *time, type src)    \
  {                                                                   \
    s_timespec tmp = {0};                                             \
    tmp.tv_sec = src;                                                 \
    tmp.tv_nsec = src * 1000000000;                                   \
    tmp.tv_nsec %= 1000000000;                                        \
    *time = tmp;                                                      \
    return time;                                                      \
  }

#define DEF_TIMESPEC_INIT_U(type)                                     \
  s_timespec * timespec_init_ ## type (s_timespec *time, type src)    \
  {                                                                   \
    s_timespec tmp = {0};                                             \
    tmp.tv_sec = src;                                                 \
    *time = tmp;                                                      \
    return time;                                                      \
  }

s_timespec * timespec_init_cast (s_timespec *time, p_sym *type,
                                 s_tag *src)
{
  assert(time);
  assert(type);
  assert(*type);
  assert(src);
  (void) type;
  switch (src->type) {
  case TAG_F32:  return timespec_init_f32(time, src->data.f32);
  case TAG_F64:  return timespec_init_f64(time, src->data.f64);
#if HAVE_F80
  case TAG_F80:  return timespec_init_f80(time, src->data.f80);
#endif
#if HAVE_F128
  case TAG_F128: return timespec_init_f128(time, src->data.f128);
#endif
  case TAG_U8:   return timespec_init_u8(time, src->data.u8);
  case TAG_U16:  return timespec_init_u16(time, src->data.u16);
  case TAG_U32:  return timespec_init_u32(time, src->data.u32);
  case TAG_U64:  return timespec_init_u64(time, src->data.u64);
  case TAG_UW:   return timespec_init_uw(time, src->data.uw);
  default:
    break;
  }
  err_puts("timespec_init_cast: cannot cast");
  assert(! "timespec_init_cast: cannot cast");
  return NULL;
}

DEF_TIMESPEC_INIT_F(f32)
DEF_TIMESPEC_INIT_F(f64)
#if HAVE_F80
DEF_TIMESPEC_INIT_F(f80)
#endif
#if HAVE_F128
DEF_TIMESPEC_INIT_F(f128)
#endif

DEF_TIMESPEC_INIT_U(u8)
DEF_TIMESPEC_INIT_U(u16)
DEF_TIMESPEC_INIT_U(u32)
DEF_TIMESPEC_INIT_U(u64)
DEF_TIMESPEC_INIT_U(uw)

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
