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
#include "kc3.h"
#include "buf_inspect.h"
#include "buf_parse.h"
#include "sym.h"
#include "tag_type.h"
#include "ratio.h"

#define TAG_TYPE_INT(i)                         \
  ((i > U16_MAX) ? TAG_U32 :                    \
   ((i > U8_MAX) ? TAG_U16 :                    \
    ((i >= 0) ? TAG_U8 :                        \
     ((i >= S8_MIN) ? TAG_S8 :                  \
      ((i >= S16_MIN) ? TAG_S16 :               \
       TAG_S32)))))

u8 * tag_type_alignment (e_tag_type tag_type, u8 *dest)
{
  assert(dest);
  switch (tag_type) {
  case TAG_ARRAY:                       *dest = 8;          return dest;
  case TAG_BOOL:                        *dest = 1;          return dest;
  case TAG_CHARACTER:                   *dest = 4;          return dest;
  case TAG_DO_BLOCK:                    *dest = 8;          return dest;
  case TAG_F32:                         *dest = 4;          return dest;
  case TAG_F64:                         *dest = 8;          return dest;
#if HAVE_F80
  case TAG_F80:                         *dest = 8;          return dest;
#endif
#if HAVE_F128
  case TAG_F128:                        *dest = 16;         return dest;
#endif
  case TAG_FACT:                        *dest = 8;          return dest;
  case TAG_IDENT:                       *dest = 8;          return dest;
  case TAG_INTEGER:                     *dest = 8;          return dest;
  case TAG_MAP:                         *dest = 8;          return dest;
  case TAG_PCALL:
  case TAG_PCALLABLE:
  case TAG_PCOMPLEX:
  case TAG_PCOW:
  case TAG_PFACTS:
  case TAG_PLIST:
  case TAG_POINTER:
  case TAG_PSTRUCT:
  case TAG_PSTRUCT_TYPE:
  case TAG_PSYM:
  case TAG_PTAG:
  case TAG_PTR:
  case TAG_PTR_FREE:
  case TAG_PVAR:                        *dest = 8;          return dest;
  case TAG_QUOTE:                       *dest = 8;          return dest;
  case TAG_RATIO:                       *dest = 8;          return dest;
  case TAG_S8:                          *dest = 1;          return dest;
  case TAG_S16:                         *dest = 2;          return dest;
  case TAG_S32:                         *dest = 4;          return dest;
  case TAG_S64:                         *dest = 8;          return dest;
  case TAG_STR:                         *dest = 8;          return dest;
  case TAG_SW:                          *dest = sizeof(sw); return dest;
  case TAG_TIME:                        *dest = 8;          return dest;
  case TAG_TUPLE:                       *dest = 8;          return dest;
  case TAG_U8:                          *dest = 1;          return dest;
  case TAG_U16:                         *dest = 2;          return dest;
  case TAG_U32:                         *dest = 4;          return dest;
  case TAG_U64:                         *dest = 8;          return dest;
  case TAG_UNQUOTE:                     *dest = 8;          return dest;
  case TAG_UW:                          *dest = sizeof(uw); return dest;
  case TAG_VOID:                        *dest = 1;          return dest;
  }
  err_write_1("tag_alignment: not implemented: ");
  err_puts(tag_type_to_string(tag_type));
  abort();
}

e_tag_type tag_type_int (int i)
{
  assert(i <= S32_MAX);
  assert(i >= S32_MIN);
  if (i > U16_MAX)
    return TAG_U32;
  if (i > U8_MAX)
    return TAG_U16;
  if (i >= 0)
    return TAG_U8;
  if (i >= S8_MIN)
    return TAG_S8;
  if (i >= S16_MIN)
    return TAG_S16;
  return S32_MIN;
}

