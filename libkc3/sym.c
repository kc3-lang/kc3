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
#include "alloc.h"
#include "assert.h"
#include "buf.h"
#include "buf_inspect.h"
#include "character.h"
#include "compare.h"
#include "ident.h"
#include "list.h"
#include "str.h"
#include "struct_type.h"
#include "sym.h"
#include "tag_type.h"

const s_sym g_sym___DIR__         = {{{NULL},  7, {"__DIR__"}}};
const s_sym g_sym___FILE__        = {{{NULL},  8, {"__FILE__"}}};
const s_sym g_sym__brackets       = {{{NULL},  2, {"[]"}}};
const s_sym g_sym__equal          = {{{NULL},  1, {"="}}};
const s_sym g_sym__paren          = {{{NULL},  2, {"()"}}};
const s_sym g_sym__plus           = {{{NULL},  1, {"+"}}};
const s_sym g_sym_Array           = {{{NULL},  5, {"Array"}}};
const s_sym g_sym_Block           = {{{NULL},  5, {"Block"}}};
const s_sym g_sym_Bool            = {{{NULL},  4, {"Bool"}}};
const s_sym g_sym_Buf             = {{{NULL},  3, {"Buf"}}};
const s_sym g_sym_BufRW           = {{{NULL},  5, {"BufRW"}}};
const s_sym g_sym_Call            = {{{NULL},  4, {"Call"}}};
const s_sym g_sym_Callable        = {{{NULL},  8, {"Callable"}}};
const s_sym g_sym_Cfn             = {{{NULL},  3, {"Cfn"}}};
const s_sym g_sym_Character       = {{{NULL},  9, {"Character"}}};
const s_sym g_sym_Char__star      = {{{NULL},  5, {"Char*"}}};
const s_sym g_sym_Complex         = {{{NULL},  7, {"Complex"}}};
const s_sym g_sym_Cow             = {{{NULL},  3, {"Cow"}}};
const s_sym g_sym_F32             = {{{NULL},  3, {"F32"}}};
const s_sym g_sym_F64             = {{{NULL},  3, {"F64"}}};
const s_sym g_sym_F128            = {{{NULL},  4, {"F128"}}};
const s_sym g_sym_Fact            = {{{NULL},  4, {"Fact"}}};
const s_sym g_sym_FactW           = {{{NULL},  5, {"FactW"}}};
const s_sym g_sym_Fn              = {{{NULL},  2, {"Fn"}}};
const s_sym g_sym_Ident           = {{{NULL},  5, {"Ident"}}};
const s_sym g_sym_Integer         = {{{NULL},  7, {"Integer"}}};
const s_sym g_sym_KC3             = {{{NULL},  3, {"KC3"}}};
const s_sym g_sym_KC3_Op          = {{{NULL},  6, {"KC3.Op"}}};
const s_sym g_sym_List            = {{{NULL},  4, {"List"}}};
const s_sym g_sym_Map             = {{{NULL},  3, {"Map"}}};
const s_sym g_sym_Ptag            = {{{NULL},  4, {"Ptag"}}};
const s_sym g_sym_Ptr             = {{{NULL},  3, {"Ptr"}}};
const s_sym g_sym_PtrFree         = {{{NULL},  7, {"PtrFree"}}};
const s_sym g_sym_Quote           = {{{NULL},  5, {"Quote"}}};
const s_sym g_sym_Unquote         = {{{NULL},  7, {"Unquote"}}};
const s_sym g_sym_Ratio           = {{{NULL},  5, {"Ratio"}}};
const s_sym g_sym_Result          = {{{NULL},  6, {"Result"}}};
const s_sym g_sym_S8              = {{{NULL},  2, {"S8"}}};
const s_sym g_sym_S16             = {{{NULL},  3, {"S16"}}};
const s_sym g_sym_S32             = {{{NULL},  3, {"S32"}}};
const s_sym g_sym_S64             = {{{NULL},  3, {"S64"}}};
const s_sym g_sym_Str             = {{{NULL},  3, {"Str"}}};
const s_sym g_sym_Struct          = {{{NULL},  6, {"Struct"}}};
const s_sym g_sym_StructType      = {{{NULL}, 10, {"StructType"}}};
const s_sym g_sym_Sw              = {{{NULL},  2, {"Sw"}}};
const s_sym g_sym_Sym             = {{{NULL},  3, {"Sym"}}};
const s_sym g_sym_Tag             = {{{NULL},  3, {"Tag"}}};
const s_sym g_sym_Time            = {{{NULL},  4, {"Time"}}};
const s_sym g_sym_Tuple           = {{{NULL},  5, {"Tuple"}}};
const s_sym g_sym_U8              = {{{NULL},  2, {"U8"}}};
const s_sym g_sym_U16             = {{{NULL},  3, {"U16"}}};
const s_sym g_sym_U32             = {{{NULL},  3, {"U32"}}};
const s_sym g_sym_U64             = {{{NULL},  3, {"U64"}}};
const s_sym g_sym_Uw              = {{{NULL},  2, {"Uw"}}};
const s_sym g_sym_Uw_brackets     = {{{NULL},  4, {"Uw[]"}}};
const s_sym g_sym_Var             = {{{NULL},  3, {"Var"}}};
const s_sym g_sym_Void            = {{{NULL},  4, {"Void"}}};
const s_sym g_sym_access          = {{{NULL},  6, {"access"}}};
const s_sym g_sym_arity           = {{{NULL},  5, {"arity"}}};
const s_sym g_sym_cast            = {{{NULL},  4, {"cast"}}};
const s_sym g_sym_clean           = {{{NULL},  5, {"clean"}}};
const s_sym g_sym_defstruct       = {{{NULL},  9, {"defstruct"}}};
const s_sym g_sym_directory       = {{{NULL},  9, {"directory"}}};
const s_sym g_sym_do              = {{{NULL},  2, {"do"}}};
const s_sym g_sym_else            = {{{NULL},  4, {"else"}}};
const s_sym g_sym_end             = {{{NULL},  3, {"end"}}};
const s_sym g_sym_file            = {{{NULL},  4, {"file"}}};
const s_sym g_sym_fn              = {{{NULL},  2, {"fn"}}};
const s_sym g_sym_if              = {{{NULL},  2, {"if"}}};
const s_sym g_sym_if_then_else    = {{{NULL}, 12, {"if_then_else"}}};
const s_sym g_sym_is_a            = {{{NULL},  4, {"is_a"}}};
const s_sym g_sym_is_loading      = {{{NULL}, 10, {"is_loading"}}};
const s_sym g_sym_load_time       = {{{NULL},  9, {"load_time"}}};
const s_sym g_sym_macro           = {{{NULL},  5, {"macro"}}};
const s_sym g_sym_module          = {{{NULL},  6, {"module"}}};
const s_sym g_sym_ncpu            = {{{NULL},  4, {"ncpu"}}};
const s_sym g_sym_op              = {{{NULL},  2, {"op"}}};
const s_sym g_sym_op_associativity =
  {{{NULL}, 16, {"op_associativity"}}};
