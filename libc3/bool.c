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
#include "assert.h"
#include "bool.h"
#include "buf.h"
#include "buf_inspect.h"
#include "integer.h"
#include "io.h"
#include "tag_type.h"

bool * bool_init_cast (bool *b, const s_tag *tag)
{
  assert(b);
  assert(tag);
  switch (tag->type) {
  case TAG_BOOL:      *b = tag->data.bool;                    return b;
  case TAG_CHARACTER: *b = (bool) tag->data.character;        return b;
  case TAG_F32:       *b = (bool) tag->data.f32;              return b;
  case TAG_F64:       *b = (bool) tag->data.f64;              return b;
  case TAG_INTEGER:   *b = integer_to_u8(&tag->data.integer); return b;
  case TAG_S8:        *b = (bool) tag->data.s8;               return b;
  case TAG_S16:       *b = (bool) tag->data.s16;              return b;
  case TAG_S32:       *b = (bool) tag->data.s32;              return b;
  case TAG_S64:       *b = (bool) tag->data.s64;              return b;
  case TAG_SW:        *b = (bool) tag->data.sw;               return b;
  case TAG_U8:        *b = (bool) tag->data.u8;               return b;
  case TAG_U16:       *b = (bool) tag->data.u16;              return b;
  case TAG_U32:       *b = (bool) tag->data.u32;              return b;
  case TAG_U64:       *b = (bool) tag->data.u64;              return b;
  case TAG_UW:        *b = (bool) tag->data.uw;               return b;
  default: break;
  }
  err_write_1("bool_cast: cannot cast ");
  err_write_1(tag_type_to_string(tag->type));
  err_puts(" to Bool");
  assert(! "bool_cast: cannot cast to Bool");
  return NULL;
}

bool * bool_init_copy (bool *dest, const bool *src)
{
  *dest = *src;
  return dest;
}

s_str * bool_inspect (bool *b, s_str *dest)
{
  sw size;
  s_buf tmp;
  size = buf_inspect_bool_size(b);
  if (size < 0) {
    err_write_1("bool_inspect: error: ");
    err_inspect_u8(b);
    assert(! "bool_inspect: error");
    return NULL;
  }
  buf_init_alloc(&tmp, size);
  buf_inspect_bool(&tmp, b);
  assert(tmp.wpos == tmp.size);
  if (tmp.wpos != tmp.size) {
    buf_clean(&tmp);
    err_write_1("bool_inspect: buf_inspect_bool");
    return NULL;
  }
  return buf_to_str(&tmp, dest);
}
