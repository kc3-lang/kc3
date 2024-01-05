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
#include <stdlib.h>
#include <string.h>
#include "c3.h"

void         sym_delete (s_sym *sym);
s_str *      sym_inspect_reserved (const s_sym *sym, s_str *dest);
sw           sym_inspect_reserved_size (const s_sym *sym);
s_sym_list * sym_list_new (s_sym *sym, s_sym_list *next);

static s_sym_list * g_sym_list = NULL;

const s_sym * sym_1 (const s8 *p)
{
  s_str stra;
  str_init_1(&stra, NULL, p);
  return str_to_sym(&stra);
}

bool sym_character_is_reserved (character c)
{
  return (character_is_space(c) ||
          c == '#' ||
          c == '(' ||
          c == ')' ||
          c == ',' ||
          c == '.' ||
          c == ':' ||
          c == ';' ||
          c == '[' ||
          c == ']' ||
          c == '{' ||
          c == '}');
}

void sym_delete (s_sym *sym)
{
  str_clean(&sym->str);
  free(sym);
}
  
void sym_delete_all (void)
{
  s_sym_list *sym_list;
  sym_list = g_sym_list;
  while (sym_list) {
    s_sym_list *tmp;
    tmp = sym_list;
    sym_list = sym_list->next;
    sym_delete(tmp->sym);
    free(tmp);
  }
  g_sym_list = NULL;
}

const s_sym * sym_find (const s_str *str)
{
  s_sym_list *sym_list;
  sym_list = g_sym_list;
  while (sym_list) {
    s_sym *sym = sym_list->sym;
    if (compare_str(str, &sym->str) == 0)
      return sym;
    sym_list = sym_list->next;
  }
  return NULL;
}

bool sym_has_reserved_characters (const s_sym *sym)
{
  character c;
  sw r;
  s_str stra;
  str_init(&stra, NULL, sym->str.size, sym->str.ptr.p);
  while (1) {
    if ((r = str_read_character_utf8(&stra, &c)) <= 0)
      break;
    if (c == '.') {
      if ((r = str_peek_character_utf8(&stra, &c)) <= 0)
        return true;
      if (! character_is_uppercase(c))
        return true;
    }
    else if (sym_character_is_reserved(c))
      return true;
  }
  if (r < 0)
    return true;
  return false;
}

const s_sym ** sym_init_1 (const s_sym **sym, const s8 *p)
{
  assert(sym);
  assert(p);
  *sym = sym_1(p);
  return sym;
}

const s_sym ** sym_init_cast (const s_sym **sym, const s_tag *tag)
{
  assert(sym);
  assert(tag);
  switch (tag->type) {
  case TAG_STR:
    return sym_init_str(sym, &tag->data.str);
  case TAG_SYM:
    return sym_init_copy(sym, &tag->data.sym);
  default:
    break;
  }
  err_write_1("sym_init_cast: cannot cast ");
  err_write_1(tag_type_to_string(tag->type));
  err_puts(" to Sym");
  assert(! "sym_init_cast: cannot cast to Sym");
  return NULL;
}

const s_sym ** sym_init_copy (const s_sym **sym,
                              const s_sym * const *src)
{
  assert(src);
  assert(sym);
  *sym = *src;
  return sym;
}

const s_sym ** sym_init_str (const s_sym **sym, const s_str *src)
{
  const s_sym *tmp;
  tmp = sym_find(src);
  if (! tmp)
    tmp = sym_new(src);
  if (! tmp)
    return NULL;
  *sym = tmp;
  return sym;
}

s_str * sym_inspect (const s_sym *sym, s_str *dest)
{
  sw size;
  s_buf tmp;
  size = buf_inspect_sym_size(&sym);
  if (size < 0) {
    assert(! "error");
    return NULL;
  }
  buf_init_alloc(&tmp, size);
  buf_inspect_sym(&tmp, &sym);
  assert(tmp.wpos == tmp.size);
  return buf_to_str(&tmp, dest);
}

bool sym_is_module (const s_sym *sym)
{
  character c;
  if (str_peek_character_utf8(&sym->str, &c) <= 0)
    return false;
  return character_is_uppercase(c);
}

