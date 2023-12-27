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
#include <err.h>
#include "c3.h"

bool tag_type_size (e_tag_type type, uw *dest)
{
  switch (type) {
  case TAG_VOID:      *dest = 0;                 return true;
  case TAG_ARRAY:     *dest = sizeof(s_array);   return true;
  case TAG_BOOL:      *dest = sizeof(bool);      return true;
  case TAG_CALL:      *dest = sizeof(s_call);    return true;
  case TAG_CFN:       *dest = sizeof(s_cfn);     return true;
  case TAG_CHARACTER: *dest = sizeof(character); return true;
  case TAG_F32:       *dest = sizeof(f32);       return true;
  case TAG_F64:       *dest = sizeof(f64);       return true;
  case TAG_FACT:      *dest = sizeof(s_fact);    return true;
  case TAG_FN:        *dest = sizeof(s_fn);      return true;
  case TAG_IDENT:     *dest = sizeof(s_ident);   return true;
  case TAG_INTEGER:   *dest = sizeof(s_integer); return true;
  case TAG_SW:        *dest = sizeof(sw);        return true;
  case TAG_S64:       *dest = sizeof(s64);       return true;
  case TAG_S32:       *dest = sizeof(s32);       return true;
  case TAG_S16:       *dest = sizeof(s16);       return true;
  case TAG_S8:        *dest = sizeof(s8);        return true;
  case TAG_U8:        *dest = sizeof(u8);        return true;
  case TAG_U16:       *dest = sizeof(u16);       return true;
  case TAG_U32:       *dest = sizeof(u32);       return true;
  case TAG_U64:       *dest = sizeof(u64);       return true;
  case TAG_UW:        *dest = sizeof(uw);        return true;
  case TAG_LIST:      *dest = sizeof(s_list *);  return true;
  case TAG_MAP:       *dest = sizeof(s_map);     return true;
  case TAG_PTAG:      *dest = sizeof(p_tag);     return true;
  case TAG_PTR:
  case TAG_PTR_FREE:  *dest = sizeof(void *);    return true;
  case TAG_QUOTE:     *dest = sizeof(s_quote);   return true;
  case TAG_STR:       *dest = sizeof(s_str);     return true;
  case TAG_STRUCT:    *dest = sizeof(s_struct);  return true;
  case TAG_SYM:       *dest = sizeof(s_sym *);   return true;
  case TAG_TUPLE:     *dest = sizeof(s_tuple);   return true;
  case TAG_VAR:       *dest = 0;                 return true;
  }
  warnx("tag_type_size: invalid tag type: %d", type);
  assert(! "tag_type_size: invalid tag type");
  return false;
}

