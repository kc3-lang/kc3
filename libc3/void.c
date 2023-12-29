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
#include "c3.h"

sw void_buf_inspect (const s_sym *type, s_buf *buf, const void *v)
{
  sw r;
  if (type == sym_1("Array"))
    return buf_inspect_array(buf, v);
  if (type == sym_1("Bool"))
    return buf_inspect_bool(buf, v);
  if (type == sym_1("Call"))
    return buf_inspect_call(buf, v);
  if (type == sym_1("Cfn"))
    return buf_inspect_cfn(buf, v);
  if (type == sym_1("Character"))
    return buf_inspect_character(buf, v);
  if (type == sym_1("F32"))
    return buf_inspect_f32(buf, v);
  if (type == sym_1("F64"))
    return buf_inspect_f64(buf, v);
  if (type == sym_1("Fact"))
    return buf_inspect_fact(buf, v);
  if (type == sym_1("Fn"))
    return buf_inspect_fn(buf, v);
  if (type == sym_1("Ident"))
    return buf_inspect_ident(buf, v);
  if (type == sym_1("Integer"))
    return buf_inspect_integer(buf, v);
  if (type == sym_1("List"))
    return buf_inspect_list(buf, v);
  if (type == sym_1("Ptag"))
    return buf_inspect_ptag(buf, v);
  if (type == sym_1("Ptr"))
    return buf_inspect_ptr(buf, v);
  if (type == sym_1("PtrFree"))
    return buf_inspect_ptr_free(buf, v);
  if (type == sym_1("Quote"))
    return buf_inspect_quote(buf, v);
  if (type == sym_1("S8"))
    return buf_inspect_s8(buf, v);
  if (type == sym_1("S16"))
    return buf_inspect_s16(buf, v);
  if (type == sym_1("S32"))
    return buf_inspect_s32(buf, v);
  if (type == sym_1("S64"))
    return buf_inspect_s64(buf, v);
  if (type == sym_1("Str"))
    return buf_inspect_str(buf, v);
  if (type == sym_1("Struct"))
    return buf_inspect_struct(buf, v);
  if (type == sym_1("Sw"))
    return buf_inspect_sw(buf, v);
  if (type == sym_1("Sym"))
    return buf_inspect_sym(buf, v);
  if (type == sym_1("Tuple"))
    return buf_inspect_tuple(buf, v);
  if (type == sym_1("U8"))
    return buf_inspect_u8(buf, v);
  if (type == sym_1("U16"))
    return buf_inspect_u16(buf, v);
  if (type == sym_1("U32"))
    return buf_inspect_u32(buf, v);
  if (type == sym_1("U64"))
    return buf_inspect_u64(buf, v);
  if (type == sym_1("Uw"))
    return buf_inspect_uw(buf, v);
  if (type == sym_1("Var"))
    return buf_inspect_var(buf, v);
  if (type == sym_1("Void"))
    return buf_inspect_void(buf, v);
  /*
  if (sym_is_array_type(type)) {
  */
  if (struct_type_exists(type)) {
    s_struct s = {0};
    struct_init_with_data(&s, type, false, (void *) v);
    r = buf_inspect_struct(buf, &s);
    struct_type_clean(&s.type);
    return r;
  }
  err_write_1("void_buf_inspect: unknown type: ");
  err_inspect_sym(&type);
  err_write_1("\n");
  assert(! "void_buf_inspect: unknown type");
  return -1;
}

