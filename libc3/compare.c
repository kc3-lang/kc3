/* c3
 * Copyright 2022 kmx.io <contact@kmx.io>
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
#include <string.h>
#include "compare.h"
#include "list.h"
#include "tag.h"

#define COMPARE_DEF(type)                       \
  s8 compare_##type (type a, type b)            \
  {                                             \
    if (a < b)                                  \
      return -1;                                \
    if (a > b)                                  \
      return 1;                                 \
    return 0;                                   \
  }                                             \

s8 compare_call (const s_call *a, const s_call *b)
{
  s8 r;
  if (a == b)
    return 0;
  if (!a)
    return -1;
  if (!b)
    return 1;
  if ((r = compare_ident(&a->ident, &b->ident)))
    return r;
  return compare_list(a->arguments, b->arguments);
}


COMPARE_DEF(f32)

COMPARE_DEF(f64)

s8 compare_fact (const s_fact *a, const s_fact *b)
{
  s8 r;
  if (a == b)
    return 0;
  if (!a)
    return -1;
  if (!b)
    return 1;
  if ((r = compare_tag(a->subject, b->subject)))
    return r;
  if ((r = compare_tag(a->predicate, b->predicate)))
    return r;
  r = compare_tag(a->object, b->object);
  return r;
}

s8 fact_compare_unbound_var_count (const s_fact *a,
                                   const s_fact *b)
{
  u8 a_count;
  u8 b_count;
  if (a == b)
    return 0;
  if (!a)
    return -1;
  if (!b)
    return 1;
  a_count = 0;
  b_count = 0;
  if (tag_is_unbound_var(a->subject))
    a_count++;
  if (tag_is_unbound_var(a->predicate))
    a_count++;
  if (tag_is_unbound_var(a->object))
    a_count++;
  if (tag_is_unbound_var(b->subject))
    b_count++;
  if (tag_is_unbound_var(b->predicate))
    b_count++;
  if (tag_is_unbound_var(b->object))
    b_count++;
  return compare_u8(a_count, b_count);
}

s8 compare_fact_pos (const s_fact *a, const s_fact *b)
{
  s8 r;
  if (a == b)
    return 0;
  if (!a)
    return -1;
  if (!b)
    return 1;
  if ((r = compare_tag(a->predicate, b->predicate)))
    return r;
  if ((r = compare_tag(a->object, b->object)))
    return r;
  r = compare_tag(a->subject, b->subject);
  return r;
}

s8 compare_fact_osp (const s_fact *a, const s_fact *b)
{
  s8 r;
  if (a == b)
    return 0;
  if (!a)
    return -1;
  if (!b)
    return 1;
  if ((r = compare_tag(a->object, b->object)))
    return r;
  if ((r = compare_tag(a->subject, b->subject)))
    return r;
  r = compare_tag(a->predicate, b->predicate);
  return r;
}

s8 compare_integer (const s_integer *a, const s_integer *b)
{
  sw r;
  assert(a);
  assert(b);
  r = mp_cmp(&a->mp_int, &b->mp_int);
  switch (r) {
  case MP_LT: return -1;
  case MP_EQ: return 0;
  case MP_GT: return 1;
  }
  errx(1, "integer_compare: %s", mp_error_to_string(r));
  return -1;
}

s8 compare_integer_s64 (const s_integer *a, s64 b)
{
  assert(a);
  if (b >= 0)
    return compare_integer_u64(a, b);
  if (a->mp_int.sign != MP_NEG)
    return 1;
  if (a->mp_int.used > 1)
    return -1;
  if (a->mp_int.dp[0] < (u64) -b)
    return 1;
  if (a->mp_int.dp[0] > (u64) -b)
    return -1;
  return 0;
}

s8 compare_integer_u64 (const s_integer *a, u64 b)
{
  sw r;
  assert(a);
  r = mp_cmp_d(&a->mp_int, b);
  switch (r) {
  case MP_LT: return -1;
  case MP_EQ: return 0;
  case MP_GT: return 1;
  }
  errx(1, "integer_compare: %s", mp_error_to_string(r));
  return -1;
}

/* FIXME: dotted lists and circular lists */
s8 compare_list (const s_list *a, const s_list *b)
{
  s8 r;
  while (1) {
    if (a == b)
      return 0;
    if (!a)
      return -1;
    if (!b)
      return 1;
    if ((r = compare_tag(&a->tag, &b->tag)))
      return r;
    a = list_next(a);
    b = list_next(b);
  }
}

