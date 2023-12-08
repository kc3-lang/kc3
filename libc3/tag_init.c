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
#include <string.h>
#include "array.h"
#include "buf.h"
#include "buf_inspect.h"
#include "buf_parse.h"
#include "call.h"
#include "cfn.h"
#include "compare.h"
#include "env.h"
#include "fn.h"
#include "frame.h"
#include "hash.h"
#include "ident.h"
#include "integer.h"
#include "list.h"
#include "map.h"
#include "ptr.h"
#include "quote.h"
#include "str.h"
#include "tag.h"
#include "tag_init.h"
#include "time.h"
#include "tuple.h"

s_tag * tag_init_array (s_tag *tag, const s_sym *type, uw dimension, 
                        const uw *dimensions)
{
  s_tag tmp = {0};
  assert(tag);
  tmp.type = TAG_ARRAY;
  if (! array_init(&tmp.data.array, type, dimension, dimensions))
    return NULL;
  *tag = tmp;
  return tag;
}

s_tag * tag_init_bool (s_tag *tag, bool b)
{
  s_tag tmp = {0};
  assert(tag);
  tmp.type = TAG_BOOL;
  tmp.data.bool = b;
  *tag = tmp;
  return tag;
}

s_tag * tag_init_call (s_tag *tag)
{
  s_tag tmp = {0};
  assert(tag);
  tmp.type = TAG_CALL;
  if (! call_init(&tmp.data.call))
    return NULL;
  *tag = tmp;
  return tag;
}

s_tag * tag_init_character (s_tag *tag, character c)
{
  s_tag tmp = {0};
  assert(tag);
  tmp.type = TAG_CHARACTER;
  tmp.data.character = c;
  *tag = tmp;
  return tag;
}

s_tag * tag_init_f32 (s_tag *tag, f32 f)
{
  s_tag tmp = {0};
  assert(tag);
  tmp.type = TAG_F32;
  tmp.data.f32 = f;
  *tag = tmp;
  return tag;
}

s_tag * tag_init_f64 (s_tag *tag, f64 f)
{
  s_tag tmp = {0};
  assert(tag);
  tmp.type = TAG_F64;
  tmp.data.f64 = f;
  *tag = tmp;
  return tag;
}

s_tag * tag_init_ident (s_tag *tag, const s_ident *ident)
{
  s_tag tmp = {0};
  assert(tag);
  tmp.type = TAG_IDENT;
  tmp.data.ident = *ident;
  *tag = tmp;
  return tag;
}

s_tag * tag_init_ident_1 (s_tag *tag, const s8 *p)
{
  s_tag tmp = {0};
  assert(tag);
  tmp.type = TAG_IDENT;
  if (! ident_init_1(&tmp.data.ident, p))
    return NULL;
  *tag = tmp;
  return tag;
}

s_tag * tag_init_integer_1 (s_tag *tag, const s8 *p)
{
  s_tag tmp = {0};
  assert(tag);
  tmp.type = TAG_INTEGER;
  if (! integer_init_1(&tmp.data.integer, p))
    return NULL;
  *tag = tmp;
  return tag;
}

s_tag * tag_init_integer_copy (s_tag *tag, const s_integer *i)
{
  s_tag tmp = {0};
  assert(tag);
  tmp.type = TAG_INTEGER;
  if (! integer_init_copy(&tmp.data.integer, i))
    return NULL;
  *tag = tmp;
  return tag;
}

s_tag * tag_init_integer_zero (s_tag *tag)
{
  s_tag tmp = {0};
  assert(tag);
  tmp.type = TAG_INTEGER;
  if (! integer_init_zero(&tmp.data.integer))
    return NULL;
  *tag = tmp;
  return tag;
}

s_tag * tag_init_list (s_tag *tag, s_list *list)
{
  s_tag tmp = {0};
  assert(tag);
  tmp.type = TAG_LIST;
  tmp.data.list = list;
  *tag = tmp;
  return tag;
}

s_tag * tag_init_map (s_tag *tag, uw count)
{
  s_tag tmp = {0};
  assert(tag);
  tmp.type = TAG_MAP;
  if (! map_init(&tmp.data.map, count))
    return NULL;
  *tag = tmp;
  return tag;
}

