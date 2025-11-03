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
  while (1) {
    if (tag->type == TAG_PTAG && tag->data.ptag)
      tag = tag->data.ptag;
    else if (tag->type == TAG_PVAR && tag->data.pvar->bound)
      tag = &tag->data.pvar->tag;
    else
      break;
  }
  switch (tag->type) {
  case TAG_BOOL:
    *b = tag->data.bool_ ? true : false;
    return b;
  case TAG_CHARACTER:
    *b = tag->data.character ? true : false;
    return b;
  case TAG_F32:
    *b = tag->data.f32 != 0.0;
    return b;
  case TAG_F64:
    *b = tag->data.f64 != 0.0;
    return b;
#if HAVE_F80
  case TAG_F80:
    *b = tag->data.f80 != 0.0;
    return b;
#endif
#if HAVE_FLOAT128
  case TAG_F128:
    *b = tag->data.f128 != 0.0;
    return b;
#endif
  case TAG_INTEGER:
    *b = ! integer_is_zero(&tag->data.integer);
    return b;
  case TAG_PCOMPLEX:
    *b = ! complex_is_zero(tag->data.pcomplex);
    return b;
  case TAG_PCOW:
    return bool_init_cast(b, type, cow_read_only(tag->data.pcow));
  case TAG_PTR:       *b = tag->data.ptr.p != 0;               return b;
  case TAG_POINTER:   *b = tag->data.pointer.ptr.p != 0;       return b;
  case TAG_PTR_FREE:  *b = tag->data.ptr_free.p != 0;          return b;
  case TAG_RATIO:
    *b = ! ratio_is_zero(&tag->data.ratio);
    return b;
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
  case TAG_ARRAY:
  case TAG_DO_BLOCK:
  case TAG_FACT:
  case TAG_IDENT:
  case TAG_PLIST:
  case TAG_MAP:
  case TAG_PCALL:
  case TAG_PCALLABLE:
  case TAG_PFACTS:
  case TAG_PSTRUCT:
  case TAG_PSTRUCT_TYPE:
  case TAG_PVAR:
  case TAG_QUOTE:
  case TAG_STR:
  case TAG_PSYM:
  case TAG_TIME:
  case TAG_TUPLE:
  case TAG_UNQUOTE:
    *b = true;
    return b;
  case TAG_PTAG:
  case TAG_VOID:
    *b = false;
    return b;
  }
  err_write_1("bool_cast: cannot cast ");
  err_write_1(tag_type_to_string(tag->type));
  if (*type == &g_sym_Bool)
    err_puts(" to Bool");
  else {
    err_write_1(" to ");
    err_inspect_psym(type);
    err_puts(" aka Bool");
  }
  assert(! "bool_cast: cannot cast to Bool");
  return NULL;
}

bool * bool_init_copy (bool *dest, const bool *src)
{
  *dest = *src;
  return dest;
}
