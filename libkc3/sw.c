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
/* Gen from s.h.in BITS=W bits=w */
#include <math.h>
#include <stdlib.h>
#include "assert.h"
#include "buf.h"
#include "buf_parse_sw.h"
#include "complex.h"
#include "integer.h"
#include "ratio.h"
#include "sw.h"
#include "str.h"
#include "sym.h"
#include "tag.h"
#include "tag_type.h"

sw * sw_init_1 (sw *s, const char *p)
{
  s_str str;
  str_init_1(&str, NULL, p);
  return sw_init_str(s, &str);
}

sw * sw_init_cast
(sw *s, const s_sym * const *type, const s_tag *tag)
{
  (void) type;
  switch (tag->type) {
  case TAG_BOOL:
    *s = tag->data.bool_ ? 1 : 0;
    return s;
  case TAG_CHARACTER:
    *s = (sw) tag->data.character;
    return s;
  case TAG_F32:
    *s = (sw) tag->data.f32;
    return s;
  case TAG_F64:
    *s = (sw) tag->data.f64;
    return s;
  case TAG_INTEGER:
    *s = integer_to_sw(&tag->data.integer);
    return s;
  case TAG_RATIO:
    *s = ratio_to_sw(&tag->data.ratio);
    return s;
  case TAG_SW:
    *s = (sw) tag->data.sw;
    return s;
  case TAG_S64:
    *s = (sw) tag->data.s64;
    return s;
  case TAG_S32:
    *s = (sw) tag->data.s32;
    return s;
  case TAG_S16:
    *s = (sw) tag->data.s16;
    return s;
  case TAG_S8:
    *s = (sw) tag->data.s8;
    return s;
  case TAG_STR:
    return sw_init_str(s, &tag->data.str);
  case TAG_U8:
    *s = (sw) tag->data.u8;
    return s;
  case TAG_U16:
    *s = (sw) tag->data.u16;
    return s;
  case TAG_U32:
    *s = (sw) tag->data.u32;
    return s;
  case TAG_U64:
    *s = (sw) tag->data.u64;
    return s;
  case TAG_UW:
    *s = (sw) tag->data.uw;
    return s;
  case TAG_VOID:
    *s = 0;
    return s;
  default:
    break;
  }
  err_write_1("sw_cast: cannot cast ");
  err_write_1(tag_type_to_string(tag->type));
  if (*type == &g_sym_Sw)
    err_puts(" to Sw");
  else {
    err_write_1(" to ");
    err_inspect_psym(type);
    err_puts(" aka Sw");
  }
  if (true) {
    err_puts("sw_cast: stacktrace:");
    err_inspect_list(env_global()->stacktrace);
    err_write_1("\n");
  }
  assert(! "sw_cast: cannot cast to Sw");
  return NULL;
}

sw * sw_init_copy (sw *s, sw src)
{
  assert(s);
  *s = src;
  return s;
}

sw * sw_init_str (sw *s, const s_str *str)
{
  s_buf buf;
  sw tmp = 0;
  buf_init_const(&buf, str->size, str->ptr.pchar);
  buf.wpos = str->size;
  if (buf_parse_sw(&buf, &tmp) <= 0) {
    err_puts("sw_init_str: buf_parse_sw");
    assert(! "sw_init_str: buf_parse_sw");
    return NULL;
  }
  *s = tmp;
  return s;
}

sw * sw_random (sw *s)
{
  arc4random_buf(s, sizeof(sw));
  return s;
}

s_tag * sw_sqrt (const sw x, s_tag *dest)
{
  assert(dest);
  if (x < 0) {
    dest->type = TAG_PCOMPLEX;
    dest->data.pcomplex = complex_new();
    tag_init_u8(&dest->data.pcomplex->x, 0);
    dest->data.pcomplex->y.type = TAG_F80;
    dest->data.pcomplex->y.data.f80 = sqrtl((f80) -x);
    return dest;
  }
  dest->type = TAG_F80;
  dest->data.f80 = sqrtl((f80) x);
  return dest;
}
