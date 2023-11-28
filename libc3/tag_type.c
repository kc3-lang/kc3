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
#include <assert.h>
#include <err.h>
#include "buf_inspect.h"
#include "buf_parse.h"
#include "sym.h"
#include "tag_type.h"

sw tag_type_size (e_tag_type type)
{
  switch (type) {
  case TAG_VOID:
    return 0;
  case TAG_ARRAY:
    assert(! "tag_type_size: TAG_ARRAY: not implemented");
    errx(1, "tag_type_size: TAG_ARRAY: not implemented");
    return -1;
  case TAG_BOOL:
    return sizeof(bool);
  case TAG_CALL:
    return sizeof(s_call);
  case TAG_CFN:
    return sizeof(s_cfn);
  case TAG_CHARACTER:
    return sizeof(character);
  case TAG_F32:
    return sizeof(f32);
  case TAG_F64:
    return sizeof(f64);
  case TAG_FACT:
    return sizeof(s_fact);
  case TAG_FN:
    return sizeof(s_fn);
  case TAG_IDENT:
    return sizeof(s_ident);
  case TAG_INTEGER:
    return sizeof(s_integer);
  case TAG_SW:
    return sizeof(sw);
  case TAG_S64:
    return sizeof(s64);
  case TAG_S32:
    return sizeof(s32);
  case TAG_S16:
    return sizeof(s16);
  case TAG_S8:
    return sizeof(s8);
  case TAG_U8:
    return sizeof(u8);
  case TAG_U16:
    return sizeof(u16);
  case TAG_U32:
    return sizeof(u32);
  case TAG_U64:
    return sizeof(u64);
  case TAG_UW:
    return sizeof(uw);
  case TAG_LIST:
    return sizeof(s_list *);
  case TAG_MAP:
    return sizeof(s_map);
  case TAG_PTAG:
    return sizeof(p_tag);
  case TAG_PTR:
    return sizeof(void *);
  case TAG_QUOTE:
    return sizeof(s_quote);
  case TAG_STR:
    return sizeof(s_str);
  case TAG_SYM:
    return sizeof(s_sym *);
  case TAG_TUPLE:
    return sizeof(s_tuple);
  case TAG_VAR:
    return sizeof(s_tag);
  }
  assert(! "tag_type_size: invalid tag type");
  errx(1, "tag_type_size: invalid tag type: %d", type);
  return -1;
}

f_buf_inspect tag_type_to_buf_inspect (e_tag_type type)
{
  switch (type) {
  case TAG_VOID:
    return (f_buf_inspect) buf_inspect_void;
  case TAG_ARRAY:
  case TAG_BOOL:
    return (f_buf_inspect) buf_inspect_bool;
  case TAG_CALL:
    return (f_buf_inspect) buf_inspect_call;
  case TAG_CFN:
    return (f_buf_inspect) buf_inspect_cfn;
  case TAG_CHARACTER:
    return (f_buf_inspect) buf_inspect_character;
  case TAG_F32:
    return (f_buf_inspect) buf_inspect_f32;
  case TAG_F64:
    return (f_buf_inspect) buf_inspect_f64;
  case TAG_FACT:
    return (f_buf_inspect) buf_inspect_fact;
  case TAG_FN:
    return (f_buf_inspect) buf_inspect_fn;
  case TAG_IDENT:
    return (f_buf_inspect) buf_inspect_ident;
  case TAG_INTEGER:
    return (f_buf_inspect) buf_inspect_integer;
  case TAG_SW:
    return (f_buf_inspect) buf_inspect_sw;
  case TAG_S64:
    return (f_buf_inspect) buf_inspect_s64;
  case TAG_S32:
    return (f_buf_inspect) buf_inspect_s32;
  case TAG_S16:
    return (f_buf_inspect) buf_inspect_s16;
  case TAG_S8:
    return (f_buf_inspect) buf_inspect_s8;
  case TAG_U8:
    return (f_buf_inspect) buf_inspect_u8;
  case TAG_U16:
    return (f_buf_inspect) buf_inspect_u16;
  case TAG_U32:
    return (f_buf_inspect) buf_inspect_u32;
  case TAG_U64:
    return (f_buf_inspect) buf_inspect_u64;
  case TAG_UW:
    return (f_buf_inspect) buf_inspect_uw;
  case TAG_LIST:
    return (f_buf_inspect) buf_inspect_list;
  case TAG_MAP:
    return (f_buf_inspect) buf_inspect_map;
  case TAG_PTAG:
    return (f_buf_inspect) buf_inspect_ptag;
  case TAG_PTR:
    return (f_buf_inspect) buf_inspect_ptr;
  case TAG_QUOTE:
    return (f_buf_inspect) buf_inspect_quote;
  case TAG_STR:
    return (f_buf_inspect) buf_inspect_str;
  case TAG_SYM:
    return (f_buf_inspect) buf_inspect_sym;
  case TAG_TUPLE:
    return (f_buf_inspect) buf_inspect_tuple;
  case TAG_VAR:
    return (f_buf_inspect) buf_inspect_var;
  }
  assert(! "buf_inspect: unknown tag type");
  errx(1, "buf_inspect: unknown tag type");
  return NULL;
}