sw void_buf_inspect_size (const s_sym *type, const void *v)
{
  sw r;
  if (type == sym_1("Array"))
    return buf_inspect_array_size(v);
  if (type == sym_1("Bool"))
    return buf_inspect_bool_size(v);
  if (type == sym_1("Call"))
    return buf_inspect_call_size(v);
  if (type == sym_1("Cfn"))
    return buf_inspect_cfn_size(v);
  if (type == sym_1("Character"))
    return buf_inspect_character_size(v);
  if (type == sym_1("F32"))
    return buf_inspect_f32_size(v);
  if (type == sym_1("F64"))
    return buf_inspect_f64_size(v);
  if (type == sym_1("Fact"))
    return buf_inspect_fact_size(v);
  if (type == sym_1("Fn"))
    return buf_inspect_fn_size(v);
  if (type == sym_1("Ident"))
    return buf_inspect_ident_size(v);
  if (type == sym_1("Integer"))
    return buf_inspect_integer_size(v);
  if (type == sym_1("List"))
    return buf_inspect_list_size(v);
  if (type == sym_1("Ptag"))
    return buf_inspect_ptag_size(v);
  if (type == sym_1("Ptr"))
    return buf_inspect_ptr_size(v);
  if (type == sym_1("PtrFree"))
    return buf_inspect_ptr_free_size(v);
  if (type == sym_1("Quote"))
    return buf_inspect_quote_size(v);
  if (type == sym_1("S8"))
    return buf_inspect_s8_size(v);
  if (type == sym_1("S16"))
    return buf_inspect_s16_size(v);
  if (type == sym_1("S32"))
    return buf_inspect_s32_size(v);
  if (type == sym_1("S64"))
    return buf_inspect_s64_size(v);
  if (type == sym_1("Str"))
    return buf_inspect_str_size(v);
  if (type == sym_1("Struct"))
    return buf_inspect_struct_size(v);
  if (type == sym_1("Sw"))
    return buf_inspect_sw_size(v);
  if (type == sym_1("Sym"))
    return buf_inspect_sym_size(v);
  if (type == sym_1("Tuple"))
    return buf_inspect_tuple_size(v);
  if (type == sym_1("U8"))
    return buf_inspect_u8_size(v);
  if (type == sym_1("U16"))
    return buf_inspect_u16_size(v);
  if (type == sym_1("U32"))
    return buf_inspect_u32_size(v);
  if (type == sym_1("U64"))
    return buf_inspect_u64_size(v);
  if (type == sym_1("Uw"))
    return buf_inspect_uw_size(v);
  if (type == sym_1("Var"))
    return buf_inspect_var_size(v);
  if (type == sym_1("Void"))
    return buf_inspect_void_size(v);
  /*
  if (sym_is_array_type(type)) {
  */
  if (struct_type_exists(type)) {
    s_struct s = {0};
    struct_init_with_data(&s, type, false, (void *) v);
    r = buf_inspect_struct_size(&s);
    struct_type_clean(&s.type);
    return r;
  }
  err_write_1("void_buf_inspect_size: unknown type: ");
  err_inspect_sym(&type);
  err_write_1("\n");
  assert(! "void_buf_inspect_size: unknown type");
  return -1;
}

