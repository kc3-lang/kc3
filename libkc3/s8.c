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
/* Gen from s.h.in BITS=8 bits=8 */
#include <math.h>
#include <stdlib.h>
#include "assert.h"
#include "buf.h"
#include "buf_parse_s8.h"
#include "complex.h"
#include "integer.h"
#include "ratio.h"
#include "s8.h"
#include "str.h"
#include "sym.h"
#include "tag.h"
#include "tag_type.h"

s8 * s8_init_1 (s8 *s, const char *p)
{
  s_str str;
  str_init_1(&str, NULL, p);
  return s8_init_str(s, &str);
}

s8 * s8_init_cast
(s8 *s, const s_sym * const *type, const s_tag *tag)
{
  (void) type;
  switch (tag->type) {
  case TAG_BOOL:
    *s = tag->data.bool_ ? 1 : 0;
    return s;
  case TAG_CHARACTER:
    *s = (s8) tag->data.character;
    return s;
  case TAG_F32:
    *s = (s8) tag->data.f32;
    return s;
  case TAG_F64:
    *s = (s8) tag->data.f64;
    return s;
  case TAG_INTEGER:
    *s = integer_to_s8(&tag->data.integer);
    return s;
  case TAG_RATIO:
    *s = ratio_to_s8(&tag->data.ratio);
    return s;
  case TAG_SW:
    *s = (s8) tag->data.sw;
    return s;
  case TAG_S64:
    *s = (s8) tag->data.s64;
    return s;
  case TAG_S32:
    *s = (s8) tag->data.s32;
    return s;
  case TAG_S16:
    *s = (s8) tag->data.s16;
    return s;
  case TAG_S8:
    *s = (s8) tag->data.s8;
    return s;
  case TAG_STR:
    return s8_init_str(s, &tag->data.str);
  case TAG_U8:
    *s = (s8) tag->data.u8;
    return s;
  case TAG_U16:
    *s = (s8) tag->data.u16;
    return s;
  case TAG_U32:
    *s = (s8) tag->data.u32;
    return s;
  case TAG_U64:
    *s = (s8) tag->data.u64;
    return s;
  case TAG_UW:
    *s = (s8) tag->data.uw;
    return s;
  default:
    break;
  }
  err_write_1("s8_cast: cannot cast ");
  err_write_1(tag_type_to_string(tag->type));
  if (*type == &g_sym_S8)
    err_puts(" to S8");
  else {
    err_write_1(" to ");
    err_inspect_psym(type);
    err_puts(" aka S8");
  }
  if (true) {
    err_puts("s8_cast: stacktrace:");
    err_inspect_list(env_global()->stacktrace);
    err_write_1("\n");
  }
  assert(! "s8_cast: cannot cast to S8");
  return NULL;
}

s8 * s8_init_copy (s8 *s, const s8 *src)
{
  assert(src);
  assert(s);
  *s = *src;
  return s;
}

s8 * s8_init_str (s8 *s, const s_str *str)
{
  s_buf buf;
  s8 tmp = 0;
  buf_init_const(&buf, str->size, str->ptr.pchar);
  buf.wpos = str->size;
  if (buf_parse_s8(&buf, &tmp) <= 0) {
    err_puts("s8_init_str: buf_parse_s8");
    assert(! "s8_init_str: buf_parse_s8");
    return NULL;
  }
  *s = tmp;
  return s;
}

s8 * s8_random (s8 *s)
{
  arc4random_buf(s, sizeof(s8));
  return s;
}

s_tag * s8_sqrt (const s8 x, s_tag *dest)
{
  assert(dest);
  if (x < 0) {
    dest->type = TAG_PCOMPLEX;
    dest->data.pcomplex = complex_new();
    tag_init_u8(&dest->data.pcomplex->x, 0);
    dest->data.pcomplex->y.type = TAG_F128;
    dest->data.pcomplex->y.data.f128 = sqrtl((long double) -x);
    return dest;
  }
  dest->type = TAG_F128;
  dest->data.f128 = sqrtl((long double) x);
  return dest;
}