const s_sym g_sym_op_callable     = {{{NULL}, 11, {"op_callable"}}};
const s_sym g_sym_op_equal        = {{{NULL},  8, {"op_equal"}}};
const s_sym g_sym_op_pin          = {{{NULL},  6, {"op_pin"}}};
const s_sym g_sym_op_precedence   = {{{NULL}, 13, {"op_precedence"}}};
const s_sym g_sym_op_sym          = {{{NULL},  6, {"op_sym"}}};
const s_sym g_sym_persist         = {{{NULL},  7, {"persist"}}};
const s_sym g_sym_r               = {{{NULL},  1, {"r"}}};
const s_sym g_sym_read            = {{{NULL},  4, {"read"}}};
const s_sym g_sym_right           = {{{NULL},  5, {"right"}}};
const s_sym g_sym_rw              = {{{NULL},  2, {"rw"}}};
const s_sym g_sym_rwx             = {{{NULL},  3, {"rwx"}}};
const s_sym g_sym_rx              = {{{NULL},  2, {"rx"}}};
const s_sym g_sym_signal          = {{{NULL},  6, {"signal"}}};
const s_sym g_sym_special_operator =
  {{{NULL}, 16, {"special_operator"}}};
const s_sym g_sym_str             = {{{NULL},  3, {"str"}}};
const s_sym g_sym_struct_type     = {{{NULL}, 11, {"struct_type"}}};
const s_sym g_sym_sym             = {{{NULL},  3, {"sym"}}};
const s_sym g_sym_symbol          = {{{NULL},  6, {"symbol"}}};
const s_sym g_sym_symbol_value    = {{{NULL}, 12, {"symbol_value"}}};
const s_sym g_sym_then            = {{{NULL},  4, {"then"}}};
const s_sym g_sym_timeout         = {{{NULL},  7, {"timeout"}}};
const s_sym g_sym_tv_nsec         = {{{NULL},  7, {"tv_nsec"}}};
const s_sym g_sym_tv_sec          = {{{NULL},  6, {"tv_sec"}}};
const s_sym g_sym_w               = {{{NULL},  1, {"w"}}};
const s_sym g_sym_write           = {{{NULL},  5, {"write"}}};
const s_sym g_sym_wx              = {{{NULL},  2, {"wx"}}};
const s_sym g_sym_x               = {{{NULL},  1, {"x"}}};

static s_sym_list * g_sym_list = NULL;

void         sym_delete (s_sym *sym);
s_sym_list * sym_list_new (const s_sym *sym, s_sym *free_sym,
                           s_sym_list *next);

const s_sym * sym_1 (const char *p)
{
  s_str stra;
  str_init_1(&stra, NULL, p);
  return str_to_sym(&stra);
}

const s_sym * sym_anon (const s_str *prefix)
{
  s_list *list;
  static uw serial = 0;
  s_str    *serial_str = NULL;
  s_str str = {0};
  const s_sym *tmp = NULL;
  const s_str underscore = {{NULL}, 1, {"_"}};
  list = list_new(list_new(list_new(NULL)));
  list_next(list_next(list))->tag.type = TAG_STR;
  serial_str = &list_next(list_next(list))->tag.data.str;
  list_next(list)->tag.type = TAG_STR;
  list_next(list)->tag.data.str = underscore;
  list->tag.type = TAG_STR;
  list->tag.data.str = *prefix;
  while (1) {
    serial++;
    str_init_uw(serial_str, serial);
    if (! str_init_concatenate_list(&str,
                                    (const s_list * const *) &list)) {
      err_puts("sym_new_anon: str_init_concatenate_list");
      assert(! "sym_new_anon: str_init_concatenate_list");
      return NULL;
    }
    if (! sym_find(&str)) {
      tmp = str_to_sym(&str);
      list_delete_all(list);
      return tmp;
    }
  }
  list_delete_all(list);
  return NULL;
}

