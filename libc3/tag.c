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
#include "ptr.h"
#include "ptr_free.h"
#include "quote.h"
#include "str.h"
#include "struct.h"
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

void tag_clean (s_tag *tag)
{
  assert(tag);
  switch (tag->type) {
  case TAG_ARRAY:    array_clean(&tag->data.array);     break;
  case TAG_CALL:     call_clean(&tag->data.call);       break;
  case TAG_CFN:      cfn_clean(&tag->data.cfn);         break;
  case TAG_FN:       fn_clean(&tag->data.fn);           break;
  case TAG_INTEGER:  integer_clean(&tag->data.integer); break;
  case TAG_LIST:     list_delete_all(tag->data.list);   break;
  case TAG_MAP:      map_clean(&tag->data.map);         break;
  case TAG_PTR_FREE: ptr_free_clean(&tag->data.ptr);    break;
  case TAG_QUOTE:    quote_clean(&tag->data.quote);     break;
  case TAG_STR:      str_clean(&tag->data.str);         break;
  case TAG_STRUCT:   struct_clean(&tag->data.struct_);  break;
  case TAG_TUPLE:    tuple_clean(&tag->data.tuple);     break;
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

s_tag * tag_brackets (const s_tag *tag, const s_tag *address,
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
  case TAG_STRUCT:
    struct_init_copy(&tag->data.struct_, &src->data.struct_);
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
  case TAG_PTR_FREE:
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

bool tag_ident_is_bound (const s_tag *tag)
{
  return env_tag_ident_is_bound(&g_c3_env, tag, &g_c3_env.facts);
}

s_tag * tag_init (s_tag *tag)
{
  *tag = (s_tag) {0};
  return tag;
}

s_tag * tag_init_1 (s_tag *tag, const s8 *p)
{
  s_buf buf;
  uw len;
  sw r;
  assert(tag);
  tag_init_void(tag);
  if (! p)
    return tag;
  buf_init_1(&buf, false, (s8 *) p);
  len = strlen(p);
  r = buf_parse_tag(&buf, tag);
  if (r < 0 || (uw) r != len) {
    warnx("invalid tag: \"%s\", %lu != %ld", p, len, r);
    assert(! "invalid tag");
    return NULL;
  }
  return tag;
}

s_tag * tag_init_time (s_tag *tag)
{
  s_time time;
  clock_gettime(CLOCK_REALTIME, &time);
  return time_to_tag(&time, tag);
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

s_tag * tag_list_1 (s_tag *tag, const s8 *p)
{
  s_tag tmp = {0};
  assert(tag);
  tag_clean(tag);
  tmp.type = TAG_LIST;
  tmp.data.list = list_new_1(p);
  *tag = tmp;
  return tag;
}

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

s_tag * tag_new_copy (const s_tag *src)
{
  s_tag *dest;
  if (! (dest = malloc(sizeof(s_tag))))
    errx(1, "tag_new_copy: out of memory");
  return tag_init_copy(dest, src);
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

uw * tag_size (const s_tag *tag, uw *dest)
{
  const s_sym *type = NULL;
  uw tmp = 0;
  assert(tag);
  type = tag_type_to_sym(tag->type);
  if (! type ||
      ! sym_type_size(type, &tmp))
    return NULL;
  *dest = tmp;
  return dest;
}

bool tag_to_const_pointer (const s_tag *tag, const s_sym *type,
                           const void **dest)
{
  e_tag_type tag_type;
  if (! sym_to_tag_type(type, &tag_type))
    return false;
  if (tag->type != tag_type) {
    warnx("tag_to_const_pointer: cannot cast %s to %s",
          tag_type_to_string(tag->type),
          type->str.ptr.ps8);
    assert(! "tag_to_const_pointer: cannot cast");
    return false;
  }
  switch (tag_type) {
  case TAG_ARRAY:     *dest = tag->data.array.data;  return true;
  case TAG_BOOL:      *dest = &tag->data.bool;       return true;
  case TAG_CALL:      *dest = &tag->data.call;       return true;
  case TAG_CFN:       *dest = &tag->data.cfn;        return true;
  case TAG_CHARACTER: *dest = &tag->data.character;  return true;
  case TAG_F32:       *dest = &tag->data.f32;        return true;
  case TAG_F64:       *dest = &tag->data.f64;        return true;
  case TAG_FACT:      *dest = &tag->data.fact;       return true;
  case TAG_FN:        *dest = &tag->data.fn;         return true;
  case TAG_IDENT:     *dest = &tag->data.ident;      return true;
  case TAG_INTEGER:   *dest = &tag->data.integer;    return true;
  case TAG_SW:        *dest = &tag->data.sw;         return true;
  case TAG_S64:       *dest = &tag->data.s64;        return true;
  case TAG_S32:       *dest = &tag->data.s32;        return true;
  case TAG_S16:       *dest = &tag->data.s16;        return true;
  case TAG_S8:        *dest = &tag->data.s8;         return true;
  case TAG_U8:        *dest = &tag->data.u8;         return true;
  case TAG_U16:       *dest = &tag->data.u16;        return true;
  case TAG_U32:       *dest = &tag->data.u32;        return true;
  case TAG_U64:       *dest = &tag->data.u64;        return true;
  case TAG_UW:        *dest = &tag->data.uw;         return true;
  case TAG_LIST:      *dest = &tag->data.list;       return true;
  case TAG_MAP:       *dest = &tag->data.map;        return true;
  case TAG_PTAG:      *dest = &tag->data.ptag;       return true;
  case TAG_PTR:       *dest = &tag->data.ptr.p;      return true;
  case TAG_PTR_FREE:  *dest = &tag->data.ptr_free.p; return true;
  case TAG_QUOTE:     *dest = &tag->data.quote;      return true;
  case TAG_STR:       *dest = &tag->data.str;        return true;
  case TAG_STRUCT:    *dest = &tag->data.struct_;    return true;
  case TAG_SYM:       *dest = &tag->data.sym;        return true;
  case TAG_TUPLE:     *dest = &tag->data.tuple;      return true;
  case TAG_VAR:       *dest = NULL;                  return true;
  case TAG_VOID:      *dest = NULL;                  return true;
  }
  warnx("tag_to_const_pointer: invalid tag type: %s",
        tag_type_to_string(tag_type));
  assert(! "tag_to_const_pointer: invalid tag type");
  return false;
}

bool tag_to_ffi_pointer (s_tag *tag, const s_sym *type, void **dest)
{
  if (type == sym_1("Tag")) {
    *dest = tag;
    return true;
  }
  switch (tag->type) {
  case TAG_VOID:
    if (type == sym_1("Void")) {
      *dest = NULL;
      return true;
    }
    goto invalid_cast;
  case TAG_ARRAY:
    if (type == sym_1("Array")) {
      *dest = tag->data.array.data;
      return true;
    }
    goto invalid_cast;
  case TAG_BOOL:
    if (type == sym_1("Bool")) {
      *dest = &tag->data.bool;
      return true;
    }
    goto invalid_cast;
  case TAG_CALL:
    if (type == sym_1("Call")) {
      *dest = &tag->data.call;
      return true;
    }
    goto invalid_cast;
  case TAG_CFN:
    if (type == sym_1("Cfn")) {
      *dest = &tag->data.cfn;
      return true;
    }
    goto invalid_cast;
  case TAG_CHARACTER:
    if (type == sym_1("Character")) {
      *dest = &tag->data.character;
      return true;
    }
    goto invalid_cast;
  case TAG_F32:
    if (type == sym_1("F32")) {
      *dest = &tag->data.f32;
      return true;
    }
    goto invalid_cast;
  case TAG_F64:
    if (type == sym_1("F64")) {
      *dest = &tag->data.f64;
      return true;
    }
    goto invalid_cast;
  case TAG_FACT:
    if (type == sym_1("Fact")) {
      *dest = &tag->data.fact;
      return true;
    }
    goto invalid_cast;
  case TAG_FN:
    if (type == sym_1("Fn")) {
      *dest = &tag->data.fn;
      return true;
    }
    goto invalid_cast;
  case TAG_IDENT:
    if (type == sym_1("Ident")) {
      *dest = &tag->data.ident;
      return true;
    }
    goto invalid_cast;
  case TAG_INTEGER:
    if (type == sym_1("Integer")) {
      *dest = &tag->data.integer;
      return true;
    }
    goto invalid_cast;
  case TAG_SW:
    if (type == sym_1("Sw")) {
      *dest = &tag->data.sw;
      return true;
    }
    goto invalid_cast;
  case TAG_S64:
    if (type == sym_1("S64")) {
      *dest = &tag->data.s64;
      return true;
    }
    goto invalid_cast;
  case TAG_S32:
    if (type == sym_1("S32")) {
      *dest = &tag->data.s32;
      return true;
    }
    goto invalid_cast;
  case TAG_S16:
    if (type == sym_1("S16")) {
      *dest = &tag->data.s16;
      return true;
    }
    goto invalid_cast;
  case TAG_S8:
    if (type == sym_1("S8")) {
      *dest = &tag->data.s8;
      return true;
    }
    goto invalid_cast;
  case TAG_U8:
    if (type == sym_1("U8")) {
      *dest = &tag->data.u8;
      return true;
    }
    goto invalid_cast;
  case TAG_U16:
    if (type == sym_1("U16")) {
      *dest = &tag->data.u16;
      return true;
    }
    goto invalid_cast;
  case TAG_U32:
    if (type == sym_1("U32")) {
      *dest = &tag->data.u32;
      return true;
    }
    goto invalid_cast;
  case TAG_U64:
    if (type == sym_1("U64")) {
      *dest = &tag->data.u64;
      return true;
    }
    goto invalid_cast;
  case TAG_UW:
    if (type == sym_1("Uw")) {
      *dest = &tag->data.uw;
      return true;
    }
    goto invalid_cast;
  case TAG_LIST:
    if (type == sym_1("List")) {
      *dest = &tag->data.list;
      return true;
    }
    goto invalid_cast;
  case TAG_MAP:
    if (type == sym_1("Map")) {
      *dest = &tag->data.map;
      return true;
    }
    goto invalid_cast;
  case TAG_PTAG:
    if (type == sym_1("Ptag")) {
      *dest = (void *) tag->data.ptag;
      return true;
    }
    goto invalid_cast;
  case TAG_PTR:
    if (type == sym_1("Ptr")) {
      *dest = &tag->data.ptr.p;
      return true;
    }
    goto invalid_cast;
  case TAG_PTR_FREE:
    if (type == sym_1("Ptr") ||
        type == sym_1("PtrFree")) {
      *dest = &tag->data.ptr_free.p;
      return true;
    }
    goto invalid_cast;
  case TAG_QUOTE:
    if (type == sym_1("Quote")) {
      *dest = &tag->data.quote;
      return true;
    }
    goto invalid_cast;
  case TAG_STR:
    if (type == sym_1("Str")) {
      *dest = &tag->data.str;
      return true;
    }
    if (type == sym_1("Char*")) {
      *dest = (void *) tag->data.str.ptr.ps8;
      return true;
    }
    goto invalid_cast;
  case TAG_STRUCT:
    if (type == sym_1("Struct")) {
      *dest = &tag->data.struct_;
      return true;
    }
    if (type == tag->data.struct_.type.module) {
      *dest = tag->data.struct_.data;
      return true;
    }
    goto invalid_cast;
  case TAG_SYM:
    if (type == sym_1("Sym")) {
      *dest = (void *) &tag->data.sym;
      return true;
    }
    if (type == sym_1("Str")) {
      *dest = (void *) &tag->data.sym->str;
      return true;
    }
    if (type == sym_1("Char*")) {
      *dest = (void *) tag->data.sym->str.ptr.ps8;
      return true;
    }
    goto invalid_cast;
  case TAG_TUPLE:
    if (type == sym_1("Tuple")) {
      *dest = &tag->data.tuple;
      return true;
    }
    goto invalid_cast;
  case TAG_VAR:
    goto invalid_cast;
  }
    warnx("tag_to_ffi_pointer: invalid tag type: %d", tag->type);
  assert(! "tag_to_ffi_pointer: invalid tag type");
  return false;
 invalid_cast:
  warnx("tag_to_ffi_pointer: cannot cast %s to %s",
        tag_type_to_string(tag->type),
        type->str.ptr.ps8);
  return false;
}

bool tag_to_pointer (s_tag *tag, const s_sym *type, void **dest)
{
  e_tag_type tag_type;
  if (! sym_to_tag_type(type, &tag_type))
    return false;
  if (tag->type != tag_type) {
    warnx("tag_to_pointer: cannot cast %s to %s",
          tag_type_to_string(tag->type),
          type->str.ptr.ps8);
    assert(! "tag_to_pointer: cannot cast");
    return false;
  }
  switch (tag_type) {
  case TAG_VOID:      *dest = NULL;                  return true;
  case TAG_ARRAY:     *dest = tag->data.array.data;  return true;
  case TAG_BOOL:      *dest = &tag->data.bool;       return true;
  case TAG_CALL:      *dest = &tag->data.call;       return true;
  case TAG_CFN:       *dest = &tag->data.cfn;        return true;
  case TAG_CHARACTER: *dest = &tag->data.character;  return true;
  case TAG_F32:       *dest = &tag->data.f32;        return true;
  case TAG_F64:       *dest = &tag->data.f64;        return true;
  case TAG_FACT:      *dest = &tag->data.fact;       return true;
  case TAG_FN:        *dest = &tag->data.fn;         return true;
  case TAG_IDENT:     *dest = &tag->data.ident;      return true;
  case TAG_INTEGER:   *dest = &tag->data.integer;    return true;
  case TAG_SW:        *dest = &tag->data.sw;         return true;
  case TAG_S64:       *dest = &tag->data.s64;        return true;
  case TAG_S32:       *dest = &tag->data.s32;        return true;
  case TAG_S16:       *dest = &tag->data.s16;        return true;
  case TAG_S8:        *dest = &tag->data.s8;         return true;
  case TAG_U8:        *dest = &tag->data.u8;         return true;
  case TAG_U16:       *dest = &tag->data.u16;        return true;
  case TAG_U32:       *dest = &tag->data.u32;        return true;
  case TAG_U64:       *dest = &tag->data.u64;        return true;
  case TAG_UW:        *dest = &tag->data.uw;         return true;
  case TAG_LIST:      *dest = &tag->data.list;       return true;
  case TAG_MAP:       *dest = &tag->data.map;        return true;
  case TAG_PTAG:      *dest = &tag->data.ptag;       return true;
  case TAG_PTR:       *dest = &tag->data.ptr.p;      return true;
  case TAG_PTR_FREE:  *dest = &tag->data.ptr_free.p; return true;
  case TAG_QUOTE:     *dest = &tag->data.quote;      return true;
  case TAG_STR:       *dest = &tag->data.str;        return true;
  case TAG_STRUCT:    *dest = &tag->data.struct_;    return true;
  case TAG_SYM:       *dest = &tag->data.sym;        return true;
  case TAG_TUPLE:     *dest = &tag->data.tuple;      return true;
  case TAG_VAR:       *dest = NULL;                  return true;
  }
  warnx("tag_to_pointer: invalid tag type: %d", tag_type);
  assert(! "tag_to_pointer: invalid tag type");
  return false;
}

const s_sym ** tag_type (const s_tag *tag, const s_sym **dest)
{
  assert(tag);
  assert(dest);
  switch (tag->type) {
  case TAG_STRUCT:
    *dest = tag->data.struct_.type.module;
    return dest;
  default:
    break;
  }
  *dest = tag_type_to_sym(tag->type);
  return dest;
}

bool tag_xor (const s_tag *a, const s_tag *b)
{
  s_tag f;
  tag_init_1(&f, "false");
  return (compare_tag(a, &f) != 0) != (compare_tag(b, &f) != 0);
}