bool tag_type_size (e_tag_type type, uw *dest)
{
  switch (type) {
  case TAG_VOID:         *dest = 0;                     return true;
  case TAG_ARRAY:        *dest = sizeof(s_array);       return true;
  case TAG_DO_BLOCK:     *dest = sizeof(s_do_block);    return true;
  case TAG_BOOL:         *dest = sizeof(bool);          return true;
  case TAG_CHARACTER:    *dest = sizeof(character);     return true;
  case TAG_F32:          *dest = sizeof(f32);           return true;
  case TAG_F64:          *dest = sizeof(f64);           return true;
#if HAVE_F80
  case TAG_F80:          *dest = sizeof(f80);           return true;
#endif
#if HAVE_F128
  case TAG_F128:         *dest = sizeof(f128);          return true;
#endif
  case TAG_FACT:         *dest = sizeof(s_fact);        return true;
  case TAG_IDENT:        *dest = sizeof(s_ident);       return true;
  case TAG_INTEGER:      *dest = sizeof(s_integer);     return true;
  case TAG_MAP:          *dest = sizeof(s_map);         return true;
  case TAG_PCALL:        *dest = sizeof(p_call);        return true;
  case TAG_PCALLABLE:    *dest = sizeof(p_callable);    return true;
  case TAG_PCOMPLEX:     *dest = sizeof(p_complex);     return true;
  case TAG_PCOW:         *dest = sizeof(p_cow);         return true;
  case TAG_PFACTS:       *dest = sizeof(p_facts);       return true;
  case TAG_PLIST:        *dest = sizeof(p_list);        return true;
  case TAG_POINTER:      *dest = sizeof(s_pointer);     return true;
  case TAG_PSTRUCT:      *dest = sizeof(p_struct);      return true;
  case TAG_PSTRUCT_TYPE: *dest = sizeof(p_struct_type); return true;
  case TAG_PSYM:         *dest = sizeof(s_sym *);       return true;
  case TAG_PTAG:         *dest = sizeof(p_tag);         return true;
  case TAG_PTR:
  case TAG_PTR_FREE:     *dest = sizeof(void *);        return true;
  case TAG_PVAR:         *dest = sizeof(p_var);         return true;
  case TAG_QUOTE:        *dest = sizeof(s_quote);       return true;
  case TAG_RATIO:        *dest = sizeof(s_ratio);       return true;
  case TAG_S8:           *dest = sizeof(s8);            return true;
  case TAG_S16:          *dest = sizeof(s16);           return true;
  case TAG_S32:          *dest = sizeof(s32);           return true;
  case TAG_S64:          *dest = sizeof(s64);           return true;
  case TAG_STR:          *dest = sizeof(s_str);         return true;
  case TAG_SW:           *dest = sizeof(sw);            return true;
  case TAG_TIME:         *dest = sizeof(s_time);        return true;
  case TAG_TUPLE:        *dest = sizeof(s_tuple);       return true;
  case TAG_U8:           *dest = sizeof(u8);            return true;
  case TAG_U16:          *dest = sizeof(u16);           return true;
  case TAG_U32:          *dest = sizeof(u32);           return true;
  case TAG_U64:          *dest = sizeof(u64);           return true;
  case TAG_UNQUOTE:      *dest = sizeof(s_unquote);     return true;
  case TAG_UW:           *dest = sizeof(uw);            return true;
  }
  err_puts("tag_type_size: invalid tag type");
  assert(! "tag_type_size: invalid tag type");
  return false;
}

