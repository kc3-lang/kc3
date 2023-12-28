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

bool void_init_copy (const s_sym *type, void *v, const void *src)
{
  if (type == sym_1("Array")) {
    array_init_copy;
    return true;
  }
  if (type == sym_1("Bool")) {
    bool_init_copy;
    return true;
  }
  if (type == sym_1("Call")) {
    call_init_copy;
    return true;
  }
  if (type == sym_1("Cfn")) {
    cfn_init_copy;
    return true;
  }
  if (type == sym_1("Character")) {
    character_init_copy;
    return true;
  }
  if (type == sym_1("F32")) {
    f32_init_copy;
    return true;
  }
  if (type == sym_1("F64")) {
    f64_init_copy;
    return true;
  }
  if (type == sym_1("Fact")) {
    fact_init_copy;
    return true;
  }
  if (type == sym_1("Fn")) {
    fn_init_copy;
    return true;
  }
  if (type == sym_1("Ident")) {
    ident_init_copy;
    return true;
  }
  if (type == sym_1("Integer")) {
    integer_init_copy;
    return true;
  }
  if (type == sym_1("List")) {
    list_init_copy;
    return true;
  }
  if (type == sym_1("Ptag")) {
    ptag_init_copy;
    return true;
  }
  if (type == sym_1("Ptr")) {
    ptr_init_copy;
    return true;
  }
  if (type == sym_1("PtrFree")) {
    ptr_free_init_copy;
    return true;
  }
  if (type == sym_1("Quote")) {
    quote_init_copy;
    return true;
  }
  if (type == sym_1("S8")) {
    s8_init_copy;
    return true;
  }
  if (type == sym_1("S16")) {
    s16_init_copy;
    return true;
  }
  if (type == sym_1("S32")) {
    s32_init_copy;
    return true;
  }
  if (type == sym_1("S64")) {
    s64_init_copy;
    return true;
  }
  if (type == sym_1("Str")) {
    str_init_copy;
    return true;
  }
  if (type == sym_1("Struct")) {
    struct_init_copy;
    return true;
  }
  if (type == sym_1("Sw")) {
    sw_init_copy;
    return true;
  }
  if (type == sym_1("Sym")) {
    sym_init_copy;
    return true;
  }
  if (type == sym_1("Tuple")) {
    tuple_init_copy;
    return true;
  }
  if (type == sym_1("U8")) {
    u8_init_copy;
    return true;
  }
  if (type == sym_1("U16")) {
    u16_init_copy;
    return true;
  }
  if (type == sym_1("U32")) {
    u32_init_copy;
    return true;
  }
  if (type == sym_1("U64")) {
    u64_init_copy;
    return true;
  }
  if (type == sym_1("Uw")) {
    uw_init_copy;
    return true;
  }
  if (type == sym_1("Var")) {
    *dest = NULL;
    return true;
  }
  if (type == sym_1("Void")) {
    *dest = NULL;
    return true;
  }
  if (struct_type_exists(type)) {
    struct_init_copy;
    return true;
  }
  err_write_1("void_init_copy: unknown type: ");
  err_inspect_sym(&type);
  err_write_1("\n");
  assert(! "void_init_copy: unknown type");
  return false;
}
