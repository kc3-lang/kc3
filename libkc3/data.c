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

sw data_buf_inspect (s_buf *buf, const s_sym *type, const void *data)
{
  s_struct s = {0};
  s_struct_type *st;
  if (type == &g_sym_Array ||
      sym_is_array_type(type))
    return buf_inspect_array(buf, data);
  if (type == &g_sym_Block)
    return buf_inspect_do_block(buf, data);
  if (type == &g_sym_Bool)
    return buf_inspect_bool(buf, *(bool *) data);
  if (type == &g_sym_Call)
    return buf_inspect_call(buf, *(p_call *) data);
  if (type == &g_sym_Callable ||
      type == &g_sym_Cfn ||
      type == &g_sym_Fn)
    return buf_inspect_pcallable(buf, data);
  if (type == &g_sym_Character)
    return buf_inspect_character(buf, *(character *) data);
  if (type == &g_sym_Complex)
    return buf_inspect_pcomplex(buf, data);
  if (type == &g_sym_Cow)
    return buf_inspect_pcow(buf, data);
  if (type == &g_sym_F32)
    return buf_inspect_f32(buf, *(f32 *) data);
  if (type == &g_sym_F64)
    return buf_inspect_f64(buf, *(f64 *) data);
#if HAVE_F80
  if (type == &g_sym_F80)
    return buf_inspect_f80(buf, *(f80 *) data);
#endif
#if HAVE_F128
  if (type == &g_sym_F128)
    return buf_inspect_f128(buf, *(f128 *) data);
#endif
  if (type == &g_sym_Fact)
    return buf_inspect_fact(buf, data);
  if (type == &g_sym_Ident)
    return buf_inspect_ident(buf, data);
  if (type == &g_sym_Integer)
    return buf_inspect_integer(buf, data);
  if (type == &g_sym_List)
    return buf_inspect_plist(buf, data);
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
  if (type == &g_sym_Ratio)
    return buf_inspect_ratio(buf, data);
  if (type == &g_sym_S8)
    return buf_inspect_s8(buf, *(s8 *) data);
  if (type == &g_sym_S16)
    return buf_inspect_s16(buf, *(s16 *) data);
  if (type == &g_sym_S32)
    return buf_inspect_s32(buf, *(s32 *) data);
  if (type == &g_sym_S64)
    return buf_inspect_s64(buf, *(s64 *) data);
  if (type == &g_sym_Str)
    return buf_inspect_str(buf, data);
  if (type == &g_sym_Struct)
    return buf_inspect_pstruct(buf, data);
  if (type == &g_sym_StructType)
    return buf_inspect_pstruct_type(buf, data);
  if (type == &g_sym_Sw)
    return buf_inspect_sw(buf, *(sw *) data);
  if (type == &g_sym_Sym)
    return buf_inspect_psym(buf, data);
  if (type == &g_sym_Tag)
    return buf_inspect_tag(buf, data);
  if (type == &g_sym_Time)
    return buf_inspect_time(buf, data);
  if (type == &g_sym_Tuple)
    return buf_inspect_tuple(buf, data);
  if (type == &g_sym_U8)
    return buf_inspect_u8(buf, *(u8 *) data);
  if (type == &g_sym_U16)
    return buf_inspect_u16(buf, *(u16 *) data);
  if (type == &g_sym_U32)
    return buf_inspect_u32(buf, *(u32 *) data);
  if (type == &g_sym_U64)
    return buf_inspect_u64(buf, *(u64 *) data);
  if (type == &g_sym_Uw)
    return buf_inspect_uw(buf, *(uw *) data);
  if (type == &g_sym_Var)
    return buf_inspect_var(buf, data);
  if (type == &g_sym_Void)
    return buf_inspect_void(buf, data);
  if (type == &g_sym_Pointer ||
      sym_is_pointer_type(type, NULL))
    return buf_inspect_pointer(buf, data);
  if (! pstruct_type_find(type, &st))
    return -1;
  if (st) {
    s.pstruct_type = st;
    s.data = (void *) data;
    return buf_inspect_struct(buf, &s);
  }
  err_write_1("data_buf_inspect: unknown type: ");
  err_inspect_sym(type);
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
  if (type == &g_sym_Block)
    return buf_inspect_do_block_size(pretty, data);
  if (type == &g_sym_Bool)
    return buf_inspect_bool_size(pretty, *(bool *) data);
  if (type == &g_sym_Call)
    return buf_inspect_call_size(pretty, *(p_call *) data);
  if (type == &g_sym_Callable ||
      type == &g_sym_Cfn ||
      type == &g_sym_Fn)
    return buf_inspect_callable_size(pretty, *(p_callable *) data);
  if (type == &g_sym_Character)
    return buf_inspect_character_size(pretty, *(character *) data);
  if (type == &g_sym_Complex)
    return buf_inspect_pcomplex_size(pretty, data);
  if (type == &g_sym_F32)
    return buf_inspect_f32_size(pretty, *(f32 *) data);
  if (type == &g_sym_F64)
    return buf_inspect_f64_size(pretty, *(f64 *) data);
#if HAVE_F80
  if (type == &g_sym_F80)
    return buf_inspect_f80_size(pretty, *(f80 *) data);
#endif
#if HAVE_F128
  if (type == &g_sym_F128)
    return buf_inspect_f128_size(pretty, *(f128 *) data);
#endif
  if (type == &g_sym_Fact)
    return buf_inspect_fact_size(pretty, data);
  if (type == &g_sym_Ident)
    return buf_inspect_ident_size(pretty, data);
  if (type == &g_sym_Integer)
    return buf_inspect_integer_size(pretty, data);
  if (type == &g_sym_List)
    return buf_inspect_plist_size(pretty, data);
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
  if (type == &g_sym_Ratio)
    return buf_inspect_ratio_size(pretty, data);
  if (type == &g_sym_S8)
    return buf_inspect_s8_size(pretty, *(s8 *) data);
  if (type == &g_sym_S16)
    return buf_inspect_s16_size(pretty, *(s16 *) data);
  if (type == &g_sym_S32)
    return buf_inspect_s32_size(pretty, *(s32 *) data);
  if (type == &g_sym_S64)
    return buf_inspect_s64_size(pretty, *(s64 *) data);
  if (type == &g_sym_Str)
    return buf_inspect_str_size(pretty, data);
  if (type == &g_sym_Struct)
    return buf_inspect_struct_size(pretty, * (p_struct *) data);
  if (type == &g_sym_StructType)
    return buf_inspect_struct_type_size(pretty,
                                        * (p_struct_type *) data);
  if (type == &g_sym_Sw)
    return buf_inspect_sw_size(pretty, *(sw *) data);
  if (type == &g_sym_Sym)
    return buf_inspect_psym_size(pretty, data);
  if (type == &g_sym_Tag)
    return buf_inspect_tag_size(pretty, data);
  if (type == &g_sym_Time)
    return buf_inspect_time_size(pretty, data);
  if (type == &g_sym_Tuple)
    return buf_inspect_tuple_size(pretty, data);
  if (type == &g_sym_U8)
    return buf_inspect_u8_size(pretty, *(u8 *) data);
  if (type == &g_sym_U16)
    return buf_inspect_u16_size(pretty, *(u16 *) data);
  if (type == &g_sym_U32)
    return buf_inspect_u32_size(pretty, *(u32 *) data);
  if (type == &g_sym_U64)
    return buf_inspect_u64_size(pretty, *(u64 *) data);
  if (type == &g_sym_Uw)
    return buf_inspect_uw_size(pretty, *(uw *) data);
  if (type == &g_sym_Var)
    return buf_inspect_var_size(pretty, data);
  if (type == &g_sym_Void)
    return buf_inspect_void_size(pretty, data);
  if (type == &g_sym_Pointer ||
      sym_is_pointer_type(type, NULL))
    return buf_inspect_pointer_size(pretty, data);
  if (! pstruct_type_find(type, &st))
    return -1;
  if (st) {
    s.pstruct_type = st;
    s.data = (void *) data;
    return buf_inspect_struct_size(pretty, &s);
  }
  err_write_1("data_buf_inspect_size: unknown type: ");
  err_inspect_sym(type);
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
  if (type == &g_sym_Block) {
    do_block_clean(data);
    return true;
  }
  if (type == &g_sym_Bool) {
    return true;
  }
  if (type == &g_sym_Call) {
    pcall_clean(data);
    return true;
  }
  if (type == &g_sym_Callable ||
      type == &g_sym_Cfn ||
      type == &g_sym_Fn) {
    pcallable_clean(data);
    return true;
  }
  if (type == &g_sym_Character) {
    return true;
  }
  if (type == &g_sym_Complex) {
    pcomplex_clean(data);
    return true;
  }
  if (type == &g_sym_F32) {
    return true;
  }
  if (type == &g_sym_F64) {
    return true;
  }
#if HAVE_F80
  if (type == &g_sym_F80) {
    return true;
  }
#endif
#if HAVE_F128
  if (type == &g_sym_F128) {
    return true;
  }
#endif
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
  if (type == &g_sym_Ratio) {
    ratio_clean(data);
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
    pstruct_clean(data);
    return true;
  }
  if (type == &g_sym_StructType) {
    pstruct_type_clean(data);
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
    var_delete(data);
    return true;
  }
  if (type == &g_sym_Void) {
    return true;
  }
  if (type == &g_sym_Pointer ||
      sym_is_pointer_type(type, NULL))
    return true;
  if (! pstruct_type_find(type, &st))
    return false;
  if (st) {
    if (! pstruct_type_init_copy(&s.pstruct_type, &st))
      return false;
    s.data = data;
    struct_clean(&s);
    return true;
  }
  err_write_1("data_clean: unknown type: ");
  err_inspect_sym(type);
  err_write_1("\n");
  assert(! "data_clean: unknown type");
  return false;
}