bool tag_type_to_ffi_type (e_tag_type type, ffi_type **dest)
{
  switch (type) {
  case TAG_ARRAY:        *dest = &ffi_type_pointer;    return true;
  case TAG_DO_BLOCK:     *dest = &ffi_type_pointer;    return true;
  case TAG_BOOL:         *dest = &ffi_type_uint8;      return true;
  case TAG_CHARACTER:    *dest = &ffi_type_uint32;     return true;
  case TAG_F32:          *dest = &ffi_type_float;      return true;
  case TAG_F64:          *dest = &ffi_type_double;     return true;
#if HAVE_F80
  case TAG_F80:          *dest = &ffi_type_double;     return true;
#endif
#if HAVE_F128
  case TAG_F128:         *dest = &ffi_type_longdouble; return true;
#endif
  case TAG_FACT:         *dest = &ffi_type_pointer;    return true;
  case TAG_IDENT:        *dest = &ffi_type_pointer;    return true;
  case TAG_INTEGER:      *dest = &ffi_type_pointer;    return true;
  case TAG_MAP:          *dest = &ffi_type_pointer;    return true;
  case TAG_POINTER:      *dest = &ffi_type_pointer;    return true;
  case TAG_PCALL:        *dest = &ffi_type_pointer;    return true;
  case TAG_PCALLABLE:    *dest = &ffi_type_pointer;    return true;
  case TAG_PCOMPLEX:     *dest = &ffi_type_pointer;    return true;
  case TAG_PCOW:         *dest = &ffi_type_pointer;    return true;
  case TAG_PFACTS:       *dest = &ffi_type_pointer;    return true;
  case TAG_PLIST:        *dest = &ffi_type_pointer;    return true;
  case TAG_PSTRUCT:      *dest = &ffi_type_pointer;    return true;
  case TAG_PSTRUCT_TYPE: *dest = &ffi_type_pointer;    return true;
  case TAG_PTAG:         *dest = &ffi_type_pointer;    return true;
  case TAG_PTR:          *dest = &ffi_type_pointer;    return true;
  case TAG_PTR_FREE:     *dest = &ffi_type_pointer;    return true;
  case TAG_PVAR:         *dest = &ffi_type_pointer;    return true;
  case TAG_QUOTE:        *dest = &ffi_type_pointer;    return true;
  case TAG_RATIO:        *dest = &ffi_type_pointer;    return true;
  case TAG_S8:           *dest = &ffi_type_sint8;      return true;
  case TAG_S16:          *dest = &ffi_type_sint16;     return true;
  case TAG_S32:          *dest = &ffi_type_sint32;     return true;
  case TAG_S64:          *dest = &ffi_type_sint64;     return true;
  case TAG_SW:           *dest = &ffi_type_slong;      return true;
  case TAG_STR:          *dest = &ffi_type_pointer;    return true;
  case TAG_PSYM:         *dest = &ffi_type_pointer;    return true;
  case TAG_TIME :        *dest = &ffi_type_pointer;    return true;
  case TAG_TUPLE:        *dest = &ffi_type_pointer;    return true;
  case TAG_U8:           *dest = &ffi_type_uint8;      return true;
  case TAG_U16:          *dest = &ffi_type_uint16;     return true;
  case TAG_U32:          *dest = &ffi_type_uint32;     return true;
  case TAG_U64:          *dest = &ffi_type_uint64;     return true;
  case TAG_UNQUOTE:      *dest = &ffi_type_pointer;    return true;
  case TAG_UW:           *dest = &ffi_type_ulong;      return true;
  case TAG_VOID:         *dest = &ffi_type_void;       return true;
  }
  err_puts("tag_type_to_ffi_type: invalid tag type");
  assert(! "tag_type_to_ffi_type: invalid tag type");
  return false;
}

const char * tag_type_to_string (e_tag_type tag_type)
{
  switch (tag_type) {
  case TAG_VOID:         return "Void";
  case TAG_ARRAY:        return "Array";
  case TAG_DO_BLOCK:     return "Block";
  case TAG_BOOL:         return "Bool";
  case TAG_CHARACTER:    return "Character";
  case TAG_F32:          return "F32";
  case TAG_F64:          return "F64";
#if HAVE_F80
  case TAG_F80:          return "F80";
#endif
#if HAVE_F128
  case TAG_F128:         return "F128";
#endif
  case TAG_FACT:         return "Fact";
  case TAG_IDENT:        return "Ident";
  case TAG_INTEGER:      return "Integer";
  case TAG_MAP:          return "Map";
  case TAG_PCALL:        return "Call";
  case TAG_PCALLABLE:    return "Callable";
  case TAG_PCOMPLEX:     return "Complex";
  case TAG_PCOW:         return "Cow";
  case TAG_PFACTS:       return "Facts*";
  case TAG_PLIST:        return "List";
  case TAG_POINTER:      return "Pointer";
  case TAG_PSTRUCT:      return "Struct";
  case TAG_PSTRUCT_TYPE: return "StructType";
  case TAG_PTAG:         return "Ptag";
  case TAG_PTR:          return "Ptr";
  case TAG_PTR_FREE:     return "PtrFree";
  case TAG_PVAR:         return "Var";
  case TAG_QUOTE:        return "Quote";
  case TAG_RATIO:        return "Ratio";
  case TAG_S8:           return "S8";
  case TAG_S16:          return "S16";
  case TAG_S32:          return "S32";
  case TAG_S64:          return "S64";
  case TAG_STR:          return "Str";
  case TAG_SW:           return "Sw";
  case TAG_PSYM:          return "Sym";
  case TAG_TIME:         return "Time";
  case TAG_TUPLE:        return "Tuple";
  case TAG_U8:           return "U8";
  case TAG_U16:          return "U16";
  case TAG_U32:          return "U32";
  case TAG_U64:          return "U64";
  case TAG_UNQUOTE:      return "Unquote";
  case TAG_UW:           return "Uw";
  }
  err_puts("tag_type_to_string: invalid tag type");
  assert(! "tag_type_to_string: invalid tag type");
  return NULL;
}
