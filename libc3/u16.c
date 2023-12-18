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
/* Gen from u.h.in BITS=16 bits=16 */
#include "assert.h"
#include <err.h>
#include <math.h>
#include <stdlib.h>
#include "integer.h"
#include "tag.h"
#include "u16.h"

u16 * u16_init_cast (u16 *u, const s_tag *tag)
{
  switch (tag->type) {
  case TAG_BOOL:
    *u = tag->data.bool ? 1 : 0;
    return u;
  case TAG_CHARACTER:
    *u = (u16) tag->data.character;
    return u;
  case TAG_F32:
    *u = (u16) tag->data.f32;
    return u;
  case TAG_F64:
    *u = (u16) tag->data.f64;
    return u;
  case TAG_INTEGER:
    *u = integer_to_u16(&tag->data.integer);
    return u;
  case TAG_SW:
    *u = (u16) tag->data.sw;
    return u;
  case TAG_S64:
    *u = (u16) tag->data.s64;
    return u;
  case TAG_S32:
    *u = (u16) tag->data.s32;
    return u;
  case TAG_S16:
    *u = (u16) tag->data.s16;
    return u;
  case TAG_S8:
    *u = (u16) tag->data.s8;
    return u;
  case TAG_U8:
    *u = (u16) tag->data.u8;
    return u;
  case TAG_U16:
    *u = (u16) tag->data.u16;
    return u;
  case TAG_U32:
    *u = (u16) tag->data.u32;
    return u;
  case TAG_U64:
    *u = (u16) tag->data.u64;
    return u;
  case TAG_UW:
    *u = (u16) tag->data.uw;
    return u;
  default:
    break;
  }
  warnx("u16_cast: cannot cast %s to u16",
        tag_type_to_string(tag->type));
  assert(! "u16_cast: cannot cast to u16");
  return NULL;
}

u16 * u16_init_copy (u16 *u, const u16 *src)
{
  assert(src);
  assert(u);
  *u = *src;
  return u;
}

u16 * u16_random (u16 *u)
{
  arc4random_buf(u, sizeof(u16));
  return u;
}

#if 16 > 32

u16 * u16_random_uniform (u16 *u, u16 max)
{
  // TODO
  (void) u;
  (void) max;
  err_puts("u16_random_uniform: not implemented");
  assert(! "u16_random_uniform: not implemented");
  return NULL;
}

#else

u16 * u16_random_uniform (u16 *u, u16 max)
{
  *u = arc4random_uniform(max);
  return u;
}

#endif