s_sym_list * sym_list_new (s_sym *sym, s_sym_list *next)
{
  s_sym_list *sym_list;
  sym_list = malloc(sizeof(s_sym_list));
  if (! sym_list) {
    err_puts("sym_list_new: failed to allocate memory");
    assert(! "sym_list_new: failed to allocate memory");
    return NULL;
  }
  sym_list->sym = sym;
  sym_list->next = next;
  return sym_list;
}

const s_sym * sym_new (const s_str *src)
{
  s_sym *sym;
  s_sym_list *tmp;
  sym = malloc(sizeof(s_sym));
  if (! sym) {
    err_puts("sym_new: failed to allocate memory");
    assert(! "sym_new: failed to allocate memory");
    return NULL;
  }
  if (! str_init_copy(&sym->str, src)) {
    free(sym);
    return NULL;
  }
  tmp = sym_list_new(sym, g_sym_list);
  if (! tmp) {
    str_clean(&sym->str);
    free(sym);
    return NULL;
  }
  g_sym_list = tmp;
  return sym;
}

/*
bool sym_to_buf_inspect (const s_sym *type, f_buf_inspect *dest)
{
  if (type == sym_1("Array")) {
    *dest = (f_buf_inspect) buf_inspect_array;
    return true;
  }
  if (type == sym_1("Bool")) {
    *dest = (f_buf_inspect) buf_inspect_bool;
    return true;
  }
  if (type == sym_1("Call")) {
    *dest = (f_buf_inspect) buf_inspect_call;
    return true;
  }
  if (type == sym_1("Cfn")) {
    *dest = (f_buf_inspect) buf_inspect_cfn;
    return true;
  }
  if (type == sym_1("Character")) {
    *dest = (f_buf_inspect) buf_inspect_character;
    return true;
  }
  if (type == sym_1("F32")) {
    *dest = (f_buf_inspect) buf_inspect_f32;
    return true;
  }
  if (type == sym_1("F64")) {
    *dest = (f_buf_inspect) buf_inspect_f64;
    return true;
  }
  if (type == sym_1("Fact")) {
    *dest = (f_buf_inspect) buf_inspect_fact;
    return true;
  }
  if (type == sym_1("Fn")) {
    *dest = (f_buf_inspect) buf_inspect_fn;
    return true;
  }
  if (type == sym_1("Ident")) {
    *dest = (f_buf_inspect) buf_inspect_ident;
    return true;
  }
  if (type == sym_1("Integer")) {
    *dest = (f_buf_inspect) buf_inspect_integer;
    return true;
  }
  if (type == sym_1("List")) {
    *dest = (f_buf_inspect) buf_inspect_list;
    return true;
  }
  if (type == sym_1("Ptag")) {
    *dest = (f_buf_inspect) buf_inspect_ptag;
    return true;
  }
  if (type == sym_1("Ptr")) {
    *dest = (f_buf_inspect) buf_inspect_ptr;
    return true;
  }
  if (type == sym_1("PtrFree")) {
    *dest = (f_buf_inspect) buf_inspect_ptr_free;
    return true;
  }
  if (type == sym_1("Quote")) {
    *dest = (f_buf_inspect) buf_inspect_quote;
    return true;
  }
  if (type == sym_1("S8")) {
    *dest = (f_buf_inspect) buf_inspect_s8;
    return true;
  }
  if (type == sym_1("S16")) {
    *dest = (f_buf_inspect) buf_inspect_s16;
    return true;
  }
  if (type == sym_1("S32")) {
    *dest = (f_buf_inspect) buf_inspect_s32;
    return true;
  }
  if (type == sym_1("S64")) {
    *dest = (f_buf_inspect) buf_inspect_s64;
    return true;
  }
  if (type == sym_1("Str")) {
    *dest = (f_buf_inspect) buf_inspect_str;
    return true;
  }
  if (type == sym_1("Struct")) {
    *dest = (f_buf_inspect) buf_inspect_struct;
    return true;
  }
  if (type == sym_1("Sw")) {
    *dest = (f_buf_inspect) buf_inspect_sw;
    return true;
  }
  if (type == sym_1("Sym")) {
    *dest = (f_buf_inspect) buf_inspect_sym;
    return true;
  }
  if (type == sym_1("Tuple")) {
    *dest = (f_buf_inspect) buf_inspect_tuple;
    return true;
  }
  if (type == sym_1("U8")) {
    *dest = (f_buf_inspect) buf_inspect_u8;
    return true;
  }
  if (type == sym_1("U16")) {
    *dest = (f_buf_inspect) buf_inspect_u16;
    return true;
  }
  if (type == sym_1("U32")) {
    *dest = (f_buf_inspect) buf_inspect_u32;
    return true;
  }
  if (type == sym_1("U64")) {
    *dest = (f_buf_inspect) buf_inspect_u64;
    return true;
  }
  if (type == sym_1("Uw")) {
    *dest = (f_buf_inspect) buf_inspect_uw;
    return true;
  }
  if (type == sym_1("Var")) {
    *dest = (f_buf_inspect) buf_inspect_var;
    return true;
  }
  if (type == sym_1("Void")) {
    *dest = (f_buf_inspect) buf_inspect_var;
    return true;
  }
  if (struct_type_exists(type)) {
    *dest = (f_buf_inspect) buf_inspect_struct;
    return true;
  }
  err_write_1("sym_to_buf_inspect: unknown type: ");
  err_write_1(type->str.ptr.ps8);
  err_write_1("\n");
  assert(! "sym_to_buf_inspect: unknown type");
  return false;
}

bool sym_to_buf_inspect_size (const s_sym *type, f_buf_inspect_size *dest)
{
  if (type == sym_1("Array")) {
    *dest = (f_buf_inspect_size) buf_inspect_array_size;
    return true;
  }
  if (type == sym_1("Bool")) {
    *dest = (f_buf_inspect_size) buf_inspect_bool_size;
    return true;
  }
  if (type == sym_1("Call")) {
    *dest = (f_buf_inspect_size) buf_inspect_call_size;
    return true;
  }
  if (type == sym_1("Cfn")) {
    *dest = (f_buf_inspect_size) buf_inspect_cfn_size;
    return true;
  }
  if (type == sym_1("Character")) {
    *dest = (f_buf_inspect_size) buf_inspect_character_size;
    return true;
  }
  if (type == sym_1("F32")) {
    *dest = (f_buf_inspect_size) buf_inspect_f32_size;
    return true;
  }
  if (type == sym_1("F64")) {
    *dest = (f_buf_inspect_size) buf_inspect_f64_size;
    return true;
  }
  if (type == sym_1("Fact")) {
    *dest = (f_buf_inspect_size) buf_inspect_fact_size;
    return true;
  }
  if (type == sym_1("Fn")) {
    *dest = (f_buf_inspect_size) buf_inspect_fn_size;
    return true;
  }
  if (type == sym_1("Ident")) {
    *dest = (f_buf_inspect_size) buf_inspect_ident_size;
    return true;
  }
  if (type == sym_1("Integer")) {
    *dest = (f_buf_inspect_size) buf_inspect_integer_size;
    return true;
  }
  if (type == sym_1("List")) {
    *dest = (f_buf_inspect_size) buf_inspect_list_size;
    return true;
  }
  if (type == sym_1("Ptag")) {
    *dest = (f_buf_inspect_size) buf_inspect_ptag_size;
    return true;
  }
  if (type == sym_1("Ptr")) {
    *dest = (f_buf_inspect_size) buf_inspect_ptr_size;
    return true;
  }
  if (type == sym_1("PtrFree")) {
    *dest = (f_buf_inspect_size) buf_inspect_ptr_free_size;
    return true;
  }
  if (type == sym_1("Quote")) {
    *dest = (f_buf_inspect_size) buf_inspect_quote_size;
    return true;
  }
  if (type == sym_1("S8")) {
    *dest = (f_buf_inspect_size) buf_inspect_s8_size;
    return true;
  }
  if (type == sym_1("S16")) {
    *dest = (f_buf_inspect_size) buf_inspect_s16_size;
    return true;
  }
  if (type == sym_1("S32")) {
    *dest = (f_buf_inspect_size) buf_inspect_s32_size;
    return true;
  }
  if (type == sym_1("S64")) {
    *dest = (f_buf_inspect_size) buf_inspect_s64_size;
    return true;
  }
  if (type == sym_1("Str")) {
    *dest = (f_buf_inspect_size) buf_inspect_str_size;
    return true;
  }
  if (type == sym_1("Struct")) {
    *dest = (f_buf_inspect_size) buf_inspect_struct_size;
    return true;
  }
  if (type == sym_1("Sw")) {
    *dest = (f_buf_inspect_size) buf_inspect_sw_size;
    return true;
  }
  if (type == sym_1("Sym")) {
    *dest = (f_buf_inspect_size) buf_inspect_sym_size;
    return true;
  }
  if (type == sym_1("Tuple")) {
    *dest = (f_buf_inspect_size) buf_inspect_tuple_size;
    return true;
  }
  if (type == sym_1("U8")) {
    *dest = (f_buf_inspect_size) buf_inspect_u8_size;
    return true;
  }
  if (type == sym_1("U16")) {
    *dest = (f_buf_inspect_size) buf_inspect_u16_size;
    return true;
  }
  if (type == sym_1("U32")) {
    *dest = (f_buf_inspect_size) buf_inspect_u32_size;
    return true;
  }
  if (type == sym_1("U64")) {
    *dest = (f_buf_inspect_size) buf_inspect_u64_size;
    return true;
  }
  if (type == sym_1("Uw")) {
    *dest = (f_buf_inspect_size) buf_inspect_uw_size;
    return true;
  }
  if (type == sym_1("Var")) {
    *dest = (f_buf_inspect_size) buf_inspect_var_size;
    return true;
  }
  if (type == sym_1("Void")) {
    *dest = (f_buf_inspect_size) buf_inspect_void_size;
    return true;
  }
  if (struct_type_exists(type)) {
    *dest = (f_buf_inspect_size) buf_inspect_struct_size;
    return true;
  }
  err_write_1("sym_to_buf_inspect_size: unknown type: ");
  err_inspect_sym(&type);
  err_write_1("\n");
  assert(! "sym_to_buf_inspect_size: unknown type");
  return false;
}
*/

