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
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
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
#include "quote.h"
#include "str.h"
#include "tag.h"
#include "tag_type.h"
#include "time.h"
#include "tuple.h"

s_tag g_tag_first;
s_tag g_tag_last;

s_tag * tag_1 (s_tag *tag, const s8 *p)
{
  tag_clean(tag);
  return tag_init_1(tag, p);
}


bool * tag_and (const s_tag *a, const s_tag *b, bool *dest)
{
  s_tag f;
  assert(a);
  assert(b);
  assert(dest);
  tag_init_bool(&f, false);
  *dest = compare_tag(a, &f) != 0 && compare_tag(b, &f) != 0 ? 1 : 0;
  return dest;
}

s_tag * tag_array (s_tag *tag, const s_sym *type, uw dimension,
                   const uw *dimensions)
{
  assert(tag);
  tag_clean(tag);
  return tag_init_array(tag, type, dimension, dimensions);
}

s_tag * tag_bnot (const s_tag *tag, s_tag *result)
{
  assert(tag);
  assert(result);
  switch (tag->type) {
  case TAG_INTEGER:
    result->type = TAG_INTEGER;
    integer_bnot(&tag->data.integer, &result->data.integer);
    return result;
  case TAG_SW:
    return tag_init_sw(result, ~tag->data.sw);
  case TAG_S64:
    return tag_init_s64(result, ~tag->data.s64);
  case TAG_S32:
    return tag_init_s32(result, ~tag->data.s32);
  case TAG_S16:
    return tag_init_s16(result, ~tag->data.s16);
  case TAG_S8:
    return tag_init_s8(result, ~tag->data.s8);
  case TAG_U8:
    return tag_init_u8(result, ~tag->data.u8);
  case TAG_U16:
    return tag_init_u16(result, ~tag->data.u16);
  case TAG_U32:
    return tag_init_u32(result, ~tag->data.u32);
  case TAG_U64:
    return tag_init_u64(result, ~tag->data.u64);
  case TAG_UW:
    return tag_init_uw(result, ~tag->data.uw);
  default:
    warnx("tag_bnot: invalid tag type: %s",
          tag_type_to_string(tag->type));
  }
  return NULL;
}

s_tag * tag_bool (s_tag *tag, bool b)
{
  assert(tag);
  tag_clean(tag);
  return tag_init_bool(tag, b);
}

s_tag * tag_cast_integer_to_s8 (s_tag *tag)
{
  s8 i;
  assert(tag);
  assert(tag->type == TAG_INTEGER);
  i = integer_to_s8(&tag->data.integer);
  return tag_s8(tag, i);
}

s_tag * tag_cast_integer_to_s16 (s_tag *tag)
{
  s16 i;
  assert(tag);
  assert(tag->type == TAG_INTEGER);
  i = integer_to_s16(&tag->data.integer);
  return tag_s16(tag, i);
}

s_tag * tag_cast_integer_to_s32 (s_tag *tag)
{
  s32 i;
  assert(tag);
  assert(tag->type == TAG_INTEGER);
  i = integer_to_s32(&tag->data.integer);
  return tag_s32(tag, i);
}

s_tag * tag_cast_integer_to_s64 (s_tag *tag)
{
  s64 i;
  assert(tag);
  assert(tag->type == TAG_INTEGER);
  i = integer_to_s64(&tag->data.integer);
  return tag_s64(tag, i);
}

s_tag * tag_cast_integer_to_u8 (s_tag *tag)
{
  u8 i;
  assert(tag);
  assert(tag->type == TAG_INTEGER);
  i = integer_to_u8(&tag->data.integer);
  return tag_u8(tag, i);
}

s_tag * tag_cast_integer_to_u16 (s_tag *tag)
{
  u16 i;
  assert(tag);
  assert(tag->type == TAG_INTEGER);
  i = integer_to_u16(&tag->data.integer);
  return tag_u16(tag, i);
}

s_tag * tag_cast_integer_to_u32 (s_tag *tag)
{
  u32 i;
  assert(tag);
  assert(tag->type == TAG_INTEGER);
  i = integer_to_u32(&tag->data.integer);
  return tag_u32(tag, i);
}

s_tag * tag_cast_integer_to_u64 (s_tag *tag)
{
  u64 i;
  assert(tag);
  assert(tag->type == TAG_INTEGER);
  i = integer_to_u64(&tag->data.integer);
  return tag_u64(tag, i);
}

s_tag * tag_character (s_tag *tag, character x)
{
  assert(tag);
  tag_clean(tag);
  return tag_init_character(tag, x);
}

