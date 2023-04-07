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
#include <stdlib.h>
#include <string.h>
#include "buf.h"
#include "buf_inspect.h"
#include "character.h"
#include "compare.h"
#include "str.h"
#include "sym.h"

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

e_bool sym_character_is_reserved (character c)
{
  return (character_is_space(c) ||
          c == '#' ||
          c == '(' ||
          c == ')' ||
          c == ',' ||
          c == '.' ||
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
  
void sym_delete_all ()
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

e_bool sym_has_reserved_characters (const s_sym *sym)
{
  character c;
  sw r;
  s_str stra;
  str_init(&stra, NULL, sym->str.size, sym->str.ptr.p);
  while ((r = str_read_character_utf8(&stra, &c)) > 0) {
    if (sym_character_is_reserved(c))
      return true;
  }
  if (r < 0)
    return true;
  return false;
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

e_bool sym_is_module (const s_sym *sym)
{
  character c;
  if (str_peek_character(&sym->str, &c) > 0)
    return character_is_uppercase(c);
  return false;
}

s_sym_list * sym_list_new (s_sym *sym, s_sym_list *next)
{
  s_sym_list *sym_list;
  sym_list = malloc(sizeof(s_sym_list));
  if (! sym_list)
    err(1, "out of memory");
  sym_list->sym = sym;
  sym_list->next = next;
  return sym_list;
}

const s_sym * sym_new (const s_str *src)
{
  s_sym *sym;
  sym = malloc(sizeof(s_sym));
  if (! sym)
    err(1, "out of memory");
  str_init_dup(&sym->str, src);
  g_sym_list = sym_list_new(sym, g_sym_list);
  return sym;
}

ffi_type * sym_to_ffi_type (const s_sym *sym, ffi_type *result_type)
{
  if (sym == sym_1("tag"))
    return &ffi_type_pointer;
  if (sym == sym_1("&result")) {
    if (! result_type)
      warnx("invalid result type: &result");
    return result_type;
  }
  if (sym == sym_1("s8"))
    return &ffi_type_sint8;
  if (sym == sym_1("s16"))
    return &ffi_type_sint16;
  if (sym == sym_1("s32"))
    return &ffi_type_sint32;
  if (sym == sym_1("s64"))
    return &ffi_type_sint64;
  if (sym == sym_1("sw"))
    return &ffi_type_sint;
  if (sym == sym_1("u8"))
    return &ffi_type_uint8;
  if (sym == sym_1("u16"))
    return &ffi_type_uint16;
  if (sym == sym_1("u32"))
    return &ffi_type_uint32;
  if (sym == sym_1("u64"))
    return &ffi_type_uint64;
  if (sym == sym_1("uw"))
    return &ffi_type_uint;
  assert(! "sym_to_ffi_type: unknown type");
  errx(1, "sym_to_ffi_type: unknown type");
  return NULL;
}

e_tag_type sym_to_tag_type (const s_sym *sym)
{
  if (sym == sym_1("void"))
    return TAG_VOID;
  if (sym == sym_1("bool"))
    return TAG_BOOL;
  if (sym == sym_1("call"))
    return TAG_CALL;
  if (sym == sym_1("call_fn"))
    return TAG_CALL_FN;
  if (sym == sym_1("call_macro"))
    return TAG_CALL_MACRO;
  if (sym == sym_1("cfn"))
    return TAG_CFN;
  if (sym == sym_1("character"))
    return TAG_CHARACTER;
  if (sym == sym_1("f32"))
    return TAG_F32;
  if (sym == sym_1("f64"))
    return TAG_F64;
  if (sym == sym_1("fn"))
    return TAG_FN;
  if (sym == sym_1("ident"))
    return TAG_IDENT;
  if (sym == sym_1("integer"))
    return TAG_INTEGER;
  if (sym == sym_1("s64"))
    return TAG_S64;
  if (sym == sym_1("s32"))
    return TAG_S32;
  if (sym == sym_1("s16"))
    return TAG_S16;
  if (sym == sym_1("s8"))
    return TAG_S8;
  if (sym == sym_1("u8"))
    return TAG_U8;
  if (sym == sym_1("u16"))
    return TAG_U16;
  if (sym == sym_1("u32"))
    return TAG_U32;
  if (sym == sym_1("u64"))
    return TAG_U64;
  if (sym == sym_1("list"))
    return TAG_LIST;
  if (sym == sym_1("ptag"))
    return TAG_PTAG;
  if (sym == sym_1("quote"))
    return TAG_QUOTE;
  if (sym == sym_1("str"))
    return TAG_STR;
  if (sym == sym_1("sym"))
    return TAG_SYM;
  if (sym == sym_1("tag"))
    return TAG_VOID;
  if (sym == sym_1("tuple"))
    return TAG_TUPLE;
  assert(! "sym_to_tag_type: unknown type");
  errx(1, "sym_to_tag_type: unknown type: %s", sym->str.ptr.ps8);
  return -1;
}
