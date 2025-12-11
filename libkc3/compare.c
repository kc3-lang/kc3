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
#include <string.h>
#include "assert.h"
#include "character.h"
#include "compare.h"
#include "complex.h"
#include "data.h"
#include "integer.h"
#include "list.h"
#include "ratio.h"
#include "str.h"
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
  if (a->dimension_count < b->dimension_count)
    return -1;
  if (a->dimension_count > b->dimension_count)
    return 1;
  while (i < a->dimension_count) {
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

s8 compare_do_block (const s_do_block *a, const s_do_block *b)
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
  if (a->short_form && ! b->short_form)
    return -1;
  if (! a->short_form && b->short_form)
    return 1;
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
  if (! a)
    return -1;
  if (! b)
    return 1;
  if ((r = compare_ident(&a->ident, &b->ident)))
    return r;
  return compare_list(a->arguments, b->arguments);
}

s8 compare_callable (const s_callable *a, const s_callable *b)
{
  if (a == b)
    return 0;
  if (! a)
    return -1;
  if (! b)
    return 1;
  if (a->type < b->type)
    return -1;
  if (a->type > b->type)
    return 1;
  switch (a->type) {
  case CALLABLE_CFN:  return compare_cfn(&a->data.cfn, &b->data.cfn);
  case CALLABLE_FN:   return compare_fn(&a->data.fn, &b->data.fn);
  case CALLABLE_VOID: return 0;
  }
  err_puts("compare_callable: error");
  assert(! "compare_callable: error");
  return COMPARE_ERROR;
}

s8 compare_cfn (const s_cfn *a, const s_cfn *b)
{
  s8 r;
  if (a == b)
    return 0;
  if (! a)
    return -1;
  if (! b)
    return 1;
  if ((r = compare_sym(a->c_name, b->c_name)) ||
      (r = compare_list(a->arg_types, b->arg_types)))
    return r;
  return compare_sym(a->result_type, b->result_type);
}

COMPARE_DEF(character)

s8 compare_complex (s_complex *a, s_complex *b)
{
  s_tag aa = {0};
  s_tag ax2 = {0};
  s_tag ay2 = {0};
  s_tag bb = {0};
  s_tag bx2 = {0};
  s_tag by2 = {0};
  s8 r;
  assert(a);
  assert(b);
  if (! tag_mul(&a->x, &a->x, &ax2) ||
      ! tag_mul(&a->y, &a->y, &ay2) ||
      ! tag_add(&ax2, &ay2, &aa) ||
      ! tag_mul(&b->x, &b->x, &bx2) ||
      ! tag_mul(&b->y, &b->y, &by2) ||
      ! tag_add(&bx2, &by2, &bb))
    abort();
  r = compare_tag(&aa, &bb);
  tag_clean(&bb);
  tag_clean(&by2);
  tag_clean(&bx2);
  tag_clean(&aa);
  tag_clean(&ay2);
  tag_clean(&ax2);
  return r;
}

u8 compare_cow (const s_cow *a, const s_cow *b)
{
  u8 r;
  if ((r = compare_sym(a->type, b->type)) ||
      (r = compare_list(a->list, b->list)))
    return r;
  return 0;
}

COMPARE_DEF(f32)
COMPARE_DEF(f64)
#if HAVE_F80
COMPARE_DEF(f80)
#endif
#if HAVE_F128
COMPARE_DEF(f128)
#endif

s8 compare_fact (const s_fact *a, const s_fact *b)
{
  s8 r;
  if (a == b)
    return 0;
  if (!a)
    return -1;
  if (!b)
    return 1;
  if ((r = compare_tag_deref(a->subject, b->subject)))
    return r;
  if ((r = compare_tag_deref(a->predicate, b->predicate)))
    return r;
  r = compare_tag_deref(a->object, b->object);
  return r;
}

s8 compare_fact_id (const s_fact *a, const s_fact *b)
{
  s8 r;
  if (a == b)
    return 0;
  if (!a)
    return -1;
  if (!b)
    return 1;
  if (a->id < b->id)
    return -1;
  if (a->id > b->id)
    return 1;
  if ((r = compare_tag_deref(a->subject, b->subject)))
    return r;
  if ((r = compare_tag_deref(a->predicate, b->predicate)))
    return r;
  r = compare_tag_deref(a->object, b->object);
  return r;
}