void tag_clean (s_tag *tag)
{
  assert(tag);
  switch (tag->type) {
  case TAG_ARRAY:   array_clean(&tag->data.array);     break;
  case TAG_CALL:    call_clean(&tag->data.call);       break;
  case TAG_CFN:     cfn_clean(&tag->data.cfn);         break;
  case TAG_FN:      fn_clean(&tag->data.fn);           break;
  case TAG_INTEGER: integer_clean(&tag->data.integer); break;
  case TAG_LIST:    list_delete_all(tag->data.list);   break;
  case TAG_MAP:     map_clean(&tag->data.map);         break;
  case TAG_QUOTE:   quote_clean(&tag->data.quote);     break;
  case TAG_STR:     str_clean(&tag->data.str);         break;
  case TAG_TUPLE:   tuple_clean(&tag->data.tuple);     break;
  case TAG_BOOL:
  case TAG_CHARACTER:
  case TAG_F32:
  case TAG_F64:
  case TAG_FACT:
  case TAG_IDENT:
  case TAG_PTAG:
  case TAG_PTR:
  case TAG_S8:
  case TAG_S16:
  case TAG_S32:
  case TAG_S64:
  case TAG_SW:
  case TAG_SYM:
  case TAG_U8:
  case TAG_U16:
  case TAG_U32:
  case TAG_U64:
  case TAG_UW:
  case TAG_VAR:
  case TAG_VOID:
    break;
  }
}

s_tag * tag_brackets (s_tag *tag, const s_tag *address,
                      s_tag *dest)
{
  assert(tag);
  assert(address);
  assert(dest);
  switch (tag->type) {
  case TAG_ARRAY:
    switch (address->type) {
    case TAG_ARRAY:
      return array_data_tag(tag, address, dest);
    default:
      break;
    }
  default:
    break;
  }
  warnx("tag_brackets: invalid arguments");
  return NULL;
}

s_tag * tag_init_copy (s_tag *tag, const s_tag *src)
{
  assert(tag);
  assert(src);
  switch (src->type) {
  case TAG_VAR:
    tag_init_var(tag);
    break;
  case TAG_VOID:
    break;
  case TAG_ARRAY:
    array_init_copy(&tag->data.array, &src->data.array);
    break;
  case TAG_CALL:
    call_init_copy(&tag->data.call, &src->data.call);
    break;
  case TAG_CFN:
    cfn_init_copy(&tag->data.cfn, &src->data.cfn);
    break;
  case TAG_FN:
    fn_init_copy(&tag->data.fn, &src->data.fn);
    break;
  case TAG_INTEGER:
    integer_init_copy(&tag->data.integer, &src->data.integer);
    break;
  case TAG_LIST:
    list_init_copy(&tag->data.list, (const s_list **) &src->data.list);
    break;
  case TAG_MAP:
    map_init_copy(&tag->data.map, &src->data.map);
    break;
  case TAG_QUOTE:
    quote_init_copy(&tag->data.quote, &src->data.quote);
    break;
  case TAG_STR:
    str_init_copy(&tag->data.str, &src->data.str);
    break;
  case TAG_TUPLE:
    tuple_init_copy(&tag->data.tuple, &src->data.tuple);
    break;
  case TAG_BOOL:
  case TAG_CHARACTER:
  case TAG_F32:
  case TAG_F64:
  case TAG_FACT:
  case TAG_IDENT:
  case TAG_PTAG:
  case TAG_PTR:
  case TAG_S8:
  case TAG_S16:
  case TAG_S32:
  case TAG_S64:
  case TAG_SW:
  case TAG_SYM:
  case TAG_U8:
  case TAG_U16:
  case TAG_U32:
  case TAG_U64:
  case TAG_UW:
    tag->data = src->data;
  }
  tag->type = src->type;
  return tag;
}

void tag_delete (s_tag *tag)
{
  tag_clean(tag);
  free(tag);
}

bool * tag_eq (const s_tag *a, const s_tag *b, bool *dest)
{
  assert(a);
  assert(b);
  assert(dest);
  *dest = compare_tag(a, b) == 0 ? 1 : 0;
  return dest;
}

s_tag * tag_equal (const s_tag *a, const s_tag *b, s_tag *dest)
{
  assert(a);
  assert(b);
  assert(dest);
  if (! env_eval_equal_tag (&g_c3_env, a, b, dest))
    return NULL;
  return dest;
}

s_tag * tag_f32 (s_tag *tag, f32 x)
{
  assert(tag);
  tag_clean(tag);
  return tag_init_f32(tag, x);
}

s_tag * tag_f64 (s_tag *tag, f64 x)
{
  assert(tag);
  tag_clean(tag);
  return tag_init_f64(tag, x);
}

bool * tag_gt (const s_tag *a, const s_tag *b, bool *dest)
{
  assert(a);
  assert(b);
  assert(dest);
  *dest = compare_tag(a, b) > 0 ? 1 : 0;
  return dest;
}

bool * tag_gte (const s_tag *a, const s_tag *b, bool *dest)
{
  assert(a);
  assert(b);
  assert(dest);
  *dest = compare_tag(a, b) >= 0 ? 1 : 0;
  return dest;
}

u64 tag_hash_u64 (const s_tag *tag)
{
  t_hash hash;
  assert(tag);
  hash_init(&hash);
  hash_update_tag(&hash, tag);
  return hash_to_u64(&hash);
}