bool tag_type_to_buf_inspect (e_tag_type type, f_buf_inspect *dest)
{
  switch (type) {
  case TAG_VOID:
    *dest = (f_buf_inspect) buf_inspect_void;      return true;
  case TAG_ARRAY:
    *dest = (f_buf_inspect) buf_inspect_array;     return true;
  case TAG_BOOL:
    *dest = (f_buf_inspect) buf_inspect_bool;      return true;
  case TAG_CALL:
    *dest = (f_buf_inspect) buf_inspect_call;      return true;
  case TAG_CFN:
    *dest = (f_buf_inspect) buf_inspect_cfn;       return true;
  case TAG_CHARACTER:
    *dest = (f_buf_inspect) buf_inspect_character; return true;
  case TAG_F32:
    *dest = (f_buf_inspect) buf_inspect_f32;       return true;
  case TAG_F64:
    *dest = (f_buf_inspect) buf_inspect_f64;       return true;
  case TAG_FACT:
    *dest = (f_buf_inspect) buf_inspect_fact;      return true;
  case TAG_FN:
    *dest = (f_buf_inspect) buf_inspect_fn;        return true;
  case TAG_IDENT:
    *dest = (f_buf_inspect) buf_inspect_ident;     return true;
  case TAG_INTEGER:
    *dest = (f_buf_inspect) buf_inspect_integer;   return true;
  case TAG_SW:
    *dest = (f_buf_inspect) buf_inspect_sw;        return true;
  case TAG_S64:
    *dest = (f_buf_inspect) buf_inspect_s64;       return true;
  case TAG_S32:
    *dest = (f_buf_inspect) buf_inspect_s32;       return true;
  case TAG_S16:
    *dest = (f_buf_inspect) buf_inspect_s16;       return true;
  case TAG_S8:
    *dest = (f_buf_inspect) buf_inspect_s8;        return true;
  case TAG_U8:
    *dest = (f_buf_inspect) buf_inspect_u8;        return true;
  case TAG_U16:
    *dest = (f_buf_inspect) buf_inspect_u16;       return true;
  case TAG_U32:
    *dest = (f_buf_inspect) buf_inspect_u32;       return true;
  case TAG_U64:
    *dest = (f_buf_inspect) buf_inspect_u64;       return true;
  case TAG_UW:
    *dest = (f_buf_inspect) buf_inspect_uw;        return true;
  case TAG_LIST:
    *dest = (f_buf_inspect) buf_inspect_list;      return true;
  case TAG_MAP:
    *dest = (f_buf_inspect) buf_inspect_map;       return true;
  case TAG_PTAG:
    *dest = (f_buf_inspect) buf_inspect_ptag;      return true;
  case TAG_PTR:
    *dest = (f_buf_inspect) buf_inspect_ptr;       return true;
  case TAG_PTR_FREE:
    *dest = (f_buf_inspect) buf_inspect_ptr_free;  return true;
  case TAG_QUOTE:
    *dest = (f_buf_inspect) buf_inspect_quote;     return true;
  case TAG_STR:
    *dest = (f_buf_inspect) buf_inspect_str;       return true;
  case TAG_STRUCT:
    *dest = (f_buf_inspect) buf_inspect_struct;    return true;
  case TAG_SYM:
    *dest = (f_buf_inspect) buf_inspect_sym;       return true;
  case TAG_TUPLE:
    *dest = (f_buf_inspect) buf_inspect_tuple;     return true;
  case TAG_VAR:
    *dest = (f_buf_inspect) buf_inspect_var;       return true;
  }
  warnx("buf_inspect: unknown tag type: %d", type);
  assert(! "buf_inspect: unknown tag type");
  return false;
}

bool tag_type_to_buf_inspect_size (e_tag_type type,
                                   f_buf_inspect_size *p)
{
  switch (type) {
  case TAG_VOID:
    *p = (f_buf_inspect_size) buf_inspect_void_size;        return true;
  case TAG_ARRAY:
    *p = (f_buf_inspect_size) buf_inspect_array_size;       return true;
  case TAG_BOOL:
    *p = (f_buf_inspect_size) buf_inspect_bool_size;        return true;
  case TAG_CALL:
    *p = (f_buf_inspect_size) buf_inspect_call_size;        return true;
  case TAG_CFN:
     *p = (f_buf_inspect_size) buf_inspect_cfn_size;        return true;
  case TAG_CHARACTER:
    *p = (f_buf_inspect_size) buf_inspect_character_size;   return true;
  case TAG_F32:
     *p = (f_buf_inspect_size) buf_inspect_f32_size;        return true;
  case TAG_F64:
     *p = (f_buf_inspect_size) buf_inspect_f64_size;        return true;
  case TAG_FACT:
    *p = (f_buf_inspect_size) buf_inspect_fact_size;        return true;
  case TAG_FN:
      *p = (f_buf_inspect_size) buf_inspect_fn_size;        return true;
  case TAG_IDENT:
   *p = (f_buf_inspect_size) buf_inspect_ident_size;        return true;
  case TAG_INTEGER:
    *p = (f_buf_inspect_size) buf_inspect_integer_size;     return true;
  case TAG_SW:
      *p = (f_buf_inspect_size) buf_inspect_sw_size;        return true;
  case TAG_S64:
     *p = (f_buf_inspect_size) buf_inspect_s64_size;        return true;
  case TAG_S32:
    *p = (f_buf_inspect_size) buf_inspect_s32_size;         return true;
  case TAG_S16:
    *p = (f_buf_inspect_size) buf_inspect_s16_size;         return true;
  case TAG_S8:
    *p = (f_buf_inspect_size) buf_inspect_s8_size;          return true;
  case TAG_U8:
    *p = (f_buf_inspect_size) buf_inspect_u8_size;          return true;
  case TAG_U16:
    *p = (f_buf_inspect_size) buf_inspect_u16_size;         return true;
  case TAG_U32:
    *p = (f_buf_inspect_size) buf_inspect_u32_size;         return true;
  case TAG_U64:
    *p = (f_buf_inspect_size) buf_inspect_u64_size;         return true;
  case TAG_UW:
    *p = (f_buf_inspect_size) buf_inspect_uw_size;          return true;
  case TAG_LIST:
    *p = (f_buf_inspect_size) buf_inspect_list_size;        return true;
  case TAG_MAP:
    *p = (f_buf_inspect_size) buf_inspect_map_size;         return true;
  case TAG_PTAG:
    *p = (f_buf_inspect_size) buf_inspect_ptag_size;        return true;
  case TAG_PTR:
    *p = (f_buf_inspect_size) buf_inspect_ptr_size;         return true;
  case TAG_PTR_FREE:
    *p = (f_buf_inspect_size) buf_inspect_ptr_free_size;    return true;
  case TAG_QUOTE:
    *p = (f_buf_inspect_size) buf_inspect_quote_size;       return true;
  case TAG_STR:
    *p = (f_buf_inspect_size) buf_inspect_str_size;         return true;
  case TAG_STRUCT:
    *p = (f_buf_inspect_size) buf_inspect_struct_size;      return true;
  case TAG_SYM:
    *p = (f_buf_inspect_size) buf_inspect_sym_size;         return true;
  case TAG_TUPLE:
    *p = (f_buf_inspect_size) buf_inspect_tuple_size;       return true;
  case TAG_VAR:
    *p = (f_buf_inspect_size) buf_inspect_var_size;         return true;
  }
  warnx("tag_type_to_buf_inspect_size: unknown tag type: %d", type);
  assert(! "tag_type_to_buf_inspect_size: unknown tag type");
  return false;
}