f_buf_inspect_size tag_type_to_buf_inspect_size (e_tag_type type)
{
  switch (type) {
  case TAG_VOID:
    return (f_buf_inspect_size) buf_inspect_void_size;
  case TAG_ARRAY:
    return (f_buf_inspect_size) buf_inspect_array_size;
  case TAG_BOOL:
    return (f_buf_inspect_size) buf_inspect_bool_size;
  case TAG_CALL:
    return (f_buf_inspect_size) buf_inspect_call_size;
  case TAG_CFN:
    return (f_buf_inspect_size) buf_inspect_cfn_size;
  case TAG_CHARACTER:
    return (f_buf_inspect_size) buf_inspect_character_size;
  case TAG_F32:
    return (f_buf_inspect_size) buf_inspect_f32_size;
  case TAG_F64:
    return (f_buf_inspect_size) buf_inspect_f64_size;
  case TAG_FACT:
    return (f_buf_inspect_size) buf_inspect_fact_size;
  case TAG_FN:
    return (f_buf_inspect_size) buf_inspect_fn_size;
  case TAG_IDENT:
    return (f_buf_inspect_size) buf_inspect_ident_size;
  case TAG_INTEGER:
    return (f_buf_inspect_size) buf_inspect_integer_size;
  case TAG_SW:
    return (f_buf_inspect_size) buf_inspect_sw_size;
  case TAG_S64:
    return (f_buf_inspect_size) buf_inspect_s64_size;
  case TAG_S32:
    return (f_buf_inspect_size) buf_inspect_s32_size;
  case TAG_S16:
    return (f_buf_inspect_size) buf_inspect_s16_size;
  case TAG_S8:
    return (f_buf_inspect_size) buf_inspect_s8_size;
  case TAG_U8:
    return (f_buf_inspect_size) buf_inspect_u8_size;
  case TAG_U16:
    return (f_buf_inspect_size) buf_inspect_u16_size;
  case TAG_U32:
    return (f_buf_inspect_size) buf_inspect_u32_size;
  case TAG_U64:
    return (f_buf_inspect_size) buf_inspect_u64_size;
  case TAG_UW:
    return (f_buf_inspect_size) buf_inspect_uw_size;
  case TAG_LIST:
    return (f_buf_inspect_size) buf_inspect_list_size;
  case TAG_MAP:
    return (f_buf_inspect_size) buf_inspect_map_size;
  case TAG_PTAG:
    return (f_buf_inspect_size) buf_inspect_ptag_size;
  case TAG_PTR:
    return (f_buf_inspect_size) buf_inspect_ptr_size;
  case TAG_QUOTE:
    return (f_buf_inspect_size) buf_inspect_quote_size;
  case TAG_STR:
    return (f_buf_inspect_size) buf_inspect_str_size;
  case TAG_SYM:
    return (f_buf_inspect_size) buf_inspect_sym_size;
  case TAG_TUPLE:
    return (f_buf_inspect_size) buf_inspect_tuple_size;
  case TAG_VAR:
    return (f_buf_inspect_size) buf_inspect_var_size;
  }
  assert(! "tag_type_to_buf_inspect_size: unknown tag type");
  errx(1, "tag_type_to_buf_inspect_size: unknown tag type");
  return NULL;
}

