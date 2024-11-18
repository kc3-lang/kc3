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

sw data_buf_inspect (s_buf *buf, const s_sym *type, const void *data)
{
  s_struct s = {0};
  s_struct_type *st;
  if (type == &g_sym_Array ||
      sym_is_array_type(type))
    return buf_inspect_array(buf, data);
  if (type == &g_sym_Bool)
    return buf_inspect_bool(buf, data);
  if (type == &g_sym_Call)
    return buf_inspect_call(buf, data);
  if (type == &g_sym_Callable ||
      type == &g_sym_Cfn ||
      type == &g_sym_Fn)
    return buf_inspect_callable(buf, *(p_callable *) data);
  if (type == &g_sym_Character)
    return buf_inspect_character(buf, data);
  if (type == &g_sym_F32)
    return buf_inspect_f32(buf, data);
  if (type == &g_sym_F64)
    return buf_inspect_f64(buf, data);
  if (type == &g_sym_Fact)
    return buf_inspect_fact(buf, data);
  if (type == &g_sym_Ident)
    return buf_inspect_ident(buf, data);
  if (type == &g_sym_Integer)
    return buf_inspect_integer(buf, data);
  if (type == &g_sym_List)
    return buf_inspect_list(buf, data);
  if (type == &g_sym_Map)
    return buf_inspect_map(buf, data);
  if (type == &g_sym_Ptag)
    return buf_inspect_ptag(buf, data);
  if (type == &g_sym_Ptr)
    return buf_inspect_ptr(buf, data);
  if (type == &g_sym_PtrFree)
    return buf_inspect_ptr_free(buf, data);
  if (type == &g_sym_Quote)
    return buf_inspect_quote(buf, data);
  if (type == &g_sym_S8)
    return buf_inspect_s8(buf, data);
  if (type == &g_sym_S16)
    return buf_inspect_s16(buf, data);
  if (type == &g_sym_S32)
    return buf_inspect_s32(buf, data);
  if (type == &g_sym_S64)
    return buf_inspect_s64(buf, data);
  if (type == &g_sym_Str)
    return buf_inspect_str(buf, data);
  if (type == &g_sym_Struct)
    return buf_inspect_struct(buf, data);
  if (type == &g_sym_StructType)
    return buf_inspect_struct_type(buf, data);
  if (type == &g_sym_Sw)
    return buf_inspect_sw(buf, data);
  if (type == &g_sym_Sym)
    return buf_inspect_sym(buf, data);
  if (type == &g_sym_Tag)
    return buf_inspect_tag(buf, data);
  if (type == &g_sym_Time)
    return buf_inspect_time(buf, data);
  if (type == &g_sym_Tuple)
    return buf_inspect_tuple(buf, data);
  if (type == &g_sym_U8)
    return buf_inspect_u8(buf, data);
  if (type == &g_sym_U16)
    return buf_inspect_u16(buf, data);
  if (type == &g_sym_U32)
    return buf_inspect_u32(buf, data);
  if (type == &g_sym_U64)
    return buf_inspect_u64(buf, data);
  if (type == &g_sym_Uw)
    return buf_inspect_uw(buf, data);
  if (type == &g_sym_Var)
    return buf_inspect_var(buf, data);
  if (type == &g_sym_Void)
    return buf_inspect_void(buf, data);
  if (! struct_type_find(type, &st))
    return -1;
  if (st) {
    s.type = st;
    s.data = (void *) data;
    return buf_inspect_struct(buf, &s);
  }
  err_write_1("data_buf_inspect: unknown type: ");
  err_inspect_sym(&type);
  err_write_1("\n");
  assert(! "data_buf_inspect: unknown type");
  return -1;
}