bool tag_type_to_buf_parse (e_tag_type type, f_buf_parse *p)
{
  switch (type) {
  case TAG_VOID:      *p = (f_buf_parse) buf_parse_void;    return true;
  case TAG_ARRAY:     *p = (f_buf_parse) buf_parse_array;   return true;
  case TAG_BOOL:      *p = (f_buf_parse) buf_parse_bool;    return true;
  case TAG_CALL:      *p = (f_buf_parse) buf_parse_call;    return true;
  case TAG_CFN:       *p = (f_buf_parse) buf_parse_cfn;     return true;
  case TAG_CHARACTER: *p = (f_buf_parse) buf_parse_character;  return 1;
  case TAG_F32:       *p = (f_buf_parse) buf_parse_f32;     return true;
  case TAG_F64:       *p = (f_buf_parse) buf_parse_f64;     return true;
  case TAG_FACT:      *p = (f_buf_parse) buf_parse_fact;    return true;
  case TAG_FN:        *p = (f_buf_parse) buf_parse_fn;      return true;
  case TAG_IDENT:     *p = (f_buf_parse) buf_parse_ident;   return true;
  case TAG_INTEGER:   *p = (f_buf_parse) buf_parse_integer; return true;
  case TAG_SW:        *p = (f_buf_parse) buf_parse_sw;      return true;
  case TAG_S64:       *p = (f_buf_parse) buf_parse_s64;     return true;
  case TAG_S32:       *p = (f_buf_parse) buf_parse_s32;     return true;
  case TAG_S16:       *p = (f_buf_parse) buf_parse_s16;     return true;
  case TAG_S8:        *p = (f_buf_parse) buf_parse_s8;      return true;
  case TAG_U8:        *p = (f_buf_parse) buf_parse_u8;      return true;
  case TAG_U16:       *p = (f_buf_parse) buf_parse_u16;     return true;
  case TAG_U32:       *p = (f_buf_parse) buf_parse_u32;     return true;
  case TAG_U64:       *p = (f_buf_parse) buf_parse_u64;     return true;
  case TAG_UW:        *p = (f_buf_parse) buf_parse_uw;      return true;
  case TAG_LIST:      *p = (f_buf_parse) buf_parse_list;    return true;
  case TAG_MAP:       *p = (f_buf_parse) buf_parse_map;     return true;
  case TAG_PTAG:      *p = (f_buf_parse) buf_parse_ptag;    return true;
  case TAG_PTR:       *p = (f_buf_parse) buf_parse_ptr;     return true;
  case TAG_PTR_FREE:  *p = (f_buf_parse) buf_parse_ptr_free;   return 1;
  case TAG_QUOTE:     *p = (f_buf_parse) buf_parse_quote;   return true;
  case TAG_STR:       *p = (f_buf_parse) buf_parse_str;     return true;
  case TAG_STRUCT:    *p = (f_buf_parse) buf_parse_struct;  return true;
  case TAG_SYM:       *p = (f_buf_parse) buf_parse_sym;     return true;
  case TAG_TUPLE:     *p = (f_buf_parse) buf_parse_tuple;   return true;
  case TAG_VAR:       *p = (f_buf_parse) buf_parse_var;     return true;
  }
  warnx("tag_type_to_buf_parse: invalid tag type: %d", type);
  assert(! "tag_type_to_buf_parse: invalid tag type");
  return false;
}

