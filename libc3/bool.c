/* c3
 * Copyright 2022-2024 kmx.io <contact@kmx.io>
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
#include "complex.h"
#include "integer.h"
#include "io.h"
#include "ratio.h"
#include "tag_type.h"

bool * bool_init_cast (bool *b, const s_tag *tag)
{
  assert(b);
  assert(tag);
  switch (tag->type) {
  case TAG_BOOL:      *b = tag->data.bool;                     return b;
  case TAG_CHARACTER: *b = (bool) tag->data.character;         return b;
    //case TAG_COMPLEX:   *b = ! complex_is_zero(tag->data.complex);
    //                                                         return b;
  case TAG_F32:       *b = (bool) tag->data.f32;               return b;
  case TAG_F64:       *b = (bool) tag->data.f64;               return b;
  case TAG_F128:      *b = (bool) tag->data.f128;              return b;
  case TAG_INTEGER:   *b = ! integer_is_zero(&tag->data.integer);
                                                               return b;
  case TAG_RATIO:     *b = ! ratio_is_zero(&tag->data.ratio);  return b;
  case TAG_S8:        *b = tag->data.s8 != 0;                  return b;
  case TAG_S16:       *b = tag->data.s16 != 0;                 return b;
  case TAG_S32:       *b = tag->data.s32 != 0;                 return b;
  case TAG_S64:       *b = tag->data.s64 != 0;                 return b;
  case TAG_SW:        *b = tag->data.sw != 0;                  return b;
  case TAG_U8:        *b = tag->data.u8 != 0;                  return b;
  case TAG_U16:       *b = tag->data.u16 != 0;                 return b;
  case TAG_U32:       *b = tag->data.u32 != 0;                 return b;
  case TAG_U64:       *b = tag->data.u64 != 0;                 return b;
  case TAG_UW:        *b = tag->data.uw != 0;                  return b;
  case TAG_PTAG:      return tag->data.ptag ?
      bool_init_cast(b, tag->data.ptag) : (*b = false, b);
  case TAG_PTR:       *b = tag->data.ptr.p != 0;               return b;
  case TAG_PTR_FREE:  *b = tag->data.ptr_free.p != 0;          return b;
  case TAG_ARRAY:
  case TAG_BLOCK:
  case TAG_CALL:
  case TAG_CFN:
  case TAG_FACT:
  case TAG_FN:
  case TAG_IDENT:
  case TAG_LIST:
  case TAG_MAP:
  case TAG_QUOTE:
  case TAG_STR:
  case TAG_STRUCT:
  case TAG_STRUCT_TYPE:
  case TAG_SYM:
  case TAG_TUPLE:
  case TAG_UNQUOTE:
  case TAG_VAR:
  case TAG_VOID:
    break;
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
