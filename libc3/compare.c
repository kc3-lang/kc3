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
#include "compare.h"
#include "data.h"
#include "integer.h"
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

s8 compare_array (const s_array *a, const s_array *b)
{
  uw i = 0;
  sw r;
  assert(a);
  assert(b);
  if (a == b)
    return 0;
  if (a->dimension < b->dimension)
    return -1;
  if (a->dimension > b->dimension)
    return 1;
  while (i < a->dimension) {
    if (a->dimensions[i].count < b->dimensions[i].count)
      return -1;
    if (a->dimensions[i].count > b->dimensions[i].count)
      return 1;
    i++;
  }
  assert(a->size == b->size);
  if ((r = memcmp(a->data, b->data, a->size)) < 0)
    return -1;
  if (r > 0)
    return 1;
  return 0;
}

s8 compare_block (const s_block *a, const s_block *b)
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

s8 compare_bool (bool a, bool b)
{
  if (! a && b)
    return -1;
  if ((! a && ! b) || (a && b))
    return 0;
  return 1;
}

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

s8 compare_cfn (const s_cfn *a, const s_cfn *b)
{
  s8 r;
  if (a == b)
    return 0;
  if (!a)
    return -1;
  if (!b)
    return 1;
  if ((r = compare_sym(a->name, b->name)))
    return r;
  return compare_list(a->arg_types, b->arg_types);
}

COMPARE_DEF(character)
COMPARE_DEF(f32)
COMPARE_DEF(f64)
COMPARE_DEF(f128)

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

s8 compare_fact_unbound_var_count (const s_fact *a,
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

s8 compare_fn (const s_fn *a, const s_fn *b)
{
  s8 r;
  assert(a);
  assert(b);
  if (a == b)
    return 0;
  if (!a)
    return -1;
  if (!b)
    return 1;
  if ((r = compare_bool(a->special_operator, b->special_operator)))
    return r;
  if ((r = compare_bool(a->macro, b->macro)))
    return r;
  if ((r = compare_fn_clause(a->clauses, b->clauses)))
    return r;
  return 0;
}

s8 compare_fn_clause (const s_fn_clause *a, const s_fn_clause *b)
{
  s8 r;
  assert(a);
  assert(b);
  while (1) {
    if (a == b)
      return 0;
    if (!a)
      return -1;
    if (!b)
      return 1;
    if ((r = compare_list(a->pattern, b->pattern)))
      return r;
    if ((r = compare_block(&a->algo, &b->algo)))
      return r;
    a = a->next_clause;
    b = b->next_clause;
  }
  assert(! "compare_fn");
  err(1, "compare_fn");
  return 0;
}

s8 compare_ident (const s_ident *a, const s_ident *b)
{
  sw r;
  if (a == b)
    return 0;
  if (!a)
    return -1;
  if (!b)
    return 1;
  if ((r = compare_sym(a->module, b->module)))
    return r;
  return compare_sym(a->sym, b->sym);
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

s8 compare_map (const s_map *a, const s_map *b)
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
    if ((r = compare_tag(a->key + i, b->key + i)) ||
        (r = compare_tag(a->value + i, b->value + i)))
      return r;
    i++;
  }
  return 0;
}

s8 compare_ptag (const p_tag a, const p_tag b)
{
  if (a < b)
    return -1;
  if (a == b)
    return 0;
  return 1;
}

s8 compare_ptr (const void *a, const void *b)
{
  if (a < b)
    return -1;
  if (a == b)
    return 0;
  return 1;
}

s8 compare_quote (const s_quote *a, const s_quote *b)
{
  if (a == b)
    return 0;
  if (! a)
    return -1;
  if (! b)
    return 1;
  return compare_tag(a->tag, b->tag);
}

COMPARE_DEF(s8)

COMPARE_DEF(s16)

COMPARE_DEF(s32)

COMPARE_DEF(s64)

COMPARE_DEF(sw)

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

