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
/* Gen from u.h.in BITS=32 bits=32 */
#include "assert.h"
#include <err.h>
#include <math.h>
#include <stdlib.h>
#include "integer.h"
#include "tag.h"
#include "u32.h"

u32 * u32_init_cast (u32 *u, const s_tag *tag)
{
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
  case TAG_SW:
    *u = (u32) tag->data.sw;
    return u;
  case TAG_S64:
    *u = (u32) tag->data.s64;
    return u;
  case TAG_S32:
    *u = (u32) tag->data.s32;
    return u;
  case TAG_S16:
    *u = (u32) tag->data.s16;
    return u;
  case TAG_S8:
    *u = (u32) tag->data.s8;
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
  warnx("u32_cast: cannot cast %s to u32",
        tag_type_to_string(tag->type));
  assert(! "u32_cast: cannot cast to u32");
  return NULL;
}

u32 * u32_init_copy (u32 *u, const u32 *src)
{
  assert(src);
  assert(u);
  *u = *src;
  return u;
}

u32 * u32_random (u32 *u)
{
  arc4random_buf(u, sizeof(u32));
  return u;
}

#if 32 > 32

u32 * u32_random_uniform (u32 *u, u32 max)
{
  // TODO
  (void) u;
  (void) max;
  err_puts("u32_random_uniform: not implemented");
  assert(! "u32_random_uniform: not implemented");
  return NULL;
}

#else

u32 * u32_random_uniform (u32 *u, u32 max)
{
  *u = arc4random_uniform(max);
  return u;
}

#endif