bool void_clean (const s_sym *type, void *data)
{
  assert(type);
  if (type == sym_1("Array")) {
    array_clean(data);
    return true;
  }
  if (type == sym_1("Bool")) {
    return true;
  }
  if (type == sym_1("Call")) {
    call_clean(data);
    return true;
  }
  if (type == sym_1("Cfn")) {
    cfn_clean(data);
    return true;
  }
  if (type == sym_1("Character")) {
    return true;
  }
  if (type == sym_1("F32")) {
    return true;
  }
  if (type == sym_1("F64")) {
    return true;
  }
  if (type == sym_1("Fact")) {
    return true;
  }
  if (type == sym_1("Fn")) {
    fn_clean(data);
    return true;
  }
  if (type == sym_1("Ident")) {
    return true;
  }
  if (type == sym_1("Integer")) {
    integer_clean(data);
    return true;
  }
  if (type == sym_1("List")) {
    list_f_clean(data);
    return true;
  }
  if (type == sym_1("Map")) {
    map_clean(data);
    return true;
  }
  if (type == sym_1("Ptag")) {
    return true;
  }
  if (type == sym_1("Ptr")) {
    return true;
  }
  if (type == sym_1("PtrFree")) {
    ptr_free_clean(data);
    return true;
  }
  if (type == sym_1("Quote")) {
    quote_clean(data);
    return true;
  }
  if (type == sym_1("S8")) {
    return true;
  }
  if (type == sym_1("S16")) {
    return true;
  }
  if (type == sym_1("S32")) {
    return true;
  }
  if (type == sym_1("S64")) {
    return true;
  }
  if (type == sym_1("Str")) {
    str_clean(data);
    return true;
  }
  if (type == sym_1("Struct")) {
    struct_clean(data);
    return true;
  }
  if (type == sym_1("Sw")) {
    return true;
  }
  if (type == sym_1("Sym")) {
    return true;
  }
  if (type == sym_1("Tuple")) {
    tuple_clean(data);
    return true;
  }
  if (type == sym_1("U8")) {
    return true;
  }
  if (type == sym_1("U16")) {
    return true;
  }
  if (type == sym_1("U32")) {
    return true;
  }
  if (type == sym_1("U64")) {
    return true;
  }
  if (type == sym_1("Uw")) {
    return true;
  }
  if (type == sym_1("Var")) {
    return true;
  }
  if (type == sym_1("Void")) {
    return true;
  }
  /*
  if (sym_is_array_type(type)) {
  */
  if (struct_type_exists(type)) {
    s_struct s = {0};
    struct_init_with_data(&s, type, false, data);
    struct_clean(&s);
    return true;
  }
  err_write_1("void_clean: unknown type: ");
  err_inspect_sym(&type);
  err_write_1("\n");
  assert(! "void_clean: unknown type");
  return false;
}

void * void_init_cast (const s_sym *type, void *v, const s_tag *tag)
{
  void *r;
  if (type == sym_1("Array"))
    return array_init_cast(v, tag);
  if (type == sym_1("Bool"))
    return bool_init_cast(v, tag);
  if (type == sym_1("Call"))
    return call_init_cast(v, tag);
  if (type == sym_1("Cfn"))
    return cfn_init_cast(v, tag);
  if (type == sym_1("Character"))
    return character_init_cast(v, tag);
  if (type == sym_1("F32"))
    return f32_init_cast(v, tag);
  if (type == sym_1("F64"))
    return f64_init_cast(v, tag);
  if (type == sym_1("Fact"))
    return fact_init_cast(v, tag);
  if (type == sym_1("Fn"))
    return fn_init_cast(v, tag);
  if (type == sym_1("Ident"))
    return ident_init_cast(v, tag);
  if (type == sym_1("Integer"))
    return integer_init_cast(v, tag);
  if (type == sym_1("List"))
    return list_init_cast(v, tag);
  if (type == sym_1("Ptag"))
    return ptag_init_cast(v, tag);
  if (type == sym_1("Ptr"))
    return ptr_init_cast(v, tag);
  if (type == sym_1("PtrFree"))
    return ptr_free_init_cast(v, tag);
  if (type == sym_1("Quote"))
    return quote_init_cast(v, tag);
  if (type == sym_1("S8"))
    return s8_init_cast(v, tag);
  if (type == sym_1("S16"))
    return s16_init_cast(v, tag);
  if (type == sym_1("S32"))
    return s32_init_cast(v, tag);
  if (type == sym_1("S64"))
    return s64_init_cast(v, tag);
  if (type == sym_1("Str"))
    return str_init_cast(v, tag);
  if (type == sym_1("Struct"))
    return struct_init_cast(v, tag);
  if (type == sym_1("Sw"))
    return sw_init_cast(v, tag);
  if (type == sym_1("Sym"))
    return sym_init_cast(v, tag);
  if (type == sym_1("Tuple"))
    return tuple_init_cast(v, tag);
  if (type == sym_1("U8"))
    return u8_init_cast(v, tag);
  if (type == sym_1("U16"))
    return u16_init_cast(v, tag);
  if (type == sym_1("U32"))
    return u32_init_cast(v, tag);
  if (type == sym_1("U64"))
    return u64_init_cast(v, tag);
  if (type == sym_1("Uw"))
    return uw_init_cast(v, tag);
  if (type == sym_1("Var"))
    return v;
  if (type == sym_1("Void"))
    return v;
  /*
  if (sym_is_array_type(type)) {
  */
  if (struct_type_exists(type)) {
    s_struct s = {0};
    struct_init_with_data(&s, type, false, v);
    r = struct_init_cast(&s, tag);
    struct_type_clean(&s.type);
    return r;
  }
  err_write_1("void_init_cast: unknown type: ");
  err_inspect_sym(&type);
  err_write_1("\n");
  assert(! "void_init_cast: unknown type");
  return NULL;
}