s8 data_compare (const s_sym *type, const void *a, const void *b)
{
  s_struct sa = {0};
  s_struct sb = {0};
  s_struct_type *st;
  if (type == &g_sym_Block)
    return compare_do_block(a, b);
  if (type == &g_sym_Bool)
    return compare_bool(*(bool *) a, *(bool *) b);
  if (type == &g_sym_Call)
    return compare_call(*(p_call *) a, *(p_call *) b);
  if (type == &g_sym_Callable ||
      type == &g_sym_Cfn ||
      type == &g_sym_Fn)
    return compare_callable(*(p_callable *) a, *(p_callable *) b);
  if (type == &g_sym_Character)
    return compare_character(*(character *) a, *(character *) b);
  if (type == &g_sym_Complex)
    return compare_pcomplex(a, b);
  if (type == &g_sym_F32)
    return compare_f32(*(f32 *) a, *(f32 *) b);
  if (type == &g_sym_F64)
    return compare_f64(*(f64 *) a, *(f64 *) b);
#if HAVE_F80
  if (type == &g_sym_F80)
    return compare_f80(*(f80 *) a, *(f80 *) b);
#endif
#if HAVE_F128
  if (type == &g_sym_F128)
    return compare_f128(*(f128 *) a, *(f128 *) b);
#endif
  if (type == &g_sym_Fact)
    return compare_fact(a, b);
  if (type == &g_sym_Ident)
    return compare_ident(a, b);
  if (type == &g_sym_Integer)
    return compare_integer(a, b);
  if (type == &g_sym_List)
    return compare_plist(a, b);
  if (type == &g_sym_Map)
    return compare_map(a, b);
  if (type == &g_sym_Ptag)
    return compare_ptag((const p_tag) a, (const p_tag) b);
  if (type == &g_sym_Ptr)
    return compare_ptr(a, b);
  if (type == &g_sym_PtrFree)
    return compare_ptr(a, b);
  if (type == &g_sym_Quote)
    return compare_quote(a, b);
  if (type == &g_sym_Ratio)
    return compare_ratio(a, b);
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
    return compare_struct(*(p_struct *) a, * (p_struct *) b);
  if (type == &g_sym_StructType)
    return compare_struct_type(* (p_struct_type *) a,
                               * (p_struct_type *) b);
  if (type == &g_sym_Sw)
    return compare_sw(*(sw *) a, *(sw *) b);
  if (type == &g_sym_Sym)
    return compare_psym(a, b);
  if (type == &g_sym_Tag)
    return compare_tag(a, b);
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
    return compare_var(a, b);
  if (type == &g_sym_Void)
    return 0;
  if (type == &g_sym_Pointer ||
      sym_is_pointer_type(type, NULL))
    return compare_pointer(a, b);
  if (env_global()->loaded) {
    if (! pstruct_type_find(type, &st))
      return COMPARE_ERROR;
    if (st) {
      sa.pstruct_type = st;
      sa.data = (void *) a;
      sb.pstruct_type = st;
      sb.data = (void *) b;
      return compare_struct(&sa, &sb);
    }
  }
  if (type == &g_sym_Array ||
      sym_is_array_type(type))
    return compare_array(a, b);
  err_write_1("data_compare: unknown type: ");
  err_inspect_sym(type);
  err_write_1("\n");
  assert(! "data_compare: unknown type");
  return COMPARE_ERROR;
}