bool tag_type_to_env_eval (e_tag_type type, f_env_eval *dest)
{
  switch (type) {
  case TAG_VOID:   *dest = (f_env_eval) env_eval_void;      return true;
  case TAG_ARRAY:  *dest = (f_env_eval) env_eval_array_tag; return true;
  case TAG_CALL:   *dest = (f_env_eval) env_eval_call;      return true;
  case TAG_IDENT:  *dest = (f_env_eval) env_eval_ident;     return true;
  case TAG_LIST:   *dest = (f_env_eval) env_eval_list;      return true;
  case TAG_MAP:    *dest = (f_env_eval) env_eval_map;       return true;
  case TAG_QUOTE:  *dest = (f_env_eval) env_eval_quote;     return true;
  case TAG_STRUCT: *dest = (f_env_eval) env_eval_struct;    return true;
  case TAG_TUPLE:  *dest = (f_env_eval) env_eval_tuple;     return true;
  case TAG_BOOL:
  case TAG_CFN:
  case TAG_CHARACTER:
  case TAG_F32:
  case TAG_F64:
  case TAG_FACT:
  case TAG_FN:
  case TAG_INTEGER:
  case TAG_PTAG:
  case TAG_PTR:
  case TAG_PTR_FREE:
  case TAG_S8:
  case TAG_S16:
  case TAG_S32:
  case TAG_S64:
  case TAG_SW:
  case TAG_STR:
  case TAG_SYM:
  case TAG_U8:
  case TAG_U16:
  case TAG_U32:
  case TAG_U64:
  case TAG_UW:
  case TAG_VAR:    *dest = NULL;                            return true;
  }
  warnx("tag_type_to_env_eval: unknown tag type: %d", type);
  assert(! "tag_type_to_env_eval: unknown tag type");
  return false;
}

bool tag_type_to_ffi_type (e_tag_type type, ffi_type **dest)
{
  switch (type) {
  case TAG_ARRAY:     *dest = &ffi_type_pointer; return true;
  case TAG_BOOL:      *dest = &ffi_type_uint8;   return true;
  case TAG_CALL:      *dest = &ffi_type_pointer; return true;
  case TAG_CFN:       *dest = &ffi_type_pointer; return true;
  case TAG_CHARACTER: *dest = &ffi_type_schar;   return true;
  case TAG_F32:       *dest = &ffi_type_float;   return true;
  case TAG_F64:       *dest = &ffi_type_double;  return true;
  case TAG_FACT:      *dest = &ffi_type_pointer; return true;
  case TAG_FN:        *dest = &ffi_type_pointer; return true;
  case TAG_IDENT:     *dest = &ffi_type_pointer; return true;
  case TAG_INTEGER:   *dest = &ffi_type_sint;    return true;
  case TAG_LIST:      *dest = &ffi_type_pointer; return true;
  case TAG_MAP:       *dest = &ffi_type_pointer; return true;
  case TAG_PTAG:      *dest = &ffi_type_pointer; return true;
  case TAG_PTR:       *dest = &ffi_type_pointer; return true;
  case TAG_PTR_FREE:  *dest = &ffi_type_pointer; return true;
  case TAG_QUOTE:     *dest = &ffi_type_pointer; return true;
  case TAG_S8:        *dest = &ffi_type_sint8;   return true;
  case TAG_S16:       *dest = &ffi_type_sint16;  return true;
  case TAG_S32:       *dest = &ffi_type_sint32;  return true;
  case TAG_S64:       *dest = &ffi_type_sint64;  return true;
  case TAG_SW:        *dest = &ffi_type_slong;   return true;
  case TAG_STR:       *dest = &ffi_type_pointer; return true;
  case TAG_STRUCT:    *dest = &ffi_type_pointer; return true;
  case TAG_SYM:       *dest = &ffi_type_pointer; return true;
  case TAG_TUPLE:     *dest = &ffi_type_pointer; return true;
  case TAG_U8:        *dest = &ffi_type_uint8;   return true;
  case TAG_U16:       *dest = &ffi_type_uint16;  return true;
  case TAG_U32:       *dest = &ffi_type_uint32;  return true;
  case TAG_U64:       *dest = &ffi_type_uint64;  return true;
  case TAG_UW:        *dest = &ffi_type_ulong;   return true;
  case TAG_VAR:       *dest = &ffi_type_pointer; return true;
  case TAG_VOID:      *dest = &ffi_type_void;    return true;
  }
  warnx("tag_type_to_ffi_type: unknown tag type: %d", type);
  assert(! "tag_type_to_ffi_type: unknown tag type");
  return false;
}

