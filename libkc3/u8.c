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
/* Gen from u.h.in BITS=8 bits=8 */
#include "assert.h"
#include "buf.h"
#include "buf_parse_u8.h"
#include <math.h>
#include <stdlib.h>
#include "f128.h"
#include "integer.h"
#include "ratio.h"
#include "sym.h"
#include "str.h"
#include "tag.h"
#include "u8.h"

u8 * u8_init_1 (u8 *u, const char *p)
{
  s_str str;
  str_init_1(&str, NULL, p);
  return u8_init_str(u, &str);
}

u8 * u8_init_cast
(u8 *u, const s_sym * const *type, const s_tag *tag)
{
  (void) type;
  switch (tag->type) {
  case TAG_BOOL:
    *u = tag->data.bool ? 1 : 0;
    return u;
  case TAG_CHARACTER:
    *u = (u8) tag->data.character;
    return u;
  case TAG_F32:
    *u = (u8) tag->data.f32;
    return u;
  case TAG_F64:
    *u = (u8) tag->data.f64;
    return u;
  case TAG_INTEGER:
    *u = integer_to_u8(&tag->data.integer);
    return u;
  case TAG_RATIO:
    *u = ratio_to_u8(&tag->data.ratio);
    return u;
  case TAG_S8:
    *u = (u8) tag->data.s8;
    return u;
  case TAG_S16:
    *u = (u8) tag->data.s16;
    return u;
  case TAG_S32:
    *u = (u8) tag->data.s32;
    return u;
  case TAG_S64:
    *u = (u8) tag->data.s64;
    return u;
  case TAG_STR:
    return u8_init_str(u, &tag->data.str);
  case TAG_SW:
    *u = (u8) tag->data.sw;
    return u;
  case TAG_U8:
    *u = (u8) tag->data.u8;
    return u;
  case TAG_U16:
    *u = (u8) tag->data.u16;
    return u;
  case TAG_U32:
    *u = (u8) tag->data.u32;
    return u;
  case TAG_U64:
    *u = (u8) tag->data.u64;
    return u;
  case TAG_UW:
    *u = (u8) tag->data.uw;
    return u;
  default:
    break;
  }
  err_write_1("u8_cast: cannot cast ");
  err_write_1(tag_type_to_string(tag->type));
  if (*type == &g_sym_U8)
    err_puts(" to U8");
  else {
    err_write_1(" to ");
    err_inspect_sym(type);
    err_puts(" aka U8");
  }
  assert(! "u8_cast: cannot cast to U8");
  return NULL;
}

u8 * u8_init_copy (u8 *u, const u8 *src)
{
  assert(src);
  assert(u);
  *u = *src;
  return u;
}

u8 * u8_init_str (u8 *u, const s_str *str)
{
  s_buf buf;
  u8 tmp = 0;
  buf_init_str_const(&buf, str);
  if (buf_parse_u8(&buf, &tmp) <= 0) {
    if (false) {
      err_puts("u8_init_str: buf_parse_u8");
      err_inspect_stacktrace(g_kc3_env->stacktrace);
      err_write_1("\n");
      assert(! "u8_init_str: buf_parse_u8");
    }
    return NULL;
  }
  *u = tmp;
  return u;
}

u8 * u8_random (u8 *u)
{
  arc4random_buf(u, sizeof(u8));
  return u;
}

#if 8 == 64 || 8 == w

u8 * u8_random_uniform (u8 *u, u8 max)
{
  f128 x;
  f128_random(&x);
  x *= max;
  *u = (u8) x;
  return u;
}

#else

u8 * u8_random_uniform (u8 *u, u8 max)
{
  *u = arc4random_uniform(max);
  return u;
}

#endif

s_tag * u8_sqrt (const u8 x, s_tag *dest)
{
  assert(dest);
  dest->type = TAG_F128;
  dest->data.f128 = sqrtl((long double) x);
  return dest;
}