bool sym_to_ffi_type (const s_sym *sym, ffi_type *result_type,
                      ffi_type **dest)
{
  assert(sym);
  if (sym == sym_1("Result")) {
    if (! result_type) {
      err_puts("sym_to_ffi_type: invalid result type: Result");
      return false;
    }
    *dest = result_type;
    return true;
  }
  if (sym == sym_1("Array")) {
    *dest = &ffi_type_pointer;
    return true;
  }
  if (sym == sym_1("Bool")) {
    *dest = &ffi_type_uint8;
    return true;
  }
  if (sym == sym_1("Character")) {
    *dest = &ffi_type_uint32;
    return true;
  }
  if (sym == sym_1("F32")) {
    *dest = &ffi_type_float;
    return true;
  }
  if (sym == sym_1("F64")) {
    *dest = &ffi_type_double;
    return true;
  }
  if (sym == sym_1("Fn")) {
    *dest = &ffi_type_pointer;
    return true;
  }
  if (sym == sym_1("Integer")) {
    *dest = &ffi_type_pointer;
    return true;
  }
  if (sym == sym_1("List")) {
    *dest = &ffi_type_pointer;
    return true;
  }
  if (sym == sym_1("Map")) {
    *dest = &ffi_type_pointer;
    return true;
  }
  if (sym == sym_1("Ptr")) {
    *dest = &ffi_type_pointer;
    return true;
  }
  if (sym == sym_1("PtrFree")) {
    *dest = &ffi_type_pointer;
    return true;
  }
  if (sym == sym_1("S8")) {
    *dest = &ffi_type_sint8;
    return true;
  }
  if (sym == sym_1("S16")) {
    *dest = &ffi_type_sint16;
    return true;
  }
  if (sym == sym_1("S32")) {
    *dest = &ffi_type_sint32;
    return true;
  }
  if (sym == sym_1("S64")) {
    *dest = &ffi_type_sint64;
    return true;
  }
  if (sym == sym_1("Str")) {
    *dest = &ffi_type_pointer;
    return true;
  }
  if (sym == sym_1("Struct")) {
    *dest = &ffi_type_pointer;
    return true;
  }
  if (sym == sym_1("Sym")) {
    *dest = &ffi_type_pointer;
    return true;
  }
  if (sym == sym_1("Sw")) {
    *dest = &ffi_type_slong;
    return true;
  }
  if (sym == sym_1("Tag")) {
    *dest = &ffi_type_pointer;
    return true;
  }
  if (sym == sym_1("U8")) {
    *dest = &ffi_type_uint8;
    return true;
  }
  if (sym == sym_1("U16")) {
    *dest = &ffi_type_uint16;
    return true;
  }
  if (sym == sym_1("U32")) {
    *dest = &ffi_type_uint32;
    return true;
  }
  if (sym == sym_1("U64")) {
    *dest = &ffi_type_uint64;
    return true;
  }
  if (sym == sym_1("Uw")) {
    *dest = &ffi_type_ulong;
    return true;
  }
  if (sym == sym_1("Void")) {
    *dest = &ffi_type_void;
    return true;
  }
  if (struct_type_exists(sym)) {
    *dest = &ffi_type_pointer;
    return true;
  }
  err_write_1("sym_to_ffi_type: unknown type: ");
  err_inspect_sym(&sym);
  err_write_1("\n");
  assert(! "sym_to_ffi_type: unknown type");
  return false;
}