s_tag * tag_init_map_1 (s_tag *tag, const s8 *p)
{
  s_tag tmp = {0};
  assert(tag);
  tmp.type = TAG_MAP;
  if (! map_init_1(&tmp.data.map, p))
    return NULL;
  *tag = tmp;
  return tag;
}

s_tag * tag_init_ptr (s_tag *tag, const s_sym *type, void *p)
{
  s_tag tmp = {0};
  assert(tag);
  tmp.type = TAG_PTR;
  if (! ptr_init(&tmp.data.ptr, type, p))
    return NULL;
  *tag = tmp;
  return tag;
}

s_tag * tag_init_s8 (s_tag *tag, s8 i)
{
  s_tag tmp = {0};
  assert(tag);
  tmp.type = TAG_S8;
  tmp.data.s8 = i;
  *tag = tmp;
  return tag;
}

s_tag * tag_init_s16 (s_tag *tag, s16 i)
{
  s_tag tmp = {0};
  assert(tag);
  tmp.type = TAG_S16;
  tmp.data.s16 = i;
  *tag = tmp;
  return tag;
}

s_tag * tag_init_s32 (s_tag *tag, s32 i)
{
  s_tag tmp = {0};
  assert(tag);
  tmp.type = TAG_S32;
  tmp.data.s32 = i;
  *tag = tmp;
  return tag;
}

s_tag * tag_init_s64 (s_tag *tag, s64 i)
{
  s_tag tmp = {0};
  assert(tag);
  tmp.type = TAG_S64;
  tmp.data.s64 = i;
  *tag = tmp;
  return tag;
}

s_tag * tag_init_str (s_tag *tag, s8 *p_free, uw size, const s8 *p)
{
  s_tag tmp = {0};
  assert(tag);
  tmp.type = TAG_STR;
  if (! str_init(&tmp.data.str, p_free, size, p))
    return NULL;
  *tag = tmp;
  return tag;
}

s_tag * tag_init_str_1 (s_tag *tag, s8 *p_free, const s8 *p)
{
  s_tag tmp = {0};
  assert(tag);
  tmp.type = TAG_STR;
  if (! str_init_1(&tmp.data.str, p_free, p))
    return NULL;
  *tag = tmp;
  return tag;
}

s_tag * tag_init_sw (s_tag *tag, sw i)
{
  s_tag tmp = {0};
  assert(tag);
  tmp.type = TAG_SW;
  tmp.data.sw = i;
  *tag = tmp;
  return tag;
}

s_tag * tag_init_sym (s_tag *tag, const s_sym *sym)
{
  s_tag tmp = {0};
  assert(tag);
  tmp.type = TAG_SYM;
  tmp.data.sym = sym;
  *tag = tmp;
  return tag;
}

s_tag * tag_init_sym_1 (s_tag *tag, const s8 *p)
{
  s_tag tmp = {0};
  assert(tag);
  tmp.type = TAG_SYM;
  if (! sym_init_1(&tmp.data.sym, p))
    return NULL;
  *tag = tmp;
  return tag;
}

s_tag * tag_init_tuple (s_tag *tag, uw count)
{
  s_tag tmp = {0};
  assert(tag);
  tmp.type = TAG_TUPLE;
  if (! tuple_init(&tmp.data.tuple, count))
    return NULL;
  *tag = tmp;
  return tag;
}

s_tag * tag_init_tuple_2 (s_tag *tag, const s_tag *a, const s_tag *b)
{
  s_tag tmp = {0};
  assert(tag);
  tmp.type = TAG_TUPLE;
  if (! tuple_init_2(&tmp.data.tuple, a, b))
    return NULL;
  *tag = tmp;
  return tag;
}

s_tag * tag_init_u8 (s_tag *tag, u8 i)
{
  s_tag tmp = {0};
  assert(tag);
  tmp.type = TAG_U8;
  tmp.data.u8 = i;
  *tag = tmp;
  return tag;
}

s_tag * tag_init_u16 (s_tag *tag, u16 i)
{
  s_tag tmp = {0};
  assert(tag);
  tmp.type = TAG_U16;
  tmp.data.u16 = i;
  *tag = tmp;
  return tag;
}

s_tag * tag_init_u32 (s_tag *tag, u32 i)
{
  s_tag tmp = {0};
  assert(tag);
  tmp.type = TAG_U32;
  tmp.data.u32 = i;
  *tag = tmp;
  return tag;
}