sw data_buf_inspect_size (s_pretty *pretty, const s_sym *type,
                          const void *data)
{
  s_struct s = {0};
  s_struct_type *st;
  if (type == &g_sym_Array ||
      sym_is_array_type(type))
    return buf_inspect_array_size(pretty, data);
  if (type == &g_sym_Bool)
    return buf_inspect_bool_size(pretty, data);
  if (type == &g_sym_Call)
    return buf_inspect_call_size(pretty, data);
  if (type == &g_sym_Callable ||
      type == &g_sym_Cfn ||
      type == &g_sym_Fn)
    return buf_inspect_callable_size(pretty, *(p_callable *) data);
  if (type == &g_sym_Character)
    return buf_inspect_character_size(pretty, data);
  if (type == &g_sym_F32)
    return buf_inspect_f32_size(pretty, data);
  if (type == &g_sym_F64)
    return buf_inspect_f64_size(pretty, data);
  if (type == &g_sym_Fact)
    return buf_inspect_fact_size(pretty, data);
  if (type == &g_sym_Ident)
    return buf_inspect_ident_size(pretty, data);
  if (type == &g_sym_Integer)
    return buf_inspect_integer_size(pretty, data);
  if (type == &g_sym_List)
    return buf_inspect_list_size(pretty, data);
  if (type == &g_sym_Map)
    return buf_inspect_map_size(pretty, data);
  if (type == &g_sym_Ptag)
    return buf_inspect_ptag_size(pretty, data);
  if (type == &g_sym_Ptr)
    return buf_inspect_ptr_size(pretty, data);
  if (type == &g_sym_PtrFree)
    return buf_inspect_ptr_free_size(pretty, data);
  if (type == &g_sym_Quote)
    return buf_inspect_quote_size(pretty, data);
  if (type == &g_sym_S8)
    return buf_inspect_s8_size(pretty, data);
  if (type == &g_sym_S16)
    return buf_inspect_s16_size(pretty, data);
  if (type == &g_sym_S32)
    return buf_inspect_s32_size(pretty, data);
  if (type == &g_sym_S64)
    return buf_inspect_s64_size(pretty, data);
  if (type == &g_sym_Str)
    return buf_inspect_str_size(pretty, data);
  if (type == &g_sym_Struct)
    return buf_inspect_struct_size(pretty, data);
  if (type == &g_sym_StructType)
    return buf_inspect_struct_type_size(pretty, data);
  if (type == &g_sym_Sw)
    return buf_inspect_sw_size(pretty, data);
  if (type == &g_sym_Sym)
    return buf_inspect_sym_size(pretty, data);
  if (type == &g_sym_Tag)
    return buf_inspect_tag_size(pretty, data);
  if (type == &g_sym_Time)
    return buf_inspect_time_size(pretty, data);
  if (type == &g_sym_Tuple)
    return buf_inspect_tuple_size(pretty, data);
  if (type == &g_sym_U8)
    return buf_inspect_u8_size(pretty, data);
  if (type == &g_sym_U16)
    return buf_inspect_u16_size(pretty, data);
  if (type == &g_sym_U32)
    return buf_inspect_u32_size(pretty, data);
  if (type == &g_sym_U64)
    return buf_inspect_u64_size(pretty, data);
  if (type == &g_sym_Uw)
    return buf_inspect_uw_size(pretty, data);
  if (type == &g_sym_Var)
    return buf_inspect_var_size(pretty, data);
  if (type == &g_sym_Void)
    return buf_inspect_void_size(pretty, data);
  if (! struct_type_find(type, &st))
    return -1;
  if (st) {
    s.type = st;
    s.data = (void *) data;
    return buf_inspect_struct_size(pretty, &s);
  }
  err_write_1("data_buf_inspect_size: unknown type: ");
  err_inspect_sym(&type);
  err_write_1("\n");
  assert(! "data_buf_inspect_size: unknown type");
  return -1;
}