/*
bool sym_to_init_cast (const s_sym *type, f_init_cast *dest)
{
  if (type == sym_1("Array")) {
    *dest = (f_init_cast) array_init_cast;
    return true;
  }
  if (type == sym_1("Bool")) {
    *dest = (f_init_cast) bool_init_cast;
    return true;
  }
  if (type == sym_1("Call")) {
    *dest = (f_init_cast) call_init_cast;
    return true;
  }
  if (type == sym_1("Cfn")) {
    *dest = (f_init_cast) cfn_init_cast;
    return true;
  }
  if (type == sym_1("Character")) {
    *dest = (f_init_cast) character_init_cast;
    return true;
  }
  if (type == sym_1("F32")) {
    *dest = (f_init_cast) f32_init_cast;
    return true;
  }
  if (type == sym_1("F64")) {
    *dest = (f_init_cast) f64_init_cast;
    return true;
  }
  if (type == sym_1("Fact")) {
    *dest = (f_init_cast) fact_init_cast;
    return true;
  }
  if (type == sym_1("Fn")) {
    *dest = (f_init_cast) fn_init_cast;
    return true;
  }
  if (type == sym_1("Ident")) {
    *dest = (f_init_cast) ident_init_cast;
    return true;
  }
  if (type == sym_1("Integer")) {
    *dest = (f_init_cast) integer_init_cast;
    return true;
  }
  if (type == sym_1("List")) {
    *dest = (f_init_cast) list_init_cast;
    return true;
  }
  if (type == sym_1("Ptag")) {
    *dest = (f_init_cast) ptag_init_cast;
    return true;
  }
  if (type == sym_1("Ptr")) {
    *dest = (f_init_cast) ptr_init_cast;
    return true;
  }
  if (type == sym_1("PtrFree")) {
    *dest = (f_init_cast) ptr_free_init_cast;
    return true;
  }
  if (type == sym_1("Quote")) {
    *dest = (f_init_cast) quote_init_cast;
    return true;
  }
  if (type == sym_1("S8")) {
    *dest = (f_init_cast) s8_init_cast;
    return true;
  }
  if (type == sym_1("S16")) {
    *dest = (f_init_cast) s16_init_cast;
    return true;
  }
  if (type == sym_1("S32")) {
    *dest = (f_init_cast) s32_init_cast;
    return true;
  }
  if (type == sym_1("S64")) {
    *dest = (f_init_cast) s64_init_cast;
    return true;
  }
  if (type == sym_1("Str")) {
    *dest = (f_init_cast) str_init_cast;
    return true;
  }
  if (type == sym_1("Struct")) {
    *dest = (f_init_cast) struct_init_cast;
    return true;
  }
  if (type == sym_1("Sw")) {
    *dest = (f_init_cast) sw_init_cast;
    return true;
  }
  if (type == sym_1("Sym")) {
    *dest = (f_init_cast) sym_init_cast;
    return true;
  }
  if (type == sym_1("Tuple")) {
    *dest = (f_init_cast) tuple_init_cast;
    return true;
  }
  if (type == sym_1("U8")) {
    *dest = (f_init_cast) u8_init_cast;
    return true;
  }
  if (type == sym_1("U16")) {
    *dest = (f_init_cast) u16_init_cast;
    return true;
  }
  if (type == sym_1("U32")) {
    *dest = (f_init_cast) u32_init_cast;
    return true;
  }
  if (type == sym_1("U64")) {
    *dest = (f_init_cast) u64_init_cast;
    return true;
  }
  if (type == sym_1("Uw")) {
    *dest = (f_init_cast) uw_init_cast;
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
    *dest = (f_init_cast) struct_init_cast;
    return true;
  }
  err_write_1("sym_to_init_cast: unknown type: ");
  err_inspect_sym(&type);
  err_write_1("\n");
  assert(! "sym_to_init_cast: unknown type");
  return false;
}

bool sym_to_init_copy (const s_sym *type, f_init_copy *dest)
{
  if (type == sym_1("Array")) {
    *dest = (f_init_copy) array_init_copy;
    return true;
  }
  if (type == sym_1("Bool")) {
    *dest = (f_init_copy) bool_init_copy;
    return true;
  }
  if (type == sym_1("Call")) {
    *dest = (f_init_copy) call_init_copy;
    return true;
  }
  if (type == sym_1("Cfn")) {
    *dest = (f_init_copy) cfn_init_copy;
    return true;
  }
  if (type == sym_1("Character")) {
    *dest = (f_init_copy) character_init_copy;
    return true;
  }
  if (type == sym_1("F32")) {
    *dest = (f_init_copy) f32_init_copy;
    return true;
  }
  if (type == sym_1("F64")) {
    *dest = (f_init_copy) f64_init_copy;
    return true;
  }
  if (type == sym_1("Fact")) {
    *dest = (f_init_copy) fact_init_copy;
    return true;
  }
  if (type == sym_1("Fn")) {
    *dest = (f_init_copy) fn_init_copy;
    return true;
  }
  if (type == sym_1("Ident")) {
    *dest = (f_init_copy) ident_init_copy;
    return true;
  }
  if (type == sym_1("Integer")) {
    *dest = (f_init_copy) integer_init_copy;
    return true;
  }
  if (type == sym_1("List")) {
    *dest = (f_init_copy) list_init_copy;
    return true;
  }
  if (type == sym_1("Ptag")) {
    *dest = (f_init_copy) ptag_init_copy;
    return true;
  }
  if (type == sym_1("Ptr")) {
    *dest = (f_init_copy) ptr_init_copy;
    return true;
  }
  if (type == sym_1("PtrFree")) {
    *dest = (f_init_copy) ptr_free_init_copy;
    return true;
  }
  if (type == sym_1("Quote")) {
    *dest = (f_init_copy) quote_init_copy;
    return true;
  }
  if (type == sym_1("S8")) {
    *dest = (f_init_copy) s8_init_copy;
    return true;
  }
  if (type == sym_1("S16")) {
    *dest = (f_init_copy) s16_init_copy;
    return true;
  }
  if (type == sym_1("S32")) {
    *dest = (f_init_copy) s32_init_copy;
    return true;
  }
  if (type == sym_1("S64")) {
    *dest = (f_init_copy) s64_init_copy;
    return true;
  }
  if (type == sym_1("Str")) {
    *dest = (f_init_copy) str_init_copy;
    return true;
  }
  if (type == sym_1("Struct")) {
    *dest = (f_init_copy) struct_init_copy;
    return true;
  }
  if (type == sym_1("Sw")) {
    *dest = (f_init_copy) sw_init_copy;
    return true;
  }
  if (type == sym_1("Sym")) {
    *dest = (f_init_copy) sym_init_copy;
    return true;
  }
  if (type == sym_1("Tuple")) {
    *dest = (f_init_copy) tuple_init_copy;
    return true;
  }
  if (type == sym_1("U8")) {
    *dest = (f_init_copy) u8_init_copy;
    return true;
  }
  if (type == sym_1("U16")) {
    *dest = (f_init_copy) u16_init_copy;
    return true;
  }
  if (type == sym_1("U32")) {
    *dest = (f_init_copy) u32_init_copy;
    return true;
  }
  if (type == sym_1("U64")) {
    *dest = (f_init_copy) u64_init_copy;
    return true;
  }
  if (type == sym_1("Uw")) {
    *dest = (f_init_copy) uw_init_copy;
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
    *dest = (f_init_copy) struct_init_copy;
    return true;
  }
  err_write_1("sym_to_init_copy: unknown type: ");
  err_inspect_sym(&type);
  err_write_1("\n");
  assert(! "sym_to_init_copy: unknown type");
  return false;
}
*/