s8 compare_struct (const s_struct *a, const s_struct *b)
{
  uw i;
  int r;
  const s_sym *type;
  assert(a);
  assert(b);
  if (a == b)
    return 0;
  r = compare_struct_type(a->type, b->type);
  if (r)
    return r;
  if (! a->data && ! b->data) {
    if (a->tag == b->tag)
      return 0;
    i = 0;
    while (i < a->type->map.count) {
      r = compare_tag(a->tag + i, b->tag + i);
      if (r)
        return r;
    }
  }
  if (a->data == b->data)
    return 0;
  if (! a->data)
    return -1;
  if (! b->data)
    return 1;
  while (i < a->type->map.count) {
    tag_type(a->type->map.value + i, &type);
    r = data_compare(type, (s8 *) a->data + a->type->offset[i],
                     (s8 *) b->data + b->type->offset[i]);
    if (r)
      return r;
  }
  return 0;
}

s8 compare_struct_type (const s_struct_type *a, const s_struct_type *b)
{
  int r;
  assert(a);
  assert(b);
  if (a == b)
    return 0;
  r = compare_sym(a->module, b->module);
  if (r)
    return r;
  r = compare_map(&a->map, &b->map);
  if (r)
    return r;
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
  s8 r;
  s_integer tmp;
  s_integer tmp2;
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
  switch (a->type) {
  case TAG_F32:
    switch (b->type) {
    case TAG_F32: return compare_f32(a->data.f32, b->data.f32);
    case TAG_F64: return compare_f64((f64) a->data.f32, b->data.f64);
    case TAG_F128:
      return compare_f128((f128) a->data.f32, b->data.f128);
    case TAG_INTEGER:
      return compare_f32(a->data.f32, integer_to_f32(&b->data.integer));
    case TAG_S8:  return compare_f32(a->data.f32, (f32) b->data.s8);
    case TAG_S16: return compare_f32(a->data.f32, (f32) b->data.s16);
    case TAG_S32: return compare_f32(a->data.f32, (f32) b->data.s32);
    case TAG_S64: return compare_f32(a->data.f32, (f32) b->data.s64);
    case TAG_SW:  return compare_f32(a->data.f32, (f32) b->data.sw);
    case TAG_U8:  return compare_f32(a->data.f32, (f32) b->data.u8);
    case TAG_U16: return compare_f32(a->data.f32, (f32) b->data.u16);
    case TAG_U32: return compare_f32(a->data.f32, (f32) b->data.u32);
    case TAG_U64: return compare_f32(a->data.f32, (f32) b->data.u64);
    case TAG_UW:  return compare_f32(a->data.f32, (f32) b->data.uw);
    default:
      break;
    }
    break;
  case TAG_F64:
    switch (b->type) {
    case TAG_F32: return compare_f64(a->data.f64, b->data.f32);
    case TAG_F64: return compare_f64((f64) a->data.f64, b->data.f64);
    case TAG_F128:
      return compare_f128((f128) a->data.f64, b->data.f128);
    case TAG_INTEGER:
      return compare_f64(a->data.f64, integer_to_f64(&b->data.integer));
    case TAG_S8:  return compare_f64(a->data.f64, (f64) b->data.s8);
    case TAG_S16: return compare_f64(a->data.f64, (f64) b->data.s16);
    case TAG_S32: return compare_f64(a->data.f64, (f64) b->data.s32);
    case TAG_S64: return compare_f64(a->data.f64, (f64) b->data.s64);
    case TAG_SW:  return compare_f64(a->data.f64, (f64) b->data.sw);
    case TAG_U8:  return compare_f64(a->data.f64, (f64) b->data.u8);
    case TAG_U16: return compare_f64(a->data.f64, (f64) b->data.u16);
    case TAG_U32: return compare_f64(a->data.f64, (f64) b->data.u32);
    case TAG_U64: return compare_f64(a->data.f64, (f64) b->data.u64);
    case TAG_UW:  return compare_f64(a->data.f64, (f64) b->data.uw);
    default:
      break;
    }
    break;
  case TAG_INTEGER:
    switch (b->type) {
    case TAG_F32: return compare_f64(integer_to_f64(&a->data.integer),
                                     (f64) b->data.f32);
    case TAG_F64: return compare_f64(integer_to_f64(&a->data.integer),
                                     b->data.f64);
    case TAG_F128:
      return compare_f128(integer_to_f128(&a->data.integer),
                          b->data.f128);
    case TAG_INTEGER:
      return compare_integer(&a->data.integer, &b->data.integer);
    case TAG_S8:
      integer_init_s8(&tmp, b->data.s8);
      r = compare_integer(&a->data.integer, &tmp);
      integer_clean(&tmp);
      return r;
    case TAG_S16:
      integer_init_s16(&tmp, b->data.s16);
      r = compare_integer(&a->data.integer, &tmp);
      integer_clean(&tmp);
      return r;
    case TAG_S32:
      integer_init_s32(&tmp, b->data.s32);
      r = compare_integer(&a->data.integer, &tmp);
      integer_clean(&tmp);
      return r;
    case TAG_S64:
      integer_init_s64(&tmp, b->data.s64);
      r = compare_integer(&a->data.integer, &tmp);
      integer_clean(&tmp);
      return r;
    case TAG_SW:
      integer_init_sw(&tmp, b->data.sw);
      r = compare_integer(&a->data.integer, &tmp);
      integer_clean(&tmp);
      return r;
    case TAG_U8:
      integer_init_u8(&tmp, b->data.u8);
      r = compare_integer(&a->data.integer, &tmp);
      integer_clean(&tmp);
      return r;
    case TAG_U16:
      integer_init_u16(&tmp, b->data.u16);
      r = compare_integer(&a->data.integer, &tmp);
      integer_clean(&tmp);
      return r;
    case TAG_U32:
      integer_init_u32(&tmp, b->data.u32);
      r = compare_integer(&a->data.integer, &tmp);
      integer_clean(&tmp);
      return r;
    case TAG_U64:
      integer_init_u64(&tmp, b->data.u64);
      r = compare_integer(&a->data.integer, &tmp);
      integer_clean(&tmp);
      return r;
    case TAG_UW:
      integer_init_uw(&tmp, b->data.uw);
      r = compare_integer(&a->data.integer, &tmp);
      integer_clean(&tmp);
      return r;
    default:
      break;
    }
    break;
  case TAG_S8:
    switch (b->type) {
    case TAG_F32: return compare_f32((f32) a->data.s8, b->data.f32);
    case TAG_F64: return compare_f64((f64) a->data.s8, b->data.f64);
    case TAG_F128:
      return compare_f128((f128) a->data.s8, b->data.f128);
    case TAG_INTEGER:
      integer_init_s8(&tmp, a->data.s8);
      r = compare_integer(&tmp, &b->data.integer);
      integer_clean(&tmp);
      return r;
    case TAG_S8:  return compare_s8(a->data.s8, b->data.s8);
    case TAG_S16: return compare_s16((s16) a->data.s8, b->data.s16);
    case TAG_S32: return compare_s32((s32) a->data.s8, b->data.s32);
    case TAG_S64: return compare_s64((s64) a->data.s8, b->data.s64);
    case TAG_SW:  return compare_s64((s64) a->data.s8, (s64) b->data.sw);
    case TAG_U8:  return compare_s16((s16) a->data.s8, (s16) b->data.u8);
    case TAG_U16: return compare_s32((s32) a->data.s8, (s32) b->data.u16);
    case TAG_U32: return compare_s64((s64) a->data.s8, (s64) b->data.u32);
    case TAG_U64:
      integer_init_s8(&tmp, a->data.s8);
      integer_init_u64(&tmp2, b->data.u64);
      r = compare_integer(&tmp, &b->data.integer);
      integer_clean(&tmp);
      integer_clean(&tmp2);
      return r;
    case TAG_UW:
      integer_init_s8(&tmp, a->data.s8);
      integer_init_u64(&tmp2, (u64) b->data.uw);
      r = compare_integer(&tmp, &b->data.integer);
      integer_clean(&tmp);
      integer_clean(&tmp2);
      return r;
    default:
      break;
    }
    break;
  case TAG_S16:
    switch (b->type) {
    case TAG_F32: return compare_f32((f32) a->data.s16, b->data.f32);
    case TAG_F64: return compare_f64((f64) a->data.s16, b->data.f64);
    case TAG_INTEGER:
      integer_init_s16(&tmp, a->data.s16);
      r = compare_integer(&tmp, &b->data.integer);
      integer_clean(&tmp);
      return r;
    case TAG_S8:  return compare_s16(a->data.s16, (s16) b->data.s8);
    case TAG_S16: return compare_s16(a->data.s16, b->data.s16);
    case TAG_S32: return compare_s32((s32) a->data.s16, b->data.s32);
    case TAG_S64: return compare_s64((s64) a->data.s16, b->data.s64);
    case TAG_SW:  return compare_s64((s64) a->data.s16, (s64) b->data.sw);
    case TAG_U8:  return compare_s16((s16) a->data.s16, (s16) b->data.u8);
    case TAG_U16: return compare_s32((s32) a->data.s16, (s32) b->data.u16);
    case TAG_U32: return compare_s64((s64) a->data.s16, (s64) b->data.u32);
    case TAG_U64:
      integer_init_s16(&tmp, a->data.s16);
      integer_init_u64(&tmp2, b->data.u64);
      r = compare_integer(&tmp, &b->data.integer);
      integer_clean(&tmp);
      integer_clean(&tmp2);
      return r;
    case TAG_UW:
      integer_init_s16(&tmp, a->data.s16);
      integer_init_u64(&tmp2, (u64) b->data.uw);
      r = compare_integer(&tmp, &b->data.integer);
      integer_clean(&tmp);
      integer_clean(&tmp2);
      return r;
    default:
      break;
    }
    break;
  case TAG_S32:
    switch (b->type) {
    case TAG_F32: return compare_f32((f32) a->data.s32, b->data.f32);
    case TAG_F64: return compare_f64((f64) a->data.s32, b->data.f64);
    case TAG_F128:
      return compare_f128((f128) a->data.s32, b->data.f128);
    case TAG_INTEGER:
      integer_init_s32(&tmp, a->data.s32);
      r = compare_integer(&tmp, &b->data.integer);
      integer_clean(&tmp);
      return r;
    case TAG_S8:  return compare_s32(a->data.s32, (s32) b->data.s8);
    case TAG_S16: return compare_s32(a->data.s32, (s32) b->data.s16);
    case TAG_S32: return compare_s32(a->data.s32, b->data.s32);
    case TAG_S64: return compare_s64((s64) a->data.s32, b->data.s64);
    case TAG_SW:  return compare_s64((s64) a->data.s32, (s64) b->data.sw);
    case TAG_U8:  return compare_s32((s32) a->data.s32, (s32) b->data.u8);
    case TAG_U16: return compare_s32((s32) a->data.s32, (s32) b->data.u16);
    case TAG_U32: return compare_s64((s64) a->data.s32, (s64) b->data.u32);
    case TAG_U64:
      integer_init_s32(&tmp, a->data.s32);
      integer_init_u64(&tmp2, b->data.u64);
      r = compare_integer(&tmp, &b->data.integer);
      integer_clean(&tmp);
      integer_clean(&tmp2);
      return r;
    case TAG_UW:
      integer_init_s32(&tmp, a->data.s32);
      integer_init_u64(&tmp2, (u64) b->data.uw);
      r = compare_integer(&tmp, &b->data.integer);
      integer_clean(&tmp);
      integer_clean(&tmp2);
      return r;
    default:
      break;
    }
    break;
  case TAG_S64:
    switch (b->type) {
    case TAG_F32: return compare_f32((f32) a->data.s64, b->data.f32);
    case TAG_F64: return compare_f64((f64) a->data.s64, b->data.f64);
    case TAG_F128:
      return compare_f128((f128) a->data.s64, b->data.f128);
    case TAG_INTEGER:
      integer_init_s64(&tmp, a->data.s64);
      r = compare_integer(&tmp, &b->data.integer);
      integer_clean(&tmp);
      return r;
    case TAG_S8:  return compare_s64(a->data.s64, (s64) b->data.s8);
    case TAG_S16: return compare_s64(a->data.s64, (s64) b->data.s16);
    case TAG_S32: return compare_s64(a->data.s64, b->data.s32);
    case TAG_S64: return compare_s64((s64) a->data.s64, b->data.s64);
    case TAG_SW:  return compare_s64((s64) a->data.s64, (s64) b->data.sw);
    case TAG_U8:  return compare_s64((s64) a->data.s64, (s64) b->data.u8);
    case TAG_U16: return compare_s64((s64) a->data.s64, (s64) b->data.u16);
    case TAG_U32: return compare_s64((s64) a->data.s64, (s64) b->data.u32);
    case TAG_U64:
      integer_init_s64(&tmp, a->data.s64);
      integer_init_u64(&tmp2, b->data.u64);
      r = compare_integer(&tmp, &tmp2);
      integer_clean(&tmp);
      integer_clean(&tmp2);
      return r;
    case TAG_UW:
      integer_init_s64(&tmp, a->data.s64);
      integer_init_u64(&tmp2, (u64) b->data.uw);
      r = compare_integer(&tmp, &tmp2);
      integer_clean(&tmp);
      integer_clean(&tmp2);
      return r;
    default:
      break;
    }
    break;
  case TAG_U8:
    switch (b->type) {
    case TAG_F32: return compare_f32((f32) a->data.u8, b->data.f32);
    case TAG_F64: return compare_f64((f64) a->data.u8, b->data.f64);
    case TAG_F128:
      return compare_f128((f128) a->data.u8, b->data.f128);
    case TAG_INTEGER:
      integer_init_u8(&tmp, a->data.u8);
      r = compare_integer(&tmp, &b->data.integer);
      integer_clean(&tmp);
      return r;
    case TAG_S8:  return compare_s16((s16) a->data.u8, (s16) b->data.s8);
    case TAG_S16: return compare_s16((s16) a->data.u8, b->data.s16);
    case TAG_S32: return compare_s32((s32) a->data.u8, b->data.s32);
    case TAG_S64: return compare_s64((s64) a->data.u8, b->data.s64);
    case TAG_SW:  return compare_s64((s64) a->data.u8, (s64) b->data.sw);
    case TAG_U8:  return compare_u8(a->data.u8, b->data.u8);
    case TAG_U16: return compare_u16((u16) a->data.u8, b->data.u16);
    case TAG_U32: return compare_u32((u32) a->data.u8, b->data.u32);
    case TAG_U64: return compare_u64((u64) a->data.u8, b->data.u64);
    case TAG_UW: return compare_u64((u64) a->data.u8, (u64) b->data.uw);
    default:
      break;
    }
    break;
  case TAG_U16:
    switch (b->type) {
    case TAG_F32: return compare_f32((f32) a->data.u16, b->data.f32);
    case TAG_F64: return compare_f64((f64) a->data.u16, b->data.f64);
    case TAG_F128:
      return compare_f128((f128) a->data.u16, b->data.f128);
    case TAG_INTEGER:
      integer_init_u16(&tmp, a->data.u16);
      r = compare_integer(&tmp, &b->data.integer);
      integer_clean(&tmp);
      return r;
    case TAG_S8:  return compare_s16((s16) a->data.u16, (s16) b->data.s8);
    case TAG_S16: return compare_s16((s16) a->data.u16, b->data.s16);
    case TAG_S32: return compare_s32((s32) a->data.u16, b->data.s32);
    case TAG_S64: return compare_s64((s64) a->data.u16, b->data.s64);
    case TAG_SW:  return compare_s64((s64) a->data.u16, (s64) b->data.sw);
    case TAG_U8:  return compare_u16(a->data.u16, (u16) b->data.u8);
    case TAG_U16: return compare_u16(a->data.u16, b->data.u16);
    case TAG_U32: return compare_u32((u32) a->data.u16, b->data.u32);
    case TAG_U64: return compare_u64((u64) a->data.u16, b->data.u64);
    case TAG_UW: return compare_u64((u64) a->data.u16, (u64) b->data.uw);
    default:
      break;
    }
    break;
  case TAG_U32:
    switch (b->type) {
    case TAG_F32: return compare_f32((f32) a->data.u32, b->data.f32);
    case TAG_F64: return compare_f64((f64) a->data.u32, b->data.f64);
    case TAG_F128:
      return compare_f128((f128) a->data.u32, b->data.f128);
    case TAG_INTEGER:
      integer_init_u32(&tmp, a->data.u32);
      r = compare_integer(&tmp, &b->data.integer);
      integer_clean(&tmp);
      return r;
    case TAG_S8:  return compare_s64((s64) a->data.u32, (s64) b->data.s8);
    case TAG_S16: return compare_s64((s64) a->data.u32, (s64) b->data.s16);
    case TAG_S32: return compare_s64((s64) a->data.u32, (s64) b->data.s32);
    case TAG_S64: return compare_s64((s64) a->data.u32, b->data.s64);
    case TAG_SW:  return compare_s64((s64) a->data.u32, (s64) b->data.sw);
    case TAG_U8:  return compare_u32(a->data.u32, (u32) b->data.u8);
    case TAG_U16: return compare_u32(a->data.u32, (u32) b->data.u16);
    case TAG_U32: return compare_u32((u32) a->data.u32, b->data.u32);
    case TAG_U64: return compare_u64((u64) a->data.u32, b->data.u64);
    case TAG_UW: return compare_u64((u64) a->data.u32, (u64) b->data.uw);
    default:
      break;
    }
    break;
  case TAG_U64:
    switch (b->type) {
    case TAG_F32: return compare_f32((f32) a->data.u64, b->data.f32);
    case TAG_F64: return compare_f64((f64) a->data.u64, b->data.f64);
    case TAG_F128:
      return compare_f128((f128) a->data.u64, b->data.f128);
    case TAG_INTEGER:
      integer_init_u64(&tmp, a->data.u64);
      r = compare_integer(&tmp, &b->data.integer);
      integer_clean(&tmp);
      return r;
    case TAG_S8:
      integer_init_u64(&tmp, a->data.u64);
      integer_init_s8(&tmp2, b->data.s8);
      r = compare_integer(&tmp, &tmp2);
      integer_clean(&tmp2);
      integer_clean(&tmp);
      return r;
    case TAG_S16:
      integer_init_u64(&tmp, a->data.u64);
      integer_init_s16(&tmp2, b->data.s16);
      r = compare_integer(&tmp, &tmp2);
      integer_clean(&tmp2);
      integer_clean(&tmp);
      return r;
    case TAG_S32:
      integer_init_u64(&tmp, a->data.u64);
      integer_init_s32(&tmp2, b->data.s32);
      r = compare_integer(&tmp, &tmp2);
      integer_clean(&tmp2);
      integer_clean(&tmp);
      return r;
    case TAG_S64:
      integer_init_u64(&tmp, a->data.u64);
      integer_init_s64(&tmp2, b->data.s64);
      r = compare_integer(&tmp, &tmp2);
      integer_clean(&tmp2);
      integer_clean(&tmp);
      return r;
    case TAG_SW:
      integer_init_u64(&tmp, a->data.u64);
      integer_init_sw(&tmp2, b->data.sw);
      r = compare_integer(&tmp, &tmp2);
      integer_clean(&tmp2);
      integer_clean(&tmp);
      return r;
    case TAG_U8:  return compare_u64(a->data.u64, (u64) b->data.u8);
    case TAG_U16: return compare_u64(a->data.u64, (u64) b->data.u16);
    case TAG_U32: return compare_u64(a->data.u64, (u64) b->data.u32);
    case TAG_U64: return compare_u64(a->data.u64, b->data.u64);
    case TAG_UW:  return compare_u64(a->data.u64, (u64) b->data.uw);
    default:
      break;
    }
    break;
  default:
    break;
  }
  if (a->type < b->type)
    return -1;
  if (a->type > b->type)
    return 1;
  switch (a->type) {
  case TAG_VOID:       return 0;
  case TAG_ARRAY:      return compare_array(&a->data.array,
                                            &b->data.array);
  case TAG_BLOCK:      return compare_block(&a->data.block,
                                            &b->data.block);
  case TAG_BOOL:       return compare_bool(a->data.bool, b->data.bool);
  case TAG_CALL:       return compare_call(&a->data.call,
                                           &b->data.call);
  case TAG_CFN:        return compare_cfn(&a->data.cfn, &b->data.cfn);
  case TAG_CHARACTER:  return compare_character(a->data.character,
                                                b->data.character);
  case TAG_FACT:       return compare_fact(&a->data.fact,
                                           &b->data.fact);
  case TAG_FN:         return compare_fn(&a->data.fn, &b->data.fn);
  case TAG_IDENT:      return compare_ident(&a->data.ident,
                                            &b->data.ident);
  case TAG_LIST:       return compare_list(a->data.list, b->data.list);
  case TAG_MAP:        return compare_map(&a->data.map, &b->data.map);
  case TAG_PTAG:       return compare_ptag(a->data.ptag, b->data.ptag);
  case TAG_PTR:        return compare_ptr(a->data.ptr.p, b->data.ptr.p);
  case TAG_PTR_FREE:   return compare_ptr(a->data.ptr_free.p,
                                          b->data.ptr_free.p);
  case TAG_QUOTE:      return compare_quote(&a->data.quote,
                                            &b->data.quote);
  case TAG_STR:        return compare_str(&a->data.str, &b->data.str);
  case TAG_STRUCT:     return compare_struct(&a->data.struct_,
                                             &b->data.struct_);
  case TAG_STRUCT_TYPE:return compare_struct_type(&a->data.struct_type,
                                                  &b->data.struct_type);
  case TAG_SYM:        return compare_str(&a->data.sym->str,
                                          &b->data.sym->str);
  case TAG_TUPLE:      return compare_tuple(&a->data.tuple,
                                            &b->data.tuple);
  case TAG_UNQUOTE:    return compare_unquote(&a->data.unquote,
                                              &b->data.unquote);
  case TAG_VAR:        return compare_ptr(a, b);
  case TAG_F32:
  case TAG_F64:
  case TAG_F128:
  case TAG_INTEGER:
  case TAG_S8:
  case TAG_S16:
  case TAG_S32:
  case TAG_S64:
  case TAG_SW:
  case TAG_U8:
  case TAG_U16:
  case TAG_U32:
  case TAG_U64:
  case TAG_UW:
    break;
  }
  warnx("compare_tag: unknown tag type: %d", a->type);
  assert(! "compare_tag: unknown tag type");
  return 0;
}

s8 compare_tag_number (const s_tag *a, const s_tag *b)
{
  assert(a);
  assert(b);
  switch (a->type) {
  case TAG_INTEGER:
    switch (b->type) {
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
    switch (b->type) {
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
    switch (b->type) {
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
    switch (b->type) {
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
    switch (b->type) {
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
    switch (b->type) {
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
    switch (b->type) {
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
    switch (b->type) {
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
    switch (b->type) {
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

s8 compare_unquote (const s_unquote *a, const s_unquote *b)
{
  if (a == b)
    return 0;
  if (! a)
    return -1;
  if (! b)
    return 1;
  return compare_tag(a->tag, b->tag);
}

COMPARE_DEF(uw)