s_tag * tag_init_u64 (s_tag *tag, u64 i)
{
  s_tag tmp = {0};
  assert(tag);
  tmp.type = TAG_U64;
  tmp.data.u64 = i;
  *tag = tmp;
  return tag;
}

s_tag * tag_init_uw (s_tag *tag, uw i)
{
  s_tag tmp = {0};
  assert(tag);
  tmp.type = TAG_UW;
  tmp.data.uw = i;
  *tag = tmp;
  return tag;
}

s_tag * tag_init_var (s_tag *tag)
{
  s_tag tmp = {0};
  assert(tag);
  tmp.type = TAG_VAR;
  *tag = tmp;
  return tag;
}

s_tag * tag_init_void (s_tag *tag)
{
  s_tag tmp = {0};
  assert(tag);
  tmp.type = TAG_VOID;
  *tag = tmp;
  return tag;
}

s_tag * tag_new_array (const s_sym *type, uw dimension, 
                       const uw *dimensions)
{
  s_tag *tag;
  if (! (tag = calloc(1, sizeof(s_tag)))) {
    warn("tag_new_array: calloc");
    return NULL;
  }
  tag->type = TAG_ARRAY;
  if (! array_init(&tag->data.array, type, dimension, dimensions)) {
    free(tag);
    return NULL;
  }
  return tag;
}

s_tag * tag_new_bool (bool b)
{
  s_tag *tag;
  if (! (tag = calloc(1, sizeof(s_tag)))) {
    warn("tag_new_bool: calloc");
    return NULL;
  }
  tag->type = TAG_BOOL;
  tag->data.bool = b;
  return tag;
}

s_tag * tag_new_call (void)
{
  s_tag *tag;
  if (! (tag = calloc(1, sizeof(s_tag)))) {
    warn("tag_new_call: calloc");
    return NULL;
  }
  tag->type = TAG_CALL;
  if (! call_init(&tag->data.call)) {
    free(tag);
    return NULL;
  }
  return tag;
}

s_tag * tag_new_character (character c)
{
  s_tag *tag;
  if (! (tag = calloc(1, sizeof(s_tag)))) {
    warn("tag_new_character: calloc");
    return NULL;
  }
  tag->type = TAG_CHARACTER;
  tag->data.character = c;
  return tag;
}

s_tag * tag_new_f32 (f32 f)
{
  s_tag *tag;
  if (! (tag = calloc(1, sizeof(s_tag)))) {
    warn("tag_new_f32: calloc");
    return NULL;
  }
  tag->type = TAG_F32;
  tag->data.f32 = f;
  return tag;
}

s_tag * tag_new_f64 (f64 f)
{
  s_tag *tag;
  if (! (tag = calloc(1, sizeof(s_tag)))) {
    warn("tag_new_f64: calloc");
    return NULL;
  }
  tag->type = TAG_F64;
  tag->data.f64 = f;
  return tag;
}

s_tag * tag_new_ident (const s_ident *ident)
{
  s_tag *tag;
  if (! (tag = calloc(1, sizeof(s_tag)))) {
    warn("tag_new_ident: calloc");
    return NULL;
  }
  tag->type = TAG_IDENT;
  tag->data.ident = *ident;
  return tag;
}

s_tag * tag_new_ident_1 (const s8 *p)
{
  s_tag *tag;
  if (! (tag = calloc(1, sizeof(s_tag)))) {
    warn("tag_new_ident_1: calloc");
    return NULL;
  }
  tag->type = TAG_IDENT;
  if (! ident_init_1(&tag->data.ident, p)) {
    free(tag);
    return NULL;
  }
  return tag;
}

s_tag * tag_new_integer_1 (const s8 *p)
{
  s_tag *tag;
  if (! (tag = calloc(1, sizeof(s_tag)))) {
    warn("tag_new_integer_1: calloc");
    return NULL;
  }
  tag->type = TAG_INTEGER;
  if (! integer_init_1(&tag->data.integer, p)) {
    free(tag);
    return NULL;
  }
  return tag;
}

s_tag * tag_new_integer_copy (const s_integer *i)
{
  s_tag *tag;
  if (! (tag = calloc(1, sizeof(s_tag)))) {
    warn("tag_new_integer_copy: calloc");
    return NULL;
  }
  tag->type = TAG_INTEGER;
  if (! integer_init_copy(&tag->data.integer, i)) {
    free(tag);
    return NULL;
  }
  return tag;
}

