/* c3
 * Copyright 2022,2023 kmx.io <contact@kmx.io>
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
#include <math.h>
#include <stdlib.h>
#include "integer.h"
#include "ratio.h"
#include "tag.h"
#include "uw.h"

uw * uw_init_cast (uw *u, const s_tag *tag)
{
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
  case TAG_SW:
    *u = (uw) tag->data.sw;
    return u;
  case TAG_S64:
    *u = (uw) tag->data.s64;
    return u;
  case TAG_S32:
    *u = (uw) tag->data.s32;
    return u;
  case TAG_S16:
    *u = (uw) tag->data.s16;
    return u;
  case TAG_S8:
    *u = (uw) tag->data.s8;
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
  err_puts(" to uw");
  assert(! "uw_cast: cannot cast to uw");
  return NULL;
}

uw * uw_init_copy (uw *u, const uw *src)
{
  assert(src);
  assert(u);
  *u = *src;
  return u;
}

uw * uw_random (uw *u)
{
  arc4random_buf(u, sizeof(uw));
  return u;
}

#if w > 32

uw * uw_random_uniform (uw *u, uw max)
{
  // TODO
  (void) u;
  (void) max;
  err_puts("uw_random_uniform: not implemented");
  assert(! "uw_random_uniform: not implemented");
  return NULL;
}

#else

uw * uw_random_uniform (uw *u, uw max)
{
  *u = arc4random_uniform(max);
  return u;
}

#endif