bool tag_type_to_hash_update (e_tag_type type, f_hash_update *p)
{
  switch (type) {
  case TAG_VOID:   *p = (f_hash_update) hash_update_void;   return true;
  case TAG_ARRAY:  *p = (f_hash_update) hash_update_array;  return true;
  case TAG_BOOL:   *p = (f_hash_update) hash_update_bool;   return true;
  case TAG_CALL:   *p = (f_hash_update) hash_update_call;   return true;
  case TAG_CFN:    *p = (f_hash_update) hash_update_cfn;    return true;
  case TAG_CHARACTER:
    *p = (f_hash_update) hash_update_character;             return true;
  case TAG_F32:    *p = (f_hash_update) hash_update_f32;    return true;
  case TAG_F64:    *p = (f_hash_update) hash_update_f64;    return true;
  case TAG_FACT:   *p = (f_hash_update) hash_update_fact;   return true;
  case TAG_FN:     *p = (f_hash_update) hash_update_fn;     return true;
  case TAG_IDENT:  *p = (f_hash_update) hash_update_ident;  return true;
  case TAG_INTEGER:
    *p = (f_hash_update) hash_update_integer;               return true;
  case TAG_SW:     *p = (f_hash_update) hash_update_sw;     return true;
  case TAG_S64:    *p = (f_hash_update) hash_update_s64;    return true;
  case TAG_S32:    *p = (f_hash_update) hash_update_s32;    return true;
  case TAG_S16:    *p = (f_hash_update) hash_update_s16;    return true;
  case TAG_S8:     *p = (f_hash_update) hash_update_s8;     return true;
  case TAG_U8:     *p = (f_hash_update) hash_update_u8;     return true;
  case TAG_U16:    *p = (f_hash_update) hash_update_u16;    return true;
  case TAG_U32:    *p = (f_hash_update) hash_update_u32;    return true;
  case TAG_U64:    *p = (f_hash_update) hash_update_u64;    return true;
  case TAG_UW:     *p = (f_hash_update) hash_update_uw;     return true;
  case TAG_LIST:   *p = (f_hash_update) hash_update_list;   return true;
  case TAG_MAP:    *p = (f_hash_update) hash_update_map;    return true;
  case TAG_PTAG:   *p = (f_hash_update) hash_update_ptag;   return true;
  case TAG_PTR:    *p = (f_hash_update) hash_update_ptr;    return true;
  case TAG_PTR_FREE:
    *p = (f_hash_update) hash_update_ptr_free;              return true;
  case TAG_QUOTE:  *p = (f_hash_update) hash_update_quote;  return true;
  case TAG_STR:    *p = (f_hash_update) hash_update_str;    return true;
  case TAG_STRUCT: *p = (f_hash_update) hash_update_struct; return true;
  case TAG_SYM:    *p = (f_hash_update) hash_update_sym;    return true;
  case TAG_TUPLE:  *p = (f_hash_update) hash_update_tuple;  return true;
  case TAG_VAR:    *p = (f_hash_update) hash_update_var;    return true;
  }
  warnx("tag_type_to_hash_update: unknown tag type: %d", type);
  assert(! "tag_type_to_hash_update: unknown tag type");
  return false;
}