s_tag * tag_new_integer_zero (void)
{
  s_tag *tag;
  if (! (tag = calloc(1, sizeof(s_tag)))) {
    warn("tag_new_integer_zero: calloc");
    return NULL;
  }
  tag->type = TAG_INTEGER;
  if (! integer_init_zero(&tag->data.integer)) {
    free(tag);
    return NULL;
  }
  return tag;
}

s_tag * tag_new_list (s_list *list)
{
  s_tag *tag;
  if (! (tag = calloc(1, sizeof(s_tag)))) {
    warn("tag_new_list: calloc");
    return NULL;
  }
  tag->type = TAG_LIST;
  tag->data.list = list;
  return tag;
}

s_tag * tag_new_map (uw count)
{
  s_tag *tag;
  if (! (tag = calloc(1, sizeof(s_tag)))) {
    warn("tag_new_map: calloc");
    return NULL;
  }
  tag->type = TAG_MAP;
  if (! map_init(&tag->data.map, count)) {
    free(tag);
    return NULL;
  }
  return tag;
}

s_tag * tag_new_map_1 (const s8 *p)
{
  s_tag *tag;
  if (! (tag = calloc(1, sizeof(s_tag)))) {
    warn("tag_new_map_1: calloc");
    return NULL;
  }
  tag->type = TAG_MAP;
  if (! map_init_1(&tag->data.map, p)) {
    free(tag);
    return NULL;
  }
  return tag;
}

s_tag * tag_new_ptr (const s_sym *type, void *p)
{
  s_tag *tag;
  if (! (tag = calloc(1, sizeof(s_tag)))) {
    warn("tag_new_ptr: calloc");
    return NULL;
  }
  tag->type = TAG_PTR;
  if (! ptr_init(&tag->data.ptr, type, p)) {
    free(tag);
    return NULL;
  }
  return tag;
}

s_tag * tag_new_s8 (s8 i)
{
  s_tag *tag;
  if (! (tag = calloc(1, sizeof(s_tag)))) {
    warn("tag_new_s8: calloc");
    return NULL;
  }
  tag->type = TAG_S8;
  tag->data.s8 = i;
  return tag;
}

s_tag * tag_new_s16 (s16 i)
{
  s_tag *tag;
  if (! (tag = calloc(1, sizeof(s_tag)))) {
    warn("tag_new_s16: calloc");
    return NULL;
  }
  tag->type = TAG_S16;
  tag->data.s16 = i;
  return tag;
}

s_tag * tag_new_s32 (s32 i)
{
  s_tag *tag;
  if (! (tag = calloc(1, sizeof(s_tag)))) {
    warn("tag_new_s32: calloc");
    return NULL;
  }
  tag->type = TAG_S32;
  tag->data.s32 = i;
  return tag;
}

s_tag * tag_new_s64 (s64 i)
{
  s_tag *tag;
  if (! (tag = calloc(1, sizeof(s_tag)))) {
    warn("tag_new_s64: calloc");
    return NULL;
  }
  tag->type = TAG_S64;
  tag->data.s64 = i;
  return tag;
}

s_tag * tag_new_str (s8 *p_free, uw size, const s8 *p)
{
  s_tag *tag;
  if (! (tag = calloc(1, sizeof(s_tag)))) {
    warn("tag_new_str: calloc");
    return NULL;
  }
  tag->type = TAG_STR;
  if (! str_init(&tag->data.str, p_free, size, p)) {
    free(tag);
    return NULL;
  }
  return tag;
}

s_tag * tag_new_str_1 (s8 *p_free, const s8 *p)
{
  s_tag *tag;
  if (! (tag = calloc(1, sizeof(s_tag)))) {
    warn("tag_new_str_1: calloc");
    return NULL;
  }
  tag->type = TAG_STR;
  if (! str_init_1(&tag->data.str, p_free, p)) {
    free(tag);
    return NULL;
  }
  return tag;
}

s_tag * tag_new_sw (sw i)
{
  s_tag *tag;
  if (! (tag = calloc(1, sizeof(s_tag)))) {
    warn("tag_new_sw: calloc");
    return NULL;
  }
  tag->type = TAG_SW;
  tag->data.sw = i;
  return tag;
}

