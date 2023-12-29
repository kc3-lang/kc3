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
