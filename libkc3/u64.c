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
/* Gen from u.h.in BITS=64 bits=64 */
#include "assert.h"
#include "buf.h"
#include "buf_parse_u64.h"
#include <math.h>
#include <stdlib.h>
#include "f64.h"
#if HAVE_F80
#include "f80.h"
#endif
#include "hash.h"
#include "integer.h"
#include "kc3_main.h"
#include "ratio.h"
#include "sym.h"
#include "str.h"
#include "tag.h"
#include "u64.h"

uw * u64_hash_uw (u64 u, uw *dest)
{
  t_hash h;
  hash_init(&h);
  if (! hash_update_u64(&h, u))
    return NULL;
  *dest = hash_to_uw(&h);
  hash_clean(&h);
  return dest;
}

u64 * u64_init_1 (u64 *u, const char *p)
{
  s_str str;
  str_init_1(&str, NULL, p);
  return u64_init_str(u, &str);
}

u64 * u64_init_cast
(u64 *u, const s_sym * const *type, const s_tag *tag)
{
  (void) type;
  switch (tag->type) {
  case TAG_BOOL:
    *u = tag->data.bool_ ? 1 : 0;
    return u;
  case TAG_CHARACTER:
    *u = (u64) tag->data.character;
    return u;
  case TAG_F32:
    *u = (u64) tag->data.f32;
    return u;
  case TAG_F64:
    *u = (u64) tag->data.f64;
    return u;
  case TAG_INTEGER:
    *u = integer_to_u64(&tag->data.integer);
    return u;
  case TAG_RATIO:
    *u = ratio_to_u64(&tag->data.ratio);
    return u;
  case TAG_S8:
    *u = (u64) tag->data.s8;
    return u;
  case TAG_S16:
    *u = (u64) tag->data.s16;
    return u;
  case TAG_S32:
    *u = (u64) tag->data.s32;
    return u;
  case TAG_S64:
    *u = (u64) tag->data.s64;
    return u;
  case TAG_STR:
    return u64_init_str(u, &tag->data.str);
  case TAG_SW:
    *u = (u64) tag->data.sw;
    return u;
  case TAG_U8:
    *u = (u64) tag->data.u8;
    return u;
  case TAG_U16:
    *u = (u64) tag->data.u16;
    return u;
  case TAG_U32:
    *u = (u64) tag->data.u32;
    return u;
  case TAG_U64:
    *u = (u64) tag->data.u64;
    return u;
  case TAG_UW:
    *u = (u64) tag->data.uw;
    return u;
  case TAG_VOID:
    *u = 0;
    return u;
  default:
    break;
  }
  err_write_1("u64_init_cast: cannot cast ");
  err_write_1(tag_type_to_string(tag->type));
  if (*type == &g_sym_U64)
    err_puts(" to U64");
  else {
    err_write_1(" to ");
    err_inspect_psym(type);
    err_puts(" aka U64");
  }
  err_inspect_stacktrace(env_global()->stacktrace);
  assert(! "u64_init_cast: cannot cast to U64");
  return NULL;
}

u64 * u64_init_copy (u64 *u, u64 src)
{
  assert(u);
  *u = src;
  return u;
}

u64 * u64_init_str (u64 *u, const s_str *str)
{
  s_buf buf;
  sw r;
  u64 tmp = 0;
  buf_init_str_const(&buf, str);
  r = buf_parse_u64(&buf, &tmp);
  buf_clean(&buf);
  if (r <= 0) {
    if (false) {
      err_puts("u64_init_str: buf_parse_u64");
      err_inspect_stacktrace(env_global()->stacktrace);
      err_write_1("\n");
      assert(! "u64_init_str: buf_parse_u64");
    }
    return NULL;
  }
  *u = tmp;
  return u;
}

u64 * u64_init_str_hexadecimal (u64 *u, const s_str *str)
{
  s_buf buf;
  sw r;
  u64 tmp = 0;
  buf_init_str_const(&buf, str);
  r = buf_parse_u64_base(&buf, g_kc3_bases_hexadecimal, &tmp);
  buf_clean(&buf);
  if (r <= 0) {
    if (false) {
      err_puts("u64_init_str: buf_parse_u64");
      err_inspect_stacktrace(env_global()->stacktrace);
      err_write_1("\n");
      assert(! "u64_init_str: buf_parse_u64");
    }
    return NULL;
  }
  *u = tmp;
  return u;
}

u64 * u64_random (u64 *u)
{
  arc4random_buf(u, sizeof(u64));
  return u;
}

#if 64 == 64 || 64 == w

u64 * u64_random_uniform (u64 *u, u64 max)
{
#if HAVE_F80
  f80 x;
  f80_random(&x);
#else
  f64 x;
  f64_random(&x);
#endif
  x *= max;
  *u = (u64) x;
  return u;
}

#else

u64 * u64_random_uniform (u64 *u, u64 max)
{
  *u = arc4random_uniform(max);
  return u;
}

#endif

s_tag * u64_sqrt (const u64 x, s_tag *dest)
{
  assert(dest);
#if HAVE_F80
  dest->type = TAG_F80;
  dest->data.f80 = sqrtl((f80) x);
#else
  dest->type = TAG_F64;
  dest->data.f64 = sqrt((f64) x);
#endif
  return dest;
}