uw tag_hash_uw (const s_tag *tag)
{
  t_hash hash;
  assert(tag);
  hash_init(&hash);
  hash_update_tag(&hash, tag);
  return hash_to_uw(&hash);
}

s_tag * tag_ident (s_tag *tag, const s_ident *x)
{
  assert(tag);
  tag_clean(tag);
  return tag_init_ident(tag, x);
}

s_tag * tag_ident_1 (s_tag *tag, const s8 *p)
{
  assert(tag);
  tag_clean(tag);
  return tag_init_ident_1(tag, p);
}

bool tag_ident_is_bound (const s_tag *tag)
{
  return env_tag_ident_is_bound(&g_c3_env, tag, &g_c3_env.facts);
}

s_tag * tag_init (s_tag *tag)
{
  bzero(tag, sizeof(s_tag));
  return tag;
}

s_tag * tag_init_1 (s_tag *tag, const s8 *p)
{
  s_buf buf;
  assert(tag);
  tag_init_void(tag);
  if (! p)
    return tag;
  buf_init_1(&buf, p);
  if (buf_parse_tag(&buf, tag) != (sw) strlen(p)) {
    assert(! "invalid tag");
    errx(1, "invalid tag");
    buf_clean(&buf);
    return NULL;
  }
  buf_clean(&buf);
  return tag;
}

s_tag * tag_init_array (s_tag *tag, const s_sym *type,
                        uw dimension, const uw *dimensions)
{
  assert(tag);
  tag->type = TAG_ARRAY;
  array_init(&tag->data.array, type, dimension, dimensions);
  return tag;
}

s_tag * tag_init_bool (s_tag *tag, bool b)
{
  assert(tag);
  bzero(tag, sizeof(s_tag));
  tag->type = TAG_BOOL;
  tag->data.bool = b;
  return tag;
}

s_tag * tag_init_character (s_tag *tag, character c)
{
  assert(tag);
  bzero(tag, sizeof(s_tag));
  tag->type = TAG_CHARACTER;
  tag->data.character = c;
  return tag;
}

s_tag * tag_init_f32 (s_tag *tag, f32 x)
{
  assert(tag);
  bzero(tag, sizeof(s_tag));
  tag->type = TAG_F32;
  tag->data.f32 = x;
  return tag;
}

s_tag * tag_init_f64 (s_tag *tag, f64 x)
{
  assert(tag);
  bzero(tag, sizeof(s_tag));
  tag->type = TAG_F64;
  tag->data.f64 = x;
  return tag;
}

s_tag * tag_init_ident (s_tag *tag, const s_ident *x)
{
  assert(tag);
  bzero(tag, sizeof(s_tag));
  tag->type = TAG_IDENT;
  tag->data.ident = *x;
  return tag;
}

s_tag * tag_init_ident_1 (s_tag *tag, const s8 *p)
{
  assert(tag);
  bzero(tag, sizeof(s_tag));
  tag->type = TAG_IDENT;
  ident_init_1(&tag->data.ident, p);
  return tag;
}
/*
s_tag * tag_init_integer (s_tag *tag, const s_integer *i)
{
  assert(tag);
  bzero(tag, sizeof(s_tag));
  tag->type = TAG_INTEGER;
  integer_init_copy(&tag->data.integer, i);
  return tag;
}
*/

s_tag * tag_init_integer_1 (s_tag *tag, const s8 *p)
{
  s_buf buf;
  assert(tag);
  bzero(tag, sizeof(s_tag));
  tag->type = TAG_INTEGER;
  buf_init_1(&buf, p);
  if (buf_parse_integer(&buf, &tag->data.integer) != (sw) strlen(p)) {
    assert(! "tag_init_integer_1: invalid integer");
    errx(1, "tag_init_integer_1: invalid integer");
    buf_clean(&buf);
    return NULL;
  }
  buf_clean(&buf);
  return tag;
}

s_tag * tag_init_integer_s64 (s_tag *tag, s64 s)
{
  assert(tag);
  bzero(tag, sizeof(s_tag));
  tag->type = TAG_INTEGER;
  integer_init_s64(&tag->data.integer, s);
  return tag;
}

s_tag * tag_init_integer_u64 (s_tag *tag, u64 u)
{
  assert(tag);
  bzero(tag, sizeof(s_tag));
  tag->type = TAG_INTEGER;
  integer_init_u64(&tag->data.integer, u);
  return tag;
}

s_tag * tag_init_integer_zero (s_tag *tag)
{
  assert(tag);
  bzero(tag, sizeof(s_tag));
  tag->type = TAG_INTEGER;
  integer_init_zero(&tag->data.integer);
  return tag;
}

s_tag * tag_init_list (s_tag *tag, s_list *list)
{
  assert(tag);
  bzero(tag, sizeof(s_tag));
  tag->type = TAG_LIST;
  tag->data.list = list;
  return tag;
}