bool data_clean (const s_sym *type, void *data)
{
  s_struct s = {0};
  s_struct_type *st;
  assert(type);
  if (! data)
    return true;
  if (type == &g_sym_Array ||
      sym_is_array_type(type)) {
    array_clean(data);
    return true;
  }
  if (type == &g_sym_Bool) {
    return true;
  }
  if (type == &g_sym_Call) {
    call_clean(data);
    return true;
  }
  if (type == &g_sym_Callable ||
      type == &g_sym_Cfn ||
      type == &g_sym_Fn) {
    p_callable_clean(data);
    return true;
  }
  if (type == &g_sym_Character) {
    return true;
  }
  if (type == &g_sym_F32) {
    return true;
  }
  if (type == &g_sym_F64) {
    return true;
  }
  if (type == &g_sym_Fact) {
    return true;
  }
  if (type == &g_sym_Ident) {
    return true;
  }
  if (type == &g_sym_Integer) {
    integer_clean(data);
    return true;
  }
  if (type == &g_sym_List) {
    list_f_clean(data);
    return true;
  }
  if (type == &g_sym_Map) {
    map_clean(data);
    return true;
  }
  if (type == &g_sym_Ptag) {
    return true;
  }
  if (type == &g_sym_Ptr) {
    return true;
  }
  if (type == &g_sym_PtrFree) {
    ptr_free_clean(data);
    return true;
  }
  if (type == &g_sym_Quote) {
    quote_clean(data);
    return true;
  }
  if (type == &g_sym_S8) {
    return true;
  }
  if (type == &g_sym_S16) {
    return true;
  }
  if (type == &g_sym_S32) {
    return true;
  }
  if (type == &g_sym_S64) {
    return true;
  }
  if (type == &g_sym_Str) {
    str_clean(data);
    return true;
  }
  if (type == &g_sym_Struct) {
    struct_clean(data);
    return true;
  }
  if (type == &g_sym_StructType) {
    struct_type_clean(data);
    return true;
  }
  if (type == &g_sym_Sw) {
    return true;
  }
  if (type == &g_sym_Sym) {
    return true;
  }
  if (type == &g_sym_Tag) {
    tag_clean(data);
    return true;
  }
  if (type == &g_sym_Time) {
    time_clean(data);
    return true;
  }
  if (type == &g_sym_Tuple) {
    tuple_clean(data);
    return true;
  }
  if (type == &g_sym_U8) {
    return true;
  }
  if (type == &g_sym_U16) {
    return true;
  }
  if (type == &g_sym_U32) {
    return true;
  }
  if (type == &g_sym_U64) {
    return true;
  }
  if (type == &g_sym_Uw) {
    return true;
  }
  if (type == &g_sym_Var) {
    tag_clean(data);
    return true;
  }
  if (type == &g_sym_Void) {
    return true;
  }
  if (! struct_type_find(type, &st))
    return false;
  if (st) {
    s.type = st;
    s.data = data;
    struct_clean(&s);
    return true;
  }
  err_write_1("data_clean: unknown type: ");
  err_inspect_sym(&type);
  err_write_1("\n");
  assert(! "data_clean: unknown type");
  return false;
}