bool tag_type_to_init_cast (e_tag_type type, f_init_cast *p)
{
  switch (type) {
  case TAG_VOID:     *p = NULL;                             return true;
  case TAG_ARRAY:    *p = (f_init_cast) array_init_cast;    return true;
  case TAG_BOOL:     *p = (f_init_cast) bool_init_cast;     return true;
  case TAG_CALL:     *p = (f_init_cast) call_init_cast;     return true;
  case TAG_CFN:      *p = (f_init_cast) cfn_init_cast;      return true;
  case TAG_CHARACTER:
    *p = (f_init_cast) character_init_cast;                 return true;
  case TAG_F32:      *p = (f_init_cast) f32_init_cast;      return true;
  case TAG_F64:      *p = (f_init_cast) f64_init_cast;      return true;
  case TAG_FACT:     *p = (f_init_cast) fact_init_cast;     return true;
  case TAG_FN:       *p = (f_init_cast) fn_init_cast;       return true;
  case TAG_IDENT:    *p = (f_init_cast) ident_init_cast;    return true;
  case TAG_INTEGER:  *p = (f_init_cast) integer_init_cast;  return true;
  case TAG_SW:       *p = (f_init_cast) sw_init_cast;       return true;
  case TAG_S64:      *p = (f_init_cast) s64_init_cast;      return true;
  case TAG_S32:      *p = (f_init_cast) s32_init_cast;      return true;
  case TAG_S16:      *p = (f_init_cast) s16_init_cast;      return true;
  case TAG_S8:       *p = (f_init_cast) s8_init_cast;       return true;
  case TAG_U8:       *p = (f_init_cast) u8_init_cast;       return true;
  case TAG_U16:      *p = (f_init_cast) u16_init_cast;      return true;
  case TAG_U32:      *p = (f_init_cast) u32_init_cast;      return true;
  case TAG_U64:      *p = (f_init_cast) u64_init_cast;      return true;
  case TAG_UW:       *p = (f_init_cast) uw_init_cast;       return true;
  case TAG_LIST:     *p = (f_init_cast) list_init_cast;     return true;
  case TAG_MAP:      *p = (f_init_cast) map_init_cast;      return true;
  case TAG_PTAG:     *p = (f_init_cast) ptag_init_cast;     return true;
  case TAG_PTR:      *p = (f_init_cast) ptr_init_cast;      return true;
  case TAG_PTR_FREE: *p = (f_init_cast) ptr_free_init_cast; return true;
  case TAG_QUOTE:    *p = (f_init_cast) quote_init_cast;    return true;
  case TAG_STR:      *p = (f_init_cast) str_init_cast;      return true;
  case TAG_STRUCT:   *p = (f_init_cast) struct_init_cast;   return true;
  case TAG_SYM:      *p = (f_init_cast) sym_init_cast;      return true;
  case TAG_TUPLE:    *p = (f_init_cast) tuple_init_cast;    return true;
  case TAG_VAR:      *p = NULL;                             return true;
  }
  warnx("tag_type_to_init_cast: invalid tag type: %d", type);
  assert(! "tag_type_to_init_cast: invalid tag type");
  return false;
}