s8 compare_ptr (const void *a, const void *b)
{
  if (a < b)
    return -1;
  if (a == b)
    return 0;
  return 1;
}

s8 compare_quote (const p_quote a, const p_quote b)
{
  if (a == b)
    return 0;
  if (! a)
    return -1;
  if (! b)
    return 1;
  return compare_tag(a, b);
}

COMPARE_DEF(s8)

COMPARE_DEF(s16)

COMPARE_DEF(s32)

COMPARE_DEF(s64)

s8 compare_s64_u64 (s64 a, u64 b)
{
  if (b >= 0x8000000000000000)
    return -1;
  if (a < (s64) b)
    return -1;
  if (a > (s64) b)
    return 1;
  return 0;
}

s8 compare_str (const s_str *a, const s_str *b)
{
  int r;
  assert(a);
  assert(b);
  if (a == b)
    return 0;
  if (a->size < b->size)
    return -1;
  if (a->size > b->size)
    return 1;
  r = memcmp(a->ptr.p, b->ptr.p, a->size);
  if (r < 0)
    return -1;
  if (r > 0)
    return 1;
  return 0;
}

s8 compare_sym (const s_sym *a, const s_sym *b)
{
  if (a == b)
    return 0;
  if (!a)
    return -1;
  if (!b)
    return 1;
  return compare_str(&a->str, &b->str);
}

s8 compare_tag (const s_tag *a, const s_tag *b) {
  if (tag_is_bound_var(a))
    a = a->data.var;
  if (tag_is_bound_var(b))
    b = b->data.var;
  if (a == b)
    return 0;
  if (!a ||
      a == TAG_FIRST ||
      b == TAG_LAST)
    return -1;
  if (!b ||
      a == TAG_LAST ||
      b == TAG_FIRST)
    return 1;
  if (a->type.type < b->type.type)
    return -1;
  if (a->type.type > b->type.type)
    return 1;
  switch (a->type.type) {
  case TAG_VOID: return 0;
  case TAG_BOOL: return compare_bool(a->data.bool, b->data.bool);
  case TAG_CALL:
  case TAG_CALL_FN:
  case TAG_CALL_MACRO:
    return compare_call(&a->data.call, &b->data.call);
  case TAG_CHARACTER: return compare_character(a->data.character,
                                               b->data.character);
  case TAG_F32: return compare_f32(a->data.f32, b->data.f32);
  case TAG_F64: return compare_f64(a->data.f64, b->data.f64);
  case TAG_FN: return compare_ptr(a, b);
  case TAG_IDENT: return compare_ident(&a->data.ident, &b->data.ident);
  case TAG_INTEGER: return compare_integer(&a->data.integer,
                                           &b->data.integer);
  case TAG_LIST: return compare_list(a->data.list, b->data.list);
  case TAG_PTAG: return compare_ptag(a->data.ptag, b->data.ptag);
  case TAG_QUOTE: return compare_quote(a->data.quote, b->data.quote);
  case TAG_S8: return compare_s8(a->data.s8, b->data.s8);
  case TAG_S16: return compare_s16(a->data.s16, b->data.s16);
  case TAG_S32: return compare_s32(a->data.s32, b->data.s32);
  case TAG_S64: return compare_s64(a->data.s64, b->data.s64);
  case TAG_STR: return compare_str(&a->data.str, &b->data.str);
  case TAG_SYM: return compare_str(&a->data.sym->str,
                                   &b->data.sym->str);
  case TAG_TUPLE: return compare_tuple(&a->data.tuple, &b->data.tuple);
  case TAG_U8: return  compare_u8(a->data.u8, b->data.u8);
  case TAG_U16: return compare_u16(a->data.u16, b->data.u16);
  case TAG_U32: return compare_u32(a->data.u32, b->data.u32);
  case TAG_U64: return compare_u64(a->data.u64, b->data.u64);
  case TAG_VAR: return compare_ptr(a->data.var, b->data.var);
  }
  assert(! "compare_tag: error");
  errx(1, "compare_tag");
  return 0;
}