s_tag * tag_new_sym (const s_sym *sym)
{
  s_tag *tag;
  if (! (tag = calloc(1, sizeof(s_tag)))) {
    warn("tag_new_sym: calloc");
    return NULL;
  }
  tag->type = TAG_SYM;
  tag->data.sym = sym;
  return tag;
}

s_tag * tag_new_sym_1 (const s8 *p)
{
  s_tag *tag;
  if (! (tag = calloc(1, sizeof(s_tag)))) {
    warn("tag_new_sym_1: calloc");
    return NULL;
  }
  tag->type = TAG_SYM;
  if (! sym_init_1(&tag->data.sym, p)) {
    free(tag);
    return NULL;
  }
  return tag;
}

s_tag * tag_new_tuple (uw count)
{
  s_tag *tag;
  if (! (tag = calloc(1, sizeof(s_tag)))) {
    warn("tag_new_tuple: calloc");
    return NULL;
  }
  tag->type = TAG_TUPLE;
  if (! tuple_init(&tag->data.tuple, count)) {
    free(tag);
    return NULL;
  }
  return tag;
}

s_tag * tag_new_tuple_2 (const s_tag *a, const s_tag *b)
{
  s_tag *tag;
  if (! (tag = calloc(1, sizeof(s_tag)))) {
    warn("tag_new_tuple_2: calloc");
    return NULL;
  }
  tag->type = TAG_TUPLE;
  if (! tuple_init_2(&tag->data.tuple, a, b)) {
    free(tag);
    return NULL;
  }
  return tag;
}

s_tag * tag_new_u8 (u8 i)
{
  s_tag *tag;
  if (! (tag = calloc(1, sizeof(s_tag)))) {
    warn("tag_new_u8: calloc");
    return NULL;
  }
  tag->type = TAG_U8;
  tag->data.u8 = i;
  return tag;
}

s_tag * tag_new_u16 (u16 i)
{
  s_tag *tag;
  if (! (tag = calloc(1, sizeof(s_tag)))) {
    warn("tag_new_u16: calloc");
    return NULL;
  }
  tag->type = TAG_U16;
  tag->data.u16 = i;
  return tag;
}

s_tag * tag_new_u32 (u32 i)
{
  s_tag *tag;
  if (! (tag = calloc(1, sizeof(s_tag)))) {
    warn("tag_new_u32: calloc");
    return NULL;
  }
  tag->type = TAG_U32;
  tag->data.u32 = i;
  return tag;
}

s_tag * tag_new_u64 (u64 i)
{
  s_tag *tag;
  if (! (tag = calloc(1, sizeof(s_tag)))) {
    warn("tag_new_u64: calloc");
    return NULL;
  }
  tag->type = TAG_U64;
  tag->data.u64 = i;
  return tag;
}

s_tag * tag_new_uw (uw i)
{
  s_tag *tag;
  if (! (tag = calloc(1, sizeof(s_tag)))) {
    warn("tag_new_uw: calloc");
    return NULL;
  }
  tag->type = TAG_UW;
  tag->data.uw = i;
  return tag;
}

s_tag * tag_new_var (void)
{
  s_tag *tag;
  if (! (tag = calloc(1, sizeof(s_tag)))) {
    warn("tag_new_var: calloc");
    return NULL;
  }
  tag->type = TAG_VAR;
  return tag;
}

s_tag * tag_new_void (void)
{
  s_tag *tag;
  if (! (tag = calloc(1, sizeof(s_tag)))) {
    warn("tag_new_void: calloc");
    return NULL;
  }
  tag->type = TAG_VOID;
  return tag;
}

s_tag * tag_array (s_tag *tag, const s_sym *type, uw dimension, 
                   const uw *dimensions)
{
  s_tag tmp = {0};
  assert(tag);
  tag_clean(tag);
  tmp.type = TAG_ARRAY;
  if (! array_init(&tmp.data.array, type, dimension, dimensions))
    return NULL;
  *tag = tmp;
  return tag;
}

s_tag * tag_bool (s_tag *tag, bool b)
{
  s_tag tmp = {0};
  assert(tag);
  tag_clean(tag);
  tmp.type = TAG_BOOL;
  tmp.data.bool = b;
  *tag = tmp;
  return tag;
}

s_tag * tag_call (s_tag *tag)
{
  s_tag tmp = {0};
  assert(tag);
  tag_clean(tag);
  tmp.type = TAG_CALL;
  if (! call_init(&tmp.data.call))
    return NULL;
  *tag = tmp;
  return tag;
}