bool data_compare (const s_sym *type, const void *a, const void *b)
{
  s_struct sa = {0};
  s_struct sb = {0};
  s_struct_type *st;
  if (type == &g_sym_Array ||
      sym_is_array_type(type))
    return compare_array(a, b);
  if (type == &g_sym_Bool)
    return compare_bool(*(bool *) a, *(bool *) b);
  if (type == &g_sym_Call)
    return compare_call(a, b);
  if (type == &g_sym_Callable ||
      type == &g_sym_Cfn ||
      type == &g_sym_Fn)
    return compare_callable(*(p_callable *) a, *(p_callable *) b);
  if (type == &g_sym_Character)
    return compare_character(*(character *) a, *(character *) b);
  if (type == &g_sym_F32)
    return compare_f32(*(f64 *) a, *(f64 *) b);
  if (type == &g_sym_F64)
    return compare_f64(*(f64 *) a, *(f64 *) b);
  if (type == &g_sym_Fact)
    return compare_fact(a, b);
  if (type == &g_sym_Ident)
    return compare_ident(a, b);
  if (type == &g_sym_Integer)
    return compare_integer(a, b);
  if (type == &g_sym_List)
    return compare_list(a, b);
  if (type == &g_sym_Ptag)
    return compare_ptag(a, b);
  if (type == &g_sym_Ptr)
    return compare_ptr(a, b);
  if (type == &g_sym_PtrFree)
    return compare_ptr(a, b);
  if (type == &g_sym_Quote)
    return compare_quote(a, b);
  if (type == &g_sym_S8)
    return compare_s8(*(s8 *) a, *(s8 *) b);
  if (type == &g_sym_S16)
    return compare_s16(*(s16 *) a, *(s16 *) b);
  if (type == &g_sym_S32)
    return compare_s32(*(s32 *) a, *(s32 *) b);
  if (type == &g_sym_S64)
    return compare_s64(*(s64 *) a, *(s64 *) b);
  if (type == &g_sym_Str)
    return compare_str(a, b);
  if (type == &g_sym_Struct)
    return compare_struct(a, b);
  if (type == &g_sym_StructType)
    return compare_struct_type(a, b);
  if (type == &g_sym_Sw)
    return compare_sw(*(sw *) a, *(sw *) b);
  if (type == &g_sym_Sym)
    return compare_sym(a, b);
  if (type == &g_sym_Time)
    return compare_time(a, b);
  if (type == &g_sym_Tuple)
    return compare_tuple(a, b);
  if (type == &g_sym_U8)
    return compare_u8(*(u8 *) a, *(u8 *) b);
  if (type == &g_sym_U16)
    return compare_u16(*(u16 *) a, *(u16 *) b);
  if (type == &g_sym_U32)
    return compare_u32(*(u32 *) a, *(u32 *) b);
  if (type == &g_sym_U64)
    return compare_u64(*(u64 *) a, *(u64 *) b);
  if (type == &g_sym_Uw)
    return compare_uw(*(uw *) a, *(uw *) b);
  if (type == &g_sym_Var)
    return compare_ptr(a, b);
  if (type == &g_sym_Void)
    return 0;
  if (! struct_type_find(type, &st))
    return COMPARE_ERROR;
  if (st) {
    sa.type = st;
    sa.data = (void *) a;
    sb.type = st;
    sb.data = (void *) b;
    return compare_struct(&sa, &sb);
  }
  err_write_1("data_compare: unknown type: ");
  err_inspect_sym(&type);
  err_write_1("\n");
  assert(! "data_compare: unknown type");
  return COMPARE_ERROR;
}

bool data_hash_update (const s_sym *type, t_hash *hash, const void *data)
{
  s_struct s = {0};
  s_struct_type *st;
  if (type == &g_sym_Array ||
      sym_is_array_type(type))
    return hash_update_array(hash, data);
  if (type == &g_sym_Bool)
    return hash_update_bool(hash, data);
  if (type == &g_sym_Call)
    return hash_update_call(hash, data);
  if (type == &g_sym_Callable ||
      type == &g_sym_Cfn ||
      type == &g_sym_Fn)
    return hash_update_callable(hash, *(p_callable *) data);
  if (type == &g_sym_Character)
    return hash_update_character(hash, *(character *) data);
  if (type == &g_sym_F32)
    return hash_update_f32(hash, *(f32 *) data);
  if (type == &g_sym_F64)
    return hash_update_f64(hash, *(f64 *) data);
  if (type == &g_sym_F128)
    return hash_update_f128(hash, *(f128 *) data);
  if (type == &g_sym_Fact)
    return hash_update_fact(hash, data);
  if (type == &g_sym_Ident)
    return hash_update_ident(hash, data);
  if (type == &g_sym_Integer)
    return hash_update_integer(hash, data);
  if (type == &g_sym_List)
    return hash_update_list(hash, data);
  if (type == &g_sym_Ptag)
    return hash_update_ptag(hash, data);
  if (type == &g_sym_Ptr)
    return hash_update_ptr(hash, data);
  if (type == &g_sym_PtrFree)
    return hash_update_ptr_free(hash, data);
  if (type == &g_sym_Quote)
    return hash_update_quote(hash, data);
  if (type == &g_sym_S8)
    return hash_update_s8(hash, *(s8 *) data);
  if (type == &g_sym_S16)
    return hash_update_s16(hash, *(s16 *) data);
  if (type == &g_sym_S32)
    return hash_update_s32(hash, *(s32 *) data);
  if (type == &g_sym_S64)
    return hash_update_s64(hash, *(s64 *) data);
  if (type == &g_sym_Str)
    return hash_update_str(hash, data);
  if (type == &g_sym_Struct)
    return hash_update_struct(hash, data);
  if (type == &g_sym_StructType)
    return hash_update_struct_type(hash, data);
  if (type == &g_sym_Sw)
    return hash_update_sw(hash, *(sw *) data);
  if (type == &g_sym_Sym)
    return hash_update_sym(hash, data);
  if (type == &g_sym_Tag)
    return hash_update_tag(hash, data);
  if (type == &g_sym_Time)
    return hash_update_time(hash, data);
  if (type == &g_sym_Tuple)
    return hash_update_tuple(hash, data);
  if (type == &g_sym_U8)
    return hash_update_u8(hash, *(u8 *) data);
  if (type == &g_sym_U16)
    return hash_update_u16(hash, *(u16 *) data);
  if (type == &g_sym_U32)
    return hash_update_u32(hash, *(u32 *) data);
  if (type == &g_sym_U64)
    return hash_update_u64(hash, *(u64 *) data);
  if (type == &g_sym_Uw)
    return hash_update_uw(hash, *(uw *) data);
  if (type == &g_sym_Var)
    return hash_update_var(hash, data);
  if (type == &g_sym_Void)
    return hash_update_void(hash);
  if (! struct_type_find(type, &st))
    return false;
  if (st) {
    s.type = st;
    s.data = (void *) data;
    return hash_update_struct(hash, &s);
  }
  err_write_1("data_hash_update: unknown type: ");
  err_inspect_sym(&type);
  err_write_1("\n");
  assert(! "data_hash_update: unknown type");
  return false;
}