const s_sym * sym_array_type (const s_sym *sym)
{
  s_buf buf;
  character c;
  sw r;
  bool separator;
  s_str str;
  const s_sym *tmp = {0};
  assert(sym);
  buf_init_alloc(&buf, sym->str.size);
  str = sym->str;
  separator = true;
  while (1) {
    if (separator) {
      if (str_read_character_utf8(&str, &c) <= 0)
        goto ko;
      if (! character_is_uppercase(c))
        goto ko;
      separator = false;
      buf_write_character_utf8(&buf, c);
    }
    else {
      r = str_read_character_utf8(&str, &c);
      if (r <= 0)
        goto ko;
      if (c == '.')
        separator = true;
      else if (c == '[') {
        buf_read_to_str(&buf, &str);
        tmp = str_to_sym(&str);
        str_clean(&str);
        buf_clean(&buf);
        return tmp;
      }
      buf_write_character_utf8(&buf, c);
    }
  }
 ko:
  buf_clean(&buf);
  return NULL;
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
  if (true)
    err_puts("sym_delete_all");
  sym_list = g_sym_list;
  g_sym_list = NULL;
  while (sym_list) {
    s_sym_list *tmp = NULL;
    tmp = sym_list;
    sym_list = sym_list->next;
    if (tmp->free_sym)
      sym_delete(tmp->free_sym);
    free(tmp);
  }
}

const s_sym * sym_find (const s_str *str)
{
  s_sym_list *sym_list;
  sym_list = g_sym_list;
  while (sym_list) {
    const s_sym *sym = sym_list->sym;
    if (compare_str(str, &sym->str) == 0)
      return sym;
    sym_list = sym_list->next;
  }
  return NULL;
}

s_tag * sym_find_to_tag (const s_str *src, s_tag *dest)
{
  s_tag tmp = {0};
  tmp.data.sym = sym_find(src);
  if (tmp.data.sym)
    tmp.type = TAG_SYM;
  *dest = tmp;
  return dest;
}

bool sym_has_ident_reserved_characters (const s_sym *sym)
{
  character c;
  sw r;
  s_str stra;
  str_init(&stra, NULL, sym->str.size, sym->str.ptr.p);
  if ((r = str_read_character_utf8(&stra, &c)) > 0) {
    if (ident_first_character_is_reserved(c))
      return true;
    while ((r = str_read_character_utf8(&stra, &c)) > 0) {
      if (ident_character_is_reserved(c))
        return true;
    }
  }
  if (r < 0)
    return true;
  return false;
}

