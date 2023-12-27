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

bool clean (const s_sym *type, void *data)
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
  if (struct_type_exists(type)) {
    s_struct s = {0};
    struct_init_with_data(&s, type, false, data);
    struct_clean(&s);
    return true;
  }
  err_write_1("sym_to_clean: unknown type: ");
  err_inspect_sym(&type);
  err_write_1("\n");
  assert(! "sym_to_clean: unknown type");
  return false;
}