void * data_init_cast (void *data, const s_sym * const *type,
                       s_tag *tag)
{
  s_struct s = {0};
  s_struct_type *st;
  const s_sym *t;
  t = *type;
  if (t == &g_sym_Array ||
      sym_is_array_type(*type))
    return array_init_cast(data, type, tag);
  if (t == &g_sym_Bool)
    return bool_init_cast(data, type, tag);
  if (t == &g_sym_Call)
    return call_init_cast(data, type, tag);
  if (t == &g_sym_Callable ||
      t == &g_sym_Cfn ||
      t == &g_sym_Fn)
    return p_callable_init_cast(data, type, tag);
  if (t == &g_sym_Character)
    return character_init_cast(data, type, tag);
  if (t == &g_sym_F32)
    return f32_init_cast(data, type, tag);
  if (t == &g_sym_F64)
    return f64_init_cast(data, type, tag);
  if (t == &g_sym_Fact)
    return fact_init_cast(data, type, tag);
  if (t == &g_sym_Ident)
    return ident_init_cast(data, type, tag);
  if (t == &g_sym_Integer)
    return integer_init_cast(data, type, tag);
  if (t == &g_sym_List)
    return list_init_cast(data, type, tag);
  if (t == &g_sym_Ptag)
    return ptag_init_cast(data, type, tag);
  if (t == &g_sym_Ptr)
    return ptr_init_cast(data, type, tag);
  if (t == &g_sym_PtrFree)
    return ptr_free_init_cast(data, type, tag);
  if (t == &g_sym_Quote)
    return quote_init_cast(data, type, tag);
  if (t == &g_sym_S8)
    return s8_init_cast(data, type, tag);
  if (t == &g_sym_S16)
    return s16_init_cast(data, type, tag);
  if (t == &g_sym_S32)
    return s32_init_cast(data, type, tag);
  if (t == &g_sym_S64)
    return s64_init_cast(data, type, tag);
  if (t == &g_sym_Str)
    return str_init_cast(data, type, tag);
  if (t == &g_sym_Struct)
    return struct_init_cast(data, type, tag);
  if (t == &g_sym_StructType)
    return struct_type_init_cast(data, type, tag);
  if (t == &g_sym_Sw)
    return sw_init_cast(data, type, tag);
  if (t == &g_sym_Sym)
    return sym_init_cast(data, type, tag);
  if (t == &g_sym_Tag)
    return tag_init_copy(data, tag);
  if (t == &g_sym_Time)
    return time_init_cast(data, type, tag);
  if (t == &g_sym_Tuple)
    return tuple_init_cast(data, type, tag);
  if (t == &g_sym_U8)
    return u8_init_cast(data, type, tag);
  if (t == &g_sym_U16)
    return u16_init_cast(data, type, tag);
  if (t == &g_sym_U32)
    return u32_init_cast(data, type, tag);
  if (t == &g_sym_U64)
    return u64_init_cast(data, type, tag);
  if (t == &g_sym_Uw)
    return uw_init_cast(data, type, tag);
  if (t == &g_sym_Var)
    return var_init_cast(data, type, tag);
  if (t == &g_sym_Void)
    return data;
  if (! struct_type_find(t, &st))
    return NULL;
  if (st) {
    s.type = st;
    s.data = data;
    return struct_init_cast(&s, type, tag);
  }
  err_write_1("data_init_cast: unknown type: ");
  err_inspect_sym(type);
  err_write_1("\n");
  assert(! "data_init_cast: unknown type");
  return NULL;
}