s_tag * tag_character (s_tag *tag, character c)
{
  s_tag tmp = {0};
  assert(tag);
  tag_clean(tag);
  tmp.type = TAG_CHARACTER;
  tmp.data.character = c;
  *tag = tmp;
  return tag;
}

s_tag * tag_f32 (s_tag *tag, f32 f)
{
  s_tag tmp = {0};
  assert(tag);
  tag_clean(tag);
  tmp.type = TAG_F32;
  tmp.data.f32 = f;
  *tag = tmp;
  return tag;
}

s_tag * tag_f64 (s_tag *tag, f64 f)
{
  s_tag tmp = {0};
  assert(tag);
  tag_clean(tag);
  tmp.type = TAG_F64;
  tmp.data.f64 = f;
  *tag = tmp;
  return tag;
}

s_tag * tag_ident (s_tag *tag, const s_ident *ident)
{
  s_tag tmp = {0};
  assert(tag);
  tag_clean(tag);
  tmp.type = TAG_IDENT;
  tmp.data.ident = *ident;
  *tag = tmp;
  return tag;
}

s_tag * tag_ident_1 (s_tag *tag, const s8 *p)
{
  s_tag tmp = {0};
  assert(tag);
  tag_clean(tag);
  tmp.type = TAG_IDENT;
  if (! ident_init_1(&tmp.data.ident, p))
    return NULL;
  *tag = tmp;
  return tag;
}

s_tag * tag_integer_1 (s_tag *tag, const s8 *p)
{
  s_tag tmp = {0};
  assert(tag);
  tag_clean(tag);
  tmp.type = TAG_INTEGER;
  if (! integer_init_1(&tmp.data.integer, p))
    return NULL;
  *tag = tmp;
  return tag;
}

s_tag * tag_integer_copy (s_tag *tag, const s_integer *i)
{
  s_tag tmp = {0};
  assert(tag);
  tag_clean(tag);
  tmp.type = TAG_INTEGER;
  if (! integer_init_copy(&tmp.data.integer, i))
    return NULL;
  *tag = tmp;
  return tag;
}

s_tag * tag_integer_zero (s_tag *tag)
{
  s_tag tmp = {0};
  assert(tag);
  tag_clean(tag);
  tmp.type = TAG_INTEGER;
  if (! integer_init_zero(&tmp.data.integer))
    return NULL;
  *tag = tmp;
  return tag;
}

s_tag * tag_list (s_tag *tag, s_list *list)
{
  s_tag tmp = {0};
  assert(tag);
  tag_clean(tag);
  tmp.type = TAG_LIST;
  tmp.data.list = list;
  *tag = tmp;
  return tag;
}

s_tag * tag_map (s_tag *tag, uw count)
{
  s_tag tmp = {0};
  assert(tag);
  tag_clean(tag);
  tmp.type = TAG_MAP;
  if (! map_init(&tmp.data.map, count))
    return NULL;
  *tag = tmp;
  return tag;
}

s_tag * tag_map_1 (s_tag *tag, const s8 *p)
{
  s_tag tmp = {0};
  assert(tag);
  tag_clean(tag);
  tmp.type = TAG_MAP;
  if (! map_init_1(&tmp.data.map, p))
    return NULL;
  *tag = tmp;
  return tag;
}

s_tag * tag_ptr (s_tag *tag, const s_sym *type, void *p)
{
  s_tag tmp = {0};
  assert(tag);
  tag_clean(tag);
  tmp.type = TAG_PTR;
  if (! ptr_init(&tmp.data.ptr, type, p))
    return NULL;
  *tag = tmp;
  return tag;
}

s_tag * tag_s8 (s_tag *tag, s8 i)
{
  s_tag tmp = {0};
  assert(tag);
  tag_clean(tag);
  tmp.type = TAG_S8;
  tmp.data.s8 = i;
  *tag = tmp;
  return tag;
}

s_tag * tag_s16 (s_tag *tag, s16 i)
{
  s_tag tmp = {0};
  assert(tag);
  tag_clean(tag);
  tmp.type = TAG_S16;
  tmp.data.s16 = i;
  *tag = tmp;
  return tag;
}

s_tag * tag_s32 (s_tag *tag, s32 i)
{
  s_tag tmp = {0};
  assert(tag);
  tag_clean(tag);
  tmp.type = TAG_S32;
  tmp.data.s32 = i;
  *tag = tmp;
  return tag;
}