/*
s_tag * tag_init_list_1 (s_tag *tag, const s8 *p)
{
  s_buf buf;
  assert(tag);
  bzero(tag, sizeof(s_tag));
  tag->type = TAG_LIST;
  buf_init_1(&buf, p);
  if (buf_parse_list(&buf, &tag->data.list) != (sw) strlen(p)) {
    assert(! "tag_init_list_1: invalid list");
    errx(1, "tag_init_list_1: invalid list");
    buf_clean(&buf);
    return NULL;
  }
  buf_clean(&buf);
  return tag;
}
*/

s_tag * tag_init_map (s_tag *tag, uw count)
{
  assert(tag);
  tag->type = TAG_MAP;
  if (! map_init(&tag->data.map, count))
    return NULL;
  return tag;
}

s_tag * tag_init_map_1 (s_tag *tag, const s8 *p)
{
  assert(tag);
  tag->type = TAG_MAP;
  if (! map_init_1(&tag->data.map, p))
    return NULL;
  return tag;
}

s_tag * tag_init_s8 (s_tag *tag, s8 i)
{
  assert(tag);
  bzero(tag, sizeof(s_tag));
  tag->type = TAG_S8;
  tag->data.s8 = i;
  return tag;
}

s_tag * tag_init_s16 (s_tag *tag, s16 i)
{
  assert(tag);
  bzero(tag, sizeof(s_tag));
  tag->type = TAG_S16;
  tag->data.s16 = i;
  return tag;
}

s_tag * tag_init_s32 (s_tag *tag, s32 i)
{
  assert(tag);
  bzero(tag, sizeof(s_tag));
  tag->type = TAG_S32;
  tag->data.s32 = i;
  return tag;
}

s_tag * tag_init_s64 (s_tag *tag, s64 i)
{
  assert(tag);
  bzero(tag, sizeof(s_tag));
  tag->type = TAG_S64;
  tag->data.s64 = i;
  return tag;
}

s_tag * tag_init_sw (s_tag *tag, sw i)
{
  assert(tag);
  bzero(tag, sizeof(s_tag));
  tag->type = TAG_SW;
  tag->data.sw = i;
  return tag;
}

s_tag * tag_init_str (s_tag *tag, s8 *free, uw size, const s8 *p)
{
  assert(tag);
  bzero(tag, sizeof(s_tag));
  tag->type = TAG_STR;
  str_init(&tag->data.str, free, size, p);
  return tag;
}

s_tag * tag_init_str_1 (s_tag *tag, s8 *free, const s8 *p)
{
  assert(tag);
  bzero(tag, sizeof(s_tag));
  tag->type = TAG_STR;
  str_init_1(&tag->data.str, free, p);
  return tag;
}

s_tag * tag_init_sym (s_tag *tag, const s_sym *p)
{
  assert(tag);
  assert(p);
  bzero(tag, sizeof(s_tag));
  tag->type = TAG_SYM;
  tag->data.sym = p;
  return tag;
}

s_tag * tag_init_sym_1 (s_tag *tag, const s8 *p)
{
  assert(tag);
  assert(p);
  bzero(tag, sizeof(s_tag));
  tag->type = TAG_SYM;
  tag->data.sym = sym_1(p);
  return tag;
}

s_tag * tag_init_time (s_tag *tag)
{
  s_time time;
  clock_gettime(CLOCK_REALTIME, &time);
  return time_to_tag(&time, tag);
}

s_tag * tag_init_tuple (s_tag *tag, uw count)
{
  assert(tag);
  assert(count);
  tag->type = TAG_TUPLE;
  tuple_init(&tag->data.tuple, count);
  return tag;
}

s_tag * tag_init_tuple_2 (s_tag *tag, const s_tag *a, const s_tag *b)
{
  assert(tag);
  assert(a);
  assert(b);
  tag->type = TAG_TUPLE;
  tuple_init_2(&tag->data.tuple, a, b);
  return tag;
}

s_tag * tag_init_u8 (s_tag *tag, u8 i)
{
  assert(tag);
  bzero(tag, sizeof(s_tag));
  tag->type = TAG_U8;
  tag->data.u8 = i;
  return tag;
}

s_tag * tag_init_u16 (s_tag *tag, u16 i)
{
  assert(tag);
  bzero(tag, sizeof(s_tag));
  tag->type = TAG_U16;
  tag->data.u16 = i;
  return tag;
}

s_tag * tag_init_u32 (s_tag *tag, u32 i)
{
  assert(tag);
  bzero(tag, sizeof(s_tag));
  tag->type = TAG_U32;
  tag->data.u32 = i;
  return tag;
}

s_tag * tag_init_u64 (s_tag *tag, u64 i)
{
  assert(tag);
  bzero(tag, sizeof(s_tag));
  tag->type = TAG_U64;
  tag->data.u64 = i;
  return tag;
}

s_tag * tag_init_uw (s_tag *tag, uw i)
{
  assert(tag);
  bzero(tag, sizeof(s_tag));
  tag->type = TAG_UW;
  tag->data.uw = i;
  return tag;
}

s_tag * tag_init_var (s_tag *tag)
{
  assert(tag);
  bzero(tag, sizeof(s_tag));
  tag->type = TAG_VAR;
  return tag;
}