s8 compare_tag_number (const s_tag *a, const s_tag *b)
{
  assert(a);
  assert(b);
  switch (a->type.type) {
  case TAG_INTEGER:
    switch (b->type.type) {
    case TAG_INTEGER:
      return compare_integer(&a->data.integer, &b->data.integer);
    case TAG_S8:
      return compare_integer_s64(&a->data.integer, b->data.s8);
    case TAG_S16:
      return compare_integer_s64(&a->data.integer, b->data.s16);
    case TAG_S32:
      return compare_integer_s64(&a->data.integer, b->data.s32);
    case TAG_S64:
      return compare_integer_s64(&a->data.integer, b->data.s64);
    case TAG_U8:
      return compare_integer_u64(&a->data.integer, b->data.u8);
    case TAG_U16:
      return compare_integer_u64(&a->data.integer, b->data.u16);
    case TAG_U32:
      return compare_integer_u64(&a->data.integer, b->data.u32);
    case TAG_U64:
      return compare_integer_u64(&a->data.integer, b->data.u64);
    default: ;
    }
    break;
  case TAG_S8:
    switch (b->type.type) {
    case TAG_INTEGER:
      return -compare_integer_s64(&b->data.integer, a->data.s8);
    case TAG_S8:
      return compare_s8(a->data.s8, b->data.s8);
    case TAG_S16:
      return compare_s16(a->data.s8, b->data.s16);
    case TAG_S32:
      return compare_s32(a->data.s8, b->data.s32);
    case TAG_S64:
      return compare_s64(a->data.s8, b->data.s64);
    case TAG_U8:
      return compare_s16(a->data.s8, b->data.u8);
    case TAG_U16:
      return compare_s32(a->data.s8, b->data.u16);
    case TAG_U32:
      return compare_s64(a->data.s8, b->data.u32);
    case TAG_U64:
      return compare_s64_u64(a->data.s8, b->data.u64);
    default: ;
    }
    break;
  case TAG_S16:
    switch (b->type.type) {
    case TAG_INTEGER:
      return -compare_integer_s64(&b->data.integer, a->data.s16);
    case TAG_S8:
      return compare_s16(a->data.s16, b->data.s8);
    case TAG_S16:
      return compare_s16(a->data.s16, b->data.s16);
    case TAG_S32:
      return compare_s32(a->data.s16, b->data.s32);
    case TAG_S64:
      return compare_s64(a->data.s16, b->data.s64);
    case TAG_U8:
      return compare_s16(a->data.s16, b->data.u8);
    case TAG_U16:
      return compare_s32(a->data.s16, b->data.u16);
    case TAG_U32:
      return compare_s64(a->data.s16, b->data.u32);
    case TAG_U64:
      return compare_s64_u64(a->data.s16, b->data.u64);
    default: ;
    }
    break;
  case TAG_S32:
    switch (b->type.type) {
    case TAG_INTEGER:
      return -compare_integer_s64(&b->data.integer, a->data.s32);
    case TAG_S8:
      return compare_s32(a->data.s32, b->data.s8);
    case TAG_S16:
      return compare_s32(a->data.s32, b->data.s16);
    case TAG_S32:
      return compare_s32(a->data.s32, b->data.s32);
    case TAG_S64:
      return compare_s64(a->data.s32, b->data.s64);
    case TAG_U8:
      return compare_s32(a->data.s32, b->data.u8);
    case TAG_U16:
      return compare_s32(a->data.s32, b->data.u16);
    case TAG_U32:
      return compare_s64(a->data.s32, b->data.u32);
    case TAG_U64:
      return compare_s64_u64(a->data.s32, b->data.u64);
    default: ;
    }
    break;
  case TAG_S64:
    switch (b->type.type) {
    case TAG_INTEGER:
      return -compare_integer_s64(&b->data.integer, a->data.s64);
    case TAG_S8:
      return compare_s64(a->data.s64, b->data.s8);
    case TAG_S16:
      return compare_s64(a->data.s64, b->data.s16);
    case TAG_S32:
      return compare_s64(a->data.s64, b->data.s32);
    case TAG_S64:
      return compare_s64(a->data.s64, b->data.s64);
    case TAG_U8:
      return compare_s64(a->data.s64, b->data.u8);
    case TAG_U16:
      return compare_s64(a->data.s64, b->data.u16);
    case TAG_U32:
      return compare_s64(a->data.s64, b->data.u32);
    case TAG_U64:
      return compare_s64_u64(a->data.s64, b->data.u64);
    default: ;
    }
    break;
  case TAG_U8:
    switch (b->type.type) {
    case TAG_INTEGER:
      return -compare_integer_u64(&b->data.integer, a->data.u8);
    case TAG_S8:
      return compare_s16(a->data.u8, b->data.s8);
    case TAG_S16:
      return compare_s16(a->data.u8, b->data.s16);
    case TAG_S32:
      return compare_s32(a->data.u8, b->data.s32);
    case TAG_S64:
      return compare_s64(a->data.u8, b->data.s64);
    case TAG_U8:
      return compare_u8(a->data.u8, b->data.u8);
    case TAG_U16:
      return compare_u16(a->data.u8, b->data.u16);
    case TAG_U32:
      return compare_u32(a->data.u8, b->data.u32);
    case TAG_U64:
      return compare_u64(a->data.u8, b->data.u64);
    default: ;
    }
    break;
  case TAG_U16:
    switch (b->type.type) {
    case TAG_INTEGER:
      return -compare_integer_u64(&b->data.integer, a->data.u16);
    case TAG_S8:
      return compare_s32(a->data.u16, b->data.s8);
    case TAG_S16:
      return compare_s32(a->data.u16, b->data.s16);
    case TAG_S32:
      return compare_s32(a->data.u16, b->data.s32);
    case TAG_S64:
      return compare_s64(a->data.u16, b->data.s64);
    case TAG_U8:
      return compare_u16(a->data.u16, b->data.u8);
    case TAG_U16:
      return compare_u16(a->data.u16, b->data.u16);
    case TAG_U32:
      return compare_u32(a->data.u16, b->data.u32);
    case TAG_U64:
      return compare_u64(a->data.u16, b->data.u64);
    default: ;
    }
    break;
  case TAG_U32:
    switch (b->type.type) {
    case TAG_INTEGER:
      return -compare_integer_u64(&b->data.integer, a->data.u32);
    case TAG_S8:
      return compare_s64(a->data.u32, b->data.s8);
    case TAG_S16:
      return compare_s64(a->data.u32, b->data.s16);
    case TAG_S32:
      return compare_s64(a->data.u32, b->data.s32);
    case TAG_S64:
      return compare_s64(a->data.u32, b->data.s64);
    case TAG_U8:
      return compare_u32(a->data.u32, b->data.u8);
    case TAG_U16:
      return compare_u32(a->data.u32, b->data.u16);
    case TAG_U32:
      return compare_u32(a->data.u32, b->data.u32);
    case TAG_U64:
      return compare_u64(a->data.u32, b->data.u64);
    default: ;
    }
    break;
  case TAG_U64:
    switch (b->type.type) {
    case TAG_INTEGER:
      return -compare_integer_u64(&b->data.integer, a->data.u64);
    case TAG_S8:
      return -compare_s64_u64(b->data.s8, a->data.u64);
    case TAG_S16:
      return -compare_s64_u64(b->data.s16, a->data.u64);
    case TAG_S32:
      return -compare_s64_u64(b->data.s32, a->data.u64);
    case TAG_S64:
      return -compare_s64_u64(b->data.s64, a->data.u64);
    case TAG_U8:
      return compare_u64(a->data.u64, b->data.u8);
    case TAG_U16:
      return compare_u16(a->data.u64, b->data.u16);
    case TAG_U32:
      return compare_u32(a->data.u64, b->data.u32);
    case TAG_U64:
      return compare_u64(a->data.u64, b->data.u64);
    default: ;
    }
    break;
  default: ;
  }
  assert(! "tag_number_compare: not a number");
  errx(1,  "tag_number_compare: not a number");
  return 0;
}

s8 compare_tuple (const s_tuple *a, const s_tuple *b)
{
  uw i = 0;
  s8 r;
  assert(a);
  assert(b);
  if (a == b)
    return 0;
  if (a->count < b->count)
    return -1;
  if (a->count > b->count)
    return 1;
  while (i < a->count) {
    if ((r = compare_tag(a->tag + i, b->tag + i)))
      return r;
    i++;
  }
  return 0;
}

COMPARE_DEF(u8)

COMPARE_DEF(u16)

COMPARE_DEF(u32)

COMPARE_DEF(u64)