void * data_init_copy (const s_sym *type, void *data, void *src)
{
  s_struct_type *st;
  if (type == &g_sym_Array ||
      sym_is_array_type(type))
    return array_init_copy(data, src);
  if (type == &g_sym_Bool)
    return bool_init_copy(data, src);
  if (type == &g_sym_Call)
    return call_init_copy(data, src);
  if (type == &g_sym_Callable ||
      type == &g_sym_Cfn ||
      type == &g_sym_Fn)
    return p_callable_init_copy(data, src);
  if (type == &g_sym_Character)
    return character_init_copy(data, src);
  if (type == &g_sym_Cow)
    return pcow_init_copy(data, src);
  if (type == &g_sym_F32)
    return f32_init_copy(data, src);
  if (type == &g_sym_F64)
    return f64_init_copy(data, src);
  if (type == &g_sym_Fact)
    return fact_init_copy(data, src);
  if (type == &g_sym_Ident)
    return ident_init_copy(data, src);
  if (type == &g_sym_Integer)
    return integer_init_copy(data, src);
  if (type == &g_sym_List)
    return list_init_copy(data, src);
  if (type == &g_sym_Map)
    return map_init_copy(data, src);
  if (type == &g_sym_Ptag)
    return ptag_init_copy(data, src);
  if (type == &g_sym_Ptr)
    return ptr_init_copy(data, src);
  if (type == &g_sym_PtrFree)
    return ptr_free_init_copy(data, src);
  if (type == &g_sym_Quote)
    return quote_init_copy(data, src);
  if (type == &g_sym_S8)
    return s8_init_copy(data, src);
  if (type == &g_sym_S16)
    return s16_init_copy(data, src);
  if (type == &g_sym_S32)
    return s32_init_copy(data, src);
  if (type == &g_sym_S64)
    return s64_init_copy(data, src);
  if (type == &g_sym_Str)
    return str_init_copy(data, src);
  if (type == &g_sym_Struct)
    return struct_init_copy(data, src);
  if (type == &g_sym_StructType)
    return struct_type_init_copy(data, src);
  if (type == &g_sym_Sw)
    return sw_init_copy(data, src);
  if (type == &g_sym_Sym)
    return sym_init_copy(data, src);
  if (type == &g_sym_Tag)
    return tag_init_copy(data, src);
  if (type == &g_sym_Time)
    return time_init_copy(data, src);
  if (type == &g_sym_Tuple)
    return tuple_init_copy(data, src);
  if (type == &g_sym_U8)
    return u8_init_copy(data, src);
  if (type == &g_sym_U16)
    return u16_init_copy(data, src);
  if (type == &g_sym_U32)
    return u32_init_copy(data, src);
  if (type == &g_sym_U64)
    return u64_init_copy(data, src);
  if (type == &g_sym_Uw)
    return uw_init_copy(data, src);
  if (type == &g_sym_Var)
    return tag_init_copy(data, src);
  if (type == &g_sym_Void)
    return data;
  if (! struct_type_find(type, &st))
    return NULL;
  if (st)
    return struct_type_copy_data(st, data, src);
  err_write_1("data_init_copy: unknown type: ");
  err_inspect_sym(&type);
  err_write_1("\n");
  assert(! "data_init_copy: unknown type");
  return NULL;
}