s_tag * tag_s64 (s_tag *tag, s64 i)
{
  s_tag tmp = {0};
  assert(tag);
  tag_clean(tag);
  tmp.type = TAG_S64;
  tmp.data.s64 = i;
  *tag = tmp;
  return tag;
}

s_tag * tag_str (s_tag *tag, s8 *p_free, uw size, const s8 *p)
{
  s_tag tmp = {0};
  assert(tag);
  tag_clean(tag);
  tmp.type = TAG_STR;
  if (! str_init(&tmp.data.str, p_free, size, p))
    return NULL;
  *tag = tmp;
  return tag;
}

s_tag * tag_str_1 (s_tag *tag, s8 *p_free, const s8 *p)
{
  s_tag tmp = {0};
  assert(tag);
  tag_clean(tag);
  tmp.type = TAG_STR;
  if (! str_init_1(&tmp.data.str, p_free, p))
    return NULL;
  *tag = tmp;
  return tag;
}

s_tag * tag_sw (s_tag *tag, sw i)
{
  s_tag tmp = {0};
  assert(tag);
  tag_clean(tag);
  tmp.type = TAG_SW;
  tmp.data.sw = i;
  *tag = tmp;
  return tag;
}

s_tag * tag_sym (s_tag *tag, const s_sym *sym)
{
  s_tag tmp = {0};
  assert(tag);
  tag_clean(tag);
  tmp.type = TAG_SYM;
  tmp.data.sym = sym;
  *tag = tmp;
  return tag;
}

s_tag * tag_sym_1 (s_tag *tag, const s8 *p)
{
  s_tag tmp = {0};
  assert(tag);
  tag_clean(tag);
  tmp.type = TAG_SYM;
  if (! sym_init_1(&tmp.data.sym, p))
    return NULL;
  *tag = tmp;
  return tag;
}

s_tag * tag_tuple (s_tag *tag, uw count)
{
  s_tag tmp = {0};
  assert(tag);
  tag_clean(tag);
  tmp.type = TAG_TUPLE;
  if (! tuple_init(&tmp.data.tuple, count))
    return NULL;
  *tag = tmp;
  return tag;
}

s_tag * tag_tuple_2 (s_tag *tag, const s_tag *a, const s_tag *b)
{
  s_tag tmp = {0};
  assert(tag);
  tag_clean(tag);
  tmp.type = TAG_TUPLE;
  if (! tuple_init_2(&tmp.data.tuple, a, b))
    return NULL;
  *tag = tmp;
  return tag;
}

s_tag * tag_u8 (s_tag *tag, u8 i)
{
  s_tag tmp = {0};
  assert(tag);
  tag_clean(tag);
  tmp.type = TAG_U8;
  tmp.data.u8 = i;
  *tag = tmp;
  return tag;
}

s_tag * tag_u16 (s_tag *tag, u16 i)
{
  s_tag tmp = {0};
  assert(tag);
  tag_clean(tag);
  tmp.type = TAG_U16;
  tmp.data.u16 = i;
  *tag = tmp;
  return tag;
}

s_tag * tag_u32 (s_tag *tag, u32 i)
{
  s_tag tmp = {0};
  assert(tag);
  tag_clean(tag);
  tmp.type = TAG_U32;
  tmp.data.u32 = i;
  *tag = tmp;
  return tag;
}

s_tag * tag_u64 (s_tag *tag, u64 i)
{
  s_tag tmp = {0};
  assert(tag);
  tag_clean(tag);
  tmp.type = TAG_U64;
  tmp.data.u64 = i;
  *tag = tmp;
  return tag;
}

s_tag * tag_uw (s_tag *tag, uw i)
{
  s_tag tmp = {0};
  assert(tag);
  tag_clean(tag);
  tmp.type = TAG_UW;
  tmp.data.uw = i;
  *tag = tmp;
  return tag;
}

s_tag * tag_var (s_tag *tag)
{
  s_tag tmp = {0};
  assert(tag);
  tag_clean(tag);
  tmp.type = TAG_VAR;
  *tag = tmp;
  return tag;
}

s_tag * tag_void (s_tag *tag)
{
  s_tag tmp = {0};
  assert(tag);
  tag_clean(tag);
  tmp.type = TAG_VOID;
  *tag = tmp;
  return tag;
}