bool data_hash_update (const s_sym *type, t_hash *hash,
                       const void *data)
{
  s_struct s = {0};
  s_struct_type *st;
  if (type == &g_sym_Array ||
      sym_is_array_type(type))
    return hash_update_array(hash, data);
  if (type == &g_sym_Block)
    return hash_update_do_block(hash, data);
  if (type == &g_sym_Bool)
    return hash_update_bool(hash, *(bool *) data);
  if (type == &g_sym_Call)
    return hash_update_call(hash, *(p_call *) data);
  if (type == &g_sym_Callable ||
      type == &g_sym_Cfn ||
      type == &g_sym_Fn)
    return hash_update_callable(hash, *(p_callable *) data);
  if (type == &g_sym_Character)
    return hash_update_character(hash, *(character *) data);
  if (type == &g_sym_Complex)
    return hash_update_pcomplex(hash, data);
  if (type == &g_sym_F32)
    return hash_update_f32(hash, *(f32 *) data);
  if (type == &g_sym_F64)
    return hash_update_f64(hash, *(f64 *) data);
#if HAVE_F80
  if (type == &g_sym_F80)
    return hash_update_f80(hash, *(f80 *) data);
#endif
#if HAVE_F128
  if (type == &g_sym_F128)
    return hash_update_f128(hash, *(f128 *) data);
#endif
  if (type == &g_sym_Fact)
    return hash_update_fact(hash, data);
  if (type == &g_sym_Ident)
    return hash_update_ident(hash, data);
  if (type == &g_sym_Integer)
    return hash_update_integer(hash, data);
  if (type == &g_sym_List)
    return hash_update_plist(hash, data);
  if (type == &g_sym_Map)
    return hash_update_map(hash, data);
  if (type == &g_sym_Ptag)
    return hash_update_ptag(hash, data);
  if (type == &g_sym_Ptr)
    return hash_update_ptr(hash, data);
  if (type == &g_sym_PtrFree)
    return hash_update_ptr_free(hash, data);
  if (type == &g_sym_Quote)
    return hash_update_quote(hash, data);
  if (type == &g_sym_Ratio)
    return hash_update_ratio(hash, data);
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
    return hash_update_struct(hash, * (p_struct *) data);
  if (type == &g_sym_StructType)
    return hash_update_struct_type(hash, * (p_struct_type *) data);
  if (type == &g_sym_Sw)
    return hash_update_sw(hash, *(sw *) data);
  if (type == &g_sym_Sym)
    return hash_update_psym(hash, data);
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
  if (type == &g_sym_Pointer ||
      sym_is_pointer_type(type, NULL))
    return hash_update_pointer(hash, data);
  if (! pstruct_type_find(type, &st))
    return false;
  if (st) {
    s.pstruct_type = st;
    s.data = (void *) data;
    return hash_update_struct(hash, &s);
  }
  err_write_1("data_hash_update: unknown type: ");
  err_inspect_sym(type);
  err_write_1("\n");
  assert(! "data_hash_update: unknown type");
  return false;
}