s_tag * tag_init_void (s_tag *tag)
{
  assert(tag);
  bzero(tag, sizeof(s_tag));
  tag->type = TAG_VOID;
  return tag;
}

s_str * tag_inspect (const s_tag *tag, s_str *dest)
{
  s_buf buf;
  sw size;
  assert(tag);
  assert(dest);
  if ((size = buf_inspect_tag_size(tag)) < 0) {
    assert(! "tag_inspect: size error");
    errx(1, "tag_inspect: size error");
    return NULL;
  }
  buf_init_alloc(&buf, size);
  if (buf_inspect_tag(&buf, tag) != size) {
    assert(! "tag_inspect: inspect error");
    errx(1, "tag_inspect: inspect error");
    return NULL;
  }
  return buf_to_str(&buf, dest);
}

s_tag * tag_integer_1 (s_tag *tag, const s8 *p)
{
  assert(tag);
  tag_clean(tag);
  return tag_init_integer_1(tag, p);
}

s_tag * tag_integer_copy (s_tag *tag, const s_integer *x)
{
  assert(tag);
  tag_clean(tag);
  return tag_init_integer_copy(tag, x);
}

s_tag * tag_integer_reduce (s_tag *tag)
{
  uw bytes;
  e_bool negative;
  assert(tag);
  switch (tag->type) {
  case TAG_INTEGER:
    bytes = integer_bytes(&tag->data.integer);
    if (bytes > 8)
      break;
    negative = integer_is_negative(&tag->data.integer);
    if (bytes > 4) {
      if (negative)
        return tag_cast_integer_to_s64(tag);
      return tag_cast_integer_to_u64(tag);
    }
    if (bytes > 2) {
      if (negative)
        return tag_cast_integer_to_s32(tag);
      return tag_cast_integer_to_u32(tag);
    }
    if (bytes > 1) {
      if (negative)
        return tag_cast_integer_to_s16(tag);
      return tag_cast_integer_to_u16(tag);
    }
    if (negative)
      return tag_cast_integer_to_s8(tag);
    return tag_cast_integer_to_u8(tag);
  default: ;
  }
  return tag;
}

bool tag_is_bound_var (const s_tag *tag)
{
  return (tag &&
          tag->type != TAG_VAR);
}

bool tag_is_number (const s_tag *tag)
{
  assert(tag);
  switch (tag->type) {
  case TAG_INTEGER:
  case TAG_S8:
  case TAG_S16:
  case TAG_S32:
  case TAG_S64:
  case TAG_U8:
  case TAG_U16:
  case TAG_U32:
  case TAG_U64:
    return true;
  default: ;
  }
  return false;
}

bool tag_is_unbound_var (const s_tag *tag)
{
  return (tag && tag->type == TAG_VAR);
}

s_tag * tag_list (s_tag *tag, s_list *x)
{
  assert(tag);
  tag_clean(tag);
  return tag_init_list(tag, x);
}

/*
s_tag * tag_list_1 (s_tag *tag, const s8 *p)
{
  assert(tag);
  tag_clean(tag);
  return tag_init_list_1(tag, p);
}
*/

bool * tag_lt (const s_tag *a, const s_tag *b, bool *dest)
{
  assert(a);
  assert(b);
  assert(dest);
  *dest = compare_tag(a, b) < 0 ? 1 : 0;
  return dest;
}

bool * tag_lte (const s_tag *a, const s_tag *b, bool *dest)
{
  assert(a);
  assert(b);
  assert(dest);
  *dest = compare_tag(a, b) <= 0 ? 1 : 0;
  return dest;
}
s_tag * tag_map (s_tag *tag, uw count)
{
  assert(tag);
  tag_clean(tag);
  return tag_init_map(tag, count);
}

/*
s_tag * tag_map_1 (s_tag *tag, const s8 *p)
{
  assert(tag);
  assert(p);
  tag_clean(tag);
  return tag_init_map_1(tag, p);
}
*/

