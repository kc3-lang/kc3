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
/* Gen from u.h.in BITS=16 bits=16 */
#include "assert.h"
#include "buf.h"
#include "buf_parse_u16.h"
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
#include "u16.h"

uw * u16_hash_uw (u16 u, uw *dest)
{
  t_hash h;
  hash_init(&h);
  if (! hash_update_u16(&h, u))
    return NULL;
  *dest = hash_to_uw(&h);
  hash_clean(&h);
  return dest;
}

u16 * u16_init_1 (u16 *u, const char *p)
{
  s_str str;
  str_init_1(&str, NULL, p);
  return u16_init_str(u, &str);
}

u16 * u16_init_cast
(u16 *u, const s_sym * const *type, const s_tag *tag)
{
  (void) type;
  switch (tag->type) {
  case TAG_BOOL:
    *u = tag->data.td_bool_ ? 1 : 0;
    return u;
  case TAG_CHARACTER:
    *u = (u16) tag->data.td_character;
    return u;
  case TAG_F32:
    *u = (u16) tag->data.td_f32;
    return u;
  case TAG_F64:
    *u = (u16) tag->data.td_f64;
    return u;
  case TAG_INTEGER:
    *u = integer_to_u16(&tag->data.td_integer);
    return u;
  case TAG_RATIO:
    *u = ratio_to_u16(&tag->data.td_ratio);
    return u;
  case TAG_S8:
    *u = (u16) tag->data.td_s8;
    return u;
  case TAG_S16:
    *u = (u16) tag->data.td_s16;
    return u;
  case TAG_S32:
    *u = (u16) tag->data.td_s32;
    return u;
  case TAG_S64:
    *u = (u16) tag->data.td_s64;
    return u;
  case TAG_STR:
    return u16_init_str(u, &tag->data.td_str);
  case TAG_SW:
    *u = (u16) tag->data.td_sw;
    return u;
  case TAG_U8:
    *u = (u16) tag->data.td_u8;
    return u;
  case TAG_U16:
    *u = (u16) tag->data.td_u16;
    return u;
  case TAG_U32:
    *u = (u16) tag->data.td_u32;
    return u;
  case TAG_U64:
    *u = (u16) tag->data.td_u64;
    return u;
  case TAG_UW:
    *u = (u16) tag->data.td_uw;
    return u;
  case TAG_VOID:
    *u = 0;
    return u;
  default:
    break;
  }
  err_write_1("u16_init_cast: cannot cast ");
  err_write_1(tag_type_to_string(tag->type));
  if (*type == &g_sym_U16)
    err_puts(" to U16");
  else {
    err_write_1(" to ");
    err_inspect_psym(type);
    err_puts(" aka U16");
  }
  err_inspect_stacktrace(env_global()->stacktrace);
  assert(! "u16_init_cast: cannot cast to U16");
  return NULL;
}

u16 * u16_init_copy (u16 *u, u16 src)
{
  assert(u);
  *u = src;
  return u;
}

u16 * u16_init_str (u16 *u, const s_str *str)
{
  s_buf buf;
  sw r;
  u16 tmp = 0;
  buf_init_str_const(&buf, str);
  r = buf_parse_u16(&buf, &tmp);
  buf_clean(&buf);
  if (r <= 0) {
    if (false) {
      err_puts("u16_init_str: buf_parse_u16");
      err_inspect_stacktrace(env_global()->stacktrace);
      err_write_1("\n");
      assert(! "u16_init_str: buf_parse_u16");
    }
    return NULL;
  }
  *u = tmp;
  return u;
}

u16 * u16_init_str_hexadecimal (u16 *u, const s_str *str)
{
  s_buf buf;
  sw r;
  u16 tmp = 0;
  buf_init_str_const(&buf, str);
  r = buf_parse_u16_base(&buf, g_kc3_bases_hexadecimal, &tmp);
  buf_clean(&buf);
  if (r <= 0) {
    if (false) {
      err_puts("u16_init_str: buf_parse_u16");
      err_inspect_stacktrace(env_global()->stacktrace);
      err_write_1("\n");
      assert(! "u16_init_str: buf_parse_u16");
    }
    return NULL;
  }
  *u = tmp;
  return u;
}

u16 * u16_random (u16 *u)
{
  arc4random_buf(u, sizeof(u16));
  return u;
}

#if 16 == 64 || 16 == w

u16 * u16_random_uniform (u16 *u, u16 max)
{
#if HAVE_F80
  f80 x;
  f80_random(&x);
#else
  f64 x;
  f64_random(&x);
#endif
  x *= max;
  *u = (u16) x;
  return u;
}

#else

u16 * u16_random_uniform (u16 *u, u16 max)
{
  *u = arc4random_uniform(max);
  return u;
}

#endif

s_tag * u16_sqrt (const u16 x, s_tag *dest)
{
  assert(dest);
#if HAVE_F80
  dest->type = TAG_F80;
  dest->data.td_f80 = sqrtl((f80) x);
#else
  dest->type = TAG_F64;
  dest->data.td_f64 = sqrt((f64) x);
#endif
  return dest;
}
