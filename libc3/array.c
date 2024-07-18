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
#include <stdlib.h>
#include <string.h>
#include <err.h>
#include "array.h"
#include "bool.h"
#include "buf.h"
#include "buf_inspect.h"
#include "buf_parse.h"
#include "call.h"
#include "cfn.h"
#include "character.h"
#include "env.h"
#include "f32.h"
#include "f64.h"
#include "fact.h"
#include "fn.h"
#include "ident.h"
#include "integer.h"
#include "list.h"
#include "ptag.h"
#include "quote.h"
#include "s8.h"
#include "s16.h"
#include "s32.h"
#include "s64.h"
#include "str.h"
#include "sw.h"
#include "sym.h"
#include "tag.h"
#include "tuple.h"
#include "type.h"
#include "u8.h"
#include "u16.h"
#include "u32.h"
#include "u64.h"
#include "uw.h"
#include "var.h"

bool array_copy_data (f_copy copy, const u8 *src, u8 *dest);

void array_clean (s_array *a)
{
  u8 *data;
  u8 *data_tag;
  uw i;
  uw size;
  s_tag tag;
  assert(a);
  free(a->dimensions);
  if (a->data) {
    data = a->data;
    size = array_type_size(a->type);
    tag.type = array_type_to_tag_type(a->type);
    i = 0;
    while (i < a->count) {
      data_tag = tag_to_pointer(&tag, a->type);
      memcpy(data_tag, data, size);
      tag_clean(&tag);
      data += size;
      i++;
    }
    free(a->data);
  }
  if (a->tags) {
    i = 0;
    while (i < a->count) {
      tag_clean(a->tags + i);
      i++;
    }
    free(a->tags);
  }
}

s_array * array_copy (const s_array *src, s_array *dest)
{
  f_copy copy;
  u8 *data_dest;
  u8 *data_src;
  uw i = 0;
  uw item_size;
  s_array tmp;
  assert(dest);
  assert(src);
  assert(src->dimension);
  assert(src->dimensions);
  (void) i;
  bzero(&tmp, sizeof(s_array));
  if (! src->dimension) {
    assert(! "array_copy: zero dimension");
    errx(1, "array_copy: zero dimension");
    return NULL;
  }
#ifdef DEBUG
  while (i < src->dimension) {
    assert(src->dimensions[i].count);
    i++;
  }
#endif
  tmp.dimension = src->dimension;
  if (! (tmp.dimensions = calloc(src->dimension,
                                 sizeof(s_array_dimension)))) {
    assert(! "array_copy: out of memory: dimensions");
    warnx("array_copy: out of memory: dimensions");
    return NULL;
  }
  memcpy(tmp.dimensions, src->dimensions,
         src->dimension * sizeof(s_array_dimension));
  tmp.count = src->count;
  tmp.size = src->size;
  tmp.type = src->type;
  if (src->data) {
    if (! (tmp.data = calloc(1, src->size)))
      errx(1, "array_copy: out of memory");
    copy = array_type_to_copy(src->type);
    data_dest = tmp.data;
    data_src = src->data;
    i = 0;
    item_size = src->dimensions[src->dimension - 1].item_size;
    while (i < src->count) {
      if (copy(data_src, data_dest) != data_dest) {
        return NULL;
      }
      data_dest += item_size;
      data_src += item_size;
      i++;
    }
  }
  else
    tmp.data = NULL;
  if (src->tags) {
    tmp.tags = calloc(src->count, sizeof(s_tag));
    i = 0;
    while (i < src->count) {
      tag_copy(src->tags + i, tmp.tags + i);
      i++;
    }
  }
  else
    tmp.tags = NULL;
  *dest = tmp;
  return dest;
}

void * array_data (const s_array *a, const uw *address)
{
  uw i = 0;
  uw offset = 0;
  assert(a);
  assert(address);
  assert(a->data);
  while (i < a->dimension) {
    if (address[i] >= a->dimensions[i].count) {
      warnx("array_data: address overflow: %lu", i);
      return NULL;
    }
    offset += address[i] * a->dimensions[i].item_size;
    i++;
  }
  assert(offset < a->size);
  return (s8 *) a->data + offset;
}

