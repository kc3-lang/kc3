/* c3
 * Copyright 2022,2023 kmx.io <contact@kmx.io>
 *
 * Permission is hereby granted to use this software excepted
 * on Apple computers granted the above copyright notice and
 * this permission paragraph are included in all copies and
 * substantial portions of this software.
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
#include <strings.h>
#include "c3.h"
#include "../libffi/ffi.h"

s_tag g_tag_first;
s_tag g_tag_last;

s_tag * tag_1 (s_tag *tag, const s8 *p)
{
  tag_clean(tag);
  return tag_init_1(tag, p);
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
  assert(tag->type.type == TAG_INTEGER);
  i = integer_to_s8(&tag->data.integer);
  return tag_s8(tag, i);
}

s_tag * tag_cast_integer_to_s16 (s_tag *tag)
{
  s16 i;
  assert(tag);
  assert(tag->type.type == TAG_INTEGER);
  i = integer_to_s16(&tag->data.integer);
  return tag_s16(tag, i);
}

s_tag * tag_cast_integer_to_s32 (s_tag *tag)
{
  s32 i;
  assert(tag);
  assert(tag->type.type == TAG_INTEGER);
  i = integer_to_s32(&tag->data.integer);
  return tag_s32(tag, i);
}

s_tag * tag_cast_integer_to_s64 (s_tag *tag)
{
  s64 i;
  assert(tag);
  assert(tag->type.type == TAG_INTEGER);
  i = integer_to_s64(&tag->data.integer);
  return tag_s64(tag, i);
}

s_tag * tag_cast_integer_to_u8 (s_tag *tag)
{
  u8 i;
  assert(tag);
  assert(tag->type.type == TAG_INTEGER);
  i = integer_to_u8(&tag->data.integer);
  return tag_u8(tag, i);
}

s_tag * tag_cast_integer_to_u16 (s_tag *tag)
{
  u16 i;
  assert(tag);
  assert(tag->type.type == TAG_INTEGER);
  i = integer_to_u16(&tag->data.integer);
  return tag_u16(tag, i);
}

s_tag * tag_cast_integer_to_u32 (s_tag *tag)
{
  u32 i;
  assert(tag);
  assert(tag->type.type == TAG_INTEGER);
  i = integer_to_u32(&tag->data.integer);
  return tag_u32(tag, i);
}

s_tag * tag_cast_integer_to_u64 (s_tag *tag)
{
  u64 i;
  assert(tag);
  assert(tag->type.type == TAG_INTEGER);
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
  switch (tag->type.type) {
  case TAG_CALL:
  case TAG_CALL_FN:
  case TAG_CALL_MACRO: call_clean(&tag->data.call);       break;
  case TAG_FN:         fn_clean(&tag->data.fn);           break;
  case TAG_INTEGER:    integer_clean(&tag->data.integer); break;
  case TAG_LIST:       list_delete_all(tag->data.list);   break;
  case TAG_QUOTE:      quote_clean(&tag->data.quote);     break;
  case TAG_STR:        str_clean(&tag->data.str);         break;
  case TAG_TUPLE:      tuple_clean(&tag->data.tuple);     break;
  case TAG_BOOL:
  case TAG_CHARACTER:
  case TAG_F32:
  case TAG_F64:
  case TAG_IDENT:
  case TAG_PTAG:
  case TAG_S8:
  case TAG_S16:
  case TAG_S32:
  case TAG_S64:
  case TAG_SYM:
  case TAG_U8:
  case TAG_U16:
  case TAG_U32:
  case TAG_U64:
  case TAG_VAR:
  case TAG_VOID:
    break;
  }
}

s_tag * tag_copy (const s_tag *src, s_tag *dest)
{
  assert(src);
  assert(dest);
  switch (src->type.type) {
  case TAG_VAR:
    error("tag_copy: TAG_VAR");
  case TAG_VOID:
    break;
  case TAG_CALL:
  case TAG_CALL_FN:
  case TAG_CALL_MACRO:
    call_copy(&src->data.call, &dest->data.call);
    break;
  case TAG_FN:
    fn_copy(&src->data.fn, &dest->data.fn);
    break;
  case TAG_INTEGER:
    integer_init(&dest->data.integer);
    integer_copy(&src->data.integer, &dest->data.integer);
    break;
  case TAG_LIST:
    list_copy(src->data.list, &dest->data.list);
    break;
  case TAG_QUOTE:
    quote_copy(&src->data.quote, &dest->data.quote);
    break;
  case TAG_STR:
    str_copy(&src->data.str, &dest->data.str);
    break;
  case TAG_TUPLE:
    tuple_copy(&src->data.tuple, &dest->data.tuple);
    break;
  case TAG_BOOL:
  case TAG_CHARACTER:
  case TAG_F32:
  case TAG_F64:
  case TAG_IDENT:
  case TAG_PTAG:
  case TAG_S8:
  case TAG_S16:
  case TAG_S32:
  case TAG_S64:
  case TAG_SYM:
  case TAG_U8:
  case TAG_U16:
  case TAG_U32:
  case TAG_U64:
    dest->data = src->data;
  }
  dest->type.type = src->type.type;
  return dest;
}

void tag_delete (s_tag *tag)
{
  tag_clean(tag);
  free(tag);
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

s_tag * tag_init_bool (s_tag *tag, bool b)
{
  assert(tag);
  bzero(tag, sizeof(s_tag));
  tag->type.type = TAG_BOOL;
  tag->data.bool = b;
  return tag;
}

s_tag * tag_init_character (s_tag *tag, character c)
{
  assert(tag);
  bzero(tag, sizeof(s_tag));
  tag->type.type = TAG_CHARACTER;
  tag->data.character = c;
  return tag;
}

s_tag * tag_init_f32 (s_tag *tag, f32 x)
{
  assert(tag);
  bzero(tag, sizeof(s_tag));
  tag->type.type = TAG_F32;
  tag->data.f32 = x;
  return tag;
}

s_tag * tag_init_f64 (s_tag *tag, f64 x)
{
  assert(tag);
  bzero(tag, sizeof(s_tag));
  tag->type.type = TAG_F64;
  tag->data.f64 = x;
  return tag;
}

s_tag * tag_init_ident (s_tag *tag, const s_ident *x)
{
  assert(tag);
  bzero(tag, sizeof(s_tag));
  tag->type.type = TAG_IDENT;
  tag->data.ident = *x;
  return tag;
}

s_tag * tag_init_ident_1 (s_tag *tag, const s8 *p)
{
  assert(tag);
  bzero(tag, sizeof(s_tag));
  tag->type.type = TAG_IDENT;
  ident_init_1(&tag->data.ident, p);
  return tag;
}

s_tag * tag_init_integer (s_tag *tag, const s_integer *i)
{
  assert(tag);
  bzero(tag, sizeof(s_tag));
  tag->type.type = TAG_INTEGER;
  integer_init(&tag->data.integer);
  integer_copy(i, &tag->data.integer);
  return tag;
}

s_tag * tag_init_integer_1 (s_tag *tag, const s8 *p)
{
  s_buf buf;
  assert(tag);
  bzero(tag, sizeof(s_tag));
  tag->type.type = TAG_INTEGER;
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

s_tag * tag_init_list (s_tag *tag, s_list *list)
{
  assert(tag);
  bzero(tag, sizeof(s_tag));
  tag->type.type = TAG_LIST;
  tag->data.list = list;
  return tag;
}

s_tag * tag_init_list_1 (s_tag *tag, const s8 *p)
{
  s_buf buf;
  assert(tag);
  bzero(tag, sizeof(s_tag));
  tag->type.type = TAG_LIST;
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

s_tag * tag_init_s8 (s_tag *tag, s8 i)
{
  assert(tag);
  bzero(tag, sizeof(s_tag));
  tag->type.type = TAG_S8;
  tag->data.s8 = i;
  return tag;
}

s_tag * tag_init_s16 (s_tag *tag, s16 i)
{
  assert(tag);
  bzero(tag, sizeof(s_tag));
  tag->type.type = TAG_S16;
  tag->data.s16 = i;
  return tag;
}

s_tag * tag_init_s32 (s_tag *tag, s32 i)
{
  assert(tag);
  bzero(tag, sizeof(s_tag));
  tag->type.type = TAG_S32;
  tag->data.s32 = i;
  return tag;
}

s_tag * tag_init_s64 (s_tag *tag, s64 i)
{
  assert(tag);
  bzero(tag, sizeof(s_tag));
  tag->type.type = TAG_S64;
  tag->data.s64 = i;
  return tag;
}

s_tag * tag_init_str (s_tag *tag, s8 *free, uw size, const s8 *p)
{
  assert(tag);
  bzero(tag, sizeof(s_tag));
  tag->type.type = TAG_STR;
  str_init(&tag->data.str, free, size, p);
  return tag;
}

s_tag * tag_init_str_1 (s_tag *tag, s8 *free, const s8 *p)
{
  assert(tag);
  bzero(tag, sizeof(s_tag));
  tag->type.type = TAG_STR;
  str_init_1(&tag->data.str, free, p);
  return tag;
}

s_tag * tag_init_sym (s_tag *tag, const s_sym *p)
{
  assert(tag);
  assert(p);
  bzero(tag, sizeof(s_tag));
  tag->type.type = TAG_SYM;
  tag->data.sym = p;
  return tag;
}

s_tag * tag_init_sym_1 (s_tag *tag, const s8 *p)
{
  s_buf buf;
  assert(tag);
  assert(p);
  bzero(tag, sizeof(s_tag));
  tag->type.type = TAG_SYM;
  buf_init_1(&buf, p);
  if (buf_parse_sym(&buf, &tag->data.sym) != (sw) strlen(p)) {
    assert(! "tag_init_sym_1: invalid symbol");
    errx(1, "tag_init_sym_1: invalid symbol");
    buf_clean(&buf);
    return NULL;
  }
  buf_clean(&buf);
  return tag;
}

s_tag * tag_init_u8 (s_tag *tag, u8 i)
{
  assert(tag);
  bzero(tag, sizeof(s_tag));
  tag->type.type = TAG_U8;
  tag->data.u8 = i;
  return tag;
}

s_tag * tag_init_u16 (s_tag *tag, u16 i)
{
  assert(tag);
  bzero(tag, sizeof(s_tag));
  tag->type.type = TAG_U16;
  tag->data.u16 = i;
  return tag;
}

s_tag * tag_init_u32 (s_tag *tag, u32 i)
{
  assert(tag);
  bzero(tag, sizeof(s_tag));
  tag->type.type = TAG_U32;
  tag->data.u32 = i;
  return tag;
}

s_tag * tag_init_u64 (s_tag *tag, u64 i)
{
  assert(tag);
  bzero(tag, sizeof(s_tag));
  tag->type.type = TAG_U64;
  tag->data.u64 = i;
  return tag;
}

s_tag * tag_init_var (s_tag *tag)
{
  assert(tag);
  bzero(tag, sizeof(s_tag));
  tag->type.type = TAG_VAR;
  return tag;
}

s_tag * tag_init_void (s_tag *tag)
{
  assert(tag);
  bzero(tag, sizeof(s_tag));
  tag->type.type = TAG_VOID;
  return tag;
}

s_str * tag_inspect (const s_tag *tag, s_str *dest)
{
  s_buf buf;
  sw size;
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

s_tag * tag_integer (s_tag *tag, const s_integer *x)
{
  assert(tag);
  tag_clean(tag);
  return tag_init_integer(tag, x);
}

s_tag * tag_integer_1 (s_tag *tag, const s8 *p)
{
  assert(tag);
  tag_clean(tag);
  return tag_init_integer_1(tag, p);
}

s_tag * tag_integer_reduce (s_tag *tag)
{
  uw bytes;
  e_bool negative;
  assert(tag);
  switch (tag->type.type) {
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

e_bool tag_is_bound_var (const s_tag *tag)
{
  return (tag &&
          tag->type.type != TAG_VAR);
}

e_bool tag_is_number (const s_tag *tag)
{
  assert(tag);
  switch (tag->type.type) {
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

e_bool tag_is_unbound_var (const s_tag *tag)
{
  return (tag &&
          tag->type.type == TAG_VAR);
}

s_tag * tag_list (s_tag *tag, s_list *x)
{
  assert(tag);
  tag_clean(tag);
  return tag_init_list(tag, x);
}

s_tag * tag_list_1 (s_tag *tag, const s8 *p)
{
  assert(tag);
  tag_clean(tag);
  return tag_init_list_1(tag, p);
}

s_tag * tag_new ()
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
  return tag_copy(src, dest);
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

ffi_type tag_to_ffi_type(const s_tag *tag)
{
  switch (tag->type.type) {
    case TAG_S8:
      return ffi_type_sint8;
    case TAG_S16:
      return ffi_type_sint16;
    case TAG_S32:
      return ffi_type_sint32;
    case TAG_S64:
      return ffi_type_sint64;
    case TAG_U8:
      return ffi_type_uint8;
    case TAG_U16:
      return ffi_type_uint16;
    case TAG_U32:
      return ffi_type_uint32;
    case TAG_U64:
      return ffi_type_uint64;
    case TAG_F32:
      return ffi_type_float;
    case TAG_F64:
      return ffi_type_double;
    case TAG_VOID:
      return ffi_type_void;
    case TAG_INTEGER:
      return ffi_type_sint;
    case TAG_STR:
      return ffi_type_pointer;
    case TAG_CHARACTER:
      return ffi_type_schar;
    default: ;
  }
  assert(! "tag_to_ffi_type: unknown type");
  errx(1, "tag_to_ffi_type: unknown type");
  return ffi_type_void;
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