s_tag * tag_neg (const s_tag *tag, s_tag *result)
{
  s_integer tmp;
  switch (tag->type) {
  case TAG_BOOL:
    return tag_init_s8(result, -(tag->data.bool ? 1 : 0));
  case TAG_CHARACTER:
    return tag_init_s64(result, -tag->data.character);
  case TAG_INTEGER:
    result->type = TAG_INTEGER;
    integer_neg(&tag->data.integer, &result->data.integer);
    return result;
  case TAG_SW:
    if (tag->data.sw == SW_MIN) {
      integer_init_sw(&tmp, tag->data.sw);
      result->type = TAG_INTEGER;
      integer_neg(&tmp, &result->data.integer);
      integer_clean(&tmp);
      return result;
    }
    return tag_init_sw(result, -tag->data.sw);
  case TAG_S64:
    if (tag->data.s64 == S64_MIN) {
      integer_init_s64(&tmp, tag->data.s64);
      result->type = TAG_INTEGER;
      integer_neg(&tmp, &result->data.integer);
      integer_clean(&tmp);
      return result;
    }
    return tag_init_s64(result, -tag->data.s64);
  case TAG_S32:
    return tag_init_s64(result, - (s64) tag->data.s32);
  case TAG_S16:
    return tag_init_s32(result, - (s32) tag->data.s16);
  case TAG_S8:
    return tag_init_s16(result, - (s16) tag->data.s8);
  case TAG_U8:
    return tag_init_s16(result, - (s16) tag->data.u8);
  case TAG_U16:
    return tag_init_s32(result, - (s32) tag->data.u16);
  case TAG_U32:
    return tag_init_s64(result, - (s64) tag->data.u32);
  case TAG_U64:
    integer_init_u64(&tmp, tag->data.u64);
    result->type = TAG_INTEGER;
    integer_neg(&tmp, &result->data.integer);
    integer_clean(&tmp);
    return result;
  case TAG_UW:
    integer_init_uw(&tmp, tag->data.uw);
    result->type = TAG_INTEGER;
    integer_neg(&tmp, &result->data.integer);
    integer_clean(&tmp);
    return result;
  default:
    warnx("tag_neg: invalid tag type: %s",
          tag_type_to_string(tag->type));
  }
  return NULL;
}

s_tag * tag_new (void)
{
  s_tag *tag;
  tag = calloc(1, sizeof(s_tag));
  return tag;
}

s_tag * tag_new_1 (const s8 *p)
{
  s_tag *tag;
  tag = calloc(1, sizeof(s_tag));
  return tag_init_1(tag, p);
}

s_tag * tag_new_array (const s_sym *type, uw dimension,
                       const uw *dimensions)
{
  s_tag *dest;
  if (! (dest = malloc(sizeof(s_tag)))) {
    warnx("tag_new_array: out of memory");
    return NULL;
  }
  if (! tag_init_array(dest, type, dimension, dimensions)) {
    free(dest);
    return NULL;
  }
  return dest;
}

s_tag * tag_new_copy (const s_tag *src)
{
  s_tag *dest;
  if (! (dest = malloc(sizeof(s_tag))))
    errx(1, "tag_new_copy: out of memory");
  return tag_init_copy(dest, src);
}

s_tag * tag_new_var (void)
{
  s_tag *tag;
  tag = calloc(1, sizeof(s_tag));
  return tag_init_var(tag);
}

bool * tag_not (const s_tag *tag, bool *dest)
{
  s_tag f;
  assert(tag);
  assert(dest);
  tag_init_bool(&f, false);
  *dest = compare_tag(tag, &f) == 0 ? 1 : 0;
  return dest;
}

bool * tag_not_eq (const s_tag *a, const s_tag *b, bool *dest)
{
  assert(a);
  assert(b);
  assert(dest);
  *dest = compare_tag(a, b) != 0 ? 1 : 0;
  return dest;
}

bool * tag_or (const s_tag *a, const s_tag *b, bool *dest)
{
  s_tag f;
  assert(a);
  assert(b);
  assert(dest);
  tag_init_bool(&f, false);
  *dest = compare_tag(a, &f) != 0 || compare_tag(b, &f) != 0 ? 1 : 0;
  return dest;
}

s_tag * tag_paren (const s_tag *tag, s_tag *dest)
{
  assert(tag);
  assert(dest);
  return tag_init_copy(dest, tag);
}

s_tag * tag_s8 (s_tag *tag, s8 x)
{
  assert(tag);
  tag_clean(tag);
  return tag_init_s8(tag, x);
}

s_tag * tag_s16 (s_tag *tag, s16 x)
{
  assert(tag);
  tag_clean(tag);
  return tag_init_s16(tag, x);
}

s_tag * tag_s32 (s_tag *tag, s32 x)
{
  assert(tag);
  tag_clean(tag);
  return tag_init_s32(tag, x);
}

s_tag * tag_s64 (s_tag *tag, s64 x)
{
  assert(tag);
  tag_clean(tag);
  return tag_init_s64(tag, x);
}

s_tag * tag_sw (s_tag *tag, sw x)
{
  assert(tag);
  tag_clean(tag);
  return tag_init_sw(tag, x);
}

s_tag * tag_str (s_tag *tag, s8 *free, uw size, const s8 *p)
{
  assert(tag);
  tag_clean(tag);
  return tag_init_str(tag, free, size, p);
}

s_tag * tag_str_1 (s_tag *tag, s8 *free, const s8 *p)
{
  assert(tag);
  tag_clean(tag);
  return tag_init_str_1(tag, free, p);
}

s_tag * tag_sym (s_tag *tag, const s_sym *x)
{
  assert(tag);
  tag_clean(tag);
  return tag_init_sym(tag, x);
}

s_tag * tag_sym_1 (s_tag *tag, const s8 *p)
{
  assert(tag);
  tag_clean(tag);
  return tag_init_sym_1(tag, p);
}

