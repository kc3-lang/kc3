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
#include "types.h"

#if HAVE_FLOAT128

#include <stdlib.h>
#include "assert.h"
#include "f128.h"
#include "integer.h"
#include "ratio.h"
#include "sym.h"
#include "tag.h"
#include "tag_type.h"
#include "u64.h"

#if defined(WIN32) || defined(WIN64)

f128 cosq (f128 x)
{
  return __builtin_cosq(x);
}

f128 sinq (f128 x)
{
  return __builtin_sinq(x);
}

f128 tanq (f128 x)
{
  return __builtin_tanq(x);
}

#endif

f128 * f128_init_cast (f128 *x, const s_sym * const *type,
                       const s_tag *tag)
{
  assert(x);
  assert(type);
  assert(tag);
  switch (tag->type) {
  case TAG_BOOL:
    *x = tag->data.bool_ ? 1.0Q : 0.0Q;
    return x;
  case TAG_CHARACTER:
    *x = (f128) tag->data.character;
    return x;
  case TAG_F32:
    *x = (f128) tag->data.f32;
    return x;
  case TAG_F64:
    *x = (f128) tag->data.f64;
    return x;
  case TAG_F80:
    *x = (f128) tag->data.f80;
    return x;
  case TAG_F128:
    *x = tag->data.f128;
    return x;
  case TAG_INTEGER:
    *x = integer_to_f128(&tag->data.integer);
    return x;
  case TAG_RATIO:
    *x = ratio_to_f128(&tag->data.ratio);
    return x;
  case TAG_SW:
    *x = (f128) tag->data.sw;
    return x;
  case TAG_S64:
    *x = (f128) tag->data.s64;
    return x;
  case TAG_S32:
    *x = (f128) tag->data.s32;
    return x;
  case TAG_S16:
    *x = (f128) tag->data.s16;
    return x;
  case TAG_S8:
    *x = (f128) tag->data.s8;
    return x;
  case TAG_U8:
    *x = (f128) tag->data.u8;
    return x;
  case TAG_U16:
    *x = (f128) tag->data.u16;
    return x;
  case TAG_U32:
    *x = (f128) tag->data.u32;
    return x;
  case TAG_U64:
    *x = (f128) tag->data.u64;
    return x;
  case TAG_UW:
    *x = (f128) tag->data.uw;
    return x;
  default:
    break;
  }
  err_write_1("f128_init_cast: cannot cast ");
  err_write_1(tag_type_to_string(tag->type));
  if (*type == &g_sym_F128)
    err_puts(" to F128");
  else {
    err_write_1(" to ");
    err_inspect_psym(type);
    err_puts(" aka F128");
  }
  assert(! "f128_init_cast: cannot cast to F128");
  return NULL;
}

f128 * f128_init_copy (f128 *x, f128 src)
{
  assert(x);
  *x = src;
  return x;
}

typedef union _128 {
  f128 f128;
  u64 u64[2];
  s64 s64[2];
} u_128;

f128 * f128_random (f128 *x)
{
  u_128 u;
  u_128 mask;
  arc4random_buf(&u, 15);
  mask.u64[0] = 0x7FFFFFFFFFFFFFFF;
  mask.u64[1] = 0xFFFFFFFFFFFFC000;
  u.u64[0] &= mask.u64[0];
  u.u64[1] &= mask.u64[1];
  *x = u.f128;
  return x;
}

#endif /* HAVE_FLOAT128 */
