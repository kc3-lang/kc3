/* kc3
 * Copyright 2022,2023,2024 kmx.io <contact@kmx.io>
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
#include "kc3.h"
#include "buf_inspect.h"
#include "buf_parse.h"
#include "sym.h"
#include "tag_type.h"
#include "ratio.h"

bool tag_type_size (e_tag_type type, uw *dest)
{
  switch (type) {
  case TAG_VOID:        *dest = 0;                     return true;
  case TAG_ARRAY:       *dest = sizeof(s_array);       return true;
  case TAG_BLOCK:       *dest = sizeof(s_block);       return true;
  case TAG_BOOL:        *dest = sizeof(bool);          return true;
  case TAG_CALL:        *dest = sizeof(s_call);        return true;
  case TAG_CFN:         *dest = sizeof(s_cfn);         return true;
  case TAG_CHARACTER:   *dest = sizeof(character);     return true;
  case TAG_COMPLEX:     *dest = sizeof(s_complex *);   return true;
  case TAG_COW:         *dest = sizeof(s_cow *);       return true;
  case TAG_F32:         *dest = sizeof(f32);           return true;
  case TAG_F64:         *dest = sizeof(f64);           return true;
  case TAG_F128:        *dest = sizeof(f128);          return true;
  case TAG_FACT:        *dest = sizeof(s_fact);        return true;
  case TAG_FN:          *dest = sizeof(s_fn);          return true;
  case TAG_IDENT:       *dest = sizeof(s_ident);       return true;
  case TAG_INTEGER:     *dest = sizeof(s_integer);     return true;
  case TAG_SW:          *dest = sizeof(sw);            return true;
  case TAG_S64:         *dest = sizeof(s64);           return true;
  case TAG_S32:         *dest = sizeof(s32);           return true;
  case TAG_S16:         *dest = sizeof(s16);           return true;
  case TAG_S8:          *dest = sizeof(s8);            return true;
  case TAG_U8:          *dest = sizeof(u8);            return true;
  case TAG_U16:         *dest = sizeof(u16);           return true;
  case TAG_U32:         *dest = sizeof(u32);           return true;
  case TAG_U64:         *dest = sizeof(u64);           return true;
  case TAG_UW:          *dest = sizeof(uw);            return true;
  case TAG_LIST:        *dest = sizeof(s_list *);      return true;
  case TAG_MAP:         *dest = sizeof(s_map);         return true;
  case TAG_PTAG:        *dest = sizeof(p_tag);         return true;
  case TAG_PTR:
  case TAG_PTR_FREE:    *dest = sizeof(void *);        return true;
  case TAG_QUOTE:       *dest = sizeof(s_quote);       return true;
  case TAG_RATIO:       *dest = sizeof(s_ratio);       return true;
  case TAG_STR:         *dest = sizeof(s_str);         return true;
  case TAG_STRUCT:      *dest = sizeof(s_struct);      return true;
  case TAG_STRUCT_TYPE: *dest = sizeof(s_struct_type); return true;
  case TAG_SYM:         *dest = sizeof(s_sym *);       return true;
  case TAG_TUPLE:       *dest = sizeof(s_tuple);       return true;
  case TAG_UNQUOTE:     *dest = sizeof(s_unquote);     return true;
  case TAG_VAR:         *dest = sizeof(s_tag);         return true;
  }
  err_puts("tag_type_size: invalid tag type");
  assert(! "tag_type_size: invalid tag type");
  return false;
}

bool tag_type_to_ffi_type (e_tag_type type, ffi_type **dest)
{
  switch (type) {
  case TAG_ARRAY:       *dest = &ffi_type_pointer;    return true;
  case TAG_BLOCK:       *dest = &ffi_type_pointer;    return true;
  case TAG_BOOL:        *dest = &ffi_type_uint8;      return true;
  case TAG_CALL:        *dest = &ffi_type_pointer;    return true;
  case TAG_CFN:         *dest = &ffi_type_pointer;    return true;
  case TAG_CHARACTER:   *dest = &ffi_type_uint32;     return true;
  case TAG_COMPLEX:     *dest = &ffi_type_pointer;    return true;
  case TAG_COW:         *dest = &ffi_type_pointer;    return true;
  case TAG_F32:         *dest = &ffi_type_float;      return true;
  case TAG_F64:         *dest = &ffi_type_double;     return true;
  case TAG_F128:        *dest = &ffi_type_longdouble; return true;
  case TAG_FACT:        *dest = &ffi_type_pointer;    return true;
  case TAG_FN:          *dest = &ffi_type_pointer;    return true;
  case TAG_IDENT:       *dest = &ffi_type_pointer;    return true;
  case TAG_INTEGER:     *dest = &ffi_type_pointer;    return true;
  case TAG_LIST:        *dest = &ffi_type_pointer;    return true;
  case TAG_MAP:         *dest = &ffi_type_pointer;    return true;
  case TAG_PTAG:        *dest = &ffi_type_pointer;    return true;
  case TAG_PTR:         *dest = &ffi_type_pointer;    return true;
  case TAG_PTR_FREE:    *dest = &ffi_type_pointer;    return true;
  case TAG_QUOTE:       *dest = &ffi_type_pointer;    return true;
  case TAG_RATIO:       *dest = &ffi_type_pointer;    return true;
  case TAG_S8:          *dest = &ffi_type_sint8;      return true;
  case TAG_S16:         *dest = &ffi_type_sint16;     return true;
  case TAG_S32:         *dest = &ffi_type_sint32;     return true;
  case TAG_S64:         *dest = &ffi_type_sint64;     return true;
  case TAG_SW:          *dest = &ffi_type_slong;      return true;
  case TAG_STR:         *dest = &ffi_type_pointer;    return true;
  case TAG_STRUCT:      *dest = &ffi_type_pointer;    return true;
  case TAG_STRUCT_TYPE: *dest = &ffi_type_pointer;    return true;
  case TAG_SYM:         *dest = &ffi_type_pointer;    return true;
  case TAG_TUPLE:       *dest = &ffi_type_pointer;    return true;
  case TAG_U8:          *dest = &ffi_type_uint8;      return true;
  case TAG_U16:         *dest = &ffi_type_uint16;     return true;
  case TAG_U32:         *dest = &ffi_type_uint32;     return true;
  case TAG_U64:         *dest = &ffi_type_uint64;     return true;
  case TAG_UNQUOTE:     *dest = &ffi_type_pointer;    return true;
  case TAG_UW:          *dest = &ffi_type_ulong;      return true;
  case TAG_VAR:         *dest = &ffi_type_pointer;    return true;
  case TAG_VOID:        *dest = &ffi_type_void;       return true;
  }
  err_puts("tag_type_to_ffi_type: invalid tag type");
  assert(! "tag_type_to_ffi_type: invalid tag type");
  return false;
}

const char * tag_type_to_string (e_tag_type tag_type)
{
  switch (tag_type) {
  case TAG_VOID:        return "Void";
  case TAG_ARRAY:       return "Array";
  case TAG_BLOCK:       return "Block";
  case TAG_BOOL:        return "Bool";
  case TAG_CALL:        return "Call";
  case TAG_CFN:         return "Cfn";
  case TAG_CHARACTER:   return "Character";
  case TAG_COMPLEX:     return "Complex";
  case TAG_COW:         return "Cow";
  case TAG_F32:         return "F32";
  case TAG_F64:         return "F64";
  case TAG_F128:        return "F128";
  case TAG_FACT:        return "Fact";
  case TAG_FN:          return "Fn";
  case TAG_IDENT:       return "Ident";
  case TAG_INTEGER:     return "Integer";
  case TAG_SW:          return "Sw";
  case TAG_S64:         return "S64";
  case TAG_S32:         return "S32";
  case TAG_S16:         return "S16";
  case TAG_S8:          return "S8";
  case TAG_U8:          return "U8";
  case TAG_U16:         return "U16";
  case TAG_U32:         return "U32";
  case TAG_U64:         return "U64";
  case TAG_UW:          return "Uw";
  case TAG_LIST:        return "List";
  case TAG_MAP:         return "Map";
  case TAG_PTAG:        return "Ptag";
  case TAG_PTR:         return "Ptr";
  case TAG_PTR_FREE:    return "PtrFree";
  case TAG_QUOTE:       return "Quote";
  case TAG_RATIO:       return "Ratio";
  case TAG_STR:         return "Str";
  case TAG_STRUCT:      return "Struct";
  case TAG_STRUCT_TYPE: return "StructType";
  case TAG_SYM:         return "Sym";
  case TAG_TUPLE:       return "Tuple";
  case TAG_UNQUOTE:     return "Unquote";
  case TAG_VAR:         return "Var";
  }
  err_puts("tag_type_to_string: invalid tag type");
  assert(! "tag_type_to_string: invalid tag type");
  return NULL;
}
