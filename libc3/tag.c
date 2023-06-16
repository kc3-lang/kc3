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
#include <strings.h>
#include "c3.h"

s_tag g_tag_first;
s_tag g_tag_last;

s_tag * tag_1 (s_tag *tag, const s8 *p)
{
  tag_clean(tag);
  return tag_init_1(tag, p);
}

s_tag * tag_add (const s_tag *a, const s_tag *b, s_tag *dest)
{
  assert(a);
  assert(b);
  assert(dest);
  switch (a->type) {
  case TAG_F32:
    switch (b->type) {
    case TAG_F32:
      return tag_init_f32(dest, a->data.f32 + b->data.f32);
    case TAG_F64:
      return tag_init_f64(dest, a->data.f32 + b->data.f64);
    case TAG_S8:
      return tag_init_f32(dest, a->data.f32 + b->data.s8);
    case TAG_S16:
      return tag_init_f32(dest, a->data.f32 + b->data.s16);
    case TAG_S32:
      return tag_init_f32(dest, a->data.f32 + b->data.s32);
    case TAG_S64:
      return tag_init_f32(dest, a->data.f32 + b->data.s64);
    case TAG_U8:
      return tag_init_f32(dest, a->data.f32 + b->data.u8);
    case TAG_U16:
      return tag_init_f32(dest, a->data.f32 + b->data.u16);
    case TAG_U32:
      return tag_init_f32(dest, a->data.f32 + b->data.u32);
    case TAG_U64:
      return tag_init_f32(dest, a->data.f32 + b->data.u64);
    default:
      goto ko;
  }
  case TAG_F64:
    switch (b->type) {
    case TAG_F32:
      return tag_init_f64(dest, a->data.f64 + b->data.f32);
    case TAG_F64:
      return tag_init_f64(dest, a->data.f64 + b->data.f64);
    case TAG_S8:
      return tag_init_f64(dest, a->data.f64 + b->data.s8);
    case TAG_S16:
      return tag_init_f64(dest, a->data.f64 + b->data.s16);
    case TAG_S32:
      return tag_init_f64(dest, a->data.f64 + b->data.s32);
    case TAG_S64:
      return tag_init_f64(dest, a->data.f64 + b->data.s64);
    case TAG_U8:
      return tag_init_f64(dest, a->data.f64 + b->data.u8);
    case TAG_U16:
      return tag_init_f64(dest, a->data.f64 + b->data.u16);
    case TAG_U32:
      return tag_init_f64(dest, a->data.f64 + b->data.u32);
    case TAG_U64:
      return tag_init_f64(dest, a->data.f64 + b->data.u64);
    default:
      goto ko;
  }
  case TAG_S8:
    switch (b->type) {
    case TAG_F32:
      return tag_init_f32(dest, a->data.s8 + b->data.f32);
    case TAG_F64:
      return tag_init_f64(dest, a->data.s8 + b->data.f64);
    case TAG_S8:
      return tag_init_s8(dest, a->data.s8 + b->data.s8);
    case TAG_S16:
      return tag_init_s16(dest, a->data.s8 + b->data.s16);
    case TAG_S32:
      return tag_init_s32(dest, a->data.s8 + b->data.s32);
    case TAG_S64:
      return tag_init_s64(dest, a->data.s8 + b->data.s64);
    case TAG_U8:
      return tag_init_s8(dest, a->data.s8 + b->data.u8);
    case TAG_U16:
      return tag_init_s16(dest, a->data.s8 + b->data.u16);
    case TAG_U32:
      return tag_init_s32(dest, a->data.s8 + b->data.u32);
    case TAG_U64:
      return tag_init_s64(dest, a->data.s8 + b->data.u64);
    default:
      goto ko;
  }
  case TAG_S16:
    switch (b->type) {
    case TAG_F32:
      return tag_init_f32(dest, a->data.s16 + b->data.f32);
    case TAG_F64:
      return tag_init_f64(dest, a->data.s16 + b->data.f64);
    case TAG_S8:
      return tag_init_s16(dest, a->data.s16 + b->data.s8);
    case TAG_S16:
      return tag_init_s16(dest, a->data.s16 + b->data.s16);
    case TAG_S32:
      return tag_init_s32(dest, a->data.s16 + b->data.s32);
    case TAG_S64:
      return tag_init_s64(dest, a->data.s16 + b->data.s64);
    case TAG_U8:
      return tag_init_s16(dest, a->data.s16 + b->data.u8);
    case TAG_U16:
      return tag_init_s16(dest, a->data.s16 + b->data.u16);
    case TAG_U32:
      return tag_init_s32(dest, a->data.s16 + b->data.u32);
    case TAG_U64:
      return tag_init_s64(dest, a->data.s16 + b->data.u64);
    default:
      goto ko;
  }
  case TAG_S32:
    switch (b->type) {
    case TAG_F32:
      return tag_init_f32(dest, a->data.s32 + b->data.f32);
    case TAG_F64:
      return tag_init_f64(dest, a->data.s32 + b->data.f64);
    case TAG_S8:
      return tag_init_s32(dest, a->data.s32 + b->data.s8);
    case TAG_S16:
      return tag_init_s32(dest, a->data.s32 + b->data.s16);
    case TAG_S32:
      return tag_init_s32(dest, a->data.s32 + b->data.s32);
    case TAG_S64:
      return tag_init_s64(dest, a->data.s32 + b->data.s64);
    case TAG_U8:
      return tag_init_s32(dest, a->data.s32 + b->data.u8);
    case TAG_U16:
      return tag_init_s32(dest, a->data.s32 + b->data.u16);
    case TAG_U32:
      return tag_init_s32(dest, a->data.s32 + b->data.u32);
    case TAG_U64:
      return tag_init_s64(dest, a->data.s32 + b->data.u64);
    default:
      goto ko;
  }
  case TAG_S64:
    switch (b->type) {
    case TAG_F32:
      return tag_init_f32(dest, a->data.s64 + b->data.f32);
    case TAG_F64:
      return tag_init_f64(dest, a->data.s64 + b->data.f64);
    case TAG_S8:
      return tag_init_s32(dest, a->data.s64 + b->data.s8);
    case TAG_S16:
      return tag_init_s32(dest, a->data.s64 + b->data.s16);
    case TAG_S32:
      return tag_init_s32(dest, a->data.s64 + b->data.s32);
    case TAG_S64:
      return tag_init_s64(dest, a->data.s64 + b->data.s64);
    case TAG_U8:
      return tag_init_s32(dest, a->data.s64 + b->data.u8);
    case TAG_U16:
      return tag_init_s32(dest, a->data.s64 + b->data.u16);
    case TAG_U32:
      return tag_init_s32(dest, a->data.s64 + b->data.u32);
    case TAG_U64:
      return tag_init_s64(dest, a->data.s64 + b->data.u64);
    default:
      goto ko;
  }
  case TAG_U8:
    switch (b->type) {
    case TAG_F32:
      return tag_init_f32(dest, a->data.u8 + b->data.f32);
    case TAG_F64:
      return tag_init_f64(dest, a->data.u8 + b->data.f64);
    case TAG_S8:
      return tag_init_s8(dest, a->data.u8 + b->data.s8);
    case TAG_S16:
      return tag_init_s16(dest, a->data.u8 + b->data.s16);
    case TAG_S32:
      return tag_init_s32(dest, a->data.u8 + b->data.s32);
    case TAG_S64:
      return tag_init_s64(dest, a->data.u8 + b->data.s64);
    case TAG_U8:
      return tag_init_u8(dest, a->data.u8 + b->data.u8);
    case TAG_U16:
      return tag_init_u16(dest, a->data.u8 + b->data.u16);
    case TAG_U32:
      return tag_init_u32(dest, a->data.u8 + b->data.u32);
    case TAG_U64:
      return tag_init_u64(dest, a->data.u8 + b->data.u64);
    default:
      goto ko;
  }
  case TAG_U16:
    switch (b->type) {
    case TAG_F32:
      return tag_init_f32(dest, a->data.u16 + b->data.f32);
    case TAG_F64:
      return tag_init_f64(dest, a->data.u16 + b->data.f64);
    case TAG_S8:
      return tag_init_s16(dest, a->data.u16 + b->data.s8);
    case TAG_S16:
      return tag_init_s16(dest, a->data.u16 + b->data.s16);
    case TAG_S32:
      return tag_init_s32(dest, a->data.u16 + b->data.s32);
    case TAG_S64:
      return tag_init_s64(dest, a->data.u16 + b->data.s64);
    case TAG_U8:
      return tag_init_u16(dest, a->data.u16 + b->data.u8);
    case TAG_U16:
      return tag_init_u16(dest, a->data.u16 + b->data.u16);
    case TAG_U32:
      return tag_init_u32(dest, a->data.u16 + b->data.u32);
    case TAG_U64:
      return tag_init_u64(dest, a->data.u16 + b->data.u64);
    default:
      goto ko;
  }
  case TAG_U32:
    switch (b->type) {
    case TAG_F32:
      return tag_init_f32(dest, a->data.u32 + b->data.f32);
    case TAG_F64:
      return tag_init_f64(dest, a->data.u32 + b->data.f64);
    case TAG_S8:
      return tag_init_s32(dest, a->data.u32 + b->data.s8);
    case TAG_S16:
      return tag_init_s32(dest, a->data.u32 + b->data.s16);
    case TAG_S32:
      return tag_init_s32(dest, a->data.u32 + b->data.s32);
    case TAG_S64:
      return tag_init_s64(dest, a->data.u32 + b->data.s64);
    case TAG_U8:
      return tag_init_u32(dest, a->data.u32 + b->data.u8);
    case TAG_U16:
      return tag_init_u32(dest, a->data.u32 + b->data.u16);
    case TAG_U32:
      return tag_init_u32(dest, a->data.u32 + b->data.u32);
    case TAG_U64:
      return tag_init_u64(dest, a->data.u32 + b->data.u64);
    default:
      goto ko;
  }
  case TAG_U64:
    switch (b->type) {
    case TAG_F32:
      return tag_init_f32(dest, a->data.u64 + b->data.f32);
    case TAG_F64:
      return tag_init_f64(dest, a->data.u64 + b->data.f64);
    case TAG_S8:
      return tag_init_s64(dest, a->data.u64 + b->data.s8);
    case TAG_S16:
      return tag_init_s64(dest, a->data.u64 + b->data.s16);
    case TAG_S32:
      return tag_init_s64(dest, a->data.u64 + b->data.s32);
    case TAG_S64:
      return tag_init_s64(dest, a->data.u64 + b->data.s64);
    case TAG_U8:
      return tag_init_u64(dest, a->data.u64 + b->data.u8);
    case TAG_U16:
      return tag_init_u64(dest, a->data.u64 + b->data.u16);
    case TAG_U32:
      return tag_init_u64(dest, a->data.u64 + b->data.u32);
    case TAG_U64:
      return tag_init_u64(dest, a->data.u64 + b->data.u64);
    default:
      goto ko;
    }
  default:
    goto ko;
  }
 ko:
  errx(1, "cannot add %s to %s",
       tag_type_to_string(a->type),
       tag_type_to_string(b->type));
}