void * tag_to_ffi_pointer (s_tag *tag, const s_sym *type)
{
  if (type == sym_1("Tag") ||
      type == sym_1("tag"))
    return tag;
  switch (tag->type) {
  case TAG_VOID:
    if (type == sym_1("Void") ||
        type == sym_1("void"))
      return NULL;
    goto invalid_type;
  case TAG_ARRAY:
    if (type == sym_1("Array") ||
        type == sym_1("array"))
      return tag->data.array.data;
    goto invalid_type;
  case TAG_BOOL:
    if (type == sym_1("Bool") ||
        type == sym_1("bool"))
      return &tag->data.bool;
    goto invalid_type;
  case TAG_CALL:
    if (type == sym_1("Call") ||
        type == sym_1("call"))
      return &tag->data.call;
    goto invalid_type;
  case TAG_CFN:
    if (type == sym_1("Cfn") ||
        type == sym_1("cfn"))
      return &tag->data.cfn;
    goto invalid_type;
  case TAG_CHARACTER:
    if (type == sym_1("Character") ||
        type == sym_1("character"))
      return &tag->data.character;
    goto invalid_type;
  case TAG_F32:
    if (type == sym_1("F32") ||
        type == sym_1("f32"))
      return &tag->data.f32;
    goto invalid_type;
  case TAG_F64:
    if (type == sym_1("f64") ||
        type == sym_1("F64"))
      return &tag->data.f64;
    goto invalid_type;
  case TAG_FACT:
    if (type == sym_1("Fact") ||
        type == sym_1("fact"))
      return &tag->data.fact;
    goto invalid_type;
  case TAG_FN:
    if (type == sym_1("Fn") ||
        type == sym_1("fn"))
      return &tag->data.fn;
    goto invalid_type;
  case TAG_IDENT:
    if (type == sym_1("Ident") ||
        type == sym_1("ident"))
      return &tag->data.ident;
    goto invalid_type;
  case TAG_INTEGER:
    if (type == sym_1("Integer") ||
        type == sym_1("integer"))
      return &tag->data.integer;
    goto invalid_type;
  case TAG_SW:
    if (type == sym_1("Sw") ||
        type == sym_1("sw"))
      return &tag->data.sw;
    goto invalid_type;
  case TAG_S64:
    if (type == sym_1("S64") ||
        type == sym_1("s64"))
      return &tag->data.s64;
    goto invalid_type;
  case TAG_S32:
    if (type == sym_1("S32") ||
        type == sym_1("s32"))
      return &tag->data.s32;
    goto invalid_type;
  case TAG_S16:
    if (type == sym_1("S16") ||
        type == sym_1("s16"))
      return &tag->data.s16;
    goto invalid_type;
  case TAG_S8:
    if (type == sym_1("S8") ||
        type == sym_1("s8"))
      return &tag->data.s8;
    goto invalid_type;
  case TAG_U8:
    if (type == sym_1("U8") ||
        type == sym_1("u8"))
      return &tag->data.u8;
    goto invalid_type;
  case TAG_U16:
    if (type == sym_1("U16") ||
        type == sym_1("u16"))
      return &tag->data.u16;
    goto invalid_type;
  case TAG_U32:
    if (type == sym_1("U32") ||
        type == sym_1("u32"))
      return &tag->data.u32;
    goto invalid_type;
  case TAG_U64:
    if (type == sym_1("U64") ||
        type == sym_1("u64"))
      return &tag->data.u64;
    goto invalid_type;
  case TAG_UW:
    if (type == sym_1("Uw") ||
        type == sym_1("uw"))
      return &tag->data.uw;
    goto invalid_type;
  case TAG_LIST:
    if (type == sym_1("List") ||
        type == sym_1("list"))
      return &tag->data.list;
    goto invalid_type;
  case TAG_MAP:
    if (type == sym_1("Map") ||
        type == sym_1("map"))
      return &tag->data.map;
    goto invalid_type;
  case TAG_PTAG:
    if (type == sym_1("Ptag") ||
        type == sym_1("ptag"))
      return (void *) tag->data.ptag;
    goto invalid_type;
  case TAG_PTR:
    if (type->str.ptr.ps8[type->str.size - 2] == '*')
      return &tag->data.ptr.p;
    goto invalid_type;
  case TAG_QUOTE:
    if (type == sym_1("Quote") ||
        type == sym_1("quote"))
      return &tag->data.quote;
    goto invalid_type;
  case TAG_STR:
    if (type == sym_1("Str") ||
        type == sym_1("str"))
      return &tag->data.str;
    if (type == sym_1("Char*") ||
        type == sym_1("char*"))
      return (void *) tag->data.str.ptr.ps8;
    goto invalid_type;
  case TAG_SYM:
    if (type == sym_1("Sym") ||
        type == sym_1("sym"))
      return (void *) &tag->data.sym;
    if (type == sym_1("Str") ||
        type == sym_1("str"))
      return (void *) &tag->data.sym->str;
    if (type == sym_1("Char*") ||
        type == sym_1("char*"))
      return (void *) tag->data.sym->str.ptr.ps8;
    goto invalid_type;
  case TAG_TUPLE:
    if (type == sym_1("Tuple") ||
        type == sym_1("tuple"))
      return &tag->data.tuple;
    goto invalid_type;
  case TAG_VAR:
    goto invalid_type;
  }
  assert(! "tag_to_ffi_pointer: invalid tag type");
  errx(1, "tag_to_ffi_pointer: invalid tag type");
  return NULL;
 invalid_type:
  warnx("tag_to_ffi_pointer: cannot cast %s to %s",
        tag_type_to_string(tag->type),
        type->str.ptr.ps8);
  return NULL;
}