bool sym_has_reserved_characters (const s_sym *sym)
{
  character c;
  bool is_array_type;
  sw r;
  s_str stra;
  str_init(&stra, NULL, sym->str.size, sym->str.ptr.p);
  is_array_type = sym_is_array_type(sym);
  while (1) {
    if ((r = str_read_character_utf8(&stra, &c)) <= 0)
      break;
    if (is_array_type && c == '[') {
      if ((r = str_read_character_utf8(&stra, &c)) <= 0)
        return true;
      if (c == ']' &&
          (r = str_peek_character_utf8(&stra, &c)) <= 0)
        return false;
    }
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

const s_sym ** sym_init_1 (const s_sym **sym, const char *p)
{
  assert(sym);
  assert(p);
  *sym = sym_1(p);
  return sym;
}

const s_sym ** sym_init_cast (const s_sym **sym,
                              const s_sym * const *type,
                              const s_tag *tag)
{
  assert(sym);
  assert(type);
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
  if (*type == &g_sym_Sym)
    err_puts(" to Sym");
  else {
    err_write_1(" to ");
    err_inspect_sym(type);
    err_puts(" aka Sym");
  }
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

void sym_init_g_sym (void)
{
  if (g_sym_list)
    return;
  sym_register(&g_sym___DIR__, NULL);
  sym_register(&g_sym___FILE__, NULL);
  sym_register(&g_sym__brackets, NULL);
  sym_register(&g_sym__equal, NULL);
  sym_register(&g_sym__paren, NULL);
  sym_register(&g_sym__plus, NULL);
  sym_register(&g_sym_Array, NULL);
  sym_register(&g_sym_Block, NULL);
  sym_register(&g_sym_Bool, NULL);
  sym_register(&g_sym_Buf, NULL);
  sym_register(&g_sym_BufRW, NULL);
  sym_register(&g_sym_Call, NULL);
  sym_register(&g_sym_Callable, NULL);
  sym_register(&g_sym_Cfn, NULL);
  sym_register(&g_sym_Character, NULL);
  sym_register(&g_sym_Char__star, NULL);
  sym_register(&g_sym_Complex, NULL);
  sym_register(&g_sym_Cow, NULL);
  sym_register(&g_sym_F32, NULL);
  sym_register(&g_sym_F64, NULL);
  sym_register(&g_sym_F128, NULL);
  sym_register(&g_sym_Fact, NULL);
  sym_register(&g_sym_FactW, NULL);
  sym_register(&g_sym_Fn, NULL);
  sym_register(&g_sym_Ident, NULL);
  sym_register(&g_sym_Integer, NULL);
  sym_register(&g_sym_KC3, NULL);
  sym_register(&g_sym_KC3_Op, NULL);
  sym_register(&g_sym_List, NULL);
  sym_register(&g_sym_Map, NULL);
  sym_register(&g_sym_Ptag, NULL);
  sym_register(&g_sym_Ptr, NULL);
  sym_register(&g_sym_PtrFree, NULL);
  sym_register(&g_sym_Quote, NULL);
  sym_register(&g_sym_Ratio, NULL);
  sym_register(&g_sym_Result, NULL);
  sym_register(&g_sym_S8, NULL);
  sym_register(&g_sym_S16, NULL);
  sym_register(&g_sym_S32, NULL);
  sym_register(&g_sym_S64, NULL);
  sym_register(&g_sym_Str, NULL);
  sym_register(&g_sym_Struct, NULL);
  sym_register(&g_sym_StructType, NULL);
  sym_register(&g_sym_Sw, NULL);
  sym_register(&g_sym_Sym, NULL);
  sym_register(&g_sym_Tag, NULL);
  sym_register(&g_sym_Time, NULL);
  sym_register(&g_sym_Tuple, NULL);
  sym_register(&g_sym_U8, NULL);
  sym_register(&g_sym_U16, NULL);
  sym_register(&g_sym_U32, NULL);
  sym_register(&g_sym_U64, NULL);
  sym_register(&g_sym_Unquote, NULL);
  sym_register(&g_sym_Uw, NULL);
  sym_register(&g_sym_Uw_brackets, NULL);
  sym_register(&g_sym_Var, NULL);
  sym_register(&g_sym_Void, NULL);
  sym_register(&g_sym_access, NULL);
  sym_register(&g_sym_arity, NULL);
  sym_register(&g_sym_cast, NULL);
  sym_register(&g_sym_clean, NULL);
  sym_register(&g_sym_defstruct, NULL);
  sym_register(&g_sym_directory, NULL);
  sym_register(&g_sym_do, NULL);
  sym_register(&g_sym_else, NULL);
  sym_register(&g_sym_end, NULL);
  sym_register(&g_sym_file, NULL);
  sym_register(&g_sym_fn, NULL);
  sym_register(&g_sym_if, NULL);
  sym_register(&g_sym_if_then_else, NULL);
  sym_register(&g_sym_is_a, NULL);
  sym_register(&g_sym_is_loading, NULL);
  sym_register(&g_sym_load_time, NULL);
  sym_register(&g_sym_macro, NULL);
  sym_register(&g_sym_module, NULL);
  sym_register(&g_sym_ncpu, NULL);
  sym_register(&g_sym_op, NULL);
  sym_register(&g_sym_op_associativity, NULL);
  sym_register(&g_sym_op_callable, NULL);
  sym_register(&g_sym_op_equal, NULL);
  sym_register(&g_sym_op_pin, NULL);
  sym_register(&g_sym_op_precedence, NULL);
  sym_register(&g_sym_op_sym, NULL);
  sym_register(&g_sym_persist, NULL);
  sym_register(&g_sym_r, NULL);
  sym_register(&g_sym_read, NULL);
  sym_register(&g_sym_right, NULL);
  sym_register(&g_sym_rw, NULL);
  sym_register(&g_sym_rwx, NULL);
  sym_register(&g_sym_rx, NULL);
  sym_register(&g_sym_signal, NULL);
  sym_register(&g_sym_special_operator, NULL);
  sym_register(&g_sym_str, NULL);
  sym_register(&g_sym_struct_type, NULL);
  sym_register(&g_sym_sym, NULL);
  sym_register(&g_sym_symbol, NULL);
  sym_register(&g_sym_symbol_value, NULL);
  sym_register(&g_sym_then, NULL);
  sym_register(&g_sym_timeout, NULL);
  sym_register(&g_sym_tv_nsec, NULL);
  sym_register(&g_sym_tv_sec, NULL);
  sym_register(&g_sym_w, NULL);
  sym_register(&g_sym_write, NULL);
  sym_register(&g_sym_wx, NULL);
  sym_register(&g_sym_x, NULL);

}

const s_sym ** sym_init_str (const s_sym **sym, const s_str *src)
{
  const s_sym *tmp = NULL;
  tmp = sym_find(src);
  if (! tmp)
    tmp = sym_new(src);
  if (! tmp)
    return NULL;
  *sym = tmp;
  return sym;
}

uw * sym_list_size (uw *dest)
{
  uw size = 0;
  const s_sym_list *l;
  l = g_sym_list;
  while (l) {
    size += sizeof(s_sym) + l->sym->str.size + 1;
    l = l->next;
  }
  *dest = size;
  return dest;
}

bool sym_register (const s_sym *sym, s_sym *free_sym)
{
  s_sym_list *tmp = NULL;
  assert(sym);
  if (sym_find(&sym->str))
    return false;
  tmp = sym_list_new(sym, free_sym, g_sym_list);
  if (! tmp)
    return false;
  g_sym_list = tmp;
  return true;
}

bool sym_is_array_type (const s_sym *sym)
{
  character c;
  sw r;
  bool separator;
  s_str str;
  assert(sym);
  str = sym->str;
  separator = true;
  while (1) {
    if (separator) {
      if (str_read_character_utf8(&str, &c) <= 0)
        return false;
      if (! character_is_uppercase(c))
        return false;
      separator = false;
    }
    else {
      r = str_read_character_utf8(&str, &c);
      if (r <= 0)
        return false;
      if (c == '.')
        separator = true;
      if (c == '[')
        return true;
    }
  }
  return false;
}

bool sym_is_module (const s_sym *sym)
{
  character c;
  sw r;
  bool separator;
  s_str str;
  assert(sym);
  str = sym->str;
  separator = true;
  while (1) {
    if (separator) {
      if (str_read_character_utf8(&str, &c) <= 0)
        return false;
      if (! character_is_uppercase(c))
        return false;
      separator = false;
    }
    else {
      r = str_read_character_utf8(&str, &c);
      if (r <= 0)
        return true;
      if (c == '.')
        separator = true;
      if (c == '[')
        return false;
    }
  }
  return true;
}

s_sym_list * sym_list_new (const s_sym *sym, s_sym *free_sym,
                           s_sym_list *next)
{
  s_sym_list *sym_list;
  sym_list = alloc(sizeof(s_sym_list));
  if (! sym_list)
    return NULL;
  sym_list->sym = sym;
  sym_list->free_sym = free_sym;
  sym_list->next = next;
  return sym_list;
}

bool * sym_must_clean (const s_sym *sym, bool *must_clean)
{
  s_struct_type *st;
  if (sym_is_array_type(sym))
    sym = sym_array_type(sym);
  if (sym == &g_sym_Block) {
    *must_clean = true;
    return must_clean;
  }
  if (sym == &g_sym_Bool) {
    *must_clean = false;
    return must_clean;
  }
  if (sym == &g_sym_Call) {
    *must_clean = true;
    return must_clean;
  }
  if (sym == &g_sym_Callable ||
      sym == &g_sym_Cfn ||
      sym == &g_sym_Fn) {
    *must_clean = true;
    return must_clean;
  }
  if (sym == &g_sym_Character) {
    *must_clean = false;
    return must_clean;
  }
  if (sym == &g_sym_Complex) {
    *must_clean = true;
    return must_clean;
  }
  if (sym == &g_sym_Cow) {
    *must_clean = true;
    return must_clean;
  }
  if (sym == &g_sym_F32) {
    *must_clean = false;
    return must_clean;
  }
  if (sym == &g_sym_F64) {
    *must_clean = false;
    return must_clean;
  }
  if (sym == &g_sym_F128) {
    *must_clean = false;
    return must_clean;
  }
  if (sym == &g_sym_Integer) {
    *must_clean = true;
    return must_clean;
  }
  if (sym == &g_sym_List) {
    *must_clean = true;
    return must_clean;
  }
  if (sym == &g_sym_Map) {
    *must_clean = true;
    return must_clean;
  }
  if (sym == &g_sym_Ptr) {
    *must_clean = true;
    return must_clean;
  }
  if (sym == &g_sym_PtrFree) {
    *must_clean = true;
    return must_clean;
  }
  if (sym == &g_sym_Ratio) {
    *must_clean = true;
    return must_clean;
  }
  if (sym == &g_sym_S8) {
    *must_clean = false;
    return must_clean;
  }
  if (sym == &g_sym_S16) {
    *must_clean = false;
    return must_clean;
  }
  if (sym == &g_sym_S32) {
    *must_clean = false;
    return must_clean;
  }
  if (sym == &g_sym_S64) {
    *must_clean = false;
    return must_clean;
  }
  if (sym == &g_sym_Str) {
    *must_clean = true;
    return must_clean;
  }
  if (sym == &g_sym_Struct) {
    *must_clean = true;
    return must_clean;
  }
  if (sym == &g_sym_StructType) {
    *must_clean = true;
    return must_clean;
  }
  if (sym == &g_sym_Sym) {
    *must_clean = false;
    return must_clean;
  }
  if (sym == &g_sym_Sw) {
    *must_clean = false;
    return must_clean;
  }
  if (sym == &g_sym_Tag) {
    *must_clean = true;
    return must_clean;
  }
  if (sym == &g_sym_U8) {
    *must_clean = false;
    return must_clean;
  }
  if (sym == &g_sym_U16) {
    *must_clean = false;
    return must_clean;
  }
  if (sym == &g_sym_U32) {
    *must_clean = false;
    return must_clean;
  }
  if (sym == &g_sym_U64) {
    *must_clean = false;
    return must_clean;
  }
  if (sym == &g_sym_Uw) {
    *must_clean = false;
    return must_clean;
  }
  if (sym == &g_sym_Var) {
    *must_clean = true;
    return must_clean;
  }
  if (sym == &g_sym_Void) {
    *must_clean = false;
    return must_clean;
  }
  if (! struct_type_find(sym, &st))
    return NULL;
  if (st) {
    *must_clean = st->must_clean;
    return must_clean;
  }
  err_write_1("sym_must_clean: unknown type: ");
  err_inspect_sym(&sym);
  err_write_1("\n");
  assert(! "sym_must_clean: unknown type");
  return NULL;
}

const s_sym * sym_new (const s_str *src)
{
  s_sym *sym = NULL;
  s_sym_list *tmp = NULL;
  sym = alloc(sizeof(s_sym));
  if (! sym)
    return NULL;
  if (! str_init_copy(&sym->str, src)) {
    free(sym);
    return NULL;
  }
  tmp = sym_list_new(sym, sym, g_sym_list);
  if (! tmp) {
    str_clean(&sym->str);
    free(sym);
    return NULL;
  }
  g_sym_list = tmp;
  return sym;
}

bool sym_search_modules (const s_sym *sym, const s_sym **dest)
{
  return env_sym_search_modules(env_global(), sym, dest);
}

bool sym_to_ffi_type (const s_sym *sym, ffi_type *result_type,
                      ffi_type **dest)
{
  bool b;
  assert(sym);
  if (sym == &g_sym_Result) {
    if (! result_type) {
      err_puts("sym_to_ffi_type: invalid result type: Result");
      return false;
    }
    *dest = result_type;
    return true;
  }
  if (sym == &g_sym_Array ||
      sym_is_array_type(sym)) {
    *dest = &ffi_type_pointer;
    return true;
  }
  if (sym == &g_sym_Block) {
    *dest = &ffi_type_pointer;
    return true;
  }
  if (sym == &g_sym_Bool) {
    *dest = &ffi_type_uint8;
    return true;
  }
  if (sym == &g_sym_Call) {
    *dest = &ffi_type_pointer;
    return true;
  }
  if (sym == &g_sym_Callable ||
      sym == &g_sym_Cfn ||
      sym == &g_sym_Fn) {
    *dest = &ffi_type_pointer;
    return true;
  }
  if (sym == &g_sym_Char__star) {
    *dest = &ffi_type_pointer;
    return true;
  }
  if (sym == &g_sym_Character) {
    *dest = &ffi_type_uint32;
    return true;
  }
  if (sym == &g_sym_Complex) {
    *dest = &ffi_type_pointer;
    return true;
  }
  if (sym == &g_sym_Cow) {
    *dest = &ffi_type_pointer;
    return true;
  }
  if (sym == &g_sym_F32) {
    *dest = &ffi_type_float;
    return true;
  }
  if (sym == &g_sym_F64) {
    *dest = &ffi_type_double;
    return true;
  }
  if (sym == &g_sym_F128) {
    *dest = &ffi_type_longdouble;
    return true;
  }
  if (sym == &g_sym_Integer) {
    *dest = &ffi_type_pointer;
    return true;
  }
  if (sym == &g_sym_List) {
    *dest = &ffi_type_pointer;
    return true;
  }
  if (sym == &g_sym_Map) {
    *dest = &ffi_type_pointer;
    return true;
  }
  if (sym == &g_sym_Ptr) {
    *dest = &ffi_type_pointer;
    return true;
  }
  if (sym == &g_sym_PtrFree) {
    *dest = &ffi_type_pointer;
    return true;
  }
  if (sym == &g_sym_Ratio) {
    *dest = &ffi_type_pointer;
    return true;
  }
  if (sym == &g_sym_S8) {
    *dest = &ffi_type_sint8;
    return true;
  }
  if (sym == &g_sym_S16) {
    *dest = &ffi_type_sint16;
    return true;
  }
  if (sym == &g_sym_S32) {
    *dest = &ffi_type_sint32;
    return true;
  }
  if (sym == &g_sym_S64) {
    *dest = &ffi_type_sint64;
    return true;
  }
  if (sym == &g_sym_Str) {
    *dest = &ffi_type_pointer;
    return true;
  }
  if (sym == &g_sym_Struct) {
    *dest = &ffi_type_pointer;
    return true;
  }
  if (sym == &g_sym_StructType) {
    *dest = &ffi_type_pointer;
    return true;
  }
  if (sym == &g_sym_Sym) {
    *dest = &ffi_type_pointer;
    return true;
  }
  if (sym == &g_sym_Sw) {
    *dest = &ffi_type_slong;
    return true;
  }
  if (sym == &g_sym_Tag) {
    *dest = &ffi_type_pointer;
    return true;
  }
  if (sym == &g_sym_Time) {
    *dest = &ffi_type_pointer;
    return true;
  }
  if (sym == &g_sym_Tuple) {
    *dest = &ffi_type_pointer;
    return true;
  }
  if (sym == &g_sym_U8) {
    *dest = &ffi_type_uint8;
    return true;
  }
  if (sym == &g_sym_U16) {
    *dest = &ffi_type_uint16;
    return true;
  }
  if (sym == &g_sym_U32) {
    *dest = &ffi_type_uint32;
    return true;
  }
  if (sym == &g_sym_U64) {
    *dest = &ffi_type_uint64;
    return true;
  }
  if (sym == &g_sym_Uw) {
    *dest = &ffi_type_ulong;
    return true;
  }
  if (sym == &g_sym_Var) {
    *dest = &ffi_type_pointer;
    return true;
  }
  if (sym == &g_sym_Void) {
    *dest = &ffi_type_void;
    return true;
  }
  if (! struct_type_exists(sym, &b))
    return false;
  if (b) {
    *dest = &ffi_type_pointer;
    return true;
  }
  err_write_1("sym_to_ffi_type: unknown type: ");
  err_inspect_sym(&sym);
  err_write_1("\n");
  assert(! "sym_to_ffi_type: unknown type");
  return false;
}

bool sym_to_tag_type (const s_sym *sym, e_tag_type *dest)
{
  bool b = false;
  if (sym == &g_sym_Array ||
      sym_is_array_type(sym)) {
    *dest = TAG_ARRAY;
    return true;
  }
  if (sym == &g_sym_Block) {
    *dest = TAG_BLOCK;
    return true;
  }
  if (sym == &g_sym_Bool) {
    *dest = TAG_BOOL;
    return true;
  }
  if (sym == &g_sym_Call) {
    *dest = TAG_CALL;
    return true;
  }
  if (sym == &g_sym_Callable ||
      sym == &g_sym_Cfn ||
      sym == &g_sym_Fn) {
    *dest = TAG_CALLABLE;
    return true;
  }
  if (sym == &g_sym_Character) {
    *dest = TAG_CHARACTER;
    return true;
  }
  if (sym == &g_sym_Complex) {
    *dest = TAG_COMPLEX;
    return true;
  }
  if (sym == &g_sym_Cow) {
    *dest = TAG_COW;
    return true;
  }
  if (sym == &g_sym_F32) {
    *dest = TAG_F32;
    return true;
  }
  if (sym == &g_sym_F64) {
    *dest = TAG_F64;
    return true;
  }
  if (sym == &g_sym_F128) {
    *dest = TAG_F128;
    return true;
  }
  if (sym == &g_sym_Ident) {
    *dest = TAG_IDENT;
    return true;
  }
  if (sym == &g_sym_Integer) {
    *dest = TAG_INTEGER;
    return true;
  }
  if (sym == &g_sym_List) {
    *dest = TAG_LIST;
    return true;
  }
  if (sym == &g_sym_Map) {
    *dest = TAG_MAP;
    return true;
  }
  if (sym == &g_sym_Ptag) {
    *dest = TAG_PTAG;
    return true;
  }
  if (sym == &g_sym_Ptr) {
    *dest = TAG_PTR;
    return true;
  }
  if (sym == &g_sym_PtrFree) {
    *dest = TAG_PTR_FREE;
    return true;
  }
  if (sym == &g_sym_Quote) {
    *dest = TAG_QUOTE;
    return true;
  }
  if (sym == &g_sym_Ratio) {
    *dest = TAG_RATIO;
    return true;
  }
  if (sym == &g_sym_S8) {
    *dest = TAG_S8;
    return true;
  }
  if (sym == &g_sym_S16) {
    *dest = TAG_S16;
    return true;
  }
  if (sym == &g_sym_S32) {
    *dest = TAG_S32;
    return true;
  }
  if (sym == &g_sym_S64) {
    *dest = TAG_S64;
    return true;
  }
  if (sym == &g_sym_Str) {
    *dest = TAG_STR;
    return true;
  }
  if (sym == &g_sym_Struct) {
    *dest = TAG_PSTRUCT;
    return true;
  }
  if (sym == &g_sym_StructType) {
    *dest = TAG_PSTRUCT_TYPE;
    return true;
  }
  if (sym == &g_sym_Sw) {
    *dest = TAG_SW;
    return true;
  }
  if (sym == &g_sym_Sym) {
    *dest = TAG_SYM;
    return true;
  }
  if (sym == &g_sym_Tag) {
    *dest = TAG_VOID;
    return true;
  }
  if (sym == &g_sym_Time) {
    *dest = TAG_TIME;
    return true;
  }
  if (sym == &g_sym_Tuple) {
    *dest = TAG_TUPLE;
    return true;
  }
  if (sym == &g_sym_U8) {
    *dest = TAG_U8;
    return true;
  }
  if (sym == &g_sym_U16) {
    *dest = TAG_U16;
    return true;
  }
  if (sym == &g_sym_U32) {
    *dest = TAG_U32;
    return true;
  }
  if (sym == &g_sym_U64) {
    *dest = TAG_U64;
    return true;
  }
  if (sym == &g_sym_Uw) {
    *dest = TAG_UW;
    return true;
  }
  if (sym == &g_sym_Var) {
    *dest = TAG_VAR;
    return true;
  }
  if (sym == &g_sym_Void) {
    *dest = TAG_VOID;
    return true;
  }
  if (! struct_type_exists(sym, &b))
    return false;
  if (b) {
    *dest = TAG_PSTRUCT;
    return true;
  }
  err_write_1("sym_to_tag_type: unknown type: ");
  err_inspect_sym(&sym);
  err_write_1("\n");
  assert(! "sym_to_tag_type: unknown type");
  return false;
}

bool sym_type_is_integer (const s_sym * const *type)
{
  const s_sym *t = *type;
  return (t == &g_sym_Integer ||
          t == &g_sym_S8      ||
          t == &g_sym_S16     ||
          t == &g_sym_S32     ||
          t == &g_sym_S64     ||
          t == &g_sym_Sw      ||
          t == &g_sym_U8      ||
          t == &g_sym_U16     ||
          t == &g_sym_U32     ||
          t == &g_sym_U64     ||
          t == &g_sym_Uw);
}

uw * sym_type_size (const s_sym * const *type, uw *dest)
{
  s_struct_type *st;
  if (*type == &g_sym_Array ||
      sym_is_array_type(*type)) {
    *dest = sizeof(s_array);
    return dest;
  }
  if (*type == &g_sym_Block) {
    *dest = sizeof(s_block);
    return dest;
  }
  if (*type == &g_sym_Bool) {
    *dest = sizeof(bool);
    return dest;
  }
  if (*type == &g_sym_Call) {
    *dest = sizeof(s_call);
    return dest;
  }
  if (*type == &g_sym_Callable ||
      *type == &g_sym_Cfn ||
      *type == &g_sym_Fn) {
    *dest = sizeof(p_callable);
    return dest;
  }
  if (*type == &g_sym_Character) {
    *dest = sizeof(character);
    return dest;
  }
  if (*type == &g_sym_Complex) {
    *dest = sizeof(s_complex);
    return dest;
  }
  if (*type == &g_sym_Cow) {
    *dest = sizeof(s_cow);
    return dest;
  }
  if (*type == &g_sym_F32) {
    *dest = sizeof(f32);
    return dest;
  }
  if (*type == &g_sym_F64) {
    *dest = sizeof(f64);
    return dest;
  }
  if (*type == &g_sym_F128) {
    *dest = sizeof(f128);
    return dest;
  }
  if (*type == &g_sym_Fact) {
    *dest = sizeof(s_fact);
    return dest;
  }
  if (*type == &g_sym_Ident) {
    *dest = sizeof(s_ident);
    return dest;
  }
  if (*type == &g_sym_Integer) {
    *dest = sizeof(s_integer);
    return dest;
  }
  if (*type == &g_sym_List) {
    *dest = sizeof(s_list *);
    return dest;
  }
  if (*type == &g_sym_Map) {
    *dest = sizeof(s_map);
    return dest;
  }
  if (*type == &g_sym_Ptag) {
    *dest = sizeof(p_tag);
    return dest;
  }
  if (*type == &g_sym_Ptr) {
    *dest = sizeof(void *);
    return dest;
  }
  if (*type == &g_sym_PtrFree) {
    *dest = sizeof(void *);
    return dest;
  }
  if (*type == &g_sym_Quote) {
    *dest = sizeof(s_quote);
    return dest;
  }
  if (*type == &g_sym_Ratio) {
    *dest = sizeof(s_ratio);
    return dest;
  }
  if (*type == &g_sym_S8) {
    *dest = sizeof(s8);
    return dest;
  }
  if (*type == &g_sym_S16) {
    *dest = sizeof(s16);
    return dest;
  }
  if (*type == &g_sym_S32) {
    *dest = sizeof(s32);
    return dest;
  }
  if (*type == &g_sym_S64) {
    *dest = sizeof(s64);
    return dest;
  }
  if (*type == &g_sym_Str) {
    *dest = sizeof(s_str);
    return dest;
  }
  if (*type == &g_sym_Struct) {
    *dest = sizeof(p_struct);
    return dest;
  }
  if (*type == &g_sym_StructType) {
    *dest = sizeof(p_struct_type);
    return dest;
  }
  if (*type == &g_sym_Sw) {
    *dest = sizeof(sw);
    return dest;
  }
  if (*type == &g_sym_Sym) {
    *dest = sizeof(p_sym);
    return dest;
  }
  if (*type == &g_sym_Tag) {
    *dest = sizeof(s_tag);
    return dest;
  }
  if (*type == &g_sym_Time) {
    *dest = sizeof(s_time);
    return dest;
  }
  if (*type == &g_sym_Tuple) {
    *dest = sizeof(s_tuple);
    return dest;
  }
  if (*type == &g_sym_U8) {
    *dest = sizeof(u8);
    return dest;
  }
  if (*type == &g_sym_U16) {
    *dest = sizeof(u16);
    return dest;
  }
  if (*type == &g_sym_U32) {
    *dest = sizeof(u32);
    return dest;
  }
  if (*type == &g_sym_U64) {
    *dest = sizeof(u64);
    return dest;
  }
  if (*type == &g_sym_Uw) {
    *dest = sizeof(uw);
    return dest;
  }
  if (*type == &g_sym_Var) {
    *dest = sizeof(s_var);
    return dest;
  }
  if (*type == &g_sym_Void) {
    *dest = 0;
    return dest;
  }
  if (! struct_type_find(*type, &st))
    return NULL;
  if (st) {
    *dest = st->size;
    return dest;
  }
  err_write_1("sym_type_size: unknown type: ");
  err_inspect_sym(type);
  err_write_1("\n");
  assert(! "sym_type_size: unknown type");
  return NULL;
}
