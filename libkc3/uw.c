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
/* Gen from u.h.in BITS=W bits=w */
#include "assert.h"
#include "buf.h"
#include "buf_parse_uw.h"
#include <math.h>
#include <stdlib.h>
#include "f128.h"
#include "integer.h"
#include "ratio.h"
#include "sym.h"
#include "str.h"
#include "tag.h"
#include "uw.h"

uw * uw_init_1 (uw *u, const char *p)
{
  s_str str;
  str_init_1(&str, NULL, p);
  return uw_init_str(u, &str);
}

uw * uw_init_cast
(uw *u, const s_sym * const *type, const s_tag *tag)
{
  (void) type;
  switch (tag->type) {
  case TAG_BOOL:
    *u = tag->data.bool ? 1 : 0;
    return u;
  case TAG_CHARACTER:
    *u = (uw) tag->data.character;
    return u;
  case TAG_F32:
    *u = (uw) tag->data.f32;
    return u;
  case TAG_F64:
    *u = (uw) tag->data.f64;
    return u;
  case TAG_INTEGER:
    *u = integer_to_uw(&tag->data.integer);
    return u;
  case TAG_RATIO:
    *u = ratio_to_uw(&tag->data.ratio);
    return u;
  case TAG_S8:
    *u = (uw) tag->data.s8;
    return u;
  case TAG_S16:
    *u = (uw) tag->data.s16;
    return u;
  case TAG_S32:
    *u = (uw) tag->data.s32;
    return u;
  case TAG_S64:
    *u = (uw) tag->data.s64;
    return u;
  case TAG_STR:
    return uw_init_str(u, &tag->data.str);
  case TAG_SW:
    *u = (uw) tag->data.sw;
    return u;
  case TAG_U8:
    *u = (uw) tag->data.u8;
    return u;
  case TAG_U16:
    *u = (uw) tag->data.u16;
    return u;
  case TAG_U32:
    *u = (uw) tag->data.u32;
    return u;
  case TAG_U64:
    *u = (uw) tag->data.u64;
    return u;
  case TAG_UW:
    *u = (uw) tag->data.uw;
    return u;
  default:
    break;
  }
  err_write_1("uw_cast: cannot cast ");
  err_write_1(tag_type_to_string(tag->type));
  if (*type == &g_sym_Uw)
    err_puts(" to Uw");
  else {
    err_write_1(" to ");
    err_inspect_sym(type);
    err_puts(" aka Uw");
  }
  assert(! "uw_cast: cannot cast to Uw");
  return NULL;
}

uw * uw_init_copy (uw *u, const uw *src)
{
  assert(src);
  assert(u);
  *u = *src;
  return u;
}

uw * uw_init_str (uw *u, const s_str *str)
{
  s_buf buf;
  uw tmp = 0;
  buf_init_str_const(&buf, str);
  if (buf_parse_uw(&buf, &tmp) <= 0) {
    if (false) {
      err_puts("uw_init_str: buf_parse_uw");
      err_inspect_stacktrace(g_kc3_env->stacktrace);
      err_write_1("\n");
      assert(! "uw_init_str: buf_parse_uw");
    }
    return NULL;
  }
  *u = tmp;
  return u;
}

uw * uw_random (uw *u)
{
  arc4random_buf(u, sizeof(uw));
  return u;
}

#if w == 64 || w == w

uw * uw_random_uniform (uw *u, uw max)
{
  f128 x;
  f128_random(&x);
  x *= max;
  *u = (uw) x;
  return u;
}

#else

uw * uw_random_uniform (uw *u, uw max)
{
  *u = arc4random_uniform(max);
  return u;
}

#endif

s_tag * uw_sqrt (const uw x, s_tag *dest)
{
  assert(dest);
  dest->type = TAG_F128;
  dest->data.f128 = sqrtl((long double) x);
  return dest;
}