s8 compare_fact_unbound_var_count (const s_fact *a,
                                   const s_fact *b)
{
  u8 a_count;
  u8 b_count;
  bool unbound;
  if (a == b)
    return 0;
  if (!a)
    return -1;
  if (!b)
    return 1;
  a_count = 0;
  b_count = 0;
  if (! tag_is_unbound_var(a->subject, &unbound))
    return COMPARE_ERROR;
  if (unbound)
    a_count++;
  if (! tag_is_unbound_var(a->predicate, &unbound))
    return COMPARE_ERROR;
  if (unbound)
    a_count++;
  if (! tag_is_unbound_var(a->object, &unbound))
    return COMPARE_ERROR;
  if (unbound)
    a_count++;
  if (! tag_is_unbound_var(b->subject, &unbound))
    return COMPARE_ERROR;
  if (unbound)
    b_count++;
  if (! tag_is_unbound_var(b->predicate, &unbound))
    return COMPARE_ERROR;
  if (unbound)
    b_count++;
  if (! tag_is_unbound_var(b->object, &unbound))
    return COMPARE_ERROR;
  if (unbound)
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
  if ((r = compare_tag_deref(a->predicate, b->predicate)))
    return r;
  if ((r = compare_tag_deref(a->object, b->object)))
    return r;
  r = compare_tag_deref(a->subject, b->subject);
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
  if ((r = compare_tag_deref(a->object, b->object)))
    return r;
  if ((r = compare_tag_deref(a->subject, b->subject)))
    return r;
  r = compare_tag_deref(a->predicate, b->predicate);
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
  if ((r = compare_sym(a->module, b->module)))
    return r;
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
    if ((r = compare_do_block(&a->algo, &b->algo)))
      return r;
    a = a->next;
    b = b->next;
  }
  err_puts("compare_fn_clause");
  assert(! "compare_fn_clause");
  abort();
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
  err_write_1("integer_compare: ");
  err_puts(mp_error_to_string(r));
  assert(! "integer_compare");
  abort();
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
  err_write_1("integer_compare: ");
  err_puts(mp_error_to_string(r));
  assert(! "integer_compare: mp_cmp_d");
  abort();
  return -2;
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

s8 compare_pcomplex (const p_complex *a, const p_complex *b)
{
  if (a == b)
    return 0;
  if (! a)
    return -1;
  if (! b)
    return 1;
  return compare_complex(*a, *b);
}

// shallow comparison
s8 compare_pfacts (const s_facts *a, const s_facts *b)
{
  if (a == b)
    return 0;
  if (a < b)
    return -1;
  return 1;
}

s8 compare_plist (const p_list *a, const p_list *b)
{
  return compare_list(*a, *b);
}

s8 compare_ptag (const p_tag a, const p_tag b)
{
  if (a < b)
    return -1;
  if (a == b)
    return 0;
  return 1;
}