f_buf_parse tag_type_to_buf_parse (e_tag_type type)
{
  switch (type) {
  case TAG_VOID:
    return (f_buf_parse) buf_parse_void;
  case TAG_ARRAY:
    return (f_buf_parse) buf_parse_array;
  case TAG_BOOL:
    return (f_buf_parse) buf_parse_bool;
  case TAG_CALL:
    return (f_buf_parse) buf_parse_call;
  case TAG_CFN:
    return (f_buf_parse) buf_parse_cfn;
  case TAG_CHARACTER:
    return (f_buf_parse) buf_parse_character;
  case TAG_F32:
    return (f_buf_parse) buf_parse_f32;
  case TAG_F64:
    return (f_buf_parse) buf_parse_f64;
  case TAG_FACT:
    return (f_buf_parse) buf_parse_fact;
  case TAG_FN:
    return (f_buf_parse) buf_parse_fn;
  case TAG_IDENT:
    return (f_buf_parse) buf_parse_ident;
  case TAG_INTEGER:
    return (f_buf_parse) buf_parse_integer;
  case TAG_SW:
    return (f_buf_parse) buf_parse_sw;
  case TAG_S64:
    return (f_buf_parse) buf_parse_s64;
  case TAG_S32:
    return (f_buf_parse) buf_parse_s32;
  case TAG_S16:
    return (f_buf_parse) buf_parse_s16;
  case TAG_S8:
    return (f_buf_parse) buf_parse_s8;
  case TAG_U8:
    return (f_buf_parse) buf_parse_u8;
  case TAG_U16:
    return (f_buf_parse) buf_parse_u16;
  case TAG_U32:
    return (f_buf_parse) buf_parse_u32;
  case TAG_U64:
    return (f_buf_parse) buf_parse_u64;
  case TAG_UW:
    return (f_buf_parse) buf_parse_uw;
  case TAG_LIST:
    return (f_buf_parse) buf_parse_list;
  case TAG_MAP:
    return (f_buf_parse) buf_parse_map;
  case TAG_PTAG:
    return (f_buf_parse) buf_parse_ptag;
  case TAG_PTR:
    return (f_buf_parse) buf_parse_ptr;
  case TAG_QUOTE:
    return (f_buf_parse) buf_parse_quote;
  case TAG_STR:
    return (f_buf_parse) buf_parse_str;
  case TAG_SYM:
    return (f_buf_parse) buf_parse_sym;
  case TAG_TUPLE:
    return (f_buf_parse) buf_parse_tuple;
  case TAG_VAR:
    return (f_buf_parse) buf_parse_var;
  }
  assert(! "tag_type_to_buf_parse: invalid tag type");
  err(1, "tag_type_to_buf_parse: invalid tag type");
  return NULL;
}

ffi_type * tag_type_to_ffi_type (e_tag_type type)
{
  switch (type) {
  case TAG_ARRAY:
    return &ffi_type_pointer;
  case TAG_BOOL:
    return &ffi_type_uint8;
  case TAG_CALL:
    return &ffi_type_pointer;
  case TAG_CFN:
    return &ffi_type_pointer;
  case TAG_CHARACTER:
    return &ffi_type_schar;
  case TAG_F32:
    return &ffi_type_float;
  case TAG_F64:
    return &ffi_type_double;
  case TAG_FACT:
    return &ffi_type_pointer;
  case TAG_FN:
    return &ffi_type_pointer;
  case TAG_IDENT:
    return &ffi_type_pointer;
  case TAG_INTEGER:
    return &ffi_type_sint;
  case TAG_LIST:
    return &ffi_type_pointer;
  case TAG_MAP:
    return &ffi_type_pointer;
  case TAG_PTAG:
    return &ffi_type_pointer;
  case TAG_PTR:
    return &ffi_type_pointer;
  case TAG_QUOTE:
    return &ffi_type_pointer;
  case TAG_S8:
    return &ffi_type_sint8;
  case TAG_S16:
    return &ffi_type_sint16;
  case TAG_S32:
    return &ffi_type_sint32;
  case TAG_S64:
    return &ffi_type_sint64;
  case TAG_SW:
    return &ffi_type_slong;
  case TAG_STR:
    return &ffi_type_pointer;
  case TAG_SYM:
    return &ffi_type_pointer;
  case TAG_TUPLE:
    return &ffi_type_pointer;
  case TAG_U8:
    return &ffi_type_uint8;
  case TAG_U16:
    return &ffi_type_uint16;
  case TAG_U32:
    return &ffi_type_uint32;
  case TAG_U64:
    return &ffi_type_uint64;
  case TAG_UW:
    return &ffi_type_ulong;
  case TAG_VAR:
    return &ffi_type_pointer;
  case TAG_VOID:
    return &ffi_type_void;
  }
  assert(! "tag_type_to_ffi_type: unknown tag type");
  errx(1, "tag_type_to_ffi_type: unknown tag type");
  return &ffi_type_void;
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
  case TAG_PTR:        return sym_1("Void*");
  case TAG_QUOTE:      return sym_1("Quote");
  case TAG_STR:        return sym_1("Str");
  case TAG_SYM:        return sym_1("Sym");
  case TAG_TUPLE:      return sym_1("Tuple");
  case TAG_VAR:        return sym_1("Var");
  }
  assert(! "tag_type_to_sym: invalid tag type");
  errx(1, "tag_type_to_sym: invalid tag type: %d", tag_type);
  return NULL;
}
