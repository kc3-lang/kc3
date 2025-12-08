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
/* Gen from s.h.in BITS=32 bits=32 */
#include <math.h>
#include <stdlib.h>
#include "assert.h"
#include "buf.h"
#include "buf_parse_s32.h"
#include "complex.h"
#include "integer.h"
#include "ratio.h"
#include "s32.h"
#include "str.h"
#include "sym.h"
#include "tag.h"
#include "tag_type.h"

s32 * s32_init_1 (s32 *s, const char *p)
{
  s_str str;
  str_init_1(&str, NULL, p);
  return s32_init_str(s, &str);
}

s32 * s32_init_cast
(s32 *s, const s_sym * const *type, const s_tag *tag)
{
  (void) type;
  switch (tag->type) {
  case TAG_BOOL:
    *s = tag->data.bool_ ? 1 : 0;
    return s;
  case TAG_CHARACTER:
    *s = (s32) tag->data.character;
    return s;
  case TAG_F32:
    *s = (s32) tag->data.f32;
    return s;
  case TAG_F64:
    *s = (s32) tag->data.f64;
    return s;
  case TAG_INTEGER:
    *s = integer_to_s32(&tag->data.integer);
    return s;
  case TAG_RATIO:
    *s = ratio_to_s32(&tag->data.ratio);
    return s;
  case TAG_SW:
    *s = (s32) tag->data.sw;
    return s;
  case TAG_S64:
    *s = (s32) tag->data.s64;
    return s;
  case TAG_S32:
    *s = (s32) tag->data.s32;
    return s;
  case TAG_S16:
    *s = (s32) tag->data.s16;
    return s;
  case TAG_S8:
    *s = (s32) tag->data.s8;
    return s;
  case TAG_STR:
    return s32_init_str(s, &tag->data.str);
  case TAG_U8:
    *s = (s32) tag->data.u8;
    return s;
  case TAG_U16:
    *s = (s32) tag->data.u16;
    return s;
  case TAG_U32:
    *s = (s32) tag->data.u32;
    return s;
  case TAG_U64:
    *s = (s32) tag->data.u64;
    return s;
  case TAG_UW:
    *s = (s32) tag->data.uw;
    return s;
  case TAG_VOID:
    *s = 0;
    return s;
  default:
    break;
  }
  err_write_1("s32_cast: cannot cast ");
  err_write_1(tag_type_to_string(tag->type));
  if (*type == &g_sym_S32)
    err_puts(" to S32");
  else {
    err_write_1(" to ");
    err_inspect_psym(type);
    err_puts(" aka S32");
  }
  if (true) {
    err_puts("s32_cast: stacktrace:");
    err_inspect_list(env_global()->stacktrace);
    err_write_1("\n");
  }
  assert(! "s32_cast: cannot cast to S32");
  return NULL;
}

s32 * s32_init_copy (s32 *s, s32 src)
{
  assert(s);
  *s = src;
  return s;
}

s32 * s32_init_str (s32 *s, const s_str *str)
{
  s_buf buf;
  sw r;
  s32 tmp = 0;
  buf_init_const(&buf, str->size, str->ptr.pchar);
  buf.wpos = str->size;
  r = buf_parse_s32(&buf, &tmp);
  buf_clean(&buf);
  if (r <= 0) {
    err_puts("s32_init_str: buf_parse_s32");
    assert(! "s32_init_str: buf_parse_s32");
    return NULL;
  }
  *s = tmp;
  return s;
}

s32 * s32_random (s32 *s)
{
  arc4random_buf(s, sizeof(s32));
  return s;
}

s_tag * s32_sqrt (const s32 x, s_tag *dest)
{
  assert(dest);
  if (x < 0) {
    dest->type = TAG_PCOMPLEX;
    dest->data.pcomplex = complex_new();
    tag_init_u8(&dest->data.pcomplex->x, 0);
#if HAVE_F80
    dest->data.pcomplex->y.type = TAG_F80;
    dest->data.pcomplex->y.data.f80 = sqrtl((f80) -x);
#else
    dest->data.pcomplex->y.type = TAG_F64;
    dest->data.pcomplex->y.data.f64 = sqrt((f64) -x);
#endif
    return dest;
  }
#if HAVE_F80
  dest->type = TAG_F80;
  dest->data.f80 = sqrtl((f80) x);
#else
  dest->type = TAG_F64;
  dest->data.f64 = sqrt((f64) x);
#endif  
  return dest;
}
