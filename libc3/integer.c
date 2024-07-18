/* c3
 * Copyright 2022-2024 kmx.io <contact@kmx.io>
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
#include "buf_parse.h"
#include "compare.h"
#include "integer.h"
#include "tag.h"
#include "tag_type.h"
#include "ratio.h"

s_integer * integer_abs (const s_integer *a, s_integer *dest)
{
  sw r;
  assert(a);
  assert(dest);
  integer_init(dest);
  if ((r = mp_abs(&a->mp_int, &dest->mp_int)) != MP_OKAY) {
    err_write_1("integer_abs: ");
    err_puts(mp_error_to_string(r));
    assert(! "integer_abs: mp_abs");
    return NULL;
  }
  return dest;
}

s_integer * integer_add (const s_integer *a, const s_integer *b,
                         s_integer *dest)
{
  sw r;
  assert(dest);
  assert(a);
  assert(b);
  integer_init(dest);
  if ((r = mp_add(&a->mp_int, &b->mp_int, &dest->mp_int)) != MP_OKAY) {
    err_write_1("integer_add: ");
    err_puts(mp_error_to_string(r));
    assert(! "integer_add: mp_add");
    return NULL;
  }
  return dest;
}

s_integer * integer_band (const s_integer *a, const s_integer *b,
                          s_integer *dest)
{
  sw r;
  assert(a);
  assert(b);
  assert(dest);
  integer_init(dest);
  if ((r = mp_and(&a->mp_int, &b->mp_int, &dest->mp_int)) != MP_OKAY) {
    err_write_1("integer_band: ");
    err_puts(mp_error_to_string(r));
    assert(! "integer_band: mp_and");
    return NULL;
  }
  return dest;
}

uw integer_bits (const s_integer *i)
{
  uw bits;
  if (integer_is_zero(i))
    return 1;
  bits = mp_count_bits(&i->mp_int);
  if (integer_is_negative(i))
    bits++;
  return bits;
}

s_integer * integer_bnot (const s_integer *a, s_integer *dest)
{
  sw r;
  assert(a);
  assert(dest);
  integer_init(dest);
  if ((r = mp_complement(&a->mp_int, &dest->mp_int)) != MP_OKAY) {
    err_write_1("integer_bnot: ");
    err_puts(mp_error_to_string(r));
    assert(! "integer_bnot: mp_complement");
    return NULL;
  }
  return dest;
}

s_integer * integer_bor (const s_integer *a, const s_integer *b,
                         s_integer *dest)
{
  sw r;
  assert(a);
  assert(b);
  assert(dest);
  integer_init(dest);
  if ((r = mp_or(&a->mp_int, &b->mp_int, &dest->mp_int)) != MP_OKAY) {
    err_write_1("integer_bor: ");
    err_puts(mp_error_to_string(r));
    assert(! "integer_bor: mp_or");
    return NULL;
  }
  return dest;
}

s_integer * integer_bxor (const s_integer *a, const s_integer *b,
                          s_integer *dest)
{
  sw r;
  assert(a);
  assert(b);
  assert(dest);
  integer_init(dest);
  if ((r = mp_xor(&a->mp_int, &b->mp_int, &dest->mp_int)) != MP_OKAY) {
    err_write_1("integer_bxor: ");
    err_puts(mp_error_to_string(r));
    assert(! "integer_bxor: mp_xor");
    return NULL;
  }
  return dest;
}

uw integer_bytes (const s_integer *i)
{
  return (integer_bits(i) + 7) / 8;
}

s_integer * integer_init_cast (s_integer *a, const s_sym *type,
                               const s_tag *tag)
{
  (void) type;
  switch (tag->type) {
  case TAG_BOOL:
    return integer_init_u8(a, tag->data.bool ? 1 : 0);
  case TAG_CHARACTER:
    return integer_init_u32(a, tag->data.character);
  case TAG_F32:
    return integer_init_f32(a, tag->data.f32);
  case TAG_F64:
    return integer_init_f64(a, tag->data.f64);
  case TAG_F128:
    return integer_init_f128(a, tag->data.f128);
  case TAG_INTEGER:
    return integer_init_copy(a, &tag->data.integer);
  case TAG_RATIO:
    return integer_init_ratio(a, &tag->data.ratio);
  case TAG_SW:
    return integer_init_sw(a, tag->data.sw);
  case TAG_S64:
    return integer_init_s64(a, tag->data.s64);
  case TAG_S32:
    return integer_init_s32(a, tag->data.s32);
  case TAG_S16:
    return integer_init_s16(a, tag->data.s16);
  case TAG_S8:
    return integer_init_s8(a, tag->data.s8);
  case TAG_U8:
    return integer_init_u8(a, tag->data.u8);
  case TAG_U16:
    return integer_init_u16(a, tag->data.u16);
  case TAG_U32:
    return integer_init_u32(a, tag->data.u32);
  case TAG_U64:
    return integer_init_u64(a, tag->data.u64);
  case TAG_UW:
    return integer_init_uw(a, tag->data.uw);
  default:
    break;
  }
  err_write_1("integer_cast: cannot cast ");
  err_write_1(tag_type_to_string(tag->type));
  err_puts(" to integer");
  assert(! "integer_cast: cannot cast");
  return NULL;
}

void integer_clean (s_integer *a)
{
  assert(a);
  mp_clear(&a->mp_int);
}

s_integer * integer_div (const s_integer *a, const s_integer *b,
                         s_integer *dest)
{
  sw r;
  assert(a);
  assert(b);
  assert(dest);
  integer_init(dest);
  r = mp_div(&a->mp_int, &b->mp_int, &dest->mp_int, NULL);
  if (r != MP_OKAY) {
    err_write_1("integer_div: ");
    err_puts(mp_error_to_string(r));
    assert(! "integer_div: mp_div");
    return NULL;
  }
  return dest;
}

s_integer * integer_gcd (const s_integer *a, const s_integer *b,
                         s_integer *dest)
{
  sw r;
  assert(a);
  assert(b);
  assert(dest);
  integer_init(dest);
  if ((r = mp_gcd(&a->mp_int, &b->mp_int, &dest->mp_int)) != MP_OKAY) {
    err_write_1("integer_gcd: ");
    err_puts(mp_error_to_string(r));
    assert(! "integer_gcd: mp_gcd");
    return NULL;
  }
  return dest;
}

s_integer * integer_init (s_integer *dest)
{
  sw r;
  assert(dest);
  if ((r = mp_init(&dest->mp_int)) != MP_OKAY) {
    err_write_1("integer_init: ");
    err_puts(mp_error_to_string(r));
    assert(! "integer_init: mp_init");
    return NULL;
  }
  return dest;
}

s_integer * integer_init_1 (s_integer *i, const char *p)
{
  s_buf buf;
  assert(i);
  assert(p);
  // FIXME
  buf_init_1(&buf, false, (char *) p);
  buf_parse_integer(&buf, i);
  return i;
}

s_integer * integer_init_copy (s_integer *a, const s_integer *src)
{
  sw r;
  assert(src);
  assert(a);
  if ((r = mp_init_copy(&a->mp_int, &src->mp_int)) != MP_OKAY) {
    err_write_1("integer_init_copy: ");
    err_puts(mp_error_to_string(r));
    assert(! "integer_init_copy: mp_init_copy");
    return NULL;
  }
  return a;
}

s_integer * integer_init_f32 (s_integer *a, f32 x)
{
  assert(a);
  integer_init(a);
  return integer_set_f32(a, x);
}

s_integer * integer_init_f64 (s_integer *a, f64 x)
{
  assert(a);
  integer_init(a);
  return integer_set_f64(a, x);
}

s_integer * integer_init_f128 (s_integer *a, f128 x)
{
  assert(a);
  integer_init(a);
  return integer_set_f128(a, x);
}

s_integer * integer_init_s8 (s_integer *a, s8 x)
{
  assert(a);
  integer_init(a);
  return integer_set_s32(a, x);
}

s_integer * integer_init_s16 (s_integer *a, s16 x)
{
  assert(a);
  integer_init(a);
  return integer_set_s32(a, x);
}

s_integer * integer_init_s32 (s_integer *a, s32 x)
{
  assert(a);
  integer_init(a);
  return integer_set_s32(a, x);
}

s_integer * integer_init_s64 (s_integer *a, s64 x)
{
  assert(a);
  integer_init(a);
  return integer_set_s64(a, x);
}

s_integer * integer_init_ratio (s_integer *a, const s_ratio *r)
{
  assert(a);
  integer_init(a);
  return integer_set_ratio(a, r);
}

s_integer * integer_init_sw (s_integer *a, sw x)
{
  assert(a);
  integer_init(a);
  return integer_set_sw(a, x);
}

s_integer * integer_init_u8 (s_integer *a, u8 x)
{
  assert(a);
  integer_init(a);
  return integer_set_u32(a, x);
}

s_integer * integer_init_u16 (s_integer *a, u16 x)
{
  assert(a);
  integer_init(a);
  return integer_set_u32(a, x);
}

s_integer * integer_init_u32 (s_integer *a, u32 x)
{
  assert(a);
  integer_init(a);
  return integer_set_u32(a, x);
}

s_integer * integer_init_u64 (s_integer *a, u64 x)
{
  assert(a);
  integer_init(a);
  return integer_set_u64(a, x);
}

s_integer * integer_init_uw (s_integer *a, uw x)
{
  assert(a);
  integer_init(a);
  return integer_set_uw(a, x);
}

s_integer * integer_init_zero (s_integer *dest)
{
  assert(dest);
  integer_init(dest);
  mp_zero(&dest->mp_int);
  return dest;
}

bool integer_is_negative (const s_integer *i)
{
  assert(i);
  return i->mp_int.sign == MP_NEG;
}

bool integer_is_positive (const s_integer *i)
{
  assert(i);
  return i->mp_int.used && i->mp_int.sign != MP_NEG;
}

bool integer_is_zero (const s_integer *i)
{
  assert(i);
  return ! i->mp_int.used;
}

s_integer * integer_lcm (const s_integer *a, const s_integer *b,
                         s_integer *dest)
{
  sw r;
  assert(a);
  assert(b);
  assert(dest);
  integer_init(dest);
  if ((r = mp_lcm(&a->mp_int, &b->mp_int, &dest->mp_int)) != MP_OKAY) {
    err_write_1("integer_lcm: ");
    err_puts(mp_error_to_string(r));
    assert(! "integer_lcm: mp_lcm");
    return NULL;
  }
  return dest;
}

s_integer * integer_lshift (const s_integer *a, sw b, s_integer *dest)
{
  sw r;
  assert(a);
  assert(dest);
  integer_init(dest);
  if ((r = mp_mul_2d(&a->mp_int, b, &dest->mp_int)) != MP_OKAY) {
    err_write_1("integer_lshift: ");
    err_puts(mp_error_to_string(r));
    assert(! "integer_lshift: mp_mul_2d");
    return NULL;
  }
  return dest;
}

s_integer * integer_mod (const s_integer *a, const s_integer *b,
                         s_integer *dest)
{
  sw r;
  assert(a);
  assert(b);
  assert(dest);
  integer_init(dest);
  if ((r = mp_mod(&a->mp_int, &b->mp_int, &dest->mp_int)) != MP_OKAY) {
    err_write_1("integer_mod: ");
    err_puts(mp_error_to_string(r));
    assert(! "integer_mod: mp_mod");
    return NULL;
  }
  return dest;
}

s_integer * integer_mul (const s_integer *a, const s_integer *b,
                         s_integer *dest)
{
  sw r;
  assert(a);
  assert(b);
  assert(dest);
  integer_init(dest);
  if ((r = mp_mul(&a->mp_int, &b->mp_int, &dest->mp_int)) != MP_OKAY) {
    err_write_1("integer_mul: ");
    err_puts(mp_error_to_string(r));
    assert(! "integer_mul: mp_mul");
    return NULL;
  }
  return dest;
}

s_integer * integer_neg (const s_integer *a, s_integer *dest)
{
  sw r;
  assert(a);
  assert(dest);
  integer_init(dest);
  if ((r = mp_neg(&a->mp_int, &dest->mp_int)) != MP_OKAY) {
    err_write_1("integer_neg: ");
    err_puts(mp_error_to_string(r));
    assert(! "integer_neg: mp_neg");
    return NULL;
  }
  return dest;
}

s_integer * integer_new (void)
{
  s_integer *a = NULL;
  a = alloc(sizeof(s_integer));
  if (! a)
    return NULL;
  if (! integer_init(a)) {
    free(a);
    return NULL;
  }
  return a;
}

s_integer * integer_new_copy (const s_integer *src)
{
  s_integer *a;
  assert(src);
  a = alloc(sizeof(s_integer));
  if (! a)
    return NULL;
  if (! integer_init_copy(a, src)) {
    free(a);
    return NULL;
  }
  return a;
}

s_integer * integer_pow (const s_integer *a, const s_integer *b,
                         s_integer *dest)
{
  sw r;
  assert(a);
  assert(b);
  assert(dest);
  integer_init(dest);
  // FIXME
  if ((r = mp_exptmod(&a->mp_int, &b->mp_int, &dest->mp_int, &dest->mp_int))
      != MP_OKAY) {
    err_write_1("integer_pow: ");
    err_puts(mp_error_to_string(r));
    assert(! "integer_pow: mp_exptmod");
    return NULL;
  }
  return dest;
}

s_integer * integer_set_f32 (s_integer *a, f32 x)
{
  sw r;
  assert(a);
  // FIXME
  if ((r = mp_set_double(&a->mp_int, x)) != MP_OKAY) {
    err_write_1("integer_set_f32: ");
    err_puts(mp_error_to_string(r));
    assert(! "integer_set_f32: mp_set_double");
    return NULL;
  }
  return a;
}

s_integer * integer_set_f64 (s_integer *a, f64 x)
{
  sw r;
  assert(a);
  if ((r = mp_set_double(&a->mp_int, x)) != MP_OKAY) {
    err_write_1("integer_set_f64: ");
    err_puts(mp_error_to_string(r));
    assert(! "integer_set_f64: mp_set_double");
    return NULL;
  }
  return a;
}

s_integer * integer_set_f128 (s_integer *a, f128 x)
{
  sw r;
  assert(a);
  // FIXME
  if ((r = mp_set_double(&a->mp_int, (f64) x)) != MP_OKAY) {
    err_write_1("integer_set_f128: ");
    err_puts(mp_error_to_string(r));
    assert(! "integer_set_f128: mp_set_double");
    return NULL;
  }
  return a;
}

s_integer * integer_set_s32 (s_integer *a, s32 x)
{
  assert(a);
  mp_set_i32(&a->mp_int, x);
  return a;
}

s_integer * integer_set_s64 (s_integer *a, s64 x)
{
  assert(a);
  mp_set_i64(&a->mp_int, x);
  return a;
}

s_integer * integer_set_ratio (s_integer *a, const s_ratio *r)
{
  assert(a);
  assert(r);
  return integer_div(&r->numerator, &r->denominator, a);
}

s_integer * integer_set_sw (s_integer *a, sw x)
{
  assert(a);
  mp_set_l(&a->mp_int, x);
  return a;
}

s_integer * integer_set_u32 (s_integer *a, u32 x)
{
  assert(a);
  mp_set_u32(&a->mp_int, x);
  return a;
}

s_integer * integer_set_u64 (s_integer *a, u64 x)
{
  assert(a);
  mp_set_u64(&a->mp_int, x);
  return a;
}

s_integer * integer_set_uw (s_integer *a, uw x)
{
  assert(a);
  mp_set_ul(&a->mp_int, x);
  return a;
}

s_tag * integer_sqrt (const s_integer *a, s_tag *dest)
{
  sw r;
  assert(a);
  assert(dest);
  if (integer_is_negative(a)) {
    // FIXME
    err_puts("integer_sqrt: not implemented");
    assert(! "integer_sqrt: not implemented");
    return NULL;
  }
  dest->type = TAG_INTEGER;
  integer_init(&dest->data.integer);
  if ((r = mp_sqrt(&a->mp_int, &dest->data.integer.mp_int)) != MP_OKAY) {
    err_write_1("integer_sqrt: ");
    err_puts(mp_error_to_string(r));
    assert(! "integer_sqrt: mp_sqrt");
    return NULL;
  }
  return dest;
}

s_integer * integer_sqrt_positive (const s_integer *a, s_integer *dest)
{
  sw r;
  assert(a);
  assert(dest);
  if (integer_is_negative(a)) {
    err_puts("integer_sqrt_positive: negative argument");
    assert(! "integer_sqrt_positive: negative argument");
    return NULL;
  }
  integer_init(dest);
  if ((r = mp_sqrt(&a->mp_int, &dest->mp_int)) != MP_OKAY) {
    err_write_1("integer_sqrt_positive: ");
    err_puts(mp_error_to_string(r));
    assert(! "integer_sqrt_positive: mp_sqrt");
    return NULL;
  }
  return dest;
}

s_integer * integer_sub (const s_integer *a, const s_integer *b,
                         s_integer *dest)
{
  sw r;
  assert(dest);
  assert(a);
  assert(b);
  integer_init(dest);
  if ((r = mp_sub(&a->mp_int, &b->mp_int, &dest->mp_int)) != MP_OKAY) {
    err_write_1("integer_sub: ");
    err_puts(mp_error_to_string(r));
    assert(! "integer_sub: mp_sub");
    return NULL;
  }
  return dest;
}

f32 integer_to_f32 (const s_integer *i)
{
  assert(i);
  return (f32) mp_get_double(&i->mp_int);
}

f64 integer_to_f64 (const s_integer *i)
{
  assert(i);
  return mp_get_double(&i->mp_int);
}

f128 integer_to_f128 (const s_integer *i)
{
  assert(i);
  // FIXME
  return mp_get_double(&i->mp_int);
}

s8 integer_to_s8 (const s_integer *i)
{
  assert(i);
  return (s8) mp_get_i32(&i->mp_int);
}

s16 integer_to_s16 (const s_integer *i)
{
  assert(i);
  return (s16) mp_get_i32(&i->mp_int);
}

s32 integer_to_s32 (const s_integer *i)
{
  assert(i);
  return mp_get_i32(&i->mp_int);
}

s64 integer_to_s64 (const s_integer *i)
{
  assert(i);
  return mp_get_i64(&i->mp_int);
}

sw integer_to_sw (const s_integer *i)
{
  assert(i);
  return (sw) mp_get_i64(&i->mp_int);
}

u8 integer_to_u8 (const s_integer *i)
{
  assert(i);
  return (u8) mp_get_u32(&i->mp_int);
}

u16 integer_to_u16 (const s_integer *i)
{
  assert(i);
  return (u16) mp_get_u32(&i->mp_int);
}

u32 integer_to_u32 (const s_integer *i)
{
  assert(i);
  return mp_get_u32(&i->mp_int);
}

u64 integer_to_u64 (const s_integer *i)
{
  assert(i);
  return mp_get_u64(&i->mp_int);
}

uw integer_to_uw (const s_integer *i)
{
  assert(i);
  return (uw) mp_get_u64(&i->mp_int);
}