s_tag * array_data_tag (s_tag *a, const s_tag *address, s_tag *dest)
{
  void *a_data;
  f_copy copy;
  void *dest_data;
  assert(a->type == TAG_ARRAY);
  assert(address->type == TAG_ARRAY);
  if ((a_data = array_data(&a->data.array,
                           address->data.array.data))) {
    tag_init(dest);
    copy = array_type_to_copy(a->data.array.type);
    sym_to_tag_type(a->data.array.type, &dest->type);
    dest_data = tag_to_pointer(dest, a->data.array.type);
    if (copy(a_data, dest_data) != dest_data)
      return NULL;
    return dest;
  }
  return NULL;
}

s_array * array_init (s_array *a, const s_sym *type, uw dimension,
                      const uw *dimensions)
{
  uw count = 1;
  uw i = 0;
  uw item_size;
  assert(a);
  assert(dimension);
  assert(dimensions);
  if (! dimension) {
    assert(! "array_init: zero dimension");
    errx(1, "array_init: zero dimension");
    return NULL;
  }
#ifdef DEBUG
  while (i < dimension) {
    assert(dimensions[i]);
    i++;
  }
#endif
  a->dimension = dimension;
  a->dimensions = calloc(dimension, sizeof(s_array_dimension));
  i = 0;
  while (i < dimension) {
    a->dimensions[i].count = dimensions[i];
    count *= dimensions[i];
    i++;
  }
  i--;
  a->type = type;
  item_size = array_type_size(type);
  a->dimensions[i].item_size = item_size;
  while (i > 0) {
    i--;
    a->dimensions[i].item_size = a->dimensions[i + 1].count *
      a->dimensions[i + 1].item_size;
  }
  a->size = a->dimensions[0].count * a->dimensions[0].item_size;
  a->count = count;
  a->data = NULL;
  a->tags = NULL;
  return a;
}

s_array * array_init_1 (s_array *array, s8 *p)
{
  s_buf buf;
  sw r;
  s_array tmp;
  buf_init_1(&buf, p);
  if ((r = buf_parse_array(&buf, &tmp)) != (sw) strlen(p)) {
    warnx("array_init_1: buf_parse_array(%s) => %ld != %ld", p, r, strlen(p));
    if (r > 0)
      array_clean(&tmp);
    buf_clean(&buf);
    return NULL;
  }
  buf_clean(&buf);
  if (! env_eval_array(&g_c3_env, &tmp, array)) {
    warnx("array_init_1: env_eval_array");
    array_clean(&tmp);
    return NULL;
  }
  array_clean(&tmp);
  return array;
}

s_str * array_inspect (const s_array *array, s_str *dest)
{
  sw size;
  s_buf tmp;
  size = buf_inspect_array_size(array);
  if (size < 0) {
    assert(! "array_inspect: error");
    errx(1, "array_inspect: error");
    return NULL;
  }
  buf_init_alloc(&tmp, size);
  buf_inspect_array(&tmp, array);
  assert(tmp.wpos == tmp.size);
  if (tmp.wpos != tmp.size) {
    buf_clean(&tmp);
    errx(1, "array_inspect: buf_inspect_array");
    return NULL;
  }
  return buf_to_str(&tmp, dest);
}

uw array_type_size (const s_sym *type)
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
  if (type == sym_1("Sw"))
    return sizeof(sw);
  if (type == sym_1("S64"))
    return sizeof(s64);
  if (type == sym_1("S32"))
    return sizeof(s32);
  if (type == sym_1("S16"))
    return sizeof(s16);
  if (type == sym_1("S8"))
    return sizeof(s8);
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
  if (type == sym_1("List"))
    return sizeof(s_list *);
  if (type == sym_1("Ptag"))
    return sizeof(p_tag);
  if (type == sym_1("Quote"))
    return sizeof(s_quote);
  if (type == sym_1("Str"))
    return sizeof(s_str);
  if (type == sym_1("Sym"))
    return sizeof(s_sym *);
  if (type == sym_1("Tuple"))
    return sizeof(s_tuple);
  if (type == sym_1("Var"))
    return sizeof(s_tag);
  assert(! "array_type_size: unknown type");
  errx(1, "array_type_size: unknown type: %s", type->str.ptr.ps8);
  return 0;
}

f_buf_inspect array_type_to_buf_inspect (const s_sym *type)
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
  assert(! "array_type_to_buf_inspect: unknown type");
  errx(1, "array_type_to_buf_inspect: unknown type");
  return NULL;
}

f_buf_inspect_size array_type_to_buf_inspect_size (const s_sym *type)
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
  assert(! "array_type_to_buf_inspect: unknown type");
  errx(1, "array_type_to_buf_inspect: unknown type");
  return NULL;
}

