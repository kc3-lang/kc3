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
#include "assert.h"
#include "bool.h"
#include "buf.h"
#include "buf_inspect.h"
#include "complex.h"
#include "cow.h"
#include "integer.h"
#include "io.h"
#include "ratio.h"
#include "sym.h"
#include "tag_type.h"

bool * bool_init_cast (bool *b, const s_sym * const *type,
                       const s_tag *tag)
{
  assert(b);
  assert(type);
  assert(tag);
  switch (tag->type) {
  case TAG_BOOL:      *b = tag->data.bool_;                    return b;
  case TAG_CHARACTER: *b = (bool) tag->data.character;         return b;
  case TAG_COMPLEX:   *b = ! complex_is_zero(tag->data.complex);
                                                               return b;
  case TAG_COW:
    return bool_init_cast(b, type, cow_read_only(tag->data.cow));
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
  case TAG_PTAG:
    return tag->data.ptag ?
      bool_init_cast(b, type, tag->data.ptag) : (*b = false, b);
  case TAG_PTR:       *b = tag->data.ptr.p != 0;               return b;
  case TAG_PTR_FREE:  *b = tag->data.ptr_free.p != 0;          return b;
  case TAG_ARRAY:
  case TAG_BLOCK:
  case TAG_CALL:
  case TAG_CALLABLE:
  case TAG_FACT:
  case TAG_IDENT:
  case TAG_LIST:
  case TAG_MAP:
  case TAG_PSTRUCT:
  case TAG_PSTRUCT_TYPE:
  case TAG_QUOTE:
  case TAG_STR:
  case TAG_SYM:
  case TAG_TIME:
  case TAG_TUPLE:
  case TAG_UNQUOTE:
  case TAG_VAR:       *b = true;                               return b;
  case TAG_VOID:      *b = false;                              return b;
  default:
    err_write_1("bool_cast: cannot cast ");
    err_write_1(tag_type_to_string(tag->type));
    if (*type == &g_sym_Bool)
      err_puts(" to Bool");
    else {
      err_write_1(" to ");
      err_inspect_sym(type);
      err_puts(" aka Bool");
    }
    assert(! "bool_cast: cannot cast to Bool");
  }
  return NULL;
}

bool * bool_init_copy (bool *dest, const bool *src)
{
  *dest = *src;
  return dest;
}