s_tag * tag_array (s_tag *tag, const s_array *a)
{
  assert(tag);
  assert(a);
  tag_clean(tag);
  return tag_init_array(tag, a);
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
  case TAG_ARRAY:      array_clean(&tag->data.array);     break;
  case TAG_CALL:
  case TAG_CALL_FN:
  case TAG_CALL_MACRO: call_clean(&tag->data.call);       break;
  case TAG_CFN:        cfn_clean(&tag->data.cfn);         break;
  case TAG_FN:         fn_delete_all(tag->data.fn);       break;
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
  switch (src->type) {
  case TAG_VAR:
    tag_init_var(dest);
    break;
  case TAG_VOID:
    break;
  case TAG_ARRAY:
    array_copy(&src->data.array, &dest->data.array);
    break;
  case TAG_CALL:
  case TAG_CALL_FN:
  case TAG_CALL_MACRO:
    call_copy(&src->data.call, &dest->data.call);
    break;
  case TAG_CFN:
    cfn_copy(&src->data.cfn, &dest->data.cfn);
    break;
  case TAG_FN:
    fn_copy(src->data.fn, &dest->data.fn);
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
  dest->type = src->type;
  return dest;
}

void tag_delete (s_tag *tag)
{
  tag_clean(tag);
  free(tag);
}

s_tag * tag_div (const s_tag *a, const s_tag *b, s_tag *dest)
{
  assert(a);
  assert(b);
  assert(dest);
  switch (a->type) {
  case TAG_F32:
    switch (b->type) {
    case TAG_F32:
      return tag_init_f32(dest, a->data.f32 / b->data.f32);
    case TAG_F64:
      return tag_init_f64(dest, a->data.f32 / b->data.f64);
    case TAG_S8:
      return tag_init_f32(dest, a->data.f32 / b->data.s8);
    case TAG_S16:
      return tag_init_f32(dest, a->data.f32 / b->data.s16);
    case TAG_S32:
      return tag_init_f32(dest, a->data.f32 / b->data.s32);
    case TAG_S64:
      return tag_init_f32(dest, a->data.f32 / b->data.s64);
    case TAG_U8:
      return tag_init_f32(dest, a->data.f32 / b->data.u8);
    case TAG_U16:
      return tag_init_f32(dest, a->data.f32 / b->data.u16);
    case TAG_U32:
      return tag_init_f32(dest, a->data.f32 / b->data.u32);
    case TAG_U64:
      return tag_init_f32(dest, a->data.f32 / b->data.u64);
    default:
      goto ko;
  }
  case TAG_F64:
    switch (b->type) {
    case TAG_F32:
      return tag_init_f64(dest, a->data.f64 / b->data.f32);
    case TAG_F64:
      return tag_init_f64(dest, a->data.f64 / b->data.f64);
    case TAG_S8:
      return tag_init_f64(dest, a->data.f64 / b->data.s8);
    case TAG_S16:
      return tag_init_f64(dest, a->data.f64 / b->data.s16);
    case TAG_S32:
      return tag_init_f64(dest, a->data.f64 / b->data.s32);
    case TAG_S64:
      return tag_init_f64(dest, a->data.f64 / b->data.s64);
    case TAG_U8:
      return tag_init_f64(dest, a->data.f64 / b->data.u8);
    case TAG_U16:
      return tag_init_f64(dest, a->data.f64 / b->data.u16);
    case TAG_U32:
      return tag_init_f64(dest, a->data.f64 / b->data.u32);
    case TAG_U64:
      return tag_init_f64(dest, a->data.f64 / b->data.u64);
    default:
      goto ko;
  }
  case TAG_S8:
    switch (b->type) {
    case TAG_F32:
      return tag_init_f32(dest, a->data.s8 / b->data.f32);
    case TAG_F64:
      return tag_init_f64(dest, a->data.s8 / b->data.f64);
    case TAG_S8:
      return tag_init_s8(dest, a->data.s8 / b->data.s8);
    case TAG_S16:
      return tag_init_s16(dest, a->data.s8 / b->data.s16);
    case TAG_S32:
      return tag_init_s32(dest, a->data.s8 / b->data.s32);
    case TAG_S64:
      return tag_init_s64(dest, a->data.s8 / b->data.s64);
    case TAG_U8:
      return tag_init_s8(dest, a->data.s8 / b->data.u8);
    case TAG_U16:
      return tag_init_s16(dest, a->data.s8 / b->data.u16);
    case TAG_U32:
      return tag_init_s32(dest, a->data.s8 / b->data.u32);
    case TAG_U64:
      return tag_init_s64(dest, a->data.s8 / b->data.u64);
    default:
      goto ko;
  }
  case TAG_S16:
    switch (b->type) {
    case TAG_F32:
      return tag_init_f32(dest, a->data.s16 / b->data.f32);
    case TAG_F64:
      return tag_init_f64(dest, a->data.s16 / b->data.f64);
    case TAG_S8:
      return tag_init_s16(dest, a->data.s16 / b->data.s8);
    case TAG_S16:
      return tag_init_s16(dest, a->data.s16 / b->data.s16);
    case TAG_S32:
      return tag_init_s32(dest, a->data.s16 / b->data.s32);
    case TAG_S64:
      return tag_init_s64(dest, a->data.s16 / b->data.s64);
    case TAG_U8:
      return tag_init_s16(dest, a->data.s16 / b->data.u8);
    case TAG_U16:
      return tag_init_s16(dest, a->data.s16 / b->data.u16);
    case TAG_U32:
      return tag_init_s32(dest, a->data.s16 / b->data.u32);
    case TAG_U64:
      return tag_init_s64(dest, a->data.s16 / b->data.u64);
    default:
      goto ko;
  }
  case TAG_S32:
    switch (b->type) {
    case TAG_F32:
      return tag_init_f32(dest, a->data.s32 / b->data.f32);
    case TAG_F64:
      return tag_init_f64(dest, a->data.s32 / b->data.f64);
    case TAG_S8:
      return tag_init_s32(dest, a->data.s32 / b->data.s8);
    case TAG_S16:
      return tag_init_s32(dest, a->data.s32 / b->data.s16);
    case TAG_S32:
      return tag_init_s32(dest, a->data.s32 / b->data.s32);
    case TAG_S64:
      return tag_init_s64(dest, a->data.s32 / b->data.s64);
    case TAG_U8:
      return tag_init_s32(dest, a->data.s32 / b->data.u8);
    case TAG_U16:
      return tag_init_s32(dest, a->data.s32 / b->data.u16);
    case TAG_U32:
      return tag_init_s32(dest, a->data.s32 / b->data.u32);
    case TAG_U64:
      return tag_init_s64(dest, a->data.s32 / b->data.u64);
    default:
      goto ko;
  }
  case TAG_S64:
    switch (b->type) {
    case TAG_F32:
      return tag_init_f32(dest, a->data.s64 / b->data.f32);
    case TAG_F64:
      return tag_init_f64(dest, a->data.s64 / b->data.f64);
    case TAG_S8:
      return tag_init_s32(dest, a->data.s64 / b->data.s8);
    case TAG_S16:
      return tag_init_s32(dest, a->data.s64 / b->data.s16);
    case TAG_S32:
      return tag_init_s32(dest, a->data.s64 / b->data.s32);
    case TAG_S64:
      return tag_init_s64(dest, a->data.s64 / b->data.s64);
    case TAG_U8:
      return tag_init_s32(dest, a->data.s64 / b->data.u8);
    case TAG_U16:
      return tag_init_s32(dest, a->data.s64 / b->data.u16);
    case TAG_U32:
      return tag_init_s32(dest, a->data.s64 / b->data.u32);
    case TAG_U64:
      return tag_init_s64(dest, a->data.s64 / b->data.u64);
    default:
      goto ko;
  }
  case TAG_U8:
    switch (b->type) {
    case TAG_F32:
      return tag_init_f32(dest, a->data.u8 / b->data.f32);
    case TAG_F64:
      return tag_init_f64(dest, a->data.u8 / b->data.f64);
    case TAG_S8:
      return tag_init_s8(dest, a->data.u8 / b->data.s8);
    case TAG_S16:
      return tag_init_s16(dest, a->data.u8 / b->data.s16);
    case TAG_S32:
      return tag_init_s32(dest, a->data.u8 / b->data.s32);
    case TAG_S64:
      return tag_init_s64(dest, a->data.u8 / b->data.s64);
    case TAG_U8:
      return tag_init_u8(dest, a->data.u8 / b->data.u8);
    case TAG_U16:
      return tag_init_u16(dest, a->data.u8 / b->data.u16);
    case TAG_U32:
      return tag_init_u32(dest, a->data.u8 / b->data.u32);
    case TAG_U64:
      return tag_init_u64(dest, a->data.u8 / b->data.u64);
    default:
      goto ko;
  }
  case TAG_U16:
    switch (b->type) {
    case TAG_F32:
      return tag_init_f32(dest, a->data.u16 / b->data.f32);
    case TAG_F64:
      return tag_init_f64(dest, a->data.u16 / b->data.f64);
    case TAG_S8:
      return tag_init_s16(dest, a->data.u16 / b->data.s8);
    case TAG_S16:
      return tag_init_s16(dest, a->data.u16 / b->data.s16);
    case TAG_S32:
      return tag_init_s32(dest, a->data.u16 / b->data.s32);
    case TAG_S64:
      return tag_init_s64(dest, a->data.u16 / b->data.s64);
    case TAG_U8:
      return tag_init_u16(dest, a->data.u16 / b->data.u8);
    case TAG_U16:
      return tag_init_u16(dest, a->data.u16 / b->data.u16);
    case TAG_U32:
      return tag_init_u32(dest, a->data.u16 / b->data.u32);
    case TAG_U64:
      return tag_init_u64(dest, a->data.u16 / b->data.u64);
    default:
      goto ko;
  }
  case TAG_U32:
    switch (b->type) {
    case TAG_F32:
      return tag_init_f32(dest, a->data.u32 / b->data.f32);
    case TAG_F64:
      return tag_init_f64(dest, a->data.u32 / b->data.f64);
    case TAG_S8:
      return tag_init_s32(dest, a->data.u32 / b->data.s8);
    case TAG_S16:
      return tag_init_s32(dest, a->data.u32 / b->data.s16);
    case TAG_S32:
      return tag_init_s32(dest, a->data.u32 / b->data.s32);
    case TAG_S64:
      return tag_init_s64(dest, a->data.u32 / b->data.s64);
    case TAG_U8:
      return tag_init_u32(dest, a->data.u32 / b->data.u8);
    case TAG_U16:
      return tag_init_u32(dest, a->data.u32 / b->data.u16);
    case TAG_U32:
      return tag_init_u32(dest, a->data.u32 / b->data.u32);
    case TAG_U64:
      return tag_init_u64(dest, a->data.u32 / b->data.u64);
    default:
      goto ko;
  }
  case TAG_U64:
    switch (b->type) {
    case TAG_F32:
      return tag_init_f32(dest, a->data.u64 / b->data.f32);
    case TAG_F64:
      return tag_init_f64(dest, a->data.u64 / b->data.f64);
    case TAG_S8:
      return tag_init_s64(dest, a->data.u64 / b->data.s8);
    case TAG_S16:
      return tag_init_s64(dest, a->data.u64 / b->data.s16);
    case TAG_S32:
      return tag_init_s64(dest, a->data.u64 / b->data.s32);
    case TAG_S64:
      return tag_init_s64(dest, a->data.u64 / b->data.s64);
    case TAG_U8:
      return tag_init_u64(dest, a->data.u64 / b->data.u8);
    case TAG_U16:
      return tag_init_u64(dest, a->data.u64 / b->data.u16);
    case TAG_U32:
      return tag_init_u64(dest, a->data.u64 / b->data.u32);
    case TAG_U64:
      return tag_init_u64(dest, a->data.u64 / b->data.u64);
    default:
      goto ko;
    }
  default:
    goto ko;
  }
 ko:
  errx(1, "cannot divide %s by %s",
       tag_type_to_string(a->type),
       tag_type_to_string(b->type));
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

s_tag * tag_init_array (s_tag *tag, const s_array *a)
{
  assert(tag);
  assert(a);
  tag->type = TAG_ARRAY;
  array_copy(a, &tag->data.array);
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

s_tag * tag_init_call (s_tag *tag, const s_call *call)
{
  assert(tag);
  assert(call);
  bzero(tag, sizeof(s_tag));
  tag->type = TAG_CALL;
  tag->data.call = *call;
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

s_tag * tag_init_integer (s_tag *tag, const s_integer *i)
{
  assert(tag);
  bzero(tag, sizeof(s_tag));
  tag->type = TAG_INTEGER;
  integer_init(&tag->data.integer);
  integer_copy(i, &tag->data.integer);
  return tag;
}

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

s_tag * tag_init_list (s_tag *tag, s_list *list)
{
  assert(tag);
  bzero(tag, sizeof(s_tag));
  tag->type = TAG_LIST;
  tag->data.list = list;
  return tag;
}

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
  s_buf buf;
  assert(tag);
  assert(p);
  bzero(tag, sizeof(s_tag));
  tag->type = TAG_SYM;
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

e_bool tag_is_bound_var (const s_tag *tag)
{
  return (tag &&
          tag->type != TAG_VAR);
}

e_bool tag_is_number (const s_tag *tag)
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

e_bool tag_is_unbound_var (const s_tag *tag)
{
  return (tag &&
          tag->type == TAG_VAR);
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

s_tag * tag_mul (const s_tag *a, const s_tag *b, s_tag *dest)
{
  assert(a);
  assert(b);
  assert(dest);
  switch (a->type) {
  case TAG_F32:
    switch (b->type) {
    case TAG_F32:
      return tag_init_f32(dest, a->data.f32 * b->data.f32);
    case TAG_F64:
      return tag_init_f64(dest, a->data.f32 * b->data.f64);
    case TAG_S8:
      return tag_init_f32(dest, a->data.f32 * b->data.s8);
    case TAG_S16:
      return tag_init_f32(dest, a->data.f32 * b->data.s16);
    case TAG_S32:
      return tag_init_f32(dest, a->data.f32 * b->data.s32);
    case TAG_S64:
      return tag_init_f32(dest, a->data.f32 * b->data.s64);
    case TAG_U8:
      return tag_init_f32(dest, a->data.f32 * b->data.u8);
    case TAG_U16:
      return tag_init_f32(dest, a->data.f32 * b->data.u16);
    case TAG_U32:
      return tag_init_f32(dest, a->data.f32 * b->data.u32);
    case TAG_U64:
      return tag_init_f32(dest, a->data.f32 * b->data.u64);
    default:
      goto ko;
  }
  case TAG_F64:
    switch (b->type) {
    case TAG_F32:
      return tag_init_f64(dest, a->data.f64 * b->data.f32);
    case TAG_F64:
      return tag_init_f64(dest, a->data.f64 * b->data.f64);
    case TAG_S8:
      return tag_init_f64(dest, a->data.f64 * b->data.s8);
    case TAG_S16:
      return tag_init_f64(dest, a->data.f64 * b->data.s16);
    case TAG_S32:
      return tag_init_f64(dest, a->data.f64 * b->data.s32);
    case TAG_S64:
      return tag_init_f64(dest, a->data.f64 * b->data.s64);
    case TAG_U8:
      return tag_init_f64(dest, a->data.f64 * b->data.u8);
    case TAG_U16:
      return tag_init_f64(dest, a->data.f64 * b->data.u16);
    case TAG_U32:
      return tag_init_f64(dest, a->data.f64 * b->data.u32);
    case TAG_U64:
      return tag_init_f64(dest, a->data.f64 * b->data.u64);
    default:
      goto ko;
  }
  case TAG_S8:
    switch (b->type) {
    case TAG_F32:
      return tag_init_f32(dest, a->data.s8 * b->data.f32);
    case TAG_F64:
      return tag_init_f64(dest, a->data.s8 * b->data.f64);
    case TAG_S8:
      return tag_init_s8(dest, a->data.s8 * b->data.s8);
    case TAG_S16:
      return tag_init_s16(dest, a->data.s8 * b->data.s16);
    case TAG_S32:
      return tag_init_s32(dest, a->data.s8 * b->data.s32);
    case TAG_S64:
      return tag_init_s64(dest, a->data.s8 * b->data.s64);
    case TAG_U8:
      return tag_init_s8(dest, a->data.s8 * b->data.u8);
    case TAG_U16:
      return tag_init_s16(dest, a->data.s8 * b->data.u16);
    case TAG_U32:
      return tag_init_s32(dest, a->data.s8 * b->data.u32);
    case TAG_U64:
      return tag_init_s64(dest, a->data.s8 * b->data.u64);
    default:
      goto ko;
  }
  case TAG_S16:
    switch (b->type) {
    case TAG_F32:
      return tag_init_f32(dest, a->data.s16 * b->data.f32);
    case TAG_F64:
      return tag_init_f64(dest, a->data.s16 * b->data.f64);
    case TAG_S8:
      return tag_init_s16(dest, a->data.s16 * b->data.s8);
    case TAG_S16:
      return tag_init_s16(dest, a->data.s16 * b->data.s16);
    case TAG_S32:
      return tag_init_s32(dest, a->data.s16 * b->data.s32);
    case TAG_S64:
      return tag_init_s64(dest, a->data.s16 * b->data.s64);
    case TAG_U8:
      return tag_init_s16(dest, a->data.s16 * b->data.u8);
    case TAG_U16:
      return tag_init_s16(dest, a->data.s16 * b->data.u16);
    case TAG_U32:
      return tag_init_s32(dest, a->data.s16 * b->data.u32);
    case TAG_U64:
      return tag_init_s64(dest, a->data.s16 * b->data.u64);
    default:
      goto ko;
  }
  case TAG_S32:
    switch (b->type) {
    case TAG_F32:
      return tag_init_f32(dest, a->data.s32 * b->data.f32);
    case TAG_F64:
      return tag_init_f64(dest, a->data.s32 * b->data.f64);
    case TAG_S8:
      return tag_init_s32(dest, a->data.s32 * b->data.s8);
    case TAG_S16:
      return tag_init_s32(dest, a->data.s32 * b->data.s16);
    case TAG_S32:
      return tag_init_s32(dest, a->data.s32 * b->data.s32);
    case TAG_S64:
      return tag_init_s64(dest, a->data.s32 * b->data.s64);
    case TAG_U8:
      return tag_init_s32(dest, a->data.s32 * b->data.u8);
    case TAG_U16:
      return tag_init_s32(dest, a->data.s32 * b->data.u16);
    case TAG_U32:
      return tag_init_s32(dest, a->data.s32 * b->data.u32);
    case TAG_U64:
      return tag_init_s64(dest, a->data.s32 * b->data.u64);
    default:
      goto ko;
  }
  case TAG_S64:
    switch (b->type) {
    case TAG_F32:
      return tag_init_f32(dest, a->data.s64 * b->data.f32);
    case TAG_F64:
      return tag_init_f64(dest, a->data.s64 * b->data.f64);
    case TAG_S8:
      return tag_init_s32(dest, a->data.s64 * b->data.s8);
    case TAG_S16:
      return tag_init_s32(dest, a->data.s64 * b->data.s16);
    case TAG_S32:
      return tag_init_s32(dest, a->data.s64 * b->data.s32);
    case TAG_S64:
      return tag_init_s64(dest, a->data.s64 * b->data.s64);
    case TAG_U8:
      return tag_init_s32(dest, a->data.s64 * b->data.u8);
    case TAG_U16:
      return tag_init_s32(dest, a->data.s64 * b->data.u16);
    case TAG_U32:
      return tag_init_s32(dest, a->data.s64 * b->data.u32);
    case TAG_U64:
      return tag_init_s64(dest, a->data.s64 * b->data.u64);
    default:
      goto ko;
  }
  case TAG_U8:
    switch (b->type) {
    case TAG_F32:
      return tag_init_f32(dest, a->data.u8 * b->data.f32);
    case TAG_F64:
      return tag_init_f64(dest, a->data.u8 * b->data.f64);
    case TAG_S8:
      return tag_init_s8(dest, a->data.u8 * b->data.s8);
    case TAG_S16:
      return tag_init_s16(dest, a->data.u8 * b->data.s16);
    case TAG_S32:
      return tag_init_s32(dest, a->data.u8 * b->data.s32);
    case TAG_S64:
      return tag_init_s64(dest, a->data.u8 * b->data.s64);
    case TAG_U8:
      return tag_init_u8(dest, a->data.u8 * b->data.u8);
    case TAG_U16:
      return tag_init_u16(dest, a->data.u8 * b->data.u16);
    case TAG_U32:
      return tag_init_u32(dest, a->data.u8 * b->data.u32);
    case TAG_U64:
      return tag_init_u64(dest, a->data.u8 * b->data.u64);
    default:
      goto ko;
  }
  case TAG_U16:
    switch (b->type) {
    case TAG_F32:
      return tag_init_f32(dest, a->data.u16 * b->data.f32);
    case TAG_F64:
      return tag_init_f64(dest, a->data.u16 * b->data.f64);
    case TAG_S8:
      return tag_init_s16(dest, a->data.u16 * b->data.s8);
    case TAG_S16:
      return tag_init_s16(dest, a->data.u16 * b->data.s16);
    case TAG_S32:
      return tag_init_s32(dest, a->data.u16 * b->data.s32);
    case TAG_S64:
      return tag_init_s64(dest, a->data.u16 * b->data.s64);
    case TAG_U8:
      return tag_init_u16(dest, a->data.u16 * b->data.u8);
    case TAG_U16:
      return tag_init_u16(dest, a->data.u16 * b->data.u16);
    case TAG_U32:
      return tag_init_u32(dest, a->data.u16 * b->data.u32);
    case TAG_U64:
      return tag_init_u64(dest, a->data.u16 * b->data.u64);
    default:
      goto ko;
  }
  case TAG_U32:
    switch (b->type) {
    case TAG_F32:
      return tag_init_f32(dest, a->data.u32 * b->data.f32);
    case TAG_F64:
      return tag_init_f64(dest, a->data.u32 * b->data.f64);
    case TAG_S8:
      return tag_init_s32(dest, a->data.u32 * b->data.s8);
    case TAG_S16:
      return tag_init_s32(dest, a->data.u32 * b->data.s16);
    case TAG_S32:
      return tag_init_s32(dest, a->data.u32 * b->data.s32);
    case TAG_S64:
      return tag_init_s64(dest, a->data.u32 * b->data.s64);
    case TAG_U8:
      return tag_init_u32(dest, a->data.u32 * b->data.u8);
    case TAG_U16:
      return tag_init_u32(dest, a->data.u32 * b->data.u16);
    case TAG_U32:
      return tag_init_u32(dest, a->data.u32 * b->data.u32);
    case TAG_U64:
      return tag_init_u64(dest, a->data.u32 * b->data.u64);
    default:
      goto ko;
  }
  case TAG_U64:
    switch (b->type) {
    case TAG_F32:
      return tag_init_f32(dest, a->data.u64 * b->data.f32);
    case TAG_F64:
      return tag_init_f64(dest, a->data.u64 * b->data.f64);
    case TAG_S8:
      return tag_init_s64(dest, a->data.u64 * b->data.s8);
    case TAG_S16:
      return tag_init_s64(dest, a->data.u64 * b->data.s16);
    case TAG_S32:
      return tag_init_s64(dest, a->data.u64 * b->data.s32);
    case TAG_S64:
      return tag_init_s64(dest, a->data.u64 * b->data.s64);
    case TAG_U8:
      return tag_init_u64(dest, a->data.u64 * b->data.u8);
    case TAG_U16:
      return tag_init_u64(dest, a->data.u64 * b->data.u16);
    case TAG_U32:
      return tag_init_u64(dest, a->data.u64 * b->data.u32);
    case TAG_U64:
      return tag_init_u64(dest, a->data.u64 * b->data.u64);
    default:
      goto ko;
    }
  default:
    goto ko;
  }
 ko:
  errx(1, "cannot multiply %s by %s",
       tag_type_to_string(a->type),
       tag_type_to_string(b->type));
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

s_tag * tag_new_array (const s_array *a)
{
  s_tag *dest;
  assert(a);
  if (! (dest = malloc(sizeof(s_tag))))
    errx(1, "tag_new_array: out of memory");
  return tag_init_array(dest, a);
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

s_tag * tag_sub (const s_tag *a, const s_tag *b, s_tag *dest)
{
  assert(a);
  assert(b);
  assert(dest);
  switch (a->type) {
  case TAG_F32:
    switch (b->type) {
    case TAG_F32:
      return tag_init_f32(dest, a->data.f32 - b->data.f32);
    case TAG_F64:
      return tag_init_f64(dest, a->data.f32 - b->data.f64);
    case TAG_S8:
      return tag_init_f32(dest, a->data.f32 - b->data.s8);
    case TAG_S16:
      return tag_init_f32(dest, a->data.f32 - b->data.s16);
    case TAG_S32:
      return tag_init_f32(dest, a->data.f32 - b->data.s32);
    case TAG_S64:
      return tag_init_f32(dest, a->data.f32 - b->data.s64);
    case TAG_U8:
      return tag_init_f32(dest, a->data.f32 - b->data.u8);
    case TAG_U16:
      return tag_init_f32(dest, a->data.f32 - b->data.u16);
    case TAG_U32:
      return tag_init_f32(dest, a->data.f32 - b->data.u32);
    case TAG_U64:
      return tag_init_f32(dest, a->data.f32 - b->data.u64);
    default:
      goto ko;
  }
  case TAG_F64:
    switch (b->type) {
    case TAG_F32:
      return tag_init_f64(dest, a->data.f64 - b->data.f32);
    case TAG_F64:
      return tag_init_f64(dest, a->data.f64 - b->data.f64);
    case TAG_S8:
      return tag_init_f64(dest, a->data.f64 - b->data.s8);
    case TAG_S16:
      return tag_init_f64(dest, a->data.f64 - b->data.s16);
    case TAG_S32:
      return tag_init_f64(dest, a->data.f64 - b->data.s32);
    case TAG_S64:
      return tag_init_f64(dest, a->data.f64 - b->data.s64);
    case TAG_U8:
      return tag_init_f64(dest, a->data.f64 - b->data.u8);
    case TAG_U16:
      return tag_init_f64(dest, a->data.f64 - b->data.u16);
    case TAG_U32:
      return tag_init_f64(dest, a->data.f64 - b->data.u32);
    case TAG_U64:
      return tag_init_f64(dest, a->data.f64 - b->data.u64);
    default:
      goto ko;
  }
  case TAG_S8:
    switch (b->type) {
    case TAG_F32:
      return tag_init_f32(dest, a->data.s8 - b->data.f32);
    case TAG_F64:
      return tag_init_f64(dest, a->data.s8 - b->data.f64);
    case TAG_S8:
      return tag_init_s8(dest, a->data.s8 - b->data.s8);
    case TAG_S16:
      return tag_init_s16(dest, a->data.s8 - b->data.s16);
    case TAG_S32:
      return tag_init_s32(dest, a->data.s8 - b->data.s32);
    case TAG_S64:
      return tag_init_s64(dest, a->data.s8 - b->data.s64);
    case TAG_U8:
      return tag_init_s8(dest, a->data.s8 - b->data.u8);
    case TAG_U16:
      return tag_init_s16(dest, a->data.s8 - b->data.u16);
    case TAG_U32:
      return tag_init_s32(dest, a->data.s8 - b->data.u32);
    case TAG_U64:
      return tag_init_s64(dest, a->data.s8 - b->data.u64);
    default:
      goto ko;
  }
  case TAG_S16:
    switch (b->type) {
    case TAG_F32:
      return tag_init_f32(dest, a->data.s16 - b->data.f32);
    case TAG_F64:
      return tag_init_f64(dest, a->data.s16 - b->data.f64);
    case TAG_S8:
      return tag_init_s16(dest, a->data.s16 - b->data.s8);
    case TAG_S16:
      return tag_init_s16(dest, a->data.s16 - b->data.s16);
    case TAG_S32:
      return tag_init_s32(dest, a->data.s16 - b->data.s32);
    case TAG_S64:
      return tag_init_s64(dest, a->data.s16 - b->data.s64);
    case TAG_U8:
      return tag_init_s16(dest, a->data.s16 - b->data.u8);
    case TAG_U16:
      return tag_init_s16(dest, a->data.s16 - b->data.u16);
    case TAG_U32:
      return tag_init_s32(dest, a->data.s16 - b->data.u32);
    case TAG_U64:
      return tag_init_s64(dest, a->data.s16 - b->data.u64);
    default:
      goto ko;
  }
  case TAG_S32:
    switch (b->type) {
    case TAG_F32:
      return tag_init_f32(dest, a->data.s32 - b->data.f32);
    case TAG_F64:
      return tag_init_f64(dest, a->data.s32 - b->data.f64);
    case TAG_S8:
      return tag_init_s32(dest, a->data.s32 - b->data.s8);
    case TAG_S16:
      return tag_init_s32(dest, a->data.s32 - b->data.s16);
    case TAG_S32:
      return tag_init_s32(dest, a->data.s32 - b->data.s32);
    case TAG_S64:
      return tag_init_s64(dest, a->data.s32 - b->data.s64);
    case TAG_U8:
      return tag_init_s32(dest, a->data.s32 - b->data.u8);
    case TAG_U16:
      return tag_init_s32(dest, a->data.s32 - b->data.u16);
    case TAG_U32:
      return tag_init_s32(dest, a->data.s32 - b->data.u32);
    case TAG_U64:
      return tag_init_s64(dest, a->data.s32 - b->data.u64);
    default:
      goto ko;
  }
  case TAG_S64:
    switch (b->type) {
    case TAG_F32:
      return tag_init_f32(dest, a->data.s64 - b->data.f32);
    case TAG_F64:
      return tag_init_f64(dest, a->data.s64 - b->data.f64);
    case TAG_S8:
      return tag_init_s32(dest, a->data.s64 - b->data.s8);
    case TAG_S16:
      return tag_init_s32(dest, a->data.s64 - b->data.s16);
    case TAG_S32:
      return tag_init_s32(dest, a->data.s64 - b->data.s32);
    case TAG_S64:
      return tag_init_s64(dest, a->data.s64 - b->data.s64);
    case TAG_U8:
      return tag_init_s32(dest, a->data.s64 - b->data.u8);
    case TAG_U16:
      return tag_init_s32(dest, a->data.s64 - b->data.u16);
    case TAG_U32:
      return tag_init_s32(dest, a->data.s64 - b->data.u32);
    case TAG_U64:
      return tag_init_s64(dest, a->data.s64 - b->data.u64);
    default:
      goto ko;
  }
  case TAG_U8:
    switch (b->type) {
    case TAG_F32:
      return tag_init_f32(dest, a->data.u8 - b->data.f32);
    case TAG_F64:
      return tag_init_f64(dest, a->data.u8 - b->data.f64);
    case TAG_S8:
      return tag_init_s8(dest, a->data.u8 - b->data.s8);
    case TAG_S16:
      return tag_init_s16(dest, a->data.u8 - b->data.s16);
    case TAG_S32:
      return tag_init_s32(dest, a->data.u8 - b->data.s32);
    case TAG_S64:
      return tag_init_s64(dest, a->data.u8 - b->data.s64);
    case TAG_U8:
      return tag_init_s8(dest, a->data.u8 - b->data.u8);
    case TAG_U16:
      return tag_init_s16(dest, a->data.u8 - b->data.u16);
    case TAG_U32:
      return tag_init_s32(dest, a->data.u8 - b->data.u32);
    case TAG_U64:
      return tag_init_s64(dest, a->data.u8 - b->data.u64);
    default:
      goto ko;
  }
  case TAG_U16:
    switch (b->type) {
    case TAG_F32:
      return tag_init_f32(dest, a->data.u16 - b->data.f32);
    case TAG_F64:
      return tag_init_f64(dest, a->data.u16 - b->data.f64);
    case TAG_S8:
      return tag_init_s16(dest, a->data.u16 - b->data.s8);
    case TAG_S16:
      return tag_init_s16(dest, a->data.u16 - b->data.s16);
    case TAG_S32:
      return tag_init_s32(dest, a->data.u16 - b->data.s32);
    case TAG_S64:
      return tag_init_s64(dest, a->data.u16 - b->data.s64);
    case TAG_U8:
      return tag_init_s16(dest, a->data.u16 - b->data.u8);
    case TAG_U16:
      return tag_init_s16(dest, a->data.u16 - b->data.u16);
    case TAG_U32:
      return tag_init_s32(dest, a->data.u16 - b->data.u32);
    case TAG_U64:
      return tag_init_s64(dest, a->data.u16 - b->data.u64);
    default:
      goto ko;
  }
  case TAG_U32:
    switch (b->type) {
    case TAG_F32:
      return tag_init_f32(dest, a->data.u32 - b->data.f32);
    case TAG_F64:
      return tag_init_f64(dest, a->data.u32 - b->data.f64);
    case TAG_S8:
      return tag_init_s32(dest, a->data.u32 - b->data.s8);
    case TAG_S16:
      return tag_init_s32(dest, a->data.u32 - b->data.s16);
    case TAG_S32:
      return tag_init_s32(dest, a->data.u32 - b->data.s32);
    case TAG_S64:
      return tag_init_s64(dest, a->data.u32 - b->data.s64);
    case TAG_U8:
      return tag_init_s32(dest, a->data.u32 - b->data.u8);
    case TAG_U16:
      return tag_init_s32(dest, a->data.u32 - b->data.u16);
    case TAG_U32:
      return tag_init_s32(dest, a->data.u32 - b->data.u32);
    case TAG_U64:
      return tag_init_s64(dest, a->data.u32 - b->data.u64);
    default:
      goto ko;
  }
  case TAG_U64:
    switch (b->type) {
    case TAG_F32:
      return tag_init_f32(dest, a->data.u64 - b->data.f32);
    case TAG_F64:
      return tag_init_f64(dest, a->data.u64 - b->data.f64);
    case TAG_S8:
      return tag_init_s64(dest, a->data.u64 - b->data.s8);
    case TAG_S16:
      return tag_init_s64(dest, a->data.u64 - b->data.s16);
    case TAG_S32:
      return tag_init_s64(dest, a->data.u64 - b->data.s32);
    case TAG_S64:
      return tag_init_s64(dest, a->data.u64 - b->data.s64);
    case TAG_U8:
      return tag_init_s64(dest, a->data.u64 - b->data.u8);
    case TAG_U16:
      return tag_init_s64(dest, a->data.u64 - b->data.u16);
    case TAG_U32:
      return tag_init_s64(dest, a->data.u64 - b->data.u32);
    case TAG_U64:
      return tag_init_s64(dest, a->data.u64 - b->data.u64);
    default:
      goto ko;
    }
  default:
    goto ko;
  }
 ko:
  errx(1, "cannot subtract %s by %s",
       tag_type_to_string(a->type),
       tag_type_to_string(b->type));
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
  if (type == sym_1("tag"))
    return tag;
  switch (tag->type) {
  case TAG_VOID:
    if (type == sym_1("void"))
      return NULL;
    goto invalid_type;
  case TAG_ARRAY:
    if (type == sym_1("array"))
      return tag->data.array.data;
    goto invalid_type;
  case TAG_BOOL:
    if (type == sym_1("bool"))
      return &tag->data.bool;
    goto invalid_type;
  case TAG_CALL:
    if (type == sym_1("call"))
      return &tag->data.call;
    goto invalid_type;
  case TAG_CALL_FN:
    if (type == sym_1("call_fn"))
      return &tag->data.call;
    goto invalid_type;
  case TAG_CALL_MACRO:
    if (type == sym_1("call_macro"))
      return &tag->data.call;
    goto invalid_type;
  case TAG_CFN:
    if (type == sym_1("cfn"))
      return &tag->data.cfn;
    goto invalid_type;
  case TAG_CHARACTER:
    if (type == sym_1("character"))
      return &tag->data.character;
    goto invalid_type;
  case TAG_F32:
    if (type == sym_1("f32"))
      return &tag->data.f32;
    goto invalid_type;
  case TAG_F64:
    if (type == sym_1("f64"))
      return &tag->data.f64;
    goto invalid_type;
  case TAG_FN:
    if (type == sym_1("fn"))
      return tag->data.fn;
    goto invalid_type;
  case TAG_IDENT:
    if (type == sym_1("ident"))
      return &tag->data.ident;
    goto invalid_type;
  case TAG_INTEGER:
    if (type == sym_1("integer"))
      return &tag->data.integer;
    goto invalid_type;
  case TAG_S64:
    if (type == sym_1("s64"))
      return &tag->data.s64;
    goto invalid_type;
  case TAG_S32:
    if (type == sym_1("s32"))
      return &tag->data.s32;
    goto invalid_type;
  case TAG_S16:
    if (type == sym_1("s16"))
      return &tag->data.s16;
    goto invalid_type;
  case TAG_S8:
    if (type == sym_1("s8"))
      return &tag->data.s8;
    goto invalid_type;
  case TAG_U8:
    if (type == sym_1("u8"))
      return &tag->data.u8;
    goto invalid_type;
  case TAG_U16:
    if (type == sym_1("u16"))
      return &tag->data.u16;
    goto invalid_type;
  case TAG_U32:
    if (type == sym_1("u32"))
      return &tag->data.u32;
    goto invalid_type;
  case TAG_U64:
    if (type == sym_1("u64"))
      return &tag->data.u64;
    goto invalid_type;
  case TAG_LIST:
    if (type == sym_1("list"))
      return tag->data.list;
    goto invalid_type;
  case TAG_PTAG:
    if (type == sym_1("ptag"))
      return (void *) tag->data.ptag;
    goto invalid_type;
  case TAG_QUOTE:
    if (type == sym_1("quote"))
      return &tag->data.quote;
    goto invalid_type;
  case TAG_STR:
    if (type == sym_1("str"))
      return &tag->data.str;
    if (type == sym_1("char*"))
      return (void *) tag->data.str.ptr.ps8;
    goto invalid_type;
  case TAG_SYM:
    if (type == sym_1("sym"))
      return (void *) tag->data.sym;
    if (type == sym_1("str"))
      return (void *) &tag->data.sym->str;
    if (type == sym_1("char*"))
      return (void *) tag->data.sym->str.ptr.ps8;
    goto invalid_type;
  case TAG_TUPLE:
    if (type == sym_1("tuple"))
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
        tag_type_to_sym(tag->type)->str.ptr.ps8,
        type->str.ptr.ps8);
  return NULL;
}

void * tag_to_pointer (s_tag *tag, e_tag_type type)
{
  if (tag->type != type) {
    warnx("tag_to_pointer: cannot cast %s to %s",
          tag_type_to_sym(tag->type)->str.ptr.ps8,
          tag_type_to_sym(type)->str.ptr.ps8);
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
  case TAG_CALL_FN:
    return &tag->data.call;
  case TAG_CALL_MACRO:
    return &tag->data.call;
  case TAG_CFN:
    return &tag->data.cfn;
  case TAG_CHARACTER:
    return &tag->data.character;
  case TAG_F32:
    return &tag->data.f32;
  case TAG_F64:
    return &tag->data.f64;
  case TAG_FN:
    return tag->data.fn;
  case TAG_IDENT:
    return &tag->data.ident;
  case TAG_INTEGER:
    return &tag->data.integer;
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
  case TAG_LIST:
    return tag->data.list;
  case TAG_PTAG:
    return (void *) tag->data.ptag;
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
        tag_type_to_sym(tag->type)->str.ptr.ps8,
        tag_type_to_sym(type)->str.ptr.ps8);
  return NULL;
  
}

sw tag_type_size (e_tag_type type)
{
  return sizeof(type);
}

f_buf_parse tag_type_to_buf_parse (e_tag_type type)
{
  switch (type) {
  case TAG_VOID:
    return (f_buf_parse) buf_parse_void;
    break;
  case TAG_ARRAY:
    return (f_buf_parse) buf_parse_array;
    break;
  case TAG_BOOL:
    return (f_buf_parse) buf_parse_bool;
    break;
  case TAG_CALL:
  case TAG_CALL_FN:
  case TAG_CALL_MACRO:
    return (f_buf_parse) buf_parse_call;
    break;
  case TAG_CFN:
    return (f_buf_parse) buf_parse_cfn;
    break;
  case TAG_CHARACTER:
    return (f_buf_parse) buf_parse_character;
    break;
  case TAG_F32:
    return (f_buf_parse) buf_parse_f32;
    break;
  case TAG_F64:
    return (f_buf_parse) buf_parse_f64;
    break;
  case TAG_FN:
    return (f_buf_parse) buf_parse_fn;
    break;
  case TAG_IDENT:
    return (f_buf_parse) buf_parse_ident;
    break;
  case TAG_INTEGER:
    return (f_buf_parse) buf_parse_integer;
    break;
  case TAG_S64:
    return (f_buf_parse) buf_parse_s64;
    break;
  case TAG_S32:
    return (f_buf_parse) buf_parse_s32;
    break;
  case TAG_S16:
    return (f_buf_parse) buf_parse_s16;
    break;
  case TAG_S8:
    return (f_buf_parse) buf_parse_s8;
    break;
  case TAG_U8:
    return (f_buf_parse) buf_parse_u8;
    break;
  case TAG_U16:
    return (f_buf_parse) buf_parse_u16;
    break;
  case TAG_U32:
    return (f_buf_parse) buf_parse_u32;
    break;
  case TAG_U64:
    return (f_buf_parse) buf_parse_u64;
    break;
  case TAG_LIST:
    return (f_buf_parse) buf_parse_list;
    break;
  case TAG_PTAG:
    return (f_buf_parse) buf_parse_ptag;
    break;
  case TAG_QUOTE:
    return (f_buf_parse) buf_parse_quote;
    break;
  case TAG_STR:
    return (f_buf_parse) buf_parse_str;
    break;
  case TAG_SYM:
    return (f_buf_parse) buf_parse_sym;
    break;
  case TAG_TUPLE:
    return (f_buf_parse) buf_parse_tuple;
    break;
  case TAG_VAR:
    return (f_buf_parse) buf_parse_var;
    break;
  }
  assert(! "tag_type_to_buf_parse: invalid tag type");
  err(1, "tag_type_to_buf_parse: invalid tag type");
  return NULL;
}

ffi_type * tag_type_to_ffi_type (e_tag_type type)
{
  switch (type) {
  case TAG_ARRAY:
    return &ffi_type_pointer;
  case TAG_BOOL:
    return &ffi_type_uint8;
  case TAG_CALL:
  case TAG_CALL_FN:
  case TAG_CALL_MACRO:
    return &ffi_type_pointer;
  case TAG_CFN:
    return &ffi_type_pointer;
  case TAG_CHARACTER:
    return &ffi_type_schar;
  case TAG_F32:
    return &ffi_type_float;
  case TAG_F64:
    return &ffi_type_double;
  case TAG_FN:
    return &ffi_type_pointer;
  case TAG_IDENT:
    return &ffi_type_pointer;
  case TAG_INTEGER:
    return &ffi_type_sint;
  case TAG_LIST:
    return &ffi_type_pointer;
  case TAG_PTAG:
    return &ffi_type_pointer;
  case TAG_QUOTE:
    return &ffi_type_pointer;
  case TAG_S8:
    return &ffi_type_sint8;
  case TAG_S16:
    return &ffi_type_sint16;
  case TAG_S32:
    return &ffi_type_sint32;
  case TAG_S64:
    return &ffi_type_sint64;
  case TAG_STR:
    return &ffi_type_pointer;
  case TAG_SYM:
    return &ffi_type_pointer;
  case TAG_TUPLE:
    return &ffi_type_pointer;
  case TAG_U8:
    return &ffi_type_uint8;
  case TAG_U16:
    return &ffi_type_uint16;
  case TAG_U32:
    return &ffi_type_uint32;
  case TAG_U64:
    return &ffi_type_uint64;
  case TAG_VAR:
    return &ffi_type_pointer;
  case TAG_VOID:
    return &ffi_type_void;
  }
  assert(! "tag_type_to_ffi_type: unknown tag type");
  errx(1, "tag_type_to_ffi_type: unknown tag type");
  return &ffi_type_void;
}

s8 * tag_type_to_string (e_tag_type type)
{
  switch (type) {
  case TAG_VOID: return "void";
  case TAG_ARRAY: return "array";
  case TAG_BOOL: return "bool";
  case TAG_CALL: return "call";
  case TAG_CALL_FN: return "call_fn";
  case TAG_CALL_MACRO: return "call_macro";
  case TAG_CFN: return "cfn";
  case TAG_CHARACTER: return "character";
  case TAG_F32: return "f32";
  case TAG_F64: return "f64";
  case TAG_FN: return "fn";
  case TAG_IDENT: return "ident";
  case TAG_INTEGER: return "integer";
  case TAG_S64: return "s64";
  case TAG_S32: return "s32";
  case TAG_S16: return "s16";
  case TAG_S8: return "s8";
  case TAG_U8: return "u8";
  case TAG_U16: return "u16";
  case TAG_U32: return "u32";
  case TAG_U64: return "u64";
  case TAG_LIST: return "list";
  case TAG_PTAG: return "ptag";
  case TAG_QUOTE: return "quote";
  case TAG_STR: return "str";
  case TAG_SYM: return "sym";
  case TAG_TUPLE: return "tuple";
  case TAG_VAR: return "var";
  }
  assert(! "tag_type_to_string: unknown tag type");
  errx(1, "tag_type_to_string: unknown tag type");
  return NULL;
}

const s_sym * tag_type_to_sym (e_tag_type tag_type)
{
  switch (tag_type) {
  case TAG_VOID:       return sym_1("void");
  case TAG_ARRAY:      return sym_1("array");
  case TAG_BOOL:       return sym_1("bool");
  case TAG_CALL:       return sym_1("call");
  case TAG_CALL_FN:    return sym_1("call_fn");
  case TAG_CALL_MACRO: return sym_1("call_macro");
  case TAG_CFN:        return sym_1("cfn");
  case TAG_CHARACTER:  return sym_1("character");
  case TAG_F32:        return sym_1("f32");
  case TAG_F64:        return sym_1("f64");
  case TAG_FN:         return sym_1("fn");
  case TAG_IDENT:      return sym_1("ident");
  case TAG_INTEGER:    return sym_1("integer");
  case TAG_S64:        return sym_1("s64");
  case TAG_S32:        return sym_1("s32");
  case TAG_S16:        return sym_1("s16");
  case TAG_S8:         return sym_1("s8");
  case TAG_U8:         return sym_1("u8");
  case TAG_U16:        return sym_1("u16");
  case TAG_U32:        return sym_1("u32");
  case TAG_U64:        return sym_1("u64");
  case TAG_LIST:       return sym_1("list");
  case TAG_PTAG:       return sym_1("ptag");
  case TAG_QUOTE:      return sym_1("quote");
  case TAG_STR:        return sym_1("str");
  case TAG_SYM:        return sym_1("sym");
  case TAG_TUPLE:      return sym_1("tuple");
  case TAG_VAR:        return sym_1("var");
  }
  assert(! "tag_type_to_sym: invalid tag type");
  errx(1, "tag_type_to_sym: invalid tag type: %d", tag_type);
  return NULL;
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