f_copy array_type_to_copy (const s_sym *type)
{
  if (type == sym_1("Bool"))
    return (f_copy) bool_copy;
  if (type == sym_1("Call"))
    return (f_copy) call_copy;
  if (type == sym_1("Cfn"))
    return (f_copy) cfn_copy;
  if (type == sym_1("Character"))
    return (f_copy) character_copy;
  if (type == sym_1("F32"))
    return (f_copy) f32_copy;
  if (type == sym_1("F64"))
    return (f_copy) f64_copy;
  if (type == sym_1("Fact"))
    return (f_copy) fact_copy;
  if (type == sym_1("Fn"))
    return (f_copy) fn_copy;
  if (type == sym_1("Ident"))
    return (f_copy) ident_copy;
  if (type == sym_1("Integer"))
    return (f_copy) integer_copy;
  if (type == sym_1("List"))
    return (f_copy) list_copy;
  if (type == sym_1("Sw"))
    return (f_copy) sw_copy;
  if (type == sym_1("S64"))
    return (f_copy) s64_copy;
  if (type == sym_1("S32"))
    return (f_copy) s32_copy;
  if (type == sym_1("S16"))
    return (f_copy) s16_copy;
  if (type == sym_1("S8"))
    return (f_copy) s8_copy;
  if (type == sym_1("U8"))
    return (f_copy) u8_copy;
  if (type == sym_1("U16"))
    return (f_copy) u16_copy;
  if (type == sym_1("U32"))
    return (f_copy) u32_copy;
  if (type == sym_1("U64"))
    return (f_copy) u64_copy;
  if (type == sym_1("Uw"))
    return (f_copy) uw_copy;
  if (type == sym_1("Ptag"))
    return (f_copy) ptag_copy;
  if (type == sym_1("Quote"))
    return (f_copy) quote_copy;
  if (type == sym_1("Str"))
    return (f_copy) str_copy;
  if (type == sym_1("Sym"))
    return (f_copy) sym_copy;
  if (type == sym_1("Tuple"))
    return (f_copy) tuple_copy;
  if (type == sym_1("Var"))
    return (f_copy) var_copy;
  assert(! "array_type_to_copy: unknown type");
  errx(1, "array_type_to_copy: unknown type");
  return NULL;
}

e_tag_type array_type_to_tag_type (const s_sym *type)
{
  if (type == sym_1("Bool"))
    return TAG_BOOL;
  if (type == sym_1("Call"))
    return TAG_CALL;
  if (type == sym_1("Cfn"))
    return TAG_CFN;
  if (type == sym_1("Character"))
    return TAG_CHARACTER;
  if (type == sym_1("F32"))
    return TAG_F32;
  if (type == sym_1("F64"))
    return TAG_F64;
  if (type == sym_1("Fact"))
    return TAG_FACT;
  if (type == sym_1("Fn"))
    return TAG_FN;
  if (type == sym_1("Ident"))
    return TAG_IDENT;
  if (type == sym_1("Integer"))
    return TAG_INTEGER;
  if (type == sym_1("Sw"))
    return TAG_SW;
  if (type == sym_1("S64"))
    return TAG_S64;
  if (type == sym_1("S32"))
    return TAG_S32;
  if (type == sym_1("S16"))
    return TAG_S16;
  if (type == sym_1("S8"))
    return TAG_S8;
  if (type == sym_1("U8"))
    return TAG_U8;
  if (type == sym_1("U16"))
    return TAG_U16;
  if (type == sym_1("U32"))
    return TAG_U32;
  if (type == sym_1("U64"))
    return TAG_U64;
  if (type == sym_1("Uw"))
    return TAG_UW;
  if (type == sym_1("List"))
    return TAG_LIST;
  if (type == sym_1("Ptag"))
    return TAG_PTAG;
  if (type == sym_1("Quote"))
    return TAG_QUOTE;
  if (type == sym_1("Str"))
    return TAG_STR;
  if (type == sym_1("Sym"))
    return TAG_SYM;
  if (type == sym_1("Tuple"))
    return TAG_TUPLE;
  if (type == sym_1("Var"))
    return TAG_VAR;
  assert(! "array_type_to_tag_type: invalid type");
  errx(1, "array_type_to_tag_type: invalid type: %s", type->str.ptr.ps8);
  return TAG_VOID;
}