s8 compare_pointer (const s_pointer *a, const s_pointer *b)
{
  s8 r = compare_sym(a->target_type, b->target_type);
  if (r)
    return r;
  if (a->ptr.p < b->ptr.p)
    return -1;
  if (a->ptr.p == b->ptr.p)
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

s8 compare_ratio (const s_ratio *a, const s_ratio *b)
{
    s_integer a_cross_product;
    s_integer b_cross_product;
    s8 r;
    assert(a);
    assert(b);
    integer_mul(&a->numerator, &b->denominator, &a_cross_product);
    integer_mul(&b->numerator, &a->denominator, &b_cross_product);
    r = compare_integer(&a_cross_product, &b_cross_product);
    integer_clean(&a_cross_product);
    integer_clean(&b_cross_product);
    return r;
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
  sw size;
  assert(a);
  assert(b);
  if (a == b)
    return 0;
  size = a->size < b->size ? a->size : b->size;
  r = memcmp(a->ptr.p, b->ptr.p, size);
  if (r < 0)
    return -1;
  if (r > 0)
    return 1;
  if (a->size < b->size)
    return -1;
  if (a->size > b->size)
    return 1;
  return 0;
}

s8 compare_str_case_insensitive (const s_str *a, const s_str *b)
{
  character ac;
  s_str     as;
  character bc;
  s_str     bs;
  sw r;
  sw r2;
  assert(a);
  assert(b);
  if (a == b)
    return 0;
  as = *a;
  bs = *b;
  while (1) {
    r = str_read_character_utf8(&as, &ac);
    r2 = str_read_character_utf8(&bs, &bc);
    if (r <= 0 && r2 <= 0)
      return 0;
    if (r <= 0)
      return -1;
    if (r2 <= 0)
      return 1;
    ac = character_to_lower(ac);
    bc = character_to_lower(bc);
    if ((r = compare_character(ac, bc)))
      return r;
  }
  return COMPARE_ERROR;
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
  if ((r = compare_struct_type(a->pstruct_type, b->pstruct_type)))
    return r;
  if (! a->data && ! b->data) {
    if (a->tag == b->tag)
      return 0;
    i = 0;
    while (i < a->pstruct_type->map.count) {
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
  while (i < a->pstruct_type->map.count) {
    if (a->pstruct_type->map.value[i].type == TAG_PVAR)
      type = a->pstruct_type->map.value[i].data.pvar->type;
    else {
      if (! tag_type(a->pstruct_type->map.value + i, &type)) {
        err_puts("compare_struct: tag_type");
        assert(! "compare_struct: tag_type");
        abort();
      }
    }
    r = data_compare(type, (s8 *) a->data + a->pstruct_type->offset[i],
                     (s8 *) b->data + b->pstruct_type->offset[i]);
    if (r)
      return r;
    i++;
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
  s_integer tmp = {0};
  s_integer tmp2 = {0};
  if (a == b)
    return 0;
  if (! a ||
      a == TAG_FIRST ||
      b == TAG_LAST)
    return -1;
  if (! b ||
      a == TAG_LAST ||
      b == TAG_FIRST)
    return 1;
  if (a->type == TAG_PVAR && a->data.pvar->bound)
    a = &a->data.pvar->tag;
  if (b->type == TAG_PVAR && b->data.pvar->bound)
    b = &b->data.pvar->tag;
  switch (a->type) {
  case TAG_PCOMPLEX:
    switch (b->type) {
    case TAG_PCOMPLEX:
      return compare_complex(a->data.pcomplex, b->data.pcomplex);
    default:
      break;
    }
    break;
  case TAG_F32:
    switch (b->type) {
    case TAG_PCOMPLEX:
      return compare_f32(a->data.f32, complex_to_f32(b->data.pcomplex));
    case TAG_F32: return compare_f32(a->data.f32, b->data.f32);
    case TAG_F64: return compare_f64((f64) a->data.f32, b->data.f64);
#if HAVE_F80
    case TAG_F80: return compare_f80((f80) a->data.f32, b->data.f80);
#endif
#if HAVE_F128
    case TAG_F128:
      return compare_f128((f128) a->data.f32, b->data.f128);
#endif
    case TAG_INTEGER:
      return compare_f32(a->data.f32, integer_to_f32(&b->data.integer));
    case TAG_RATIO:
      return compare_f32(a->data.f32, ratio_to_f32(&b->data.ratio));
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
    case TAG_PCOMPLEX:
      return compare_f64(a->data.f64, complex_to_f64(b->data.pcomplex));
    case TAG_F32: return compare_f64(a->data.f64, (f64) b->data.f32);
    case TAG_F64: return compare_f64(a->data.f64, b->data.f64);
#if HAVE_F80
    case TAG_F80: return compare_f80(a->data.f64, b->data.f80);
#endif
#if HAVE_F128
    case TAG_F128:
      return compare_f128((f128) a->data.f64, b->data.f128);
#endif
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
#if HAVE_F80
  case TAG_F80:
    switch (b->type) {
    case TAG_PCOMPLEX:
      return compare_f80(a->data.f80,
                          complex_to_f80(b->data.pcomplex));
    case TAG_F32: return compare_f80(a->data.f80, (f80) b->data.f32);
    case TAG_F64: return compare_f80(a->data.f80, (f80) b->data.f64);
    case TAG_F80: return compare_f80(a->data.f80, b->data.f80);
#if HAVE_F128
    case TAG_F128:
      return compare_f128((f128) a->data.f80, b->data.f128);
#endif
    case TAG_INTEGER:
      return compare_f80(a->data.f80, integer_to_f80(&b->data.integer));
    case TAG_S8:  return compare_f80(a->data.f80, (f80) b->data.s8);
    case TAG_S16: return compare_f80(a->data.f80, (f80) b->data.s16);
    case TAG_S32: return compare_f80(a->data.f80, (f80) b->data.s32);
    case TAG_S64: return compare_f80(a->data.f80, (f80) b->data.s64);
    case TAG_SW:  return compare_f80(a->data.f80, (f80) b->data.sw);
    case TAG_U8:  return compare_f80(a->data.f80, (f80) b->data.u8);
    case TAG_U16: return compare_f80(a->data.f80, (f80) b->data.u16);
    case TAG_U32: return compare_f80(a->data.f80, (f80) b->data.u32);
    case TAG_U64: return compare_f80(a->data.f80, (f80) b->data.u64);
    case TAG_UW:  return compare_f80(a->data.f80, (f80) b->data.uw);
    default:
      break;
    }
    break;
#endif
#if HAVE_F128
  case TAG_F128:
    switch (b->type) {
    case TAG_PCOMPLEX:
      return compare_f128(a->data.f128,
                          complex_to_f128(b->data.pcomplex));
    case TAG_F32: return compare_f128(a->data.f128, (f128) b->data.f32);
    case TAG_F64: return compare_f128(a->data.f128, (f128) b->data.f64);
#if HAVE_F80
    case TAG_F80: return compare_f128(a->data.f128, (f128) b->data.f80);
#endif
    case TAG_F128:
      return compare_f128(a->data.f128, b->data.f128);
    case TAG_INTEGER:
      return compare_f128(a->data.f128,
                          integer_to_f128(&b->data.integer));
    case TAG_S8:  return compare_f128(a->data.f128, (f128) b->data.s8);
    case TAG_S16: return compare_f128(a->data.f128, (f128) b->data.s16);
    case TAG_S32: return compare_f128(a->data.f128, (f128) b->data.s32);
    case TAG_S64: return compare_f128(a->data.f128, (f128) b->data.s64);
    case TAG_SW:  return compare_f128(a->data.f128, (f128) b->data.sw);
    case TAG_U8:  return compare_f128(a->data.f128, (f128) b->data.u8);
    case TAG_U16: return compare_f128(a->data.f128, (f128) b->data.u16);
    case TAG_U32: return compare_f128(a->data.f128, (f128) b->data.u32);
    case TAG_U64: return compare_f128(a->data.f128, (f128) b->data.u64);
    case TAG_UW:  return compare_f128(a->data.f128, (f128) b->data.uw);
    default:
      break;
    }
    break;
#endif
  case TAG_INTEGER:
    switch (b->type) {
    case TAG_F32: return compare_f64(integer_to_f64(&a->data.integer),
                                     (f64) b->data.f32);
    case TAG_F64: return compare_f64(integer_to_f64(&a->data.integer),
                                     b->data.f64);
#if HAVE_F80
    case TAG_F80: return compare_f80(integer_to_f80(&a->data.integer),
                                     b->data.f80);
#endif
#if HAVE_F128
    case TAG_F128:
      return compare_f128(integer_to_f128(&a->data.integer),
                          b->data.f128);
#endif
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
  case TAG_RATIO:
    switch (b->type) {
    case TAG_RATIO:
      return compare_ratio(&a->data.ratio, &b->data.ratio);
    default:
      break;
    }
    break;
  case TAG_S8:
    switch (b->type) {
    case TAG_F32: return compare_f32((f32) a->data.s8, b->data.f32);
    case TAG_F64: return compare_f64((f64) a->data.s8, b->data.f64);
#if HAVE_F80
    case TAG_F80: return compare_f80((f80) a->data.s8, b->data.f80);
#endif
#if HAVE_F128
    case TAG_F128:
      return compare_f128((f128) a->data.s8, b->data.f128);
#endif
    case TAG_INTEGER:
      integer_init_s8(&tmp, a->data.s8);
      r = compare_integer(&tmp, &b->data.integer);
      integer_clean(&tmp);
      return r;
    case TAG_S8:  return compare_s8(a->data.s8, b->data.s8);
    case TAG_S16: return compare_s16((s16) a->data.s8, b->data.s16);
    case TAG_S32: return compare_s32((s32) a->data.s8, b->data.s32);
    case TAG_S64: return compare_s64((s64) a->data.s8, b->data.s64);
    case TAG_SW:
      return compare_s64((s64) a->data.s8, (s64) b->data.sw);
    case TAG_U8:
      return compare_s16((s16) a->data.s8, (s16) b->data.u8);
    case TAG_U16:
      return compare_s32((s32) a->data.s8, (s32) b->data.u16);
    case TAG_U32:
      return compare_s64((s64) a->data.s8, (s64) b->data.u32);
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
    case TAG_SW:
      return compare_s64((s64) a->data.s16, (s64) b->data.sw);
    case TAG_U8:
      return compare_s16((s16) a->data.s16, (s16) b->data.u8);
    case TAG_U16:
      return compare_s32((s32) a->data.s16, (s32) b->data.u16);
    case TAG_U32:
      return compare_s64((s64) a->data.s16, (s64) b->data.u32);
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
#if HAVE_F80
    case TAG_F80: return compare_f80((f80) a->data.s32, b->data.f80);
#endif
#if HAVE_F128
    case TAG_F128:
      return compare_f128((f128) a->data.s32, b->data.f128);
#endif
    case TAG_INTEGER:
      integer_init_s32(&tmp, a->data.s32);
      r = compare_integer(&tmp, &b->data.integer);
      integer_clean(&tmp);
      return r;
    case TAG_S8:  return compare_s32(a->data.s32, (s32) b->data.s8);
    case TAG_S16: return compare_s32(a->data.s32, (s32) b->data.s16);
    case TAG_S32: return compare_s32(a->data.s32, b->data.s32);
    case TAG_S64: return compare_s64((s64) a->data.s32, b->data.s64);
    case TAG_SW:
      return compare_s64((s64) a->data.s32, (s64) b->data.sw);
    case TAG_U8:
      return compare_s32((s32) a->data.s32, (s32) b->data.u8);
    case TAG_U16:
      return compare_s32((s32) a->data.s32, (s32) b->data.u16);
    case TAG_U32:
      return compare_s64((s64) a->data.s32, (s64) b->data.u32);
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
#if HAVE_F80
    case TAG_F80: return compare_f80((f80) a->data.s64, b->data.f80);
#endif
#if HAVE_F128
    case TAG_F128:
      return compare_f128((f128) a->data.s64, b->data.f128);
#endif
    case TAG_INTEGER:
      integer_init_s64(&tmp, a->data.s64);
      r = compare_integer(&tmp, &b->data.integer);
      integer_clean(&tmp);
      return r;
    case TAG_S8:  return compare_s64(a->data.s64, (s64) b->data.s8);
    case TAG_S16: return compare_s64(a->data.s64, (s64) b->data.s16);
    case TAG_S32: return compare_s64(a->data.s64, b->data.s32);
    case TAG_S64: return compare_s64((s64) a->data.s64, b->data.s64);
    case TAG_SW:
      return compare_s64((s64) a->data.s64, (s64) b->data.sw);
    case TAG_U8:
      return compare_s64((s64) a->data.s64, (s64) b->data.u8);
    case TAG_U16:
      return compare_s64((s64) a->data.s64, (s64) b->data.u16);
    case TAG_U32:
      return compare_s64((s64) a->data.s64, (s64) b->data.u32);
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
  case TAG_SW:
    switch (b->type) {
    case TAG_F32: return compare_f32((f32) a->data.sw, b->data.f32);
    case TAG_F64: return compare_f64((f64) a->data.sw, b->data.f64);
#if HAVE_F80
    case TAG_F80: return compare_f80((f80) a->data.sw, b->data.f80);
#endif
#if HAVE_F128
    case TAG_F128:
      return compare_f128((f128) a->data.sw, b->data.f128);
#endif
    case TAG_INTEGER:
      integer_init_s64(&tmp, a->data.sw);
      r = compare_integer(&tmp, &b->data.integer);
      integer_clean(&tmp);
      return r;
    case TAG_S8:  return compare_sw(a->data.sw, (sw) b->data.s8);
    case TAG_S16: return compare_sw(a->data.sw, (sw) b->data.s16);
    case TAG_S32: return compare_sw(a->data.sw, (sw) b->data.s32);
    case TAG_S64: return compare_sw(a->data.sw, (sw) b->data.s64);
    case TAG_SW:  return compare_sw(a->data.sw, b->data.sw);
    case TAG_U8:  return compare_sw(a->data.sw, (sw) b->data.u8);
    case TAG_U16: return compare_sw(a->data.sw, (sw) b->data.u16);
    case TAG_U32: return compare_sw(a->data.sw, (sw) b->data.u32);
    case TAG_U64:
      integer_init_sw(&tmp, a->data.sw);
      integer_init_u64(&tmp2, b->data.u64);
      r = compare_integer(&tmp, &tmp2);
      integer_clean(&tmp);
      integer_clean(&tmp2);
      return r;
    case TAG_UW:
      integer_init_sw(&tmp, a->data.sw);
      integer_init_uw(&tmp2, b->data.uw);
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
#if HAVE_F80
    case TAG_F80: return compare_f80((f80) a->data.u8, b->data.f80);
#endif
#if HAVE_F128
    case TAG_F128:
      return compare_f128((f128) a->data.u8, b->data.f128);
#endif
    case TAG_INTEGER:
      integer_init_u8(&tmp, a->data.u8);
      r = compare_integer(&tmp, &b->data.integer);
      integer_clean(&tmp);
      return r;
    case TAG_S8:
      return compare_s16((s16) a->data.u8, (s16) b->data.s8);
    case TAG_S16: return compare_s16((s16) a->data.u8, b->data.s16);
    case TAG_S32: return compare_s32((s32) a->data.u8, b->data.s32);
    case TAG_S64: return compare_s64((s64) a->data.u8, b->data.s64);
    case TAG_SW:  return compare_sw((sw) a->data.u8, b->data.sw);
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
#if HAVE_F80
    case TAG_F80: return compare_f80((f80) a->data.u16, b->data.f80);
#endif
#if HAVE_F128
    case TAG_F128:
      return compare_f128((f128) a->data.u16, b->data.f128);
#endif
    case TAG_INTEGER:
      integer_init_u16(&tmp, a->data.u16);
      r = compare_integer(&tmp, &b->data.integer);
      integer_clean(&tmp);
      return r;
    case TAG_S8:
      return compare_s16((s16) a->data.u16, (s16) b->data.s8);
    case TAG_S16: return compare_s16((s16) a->data.u16, b->data.s16);
    case TAG_S32: return compare_s32((s32) a->data.u16, b->data.s32);
    case TAG_S64: return compare_s64((s64) a->data.u16, b->data.s64);
    case TAG_SW:  return compare_sw((sw) a->data.u16, b->data.sw);
    case TAG_U8:  return compare_u16(a->data.u16, (u16) b->data.u8);
    case TAG_U16: return compare_u16(a->data.u16, b->data.u16);
    case TAG_U32: return compare_u32((u32) a->data.u16, b->data.u32);
    case TAG_U64: return compare_u64((u64) a->data.u16, b->data.u64);
    case TAG_UW: return compare_uw((uw) a->data.u16, b->data.uw);
    default:
      break;
    }
    break;
  case TAG_U32:
    switch (b->type) {
    case TAG_F32: return compare_f32((f32) a->data.u32, b->data.f32);
    case TAG_F64: return compare_f64((f64) a->data.u32, b->data.f64);
#if HAVE_F80
    case TAG_F80: return compare_f80((f80) a->data.u32, b->data.f80);
#endif
#if HAVE_F128
    case TAG_F128:
      return compare_f128((f128) a->data.u32, b->data.f128);
#endif
    case TAG_INTEGER:
      integer_init_u32(&tmp, a->data.u32);
      r = compare_integer(&tmp, &b->data.integer);
      integer_clean(&tmp);
      return r;
    case TAG_S8:
      return compare_s64((s64) a->data.u32, (s64) b->data.s8);
    case TAG_S16:
      return compare_s64((s64) a->data.u32, (s64) b->data.s16);
    case TAG_S32:
      return compare_s64((s64) a->data.u32, (s64) b->data.s32);
    case TAG_S64: return compare_s64((s64) a->data.u32, b->data.s64);
    case TAG_SW:  return compare_sw((sw) a->data.u32, b->data.sw);
    case TAG_U8:  return compare_u32(a->data.u32, (u32) b->data.u8);
    case TAG_U16: return compare_u32(a->data.u32, (u32) b->data.u16);
    case TAG_U32: return compare_u32((u32) a->data.u32, b->data.u32);
    case TAG_U64: return compare_u64((u64) a->data.u32, b->data.u64);
    case TAG_UW:
      return compare_u64((u64) a->data.u32, (u64) b->data.uw);
    default:
      break;
    }
    break;
  case TAG_U64:
    switch (b->type) {
    case TAG_F32: return compare_f32((f32) a->data.u64, b->data.f32);
    case TAG_F64: return compare_f64((f64) a->data.u64, b->data.f64);
#if HAVE_F80
    case TAG_F80: return compare_f80((f80) a->data.u64, b->data.f80);
#endif
#if HAVE_F128
    case TAG_F128:
      return compare_f128((f128) a->data.u64, b->data.f128);
#endif
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
  case TAG_UW:
    switch (b->type) {
    case TAG_F32: return compare_f32((f32) a->data.uw, b->data.f32);
    case TAG_F64: return compare_f64((f64) a->data.uw, b->data.f64);
#if HAVE_F80
    case TAG_F80: return compare_f80((f80) a->data.uw, b->data.f80);
#endif
#if HAVE_F128
    case TAG_F128:
      return compare_f128((f128) a->data.uw, b->data.f128);
#endif
    case TAG_INTEGER:
      integer_init_uw(&tmp, a->data.uw);
      r = compare_integer(&tmp, &b->data.integer);
      integer_clean(&tmp);
      return r;
    case TAG_S8:
      integer_init_uw(&tmp, a->data.uw);
      integer_init_s8(&tmp2, b->data.s8);
      r = compare_integer(&tmp, &tmp2);
      integer_clean(&tmp2);
      integer_clean(&tmp);
      return r;
    case TAG_S16:
      integer_init_uw(&tmp, a->data.uw);
      integer_init_s16(&tmp2, b->data.s16);
      r = compare_integer(&tmp, &tmp2);
      integer_clean(&tmp2);
      integer_clean(&tmp);
      return r;
    case TAG_S32:
      integer_init_uw(&tmp, a->data.uw);
      integer_init_s32(&tmp2, b->data.s32);
      r = compare_integer(&tmp, &tmp2);
      integer_clean(&tmp2);
      integer_clean(&tmp);
      return r;
    case TAG_S64:
      integer_init_uw(&tmp, a->data.uw);
      integer_init_s64(&tmp2, b->data.s64);
      r = compare_integer(&tmp, &tmp2);
      integer_clean(&tmp2);
      integer_clean(&tmp);
      return r;
    case TAG_SW:
      integer_init_uw(&tmp, a->data.uw);
      integer_init_sw(&tmp2, b->data.sw);
      r = compare_integer(&tmp, &tmp2);
      integer_clean(&tmp2);
      integer_clean(&tmp);
      return r;
    case TAG_U8:  return compare_uw(a->data.uw, (uw) b->data.u8);
    case TAG_U16: return compare_uw(a->data.uw, (uw) b->data.u16);
    case TAG_U32: return compare_uw(a->data.uw, (uw) b->data.u32);
    case TAG_U64: return compare_uw(a->data.uw, b->data.u64);
    case TAG_UW:  return compare_uw(a->data.uw, (uw) b->data.uw);
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
  case TAG_BOOL:       return compare_bool(a->data.bool_,
                                           b->data.bool_);
  case TAG_CHARACTER:  return compare_character(a->data.character,
                                                b->data.character);
  case TAG_DO_BLOCK:   return compare_do_block(&a->data.do_block,
                                               &b->data.do_block);
  case TAG_FACT:       return compare_fact(&a->data.fact,
                                           &b->data.fact);
  case TAG_IDENT:      return compare_ident(&a->data.ident,
                                            &b->data.ident);
  case TAG_MAP:        return compare_map(&a->data.map, &b->data.map);
  case TAG_PCALL:      return compare_call(a->data.pcall,
                                           b->data.pcall);
  case TAG_PCALLABLE:  return compare_callable(a->data.pcallable,
                                               b->data.pcallable);
  case TAG_PCOW:       return compare_cow(a->data.pcow, b->data.pcow);
  case TAG_PFACTS:     return compare_pfacts(a->data.pfacts,
                                             b->data.pfacts);
  case TAG_PLIST:      return compare_list(a->data.plist,
                                           b->data.plist);
  case TAG_PSTRUCT:    return compare_struct(a->data.pstruct,
                                             b->data.pstruct);
  case TAG_PSTRUCT_TYPE:
    return compare_struct_type(a->data.pstruct_type,
                               b->data.pstruct_type);
  case TAG_PSYM:       return compare_str(&a->data.psym->str,
                                          &b->data.psym->str);
  case TAG_PTAG:       return compare_ptag(a->data.ptag, b->data.ptag);
  case TAG_PTR:        return compare_ptr(a->data.ptr.p, b->data.ptr.p);
  case TAG_PTR_FREE:   return compare_ptr(a->data.ptr_free.p,
                                          b->data.ptr_free.p);
  case TAG_POINTER:    return compare_pointer(&a->data.pointer,
                                              &b->data.pointer);
  case TAG_PVAR:       return compare_var(a->data.pvar, b->data.pvar);
  case TAG_QUOTE:      return compare_quote(&a->data.quote,
                                            &b->data.quote);
  case TAG_STR:        return compare_str(&a->data.str, &b->data.str);
  case TAG_TIME:       return compare_time(&a->data.time,
                                           &b->data.time);
  case TAG_TUPLE:      return compare_tuple(&a->data.tuple,
                                            &b->data.tuple);
  case TAG_UNQUOTE:    return compare_unquote(&a->data.unquote,
                                              &b->data.unquote);
  case TAG_F32:
  case TAG_F64:
#if HAVE_F80
  case TAG_F80:
#endif
#if HAVE_F128
  case TAG_F128:
#endif
  case TAG_INTEGER:
  case TAG_PCOMPLEX:
  case TAG_RATIO:
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
  err_puts("compare_tag: invalid tag type");
  assert(! "compare_tag: invalid tag type");
  abort();
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
    case TAG_SW:
      return compare_integer_s64(&a->data.integer, b->data.sw);
    case TAG_U8:
      return compare_integer_u64(&a->data.integer, b->data.u8);
    case TAG_U16:
      return compare_integer_u64(&a->data.integer, b->data.u16);
    case TAG_U32:
      return compare_integer_u64(&a->data.integer, b->data.u32);
    case TAG_U64:
      return compare_integer_u64(&a->data.integer, b->data.u64);
    case TAG_UW:
      return compare_integer_u64(&a->data.integer, b->data.uw);
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
    case TAG_SW:
      return compare_s64(a->data.s8, b->data.sw);
    case TAG_U8:
      return compare_s16(a->data.s8, b->data.u8);
    case TAG_U16:
      return compare_s32(a->data.s8, b->data.u16);
    case TAG_U32:
      return compare_s64(a->data.s8, b->data.u32);
    case TAG_U64:
      return compare_s64_u64(a->data.s8, b->data.u64);
    case TAG_UW:
      return compare_s64_u64(a->data.s8, b->data.uw);
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
    case TAG_SW:
      return compare_sw(a->data.s16, b->data.sw);
    case TAG_U8:
      return compare_s16(a->data.s16, b->data.u8);
    case TAG_U16:
      return compare_s32(a->data.s16, b->data.u16);
    case TAG_U32:
      return compare_s64(a->data.s16, b->data.u32);
    case TAG_U64:
      return compare_s64_u64(a->data.s16, b->data.u64);
    case TAG_UW:
      return compare_s64_u64(a->data.s16, b->data.uw);
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
    case TAG_SW:
      return compare_sw(a->data.s32, b->data.sw);
    case TAG_U8:
      return compare_s32(a->data.s32, b->data.u8);
    case TAG_U16:
      return compare_s32(a->data.s32, b->data.u16);
    case TAG_U32:
      return compare_s64(a->data.s32, b->data.u32);
    case TAG_U64:
      return compare_s64_u64(a->data.s32, b->data.u64);
    case TAG_UW:
      return compare_s64_u64(a->data.s32, b->data.uw);
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
    case TAG_SW:
      return compare_s64(a->data.s64, b->data.sw);
    case TAG_U8:
      return compare_s64(a->data.s64, b->data.u8);
    case TAG_U16:
      return compare_s64(a->data.s64, b->data.u16);
    case TAG_U32:
      return compare_s64(a->data.s64, b->data.u32);
    case TAG_U64:
      return compare_s64_u64(a->data.s64, b->data.u64);
    case TAG_UW:
      return compare_s64_u64(a->data.s64, b->data.uw);
    default: ;
    }
    break;
  case TAG_SW:
    switch (b->type) {
    case TAG_INTEGER:
      return -compare_integer_s64(&b->data.integer, a->data.sw);
    case TAG_S8:
      return compare_sw(a->data.sw, b->data.s8);
    case TAG_S16:
      return compare_sw(a->data.sw, b->data.s16);
    case TAG_S32:
      return compare_sw(a->data.sw, b->data.s32);
    case TAG_S64:
      return compare_s64(a->data.sw, b->data.s64);
    case TAG_SW:
      return compare_sw(a->data.sw, b->data.sw);
    case TAG_U8:
      return compare_sw(a->data.sw, b->data.u8);
    case TAG_U16:
      return compare_sw(a->data.sw, b->data.u16);
    case TAG_U32:
      return compare_sw(a->data.sw, b->data.u32);
    case TAG_U64:
      return compare_s64_u64(a->data.sw, b->data.u64);
    case TAG_UW:
      return compare_s64_u64(a->data.sw, b->data.uw);
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
    case TAG_SW:
      return compare_sw(a->data.u8, b->data.sw);
    case TAG_U8:
      return compare_u8(a->data.u8, b->data.u8);
    case TAG_U16:
      return compare_u16(a->data.u8, b->data.u16);
    case TAG_U32:
      return compare_u32(a->data.u8, b->data.u32);
    case TAG_U64:
      return compare_u64(a->data.u8, b->data.u64);
    case TAG_UW:
      return compare_uw(a->data.u8, b->data.uw);
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
    case TAG_SW:
      return compare_sw(a->data.u16, b->data.sw);
    case TAG_U8:
      return compare_u16(a->data.u16, b->data.u8);
    case TAG_U16:
      return compare_u16(a->data.u16, b->data.u16);
    case TAG_U32:
      return compare_u32(a->data.u16, b->data.u32);
    case TAG_U64:
      return compare_u64(a->data.u16, b->data.u64);
    case TAG_UW:
      return compare_uw(a->data.u16, b->data.uw);
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
    case TAG_SW:
      return compare_sw(a->data.u32, b->data.sw);
    case TAG_U8:
      return compare_u32(a->data.u32, b->data.u8);
    case TAG_U16:
      return compare_u32(a->data.u32, b->data.u16);
    case TAG_U32:
      return compare_u32(a->data.u32, b->data.u32);
    case TAG_U64:
      return compare_u64(a->data.u32, b->data.u64);
    case TAG_UW:
      return compare_uw(a->data.u32, b->data.uw);
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
    case TAG_SW:
      return -compare_s64_u64(b->data.sw, a->data.u64);
    case TAG_U8:
      return compare_u64(a->data.u64, b->data.u8);
    case TAG_U16:
      return compare_u64(a->data.u64, b->data.u16);
    case TAG_U32:
      return compare_u64(a->data.u64, b->data.u32);
    case TAG_U64:
      return compare_u64(a->data.u64, b->data.u64);
    case TAG_UW:
      return compare_u64(a->data.u64, b->data.uw);
    default: ;
    }
    break;
  case TAG_UW:
    switch (b->type) {
    case TAG_INTEGER:
      return -compare_integer_u64(&b->data.integer, a->data.uw);
    case TAG_S8:
      return -compare_s64_u64(b->data.s8, a->data.uw);
    case TAG_S16:
      return -compare_s64_u64(b->data.s16, a->data.uw);
    case TAG_S32:
      return -compare_s64_u64(b->data.s32, a->data.uw);
    case TAG_S64:
      return -compare_s64_u64(b->data.s64, a->data.uw);
    case TAG_SW:
      return -compare_s64_u64(b->data.sw, a->data.uw);
    case TAG_U8:
      return compare_uw(a->data.uw, b->data.u8);
    case TAG_U16:
      return compare_uw(a->data.uw, b->data.u16);
    case TAG_U32:
      return compare_uw(a->data.uw, b->data.u32);
    case TAG_U64:
      return compare_uw(a->data.uw, b->data.u64);
    case TAG_UW:
      return compare_uw(a->data.uw, b->data.uw);
    default: ;
    }
    break;
  default: ;
  }
  err_puts("compare_tag: not a number");
  assert(! "compare_tag: not a number");
  abort();
  return 0;
}
     
s8 compare_tag_deref (const s_tag *a, const s_tag *b)
{
  const s_tag *a_deref;
  const s_tag *b_deref;
  a_deref = a;
  if (a_deref &&
      a_deref->type == TAG_PVAR &&
      a_deref->data.pvar->bound)
    a_deref = &a_deref->data.pvar->tag;
  b_deref = b;
  if (b_deref &&
      b_deref->type == TAG_PVAR &&
      b_deref->data.pvar->bound)
    b_deref = &b_deref->data.pvar->tag;
  return compare_tag(a_deref, b_deref);
}

s8 compare_time (const s_time *a, const s_time *b)
{
  s_tag *a2;
  s_tag  a_tag[2] = {0};
  s_tag *b2;
  s_tag  b_tag[2] = {0};
  s8 r;
  if (a == b)
    return 0;
  if (! a)
    return -1;
  if (! b)
    return 1;
  a2 = a->tag;
  b2 = b->tag;
  if (! (a2 || b2)) {
    if (a->tv_sec < b->tv_sec)
      return -1;
    if (a->tv_sec > b->tv_sec)
      return 1;
    if (a->tv_nsec < b->tv_nsec)
      return -1;
    if (a->tv_nsec > b->tv_nsec)
      return 1;
    return 0;
  }
  if (! a2) {
    a2 = a_tag;
    a2[0].type = TAG_SW;
    a2[0].data.sw = a->tv_sec;
    a2[1].type = TAG_SW;
    a2[1].data.sw = a->tv_nsec;
  }
  if (! b2) {
    b2 = b_tag;
    b2[0].type = TAG_SW;
    b2[0].data.sw = b->tv_sec;
    b2[1].type = TAG_SW;
    b2[1].data.sw = b->tv_nsec;
  }
  if ((r = compare_tag(a2, b2)))
    return r;
  if ((r = compare_tag(a2 + 1, b2 + 1)))
    return r;
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

s8 compare_var (const s_var *a, const s_var *b)
{
  return compare_uw((uw) a, (uw) b);
}