void * void_init_copy (const s_sym *type, void *v, const void *src)
{
  void *r;
  if (type == sym_1("Array"))
    return array_init_copy(v, src);
  if (type == sym_1("Bool"))
    return bool_init_copy(v, src);
  if (type == sym_1("Call"))
    return call_init_copy(v, src);
  if (type == sym_1("Cfn"))
    return cfn_init_copy(v, src);
  if (type == sym_1("Character"))
    return character_init_copy(v, src);
  if (type == sym_1("F32"))
    return f32_init_copy(v, src);
  if (type == sym_1("F64"))
    return f64_init_copy(v, src);
  if (type == sym_1("Fact"))
    return fact_init_copy(v, src);
  if (type == sym_1("Fn"))
    return fn_init_copy(v, src);
  if (type == sym_1("Ident"))
    return ident_init_copy(v, src);
  if (type == sym_1("Integer"))
    return integer_init_copy(v, src);
  if (type == sym_1("List"))
    return list_init_copy(v, src);
  if (type == sym_1("Ptag"))
    return ptag_init_copy(v, src);
  if (type == sym_1("Ptr"))
    return ptr_init_copy(v, src);
  if (type == sym_1("PtrFree"))
    return ptr_free_init_copy(v, src);
  if (type == sym_1("Quote"))
    return quote_init_copy(v, src);
  if (type == sym_1("S8"))
    return s8_init_copy(v, src);
  if (type == sym_1("S16"))
    return s16_init_copy(v, src);
  if (type == sym_1("S32"))
    return s32_init_copy(v, src);
  if (type == sym_1("S64"))
    return s64_init_copy(v, src);
  if (type == sym_1("Str"))
    return str_init_copy(v, src);
  if (type == sym_1("Struct"))
    return struct_init_copy(v, src);
  if (type == sym_1("Sw"))
    return sw_init_copy(v, src);
  if (type == sym_1("Sym"))
    return sym_init_copy(v, src);
  if (type == sym_1("Tuple"))
    return tuple_init_copy(v, src);
  if (type == sym_1("U8"))
    return u8_init_copy(v, src);
  if (type == sym_1("U16"))
    return u16_init_copy(v, src);
  if (type == sym_1("U32"))
    return u32_init_copy(v, src);
  if (type == sym_1("U64"))
    return u64_init_copy(v, src);
  if (type == sym_1("Uw"))
    return uw_init_copy(v, src);
  if (type == sym_1("Var"))
    return v;
  if (type == sym_1("Void"))
    return v;
  if (struct_type_exists(type)) {
    s_struct s = {0};
    s_struct t = {0};
    struct_init_with_data(&s, type, false, v);
    struct_init_with_data(&t, type, false, (void *) src);
    r = struct_init_copy(&s, &t);
    struct_type_clean(&s.type);
    struct_type_clean(&t.type);
    return r;
  }
  err_write_1("void_init_copy: unknown type: ");
  err_inspect_sym(&type);
  err_write_1("\n");
  assert(! "void_init_copy: unknown type");
  return NULL;
}