bool tag_type_to_init_copy (e_tag_type type, f_init_copy *p)
{
  switch (type) {
  case TAG_ARRAY:     *p = (f_init_copy) array_init_copy;   return true;
  case TAG_BOOL:      *p = (f_init_copy) bool_init_copy;    return true;
  case TAG_CALL:      *p = (f_init_copy) call_init_copy;    return true;
  case TAG_CFN:       *p = (f_init_copy) cfn_init_copy;     return true;
  case TAG_CHARACTER: *p = (f_init_copy) character_init_copy;  return 1;
  case TAG_F32:       *p = (f_init_copy) f32_init_copy;     return true;
  case TAG_F64:       *p = (f_init_copy) f64_init_copy;     return true;
  case TAG_FACT:      *p = (f_init_copy) fact_init_copy;    return true;
  case TAG_FN:        *p = (f_init_copy) fn_init_copy;      return true;
  case TAG_IDENT:     *p = (f_init_copy) ident_init_copy;   return true;
  case TAG_INTEGER:   *p = (f_init_copy) integer_init_copy; return true;
  case TAG_SW:        *p = (f_init_copy) sw_init_copy;      return true;
  case TAG_S64:       *p = (f_init_copy) s64_init_copy;     return true;
  case TAG_S32:       *p = (f_init_copy) s32_init_copy;     return true;
  case TAG_S16:       *p = (f_init_copy) s16_init_copy;     return true;
  case TAG_S8:        *p = (f_init_copy) s8_init_copy;      return true;
  case TAG_U8:        *p = (f_init_copy) u8_init_copy;      return true;
  case TAG_U16:       *p = (f_init_copy) u16_init_copy;     return true;
  case TAG_U32:       *p = (f_init_copy) u32_init_copy;     return true;
  case TAG_U64:       *p = (f_init_copy) u64_init_copy;     return true;
  case TAG_UW:        *p = (f_init_copy) uw_init_copy;      return true;
  case TAG_LIST:      *p = (f_init_copy) list_init_copy;    return true;
  case TAG_MAP:       *p = (f_init_copy) map_init_copy;     return true;
  case TAG_PTAG:      *p = (f_init_copy) ptag_init_copy;    return true;
  case TAG_PTR:       *p = (f_init_copy) ptr_init_copy;     return true;
  case TAG_PTR_FREE:  *p = (f_init_copy) ptr_free_init_copy;   return 1;
  case TAG_QUOTE:     *p = (f_init_copy) quote_init_copy;   return true;
  case TAG_STR:       *p = (f_init_copy) str_init_copy;     return true;
  case TAG_STRUCT:    *p = (f_init_copy) struct_init_copy;  return true;
  case TAG_SYM:       *p = (f_init_copy) sym_init_copy;     return true;
  case TAG_TUPLE:     *p = (f_init_copy) tuple_init_copy;   return true;
  case TAG_VAR:       *p = NULL;                            return true;
  case TAG_VOID:      *p = NULL;                            return true;
  }
  warnx("tag_type_to_init_copy: invalid tag type: %d", type);
  assert(! "tag_type_to_init_copy: invalid tag type");
  return false;
}

const s8 * tag_type_to_string (e_tag_type type)
{
  const s_sym *sym;
  if (! (sym = tag_type_to_sym(type)))
    return NULL;
  return sym->str.ptr.ps8;
}

const s_sym * tag_type_to_sym (e_tag_type tag_type)
{
  switch (tag_type) {
  case TAG_VOID:       return sym_1("Void");
  case TAG_ARRAY:      return sym_1("Array");
  case TAG_BOOL:       return sym_1("Bool");
  case TAG_CALL:       return sym_1("Call");
  case TAG_CFN:        return sym_1("Cfn");
  case TAG_CHARACTER:  return sym_1("Character");
  case TAG_F32:        return sym_1("F32");
  case TAG_F64:        return sym_1("F64");
  case TAG_FACT:       return sym_1("Fact");
  case TAG_FN:         return sym_1("Fn");
  case TAG_IDENT:      return sym_1("Ident");
  case TAG_INTEGER:    return sym_1("Integer");
  case TAG_SW:         return sym_1("Sw");
  case TAG_S64:        return sym_1("S64");
  case TAG_S32:        return sym_1("S32");
  case TAG_S16:        return sym_1("S16");
  case TAG_S8:         return sym_1("S8");
  case TAG_U8:         return sym_1("U8");
  case TAG_U16:        return sym_1("U16");
  case TAG_U32:        return sym_1("U32");
  case TAG_U64:        return sym_1("U64");
  case TAG_UW:         return sym_1("Uw");
  case TAG_LIST:       return sym_1("List");
  case TAG_MAP:        return sym_1("Map");
  case TAG_PTAG:       return sym_1("Ptag");
  case TAG_PTR:        return sym_1("Ptr");
  case TAG_PTR_FREE:   return sym_1("PtrFree");
  case TAG_QUOTE:      return sym_1("Quote");
  case TAG_STR:        return sym_1("Str");
  case TAG_STRUCT:     return sym_1("Struct");
  case TAG_SYM:        return sym_1("Sym");
  case TAG_TUPLE:      return sym_1("Tuple");
  case TAG_VAR:        return sym_1("Var");
  }
  assert(! "tag_type_to_sym: invalid tag type");
  errx(1, "tag_type_to_sym: invalid tag type: %d", tag_type);
  return NULL;
}
