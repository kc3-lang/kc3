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

const s_sym ** sym_init_copy (const s_sym **sym,
                              const s_sym * const *src)
{
  assert(src);
  assert(sym);
  *sym = *src;
  return sym;
}

s_str * sym_inspect (const s_sym *sym, s_str *dest)
{
  sw size;
  s_buf tmp;
  size = buf_inspect_sym_size(sym);
  if (size < 0) {
    assert(! "error");
    return NULL;
  }
  buf_init_alloc(&tmp, size);
  buf_inspect_sym(&tmp, sym);
  assert(tmp.wpos == tmp.size);
  return buf_to_str(&tmp, dest);
}

bool sym_is_module (const s_sym *sym)
{
  character c;
  if (str_peek_character_utf8(&sym->str, &c) > 0)
    return character_is_uppercase(c);
  return false;
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

f_buf_inspect sym_to_buf_inspect (const s_sym *type)
{
  if (type == sym_1("Bool"))
    return (f_buf_inspect) buf_inspect_bool;
  if (type == sym_1("Call"))
    return (f_buf_inspect) buf_inspect_call;
  if (type == sym_1("Cfn"))
    return (f_buf_inspect) buf_inspect_cfn;
  if (type == sym_1("Character"))
    return (f_buf_inspect) buf_inspect_character;
  if (type == sym_1("F32"))
    return (f_buf_inspect) buf_inspect_f32;
  if (type == sym_1("F64"))
    return (f_buf_inspect) buf_inspect_f64;
  if (type == sym_1("Fact"))
    return (f_buf_inspect) buf_inspect_fact;
  if (type == sym_1("Fn"))
    return (f_buf_inspect) buf_inspect_fn;
  if (type == sym_1("Ident"))
    return (f_buf_inspect) buf_inspect_ident;
  if (type == sym_1("Integer"))
    return (f_buf_inspect) buf_inspect_integer;
  if (type == sym_1("Sw"))
    return (f_buf_inspect) buf_inspect_sw;
  if (type == sym_1("S64"))
    return (f_buf_inspect) buf_inspect_s64;
  if (type == sym_1("S32"))
    return (f_buf_inspect) buf_inspect_s32;
  if (type == sym_1("S16"))
    return (f_buf_inspect) buf_inspect_s16;
  if (type == sym_1("S8"))
    return (f_buf_inspect) buf_inspect_s8;
  if (type == sym_1("U8"))
    return (f_buf_inspect) buf_inspect_u8;
  if (type == sym_1("U16"))
    return (f_buf_inspect) buf_inspect_u16;
  if (type == sym_1("U32"))
    return (f_buf_inspect) buf_inspect_u32;
  if (type == sym_1("U64"))
    return (f_buf_inspect) buf_inspect_u64;
  if (type == sym_1("Uw"))
    return (f_buf_inspect) buf_inspect_uw;
  if (type == sym_1("List"))
    return (f_buf_inspect) buf_inspect_list;
  if (type == sym_1("Ptag"))
    return (f_buf_inspect) buf_inspect_ptag;
  if (type == sym_1("Ptr"))
    return (f_buf_inspect) buf_inspect_ptr;
  if (type == sym_1("PtrFree"))
    return (f_buf_inspect) buf_inspect_ptr_free;
  if (type == sym_1("Quote"))
    return (f_buf_inspect) buf_inspect_quote;
  if (type == sym_1("Str"))
    return (f_buf_inspect) buf_inspect_str;
  if (type == sym_1("Sym"))
    return (f_buf_inspect) buf_inspect_sym;
  if (type == sym_1("Tuple"))
    return (f_buf_inspect) buf_inspect_tuple;
  if (type == sym_1("Var"))
    return (f_buf_inspect) buf_inspect_var;
  err_write_1("sym_to_buf_inspect: unknown type: ");
  err_write_1(type->str.ptr.ps8);
  err_write_1("\n");
  assert(! "sym_to_buf_inspect: unknown type");
  return NULL;
}

f_buf_inspect_size sym_to_buf_inspect_size (const s_sym *type)
{
  if (type == sym_1("Bool"))
    return (f_buf_inspect_size) buf_inspect_bool_size;
  if (type == sym_1("Call"))
    return (f_buf_inspect_size) buf_inspect_call_size;
  if (type == sym_1("Cfn"))
    return (f_buf_inspect_size) buf_inspect_cfn_size;
  if (type == sym_1("Character"))
    return (f_buf_inspect_size) buf_inspect_character_size;
  if (type == sym_1("F32"))
    return (f_buf_inspect_size) buf_inspect_f32_size;
  if (type == sym_1("F64"))
    return (f_buf_inspect_size) buf_inspect_f64_size;
  if (type == sym_1("Fact"))
    return (f_buf_inspect_size) buf_inspect_fact_size;
  if (type == sym_1("Fn"))
    return (f_buf_inspect_size) buf_inspect_fn_size;
  if (type == sym_1("Ident"))
    return (f_buf_inspect_size) buf_inspect_ident_size;
  if (type == sym_1("Integer"))
    return (f_buf_inspect_size) buf_inspect_integer_size;
  if (type == sym_1("Sw"))
    return (f_buf_inspect_size) buf_inspect_sw_size;
  if (type == sym_1("S64"))
    return (f_buf_inspect_size) buf_inspect_s64_size;
  if (type == sym_1("S32"))
    return (f_buf_inspect_size) buf_inspect_s32_size;
  if (type == sym_1("S16"))
    return (f_buf_inspect_size) buf_inspect_s16_size;
  if (type == sym_1("S8"))
    return (f_buf_inspect_size) buf_inspect_s8_size;
  if (type == sym_1("U8"))
    return (f_buf_inspect_size) buf_inspect_u8_size;
  if (type == sym_1("U16"))
    return (f_buf_inspect_size) buf_inspect_u16_size;
  if (type == sym_1("U32"))
    return (f_buf_inspect_size) buf_inspect_u32_size;
  if (type == sym_1("U64"))
    return (f_buf_inspect_size) buf_inspect_u64_size;
  if (type == sym_1("Uw"))
    return (f_buf_inspect_size) buf_inspect_uw_size;
  if (type == sym_1("List"))
    return (f_buf_inspect_size) buf_inspect_list_size;
  if (type == sym_1("Ptag"))
    return (f_buf_inspect_size) buf_inspect_ptag_size;
  if (type == sym_1("Ptr"))
    return (f_buf_inspect_size) buf_inspect_ptr_size;
  if (type == sym_1("PtrFree"))
    return (f_buf_inspect_size) buf_inspect_ptr_free_size;
  if (type == sym_1("Quote"))
    return (f_buf_inspect_size) buf_inspect_quote_size;
  if (type == sym_1("Str"))
    return (f_buf_inspect_size) buf_inspect_str_size;
  if (type == sym_1("Sym"))
    return (f_buf_inspect_size) buf_inspect_sym_size;
  if (type == sym_1("Tuple"))
    return (f_buf_inspect_size) buf_inspect_tuple_size;
  if (type == sym_1("Var"))
    return (f_buf_inspect_size) buf_inspect_var_size;
  err_write_1("sym_to_buf_inspect_size: unknown type: ");
  err_write_1(type->str.ptr.ps8);
  err_write_1("\n");
  assert(! "sym_to_buf_inspect_size: unknown type");
  return NULL;
}

f_clean sym_to_clean (const s_sym *type)
{
  if (type == sym_1("Bool"))
    return NULL;
  if (type == sym_1("Call"))
    return (f_clean) call_clean;
  if (type == sym_1("Cfn"))
    return (f_clean) cfn_clean;
  if (type == sym_1("Character"))
    return NULL;
  if (type == sym_1("F32"))
    return NULL;
  if (type == sym_1("F64"))
    return NULL;
  if (type == sym_1("Fact"))
    return NULL;
  if (type == sym_1("Fn"))
    return (f_clean) fn_clean;
  if (type == sym_1("Ident"))
    return NULL;
  if (type == sym_1("Integer"))
    return (f_clean) integer_clean;
  if (type == sym_1("Sw"))
    return NULL;
  if (type == sym_1("S64"))
    return NULL;
  if (type == sym_1("S32"))
    return NULL;
  if (type == sym_1("S16"))
    return NULL;
  if (type == sym_1("S8"))
    return NULL;
  if (type == sym_1("U8"))
    return NULL;
  if (type == sym_1("U16"))
    return NULL;
  if (type == sym_1("U32"))
    return NULL;
  if (type == sym_1("U64"))
    return NULL;
  if (type == sym_1("Uw"))
    return NULL;
  if (type == sym_1("List"))
    return (f_clean) list_clean;
  if (type == sym_1("Map"))
    return (f_clean) map_clean;
  if (type == sym_1("Ptag"))
    return NULL;
  if (type == sym_1("Ptr"))
    return NULL;
  if (type == sym_1("PtrFree"))
    return (f_clean) ptr_free_clean;
  if (type == sym_1("Quote"))
    return (f_clean) quote_clean;
  if (type == sym_1("Str"))
    return (f_clean) str_clean;
  if (type == sym_1("Sym"))
    return NULL;
  if (type == sym_1("Tuple"))
    return (f_clean) tuple_clean;
  if (type == sym_1("Var"))
    return NULL;
  if (type == sym_1("Void"))
    return NULL;
  err_write_1("sym_to_clean: unknown type: ");
  err_write_1(type->str.ptr.ps8);
  err_write_1("\n");
  assert(! "sym_to_clean: unknown type");
  return NULL;
}

ffi_type * sym_to_ffi_type (const s_sym *sym, ffi_type *result_type)
{
  assert(sym);
  if (sym == sym_1("Result")) {
    if (! result_type) {
      err_puts("sym_to_ffi_type: invalid result type: Result");
      return NULL;
    }
    return result_type;
  }
  if (sym->str.ptr.ps8[sym->str.size - 1] == '*')
    return &ffi_type_pointer;
  if (sym == sym_1("Bool"))
    return &ffi_type_uint8;
  if (sym == sym_1("Fn"))
    return &ffi_type_pointer;
  if (sym == sym_1("Integer"))
    return &ffi_type_pointer;
  if (sym == sym_1("List"))
    return &ffi_type_pointer;
  if (sym == sym_1("Map"))
    return &ffi_type_pointer;
  if (sym == sym_1("Ptr"))
    return &ffi_type_pointer;
  if (sym == sym_1("PtrFree"))
    return &ffi_type_pointer;
  if (sym == sym_1("S8"))
    return &ffi_type_sint8;
  if (sym == sym_1("S16"))
    return &ffi_type_sint16;
  if (sym == sym_1("S32"))
    return &ffi_type_sint32;
  if (sym == sym_1("S64"))
    return &ffi_type_sint64;
  if (sym == sym_1("Sym"))
    return &ffi_type_pointer;
  if (sym == sym_1("Sw"))
    return &ffi_type_slong;
  if (sym == sym_1("Tag"))
    return &ffi_type_pointer;
  if (sym == sym_1("U8"))
    return &ffi_type_uint8;
  if (sym == sym_1("U16"))
    return &ffi_type_uint16;
  if (sym == sym_1("U32"))
    return &ffi_type_uint32;
  if (sym == sym_1("U64"))
    return &ffi_type_uint64;
  if (sym == sym_1("Uw"))
    return &ffi_type_ulong;
  if (sym == sym_1("Void"))
    return &ffi_type_void;
  err_write_1("sym_to_ffi_type: unknown type: ");
  err_write_1(sym->str.ptr.ps8);
  err_write_1("\n");
  assert(! "sym_to_ffi_type: unknown type");
  return NULL;
}

f_init_copy sym_to_init_copy (const s_sym *type)
{
  if (type == sym_1("Bool"))
    return (f_init_copy) bool_init_copy;
  if (type == sym_1("Call"))
    return (f_init_copy) call_init_copy;
  if (type == sym_1("Cfn"))
    return (f_init_copy) cfn_init_copy;
  if (type == sym_1("Character"))
    return (f_init_copy) character_init_copy;
  if (type == sym_1("F32"))
    return (f_init_copy) f32_init_copy;
  if (type == sym_1("F64"))
    return (f_init_copy) f64_init_copy;
  if (type == sym_1("Fact"))
    return (f_init_copy) fact_init_copy;
  if (type == sym_1("Fn"))
    return (f_init_copy) fn_init_copy;
  if (type == sym_1("Ident"))
    return (f_init_copy) ident_init_copy;
  if (type == sym_1("Integer"))
    return (f_init_copy) integer_init_copy;
  if (type == sym_1("List"))
    return (f_init_copy) list_init_copy;
  if (type == sym_1("Sw"))
    return (f_init_copy) sw_init_copy;
  if (type == sym_1("S64"))
    return (f_init_copy) s64_init_copy;
  if (type == sym_1("S32"))
    return (f_init_copy) s32_init_copy;
  if (type == sym_1("S16"))
    return (f_init_copy) s16_init_copy;
  if (type == sym_1("S8"))
    return (f_init_copy) s8_init_copy;
  if (type == sym_1("U8"))
    return (f_init_copy) u8_init_copy;
  if (type == sym_1("U16"))
    return (f_init_copy) u16_init_copy;
  if (type == sym_1("U32"))
    return (f_init_copy) u32_init_copy;
  if (type == sym_1("U64"))
    return (f_init_copy) u64_init_copy;
  if (type == sym_1("Uw"))
    return (f_init_copy) uw_init_copy;
  if (type == sym_1("Ptag"))
    return (f_init_copy) ptag_init_copy;
  if (type == sym_1("Ptr"))
    return (f_init_copy) ptr_init_copy;
  if (type == sym_1("PtrFree"))
    return (f_init_copy) ptr_free_init_copy;
  if (type == sym_1("Quote"))
    return (f_init_copy) quote_init_copy;
  if (type == sym_1("Str"))
    return (f_init_copy) str_init_copy;
  if (type == sym_1("Sym"))
    return (f_init_copy) sym_init_copy;
  if (type == sym_1("Tuple"))
    return (f_init_copy) tuple_init_copy;
  if (type == sym_1("Var"))
    return (f_init_copy) var_init_copy;
  err_write_1("sym_to_init_copy: unknown type: ");
  err_write_1(type->str.ptr.ps8);
  err_write_1("\n");
  assert(! "sym_to_init_copy: unknown type");
  return NULL;
}

bool sym_to_tag_type (const s_sym *sym, e_tag_type *dest)
{
  if (sym == sym_1("Void")) {
    *dest = TAG_VOID;
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
  err_write_1("sym_to_tag_type: unknown type: ");
  err_write_1(sym->str.ptr.ps8);
  err_write_1("\n");
  assert(! "sym_to_tag_type: unknown type");
  return false;
}

uw sym_type_size (const s_sym *type)
{
  if (type == sym_1("Bool"))
    return sizeof(bool);
  if (type == sym_1("Call"))
    return sizeof(s_call);
  if (type == sym_1("Cfn"))
    return sizeof(s_cfn);
  if (type == sym_1("Character"))
    return sizeof(character);
  if (type == sym_1("F32"))
    return sizeof(f32);
  if (type == sym_1("F64"))
    return sizeof(f64);
  if (type == sym_1("Fact"))
    return sizeof(s_fact);
  if (type == sym_1("Fn"))
    return sizeof(s_fn);
  if (type == sym_1("Ident"))
    return sizeof(s_ident);
  if (type == sym_1("Integer"))
    return sizeof(s_integer);
  if (type == sym_1("List"))
    return sizeof(s_list *);
  if (type == sym_1("Ptag"))
    return sizeof(p_tag);
  if (type == sym_1("Ptr"))
    return sizeof(void *);
  if (type == sym_1("PtrFree"))
    return sizeof(void *);
  if (type == sym_1("Quote"))
    return sizeof(s_quote);
  if (type == sym_1("S8"))
    return sizeof(s8);
  if (type == sym_1("S16"))
    return sizeof(s16);
  if (type == sym_1("S32"))
    return sizeof(s32);
  if (type == sym_1("S64"))
    return sizeof(s64);
  if (type == sym_1("Str"))
    return sizeof(s_str);
  if (type == sym_1("Sw"))
    return sizeof(sw);
  if (type == sym_1("Sym"))
    return sizeof(s_sym *);
  if (type == sym_1("Tuple"))
    return sizeof(s_tuple);
  if (type == sym_1("U8"))
    return sizeof(u8);
  if (type == sym_1("U16"))
    return sizeof(u16);
  if (type == sym_1("U32"))
    return sizeof(u32);
  if (type == sym_1("U64"))
    return sizeof(u64);
  if (type == sym_1("Uw"))
    return sizeof(uw);
  if (type == sym_1("Var"))
    return sizeof(s_tag);
  if (type == sym_1("Void"))
    return 0;
  err_write_1("sym_type_size: unknown type: ");
  err_write_1(type->str.ptr.ps8);
  err_write_1("\n");
  assert(! "sym_type_size: unknown type");
  return 0;
}