bool sym_to_tag_type (const s_sym *sym, e_tag_type *dest)
{
  if (sym == sym_1("Array")) {
    *dest = TAG_ARRAY;
    return true;
  }
  if (sym == sym_1("Bool")) {
    *dest = TAG_BOOL;
    return true;
  }
  if (sym == sym_1("Call")) {
    *dest = TAG_CALL;
    return true;
  }
  if (sym == sym_1("Cfn")) {
    *dest = TAG_CFN;
    return true;
  }
  if (sym == sym_1("Character")) {
    *dest = TAG_CHARACTER;
    return true;
  }
  if (sym == sym_1("F32")) {
    *dest = TAG_F32;
    return true;
  }
  if (sym == sym_1("F64")) {
    *dest = TAG_F64;
    return true;
  }
  if (sym == sym_1("Fn")) {
    *dest = TAG_FN;
    return true;
  }
  if (sym == sym_1("Ident")) {
    *dest = TAG_IDENT;
    return true;
  }
  if (sym == sym_1("Integer")) {
    *dest = TAG_INTEGER;
    return true;
  }
  if (sym == sym_1("List")) {
    *dest = TAG_LIST;
    return true;
  }
  if (sym == sym_1("Ptag")) {
    *dest = TAG_PTAG;
    return true;
  }
  if (sym == sym_1("Ptr")) {
    *dest = TAG_PTR;
    return true;
  }
  if (sym == sym_1("PtrFree")) {
    *dest = TAG_PTR_FREE;
    return true;
  }
  if (sym == sym_1("Quote")) {
    *dest = TAG_QUOTE;
    return true;
  }
  if (sym == sym_1("S8")) {
    *dest = TAG_S8;
    return true;
  }
  if (sym == sym_1("S16")) {
    *dest = TAG_S16;
    return true;
  }
  if (sym == sym_1("S32")) {
    *dest = TAG_S32;
    return true;
  }
  if (sym == sym_1("S64")) {
    *dest = TAG_S64;
    return true;
  }
  if (sym == sym_1("Str")) {
    *dest = TAG_STR;
    return true;
  }
  if (sym == sym_1("Struct")) {
    *dest = TAG_STRUCT;
    return true;
  }
  if (sym == sym_1("Sw")) {
    *dest = TAG_SW;
    return true;
  }
  if (sym == sym_1("Sym")) {
    *dest = TAG_SYM;
    return true;
  }
  if (sym == sym_1("Tag")) {
    *dest = TAG_VOID;
    return true;
  }
  if (sym == sym_1("Tuple")) {
    *dest = TAG_TUPLE;
    return true;
  }
  if (sym == sym_1("U8")) {
    *dest = TAG_U8;
    return true;
  }
  if (sym == sym_1("U16")) {
    *dest = TAG_U16;
    return true;
  }
  if (sym == sym_1("U32")) {
    *dest = TAG_U32;
    return true;
  }
  if (sym == sym_1("U64")) {
    *dest = TAG_U64;
    return true;
  }
  if (sym == sym_1("Uw")) {
    *dest = TAG_UW;
    return true;
  }
  if (sym == sym_1("Var")) {
    *dest = TAG_VAR;
    return true;
  }
  if (sym == sym_1("Void")) {
    *dest = TAG_VOID;
    return true;
  }
  if (struct_type_exists(sym)) {
    *dest = TAG_STRUCT;
    return true;
  }
  err_write_1("sym_to_tag_type: unknown type: ");
  err_inspect_sym(&sym);
  err_write_1("\n");
  assert(! "sym_to_tag_type: unknown type");
  return false;
}

