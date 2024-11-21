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
/* Gen from u.h.in BITS=32 bits=32 */
#include "assert.h"
#include "buf.h"
#include "buf_parse_u32.h"
#include <math.h>
#include <stdlib.h>
#include "f128.h"
#include "integer.h"
#include "ratio.h"
#include "sym.h"
#include "str.h"
#include "tag.h"
#include "u32.h"

u32 * u32_init_1 (u32 *u, const char *p)
{
  s_str str;
  str_init_1(&str, NULL, p);
  return u32_init_str(u, &str);
}

u32 * u32_init_cast
(u32 *u, const s_sym * const *type, const s_tag *tag)
{
  (void) type;
  switch (tag->type) {
  case TAG_BOOL:
    *u = tag->data.bool ? 1 : 0;
    return u;
  case TAG_CHARACTER:
    *u = (u32) tag->data.character;
    return u;
  case TAG_F32:
    *u = (u32) tag->data.f32;
    return u;
  case TAG_F64:
    *u = (u32) tag->data.f64;
    return u;
  case TAG_INTEGER:
    *u = integer_to_u32(&tag->data.integer);
    return u;
  case TAG_RATIO:
    *u = ratio_to_u32(&tag->data.ratio);
    return u;
  case TAG_S8:
    *u = (u32) tag->data.s8;
    return u;
  case TAG_S16:
    *u = (u32) tag->data.s16;
    return u;
  case TAG_S32:
    *u = (u32) tag->data.s32;
    return u;
  case TAG_S64:
    *u = (u32) tag->data.s64;
    return u;
  case TAG_STR:
    return u32_init_str(u, &tag->data.str);
  case TAG_SW:
    *u = (u32) tag->data.sw;
    return u;
  case TAG_U8:
    *u = (u32) tag->data.u8;
    return u;
  case TAG_U16:
    *u = (u32) tag->data.u16;
    return u;
  case TAG_U32:
    *u = (u32) tag->data.u32;
    return u;
  case TAG_U64:
    *u = (u32) tag->data.u64;
    return u;
  case TAG_UW:
    *u = (u32) tag->data.uw;
    return u;
  default:
    break;
  }
  err_write_1("u32_cast: cannot cast ");
  err_write_1(tag_type_to_string(tag->type));
  if (*type == &g_sym_U32)
    err_puts(" to U32");
  else {
    err_write_1(" to ");
    err_inspect_sym(type);
    err_puts(" aka U32");
  }
  assert(! "u32_cast: cannot cast to U32");
  return NULL;
}

u32 * u32_init_copy (u32 *u, const u32 *src)
{
  assert(src);
  assert(u);
  *u = *src;
  return u;
}

u32 * u32_init_str (u32 *u, const s_str *str)
{
  s_buf buf;
  u32 tmp = 0;
  buf_init_str_const(&buf, str);
  if (buf_parse_u32(&buf, &tmp) <= 0) {
    err_puts("u32_init_str: buf_parse_u32");
    assert(! "u32_init_str: buf_parse_u32");
    return NULL;
  }
  *u = tmp;
  return u;
}

u32 * u32_random (u32 *u)
{
  arc4random_buf(u, sizeof(u32));
  return u;
}

#if 32 == 64 || 32 == w

u32 * u32_random_uniform (u32 *u, u32 max)
{
  f128 x;
  f128_random(&x);
  x *= max;
  *u = (u32) x;
  return u;
}

#else

u32 * u32_random_uniform (u32 *u, u32 max)
{
  *u = arc4random_uniform(max);
  return u;
}

#endif

s_tag * u32_sqrt (const u32 x, s_tag *dest)
{
  assert(dest);
  dest->type = TAG_F128;
  dest->data.f128 = sqrtl((long double) x);
  return dest;
}