void * data_init_cast (void *data, p_sym *type, s_tag *tag)
{
  p_struct s = NULL;
  s_struct_type *st;
  const s_sym *t;
  t = *type;
  if (t == &g_sym_Array ||
      sym_is_array_type(*type))
    return array_init_cast(data, type, tag);
  if (t == &g_sym_Block)
    return do_block_init_cast(data, type, tag);
  if (t == &g_sym_Bool)
    return bool_init_cast(data, type, tag);
  if (t == &g_sym_Call)
    return pcall_init_cast(data, type, tag);
  if (t == &g_sym_Callable ||
      t == &g_sym_Cfn ||
      t == &g_sym_Fn)
    return pcallable_init_cast(data, type, tag);
  if (t == &g_sym_Character)
    return character_init_cast(data, type, tag);
  if (t == &g_sym_Complex)
    return pcomplex_init_cast(data, type, tag);
  if (t == &g_sym_F32)
    return f32_init_cast(data, type, tag);
  if (t == &g_sym_F64)
    return f64_init_cast(data, type, tag);
#if HAVE_F80
  if (t == &g_sym_F80)
    return f80_init_cast(data, type, tag);
#endif
#if HAVE_F128
  if (t == &g_sym_F128)
    return f128_init_cast(data, type, tag);
#endif
  if (t == &g_sym_Fact)
    return fact_init_cast(data, type, tag);
  if (t == &g_sym_Ident)
    return ident_init_cast(data, type, tag);
  if (t == &g_sym_Integer)
    return integer_init_cast(data, type, tag);
  if (t == &g_sym_List)
    return plist_init_cast(data, type, tag);
  if (t == &g_sym_Map)
    return map_init_cast(data, type, tag);
  if (t == &g_sym_Ptag)
    return ptag_init_cast(data, type, tag);
  if (t == &g_sym_Ptr)
    return ptr_init_cast(data, type, tag);
  if (t == &g_sym_PtrFree)
    return ptr_free_init_cast(data, type, tag);
  if (t == &g_sym_Quote)
    return quote_init_cast(data, type, tag);
  if (t == &g_sym_Ratio)
    return ratio_init_cast(data, type, tag);
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
    return pstruct_init_cast(data, type, tag);
  if (t == &g_sym_StructType)
    return pstruct_type_init_cast(data, type, tag);
  if (t == &g_sym_Sw)
    return sw_init_cast(data, type, tag);
  if (t == &g_sym_Sym)
    return psym_init_cast(data, type, tag);
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
    return pvar_init_cast(data, type, tag);
  if (t == &g_sym_Void)
    return data;
  if (t == &g_sym_Pointer ||
      sym_is_pointer_type(t, NULL))
    return pointer_init_cast(data, type, tag);
  if (! pstruct_type_find(t, &st))
    return NULL;
  if (st)
    return pstruct_init_cast(&s, type, tag);
  err_write_1("data_init_cast: unknown type: ");
  err_inspect_sym(t);
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
  if (type == &g_sym_Block)
    return do_block_init_copy(data, src);
  if (type == &g_sym_Bool)
    return bool_init_copy(data, src);
  if (type == &g_sym_Call)
    return pcall_init_copy(data, src);
  if (type == &g_sym_Callable ||
      type == &g_sym_Cfn ||
      type == &g_sym_Fn)
    return pcallable_init_copy(data, src);
  if (type == &g_sym_Character)
    return character_init_copy(data, src);
  if (type == &g_sym_Complex)
    return pcomplex_init_copy(data, src);
  if (type == &g_sym_Cow)
    return pcow_init_copy(data, src);
  if (type == &g_sym_F32)
    return f32_init_copy(data, *(f32 *) src);
  if (type == &g_sym_F64)
    return f64_init_copy(data, *(f64 *) src);
#if HAVE_F80
  if (type == &g_sym_F80)
    return f80_init_copy(data, *(f80 *) src);
#endif
#if HAVE_F128
  if (type == &g_sym_F128)
    return f128_init_copy(data, *(f128 *) src);
#endif
  if (type == &g_sym_Fact)
    return fact_init_copy(data, src);
  if (type == &g_sym_Ident)
    return ident_init_copy(data, src);
  if (type == &g_sym_Integer)
    return integer_init_copy(data, src);
  if (type == &g_sym_List)
    return plist_init_copy(data, src);
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
  if (type == &g_sym_Ratio)
    return ratio_init_copy(data, src);
  if (type == &g_sym_S8)
    return s8_init_copy(data, *(s8 *) src);
  if (type == &g_sym_S16)
    return s16_init_copy(data, *(s16 *) src);
  if (type == &g_sym_S32)
    return s32_init_copy(data, *(s32 *) src);
  if (type == &g_sym_S64)
    return s64_init_copy(data, *(s64 *) src);
  if (type == &g_sym_Str)
    return str_init_copy(data, src);
  if (type == &g_sym_Struct)
    return pstruct_init_copy(data, src);
  if (type == &g_sym_StructType)
    return pstruct_type_init_copy(data, src);
  if (type == &g_sym_Sw)
    return sw_init_copy(data, *(sw *) src);
  if (type == &g_sym_Sym)
    return psym_init_copy(data, src);
  if (type == &g_sym_Tag)
    return tag_init_copy(data, src);
  if (type == &g_sym_Time)
    return time_init_copy(data, src);
  if (type == &g_sym_Tuple)
    return tuple_init_copy(data, src);
  if (type == &g_sym_U8)
    return u8_init_copy(data, *(u8 *) src);
  if (type == &g_sym_U16)
    return u16_init_copy(data, *(u16 *) src);
  if (type == &g_sym_U32)
    return u32_init_copy(data, *(u32 *) src);
  if (type == &g_sym_U64)
    return u64_init_copy(data, *(u64 *) src);
  if (type == &g_sym_Uw)
    return uw_init_copy(data, *(uw *) src);
  if (type == &g_sym_Var)
    return tag_init_copy(data, src);
  if (type == &g_sym_Void)
    return data;
  if (type == &g_sym_Pointer ||
      sym_is_pointer_type(type, NULL))
    return pointer_init_copy(data, src);
  if (! pstruct_type_find(type, &st))
    return NULL;
  if (st)
    return struct_type_copy_data(st, data, src);
  err_write_1("data_init_copy: unknown type: ");
  err_inspect_sym(type);
  err_write_1("\n");
  assert(! "data_init_copy: unknown type");
  return NULL;
}
