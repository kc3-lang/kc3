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
/* Gen from s.h.in BITS=64 bits=64 */
#include <math.h>
#include <stdlib.h>
#include "assert.h"
#include "buf.h"
#include "buf_parse_s64.h"
#include "complex.h"
#include "integer.h"
#include "ratio.h"
#include "s64.h"
#include "str.h"
#include "sym.h"
#include "tag.h"
#include "tag_type.h"

s64 * s64_init_1 (s64 *s, const char *p)
{
  s_str str;
  str_init_1(&str, NULL, p);
  return s64_init_str(s, &str);
}

s64 * s64_init_cast
(s64 *s, const s_sym * const *type, const s_tag *tag)
{
  (void) type;
  switch (tag->type) {
  case TAG_BOOL:
    *s = tag->data.bool ? 1 : 0;
    return s;
  case TAG_CHARACTER:
    *s = (s64) tag->data.character;
    return s;
  case TAG_F32:
    *s = (s64) tag->data.f32;
    return s;
  case TAG_F64:
    *s = (s64) tag->data.f64;
    return s;
  case TAG_INTEGER:
    *s = integer_to_s64(&tag->data.integer);
    return s;
  case TAG_RATIO:
    *s = ratio_to_s64(&tag->data.ratio);
    return s;
  case TAG_SW:
    *s = (s64) tag->data.sw;
    return s;
  case TAG_S64:
    *s = (s64) tag->data.s64;
    return s;
  case TAG_S32:
    *s = (s64) tag->data.s32;
    return s;
  case TAG_S16:
    *s = (s64) tag->data.s16;
    return s;
  case TAG_S8:
    *s = (s64) tag->data.s8;
    return s;
  case TAG_STR:
    if (! s64_init_str(s, &tag->data.str))
      return NULL;
    return s;
  case TAG_U8:
    *s = (s64) tag->data.u8;
    return s;
  case TAG_U16:
    *s = (s64) tag->data.u16;
    return s;
  case TAG_U32:
    *s = (s64) tag->data.u32;
    return s;
  case TAG_U64:
    *s = (s64) tag->data.u64;
    return s;
  case TAG_UW:
    *s = (s64) tag->data.uw;
    return s;
  default:
    break;
  }
  err_write_1("s64_cast: cannot cast ");
  err_write_1(tag_type_to_string(tag->type));
  if (*type == &g_sym_S64)
    err_puts(" to S64");
  else {
    err_write_1(" to ");
    err_inspect_sym(type);
    err_puts(" aka S64");
  }
  if (true) {
    err_puts("s64_cast: stacktrace:");
    err_inspect_list(g_kc3_env.stacktrace);
    err_write_1("\n");
  }
  assert(! "s64_cast: cannot cast to S64");
  return NULL;
}

s64 * s64_init_copy (s64 *s, const s64 *src)
{
  assert(src);
  assert(s);
  *s = *src;
  return s;
}

s64 * s64_init_str (s64 *s, const s_str *str)
{
  s_buf buf;
  s64 tmp = 0;
  buf_init_const(&buf, str->size, str->ptr.pchar);
  buf.wpos = str->size;
  if (buf_parse_s64(&buf, &tmp) <= 0) {
    err_puts("s64_init_str: buf_parse_s64");
    assert(! "s64_init_str: buf_parse_s64");
    return NULL;
  }
  *s = tmp;
  return s;
}

s64 * s64_random (s64 *s)
{
  arc4random_buf(s, sizeof(s64));
  return s;
}

s_tag * s64_sqrt (const s64 x, s_tag *dest)
{
  assert(dest);
  if (x < 0) {
    dest->type = TAG_COMPLEX;
    dest->data.complex = complex_new();
    tag_init_u8(&dest->data.complex->x, 0);
    dest->data.complex->y.type = TAG_F128;
    dest->data.complex->y.data.f128 = sqrtl((long double) -x);
    return dest;
  }
  dest->type = TAG_F128;
  dest->data.f128 = sqrtl((long double) x);
  return dest;
}