void * tag_to_pointer (s_tag *tag, const s_sym *type)
{
  e_tag_type tag_type;
  sym_to_tag_type(type, &tag_type);
  if (tag->type != tag_type) {
    assert(! "tag_to_pointer: cannot cast");
    errx(1, "tag_to_pointer: cannot cast %s to %s",
         tag_type_to_string(tag->type),
         type->str.ptr.ps8);
    return NULL;
  }
  switch (tag->type) {
  case TAG_VOID:
    return NULL;
  case TAG_ARRAY:
    return tag->data.array.data;
  case TAG_BOOL:
    return &tag->data.bool;
  case TAG_CALL:
    return &tag->data.call;
  case TAG_CFN:
    return &tag->data.cfn;
  case TAG_CHARACTER:
    return &tag->data.character;
  case TAG_F32:
    return &tag->data.f32;
  case TAG_F64:
    return &tag->data.f64;
  case TAG_FACT:
    return &tag->data.fact;
  case TAG_FN:
    return &tag->data.fn;
  case TAG_IDENT:
    return &tag->data.ident;
  case TAG_INTEGER:
    return &tag->data.integer;
  case TAG_SW:
    return &tag->data.sw;
  case TAG_S64:
    return &tag->data.s64;
  case TAG_S32:
    return &tag->data.s32;
  case TAG_S16:
    return &tag->data.s16;
  case TAG_S8:
    return &tag->data.s8;
  case TAG_U8:
    return &tag->data.u8;
  case TAG_U16:
    return &tag->data.u16;
  case TAG_U32:
    return &tag->data.u32;
  case TAG_U64:
    return &tag->data.u64;
  case TAG_UW:
    return &tag->data.uw;
  case TAG_LIST:
    return &tag->data.list;
  case TAG_MAP:
    return &tag->data.map;
  case TAG_PTAG:
    return &tag->data.ptag;
  case TAG_PTR:
    return &tag->data.ptr.p;
  case TAG_QUOTE:
    return &tag->data.quote;
  case TAG_STR:
    return &tag->data.str;
  case TAG_SYM:
    return (void *) tag->data.sym;
  case TAG_TUPLE:
    return &tag->data.tuple;
  case TAG_VAR:
    goto invalid_type;
  }
  assert(! "tag_to_pointer: invalid tag type");
  errx(1, "tag_to_pointer: invalid tag type");
  return NULL;
 invalid_type:
  warnx("tag_to_pointer: cannot cast %s to %s",
        tag_type_to_string(tag->type),
        type->str.ptr.ps8);
  return NULL;

}

s_tag * tag_tuple (s_tag *tag, uw count)
{
  assert(tag);
  tag_clean(tag);
  return tag_init_tuple(tag, count);
}

s_tag * tag_tuple_2 (s_tag *tag, const s_tag *a, const s_tag *b)
{
  assert(tag);
  tag_clean(tag);
  return tag_init_tuple_2(tag, a, b);
}

const s_sym ** tag_type (const s_tag *tag, const s_sym **dest)
{
  assert(tag);
  assert(dest);
  *dest = tag_type_to_sym(tag->type);
  return dest;
}

s_tag * tag_u8 (s_tag *tag, u8 x)
{
  assert(tag);
  tag_clean(tag);
  return tag_init_u8(tag, x);
}

s_tag * tag_u16 (s_tag *tag, u16 x)
{
  assert(tag);
  tag_clean(tag);
  return tag_init_u16(tag, x);
}

s_tag * tag_u32 (s_tag *tag, u32 x)
{
  assert(tag);
  tag_clean(tag);
  return tag_init_u32(tag, x);
}

s_tag * tag_u64 (s_tag *tag, u64 x)
{
  assert(tag);
  tag_clean(tag);
  return tag_init_u64(tag, x);
}

s_tag * tag_uw (s_tag *tag, uw x)
{
  assert(tag);
  tag_clean(tag);
  return tag_init_uw(tag, x);
}

s_tag * tag_var (s_tag *tag)
{
  assert(tag);
  tag_clean(tag);
  return tag_init_var(tag);
}

s_tag * tag_void (s_tag *tag)
{
  assert(tag);
  tag_clean(tag);
  return tag_init_void(tag);
}

bool tag_xor (const s_tag *a, const s_tag *b)
{
  s_tag f;
  tag_init_1(&f, "false");
  return (compare_tag(a, &f) != 0) != (compare_tag(b, &f) != 0);
}