bool sym_type_size (const s_sym *type, uw *dest)
{
  const s_struct_type *st;
  if (type == sym_1("Array")) {
    *dest = sizeof(s_array);
    return true;
  }
  if (type == sym_1("Bool")) {
    *dest = sizeof(bool);
    return true;
  }
  if (type == sym_1("Call")) {
    *dest = sizeof(s_call);
    return true;
  }
  if (type == sym_1("Cfn")) {
    *dest = sizeof(s_cfn);
    return true;
  }
  if (type == sym_1("Character")) {
    *dest = sizeof(character);
    return true;
  }
  if (type == sym_1("F32")) {
    *dest = sizeof(f32);
    return true;
  }
  if (type == sym_1("F64")) {
    *dest = sizeof(f64);
    return true;
  }
  if (type == sym_1("Fact")) {
    *dest = sizeof(s_fact);
    return true;
  }
  if (type == sym_1("Fn")) {
    *dest = sizeof(s_fn);
    return true;
  }
  if (type == sym_1("Ident")) {
    *dest = sizeof(s_ident);
    return true;
  }
  if (type == sym_1("Integer")) {
    *dest = sizeof(s_integer);
    return true;
  }
  if (type == sym_1("List")) {
    *dest = sizeof(s_list *);
    return true;
  }
  if (type == sym_1("Ptag")) {
    *dest = sizeof(p_tag);
    return true;
  }
  if (type == sym_1("Ptr")) {
    *dest = sizeof(void *);
    return true;
  }
  if (type == sym_1("PtrFree")) {
    *dest = sizeof(void *);
    return true;
  }
  if (type == sym_1("Quote")) {
    *dest = sizeof(s_quote);
    return true;
  }
  if (type == sym_1("S8")) {
    *dest = sizeof(s8);
    return true;
  }
  if (type == sym_1("S16")) {
    *dest = sizeof(s16);
    return true;
  }
  if (type == sym_1("S32")) {
    *dest = sizeof(s32);
    return true;
  }
  if (type == sym_1("S64")) {
    *dest = sizeof(s64);
    return true;
  }
  if (type == sym_1("Str")) {
    *dest = sizeof(s_str);
    return true;
  }
  if (type == sym_1("Struct")) {
    *dest = sizeof(s_struct);
    return true;
  }
  if (type == sym_1("Sw")) {
    *dest = sizeof(sw);
    return true;
  }
  if (type == sym_1("Sym")) {
    *dest = sizeof(s_sym *);
    return true;
  }
  if (type == sym_1("Tuple")) {
    *dest = sizeof(s_tuple);
    return true;
  }
  if (type == sym_1("U8")) {
    *dest = sizeof(u8);
    return true;
  }
  if (type == sym_1("U16")) {
    *dest = sizeof(u16);
    return true;
  }
  if (type == sym_1("U32")) {
    *dest = sizeof(u32);
    return true;
  }
  if (type == sym_1("U64")) {
    *dest = sizeof(u64);
    return true;
  }
  if (type == sym_1("Uw")) {
    *dest = sizeof(uw);
    return true;
  }
  if (type == sym_1("Var")) {
    *dest = 0;
    return true;
  }
  if (type == sym_1("Void")) {
    *dest = 0;
    return true;
  }
  st = struct_type_find(type);
  if (st) {
    *dest = st->size;
    return true;
  }
  err_write_1("sym_type_size: unknown type: ");
  err_inspect_sym(&type);
  err_write_1("\n");
  assert(! "sym_type_size: unknown type");
  return false;
}
