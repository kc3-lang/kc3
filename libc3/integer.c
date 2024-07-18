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
#include "buf.h"
#include "buf_parse.h"
#include "compare.h"
#include "integer.h"
#include "tag.h"

s_integer * integer_abs (const s_integer *a, s_integer *dest)
{
  sw r;
  assert(a);
  assert(dest);
  integer_init(dest);
  if ((r = mp_abs(&a->mp_int, &dest->mp_int)) != MP_OKAY)
    errx(1, "integer_abs: %s", mp_error_to_string(r));
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
  if ((r = mp_add(&a->mp_int, &b->mp_int, &dest->mp_int)) != MP_OKAY)
    errx(1, "integer_add: %s", mp_error_to_string(r));
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
  if ((r = mp_and(&a->mp_int, &b->mp_int, &dest->mp_int)) != MP_OKAY)
    errx(1, "integer_band: %s", mp_error_to_string(r));
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
  if ((r = mp_complement(&a->mp_int, &dest->mp_int)) != MP_OKAY)
    errx(1, "integer_bnot: %s", mp_error_to_string(r));
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
  if ((r = mp_or(&a->mp_int, &b->mp_int, &dest->mp_int)) != MP_OKAY)
    errx(1, "integer_bor: %s", mp_error_to_string(r));
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
  if ((r = mp_xor(&a->mp_int, &b->mp_int, &dest->mp_int)) != MP_OKAY)
    errx(1, "integer_bxor: %s", mp_error_to_string(r));
  return dest;
}

uw integer_bytes (const s_integer *i)
{
  return (integer_bits(i) + 7) / 8;
}

s_integer * integer_cast (const s_tag *tag, s_integer *dest)
{
  switch (tag->type) {
  case TAG_VOID:
    return integer_init_zero(dest);
  case TAG_ARRAY:
    goto ko;
  case TAG_BOOL:
    return integer_init_u8(dest, tag->data.bool ? 1 : 0);
  case TAG_CALL:
    goto ko;
  case TAG_CFN:
    goto ko;
  case TAG_CHARACTER:
    return integer_init_u64(dest, tag->data.character);
  case TAG_F32:
    return integer_init_f32(dest, tag->data.f32);
  case TAG_F64:
    return integer_init_f64(dest, tag->data.f64);
  case TAG_FACT:
  case TAG_FN:
  case TAG_IDENT:
    goto ko;
  case TAG_INTEGER:
    return integer_copy(&tag->data.integer, dest);
  case TAG_SW:
    return integer_init_sw(dest, tag->data.sw);
  case TAG_S64:
    return integer_init_s64(dest, tag->data.s64);
  case TAG_S32:
    return integer_init_s32(dest, tag->data.s32);
  case TAG_S16:
    return integer_init_s16(dest, tag->data.s16);
  case TAG_S8:
    return integer_init_s8(dest, tag->data.s8);
  case TAG_U8:
    return integer_init_u8(dest, tag->data.u8);
  case TAG_U16:
    return integer_init_u16(dest, tag->data.u16);
  case TAG_U32:
    return integer_init_u32(dest, tag->data.u32);
  case TAG_U64:
    return integer_init_u64(dest, tag->data.u64);
  case TAG_UW:
    return integer_init_uw(dest, tag->data.uw);
  case TAG_LIST:
  case TAG_PTAG:
  case TAG_PTR:
  case TAG_QUOTE:
  case TAG_STR:
  case TAG_SYM:
  case TAG_TUPLE:
  case TAG_VAR:
    goto ko;
  }
  assert(! "u8_cast: unknown tag type");
  errx(1, "u8_cast: unknown tag type: %d", tag->type);
  return 0;
 ko:
  warnx("u8_cast: cannot cast %s to u8",
        tag_type_to_sym(tag->type)->str.ptr.ps8);
  return 0;
}

void integer_clean (s_integer *dest)
{
  assert(dest);
  mp_clear(&dest->mp_int);
}

s_integer * integer_copy (const s_integer *a, s_integer *dest)
{
  sw r;
  assert(a);
  assert(dest);
  if ((r = mp_init_copy(&dest->mp_int, &a->mp_int)) != MP_OKAY)
    errx(1, "integer_copy: %s", mp_error_to_string(r));
  return dest;
}

s_integer * integer_div (const s_integer *a, const s_integer *b,
                         s_integer *dest)
{
  sw r;
  assert(a);
  assert(b);
  assert(dest);
  integer_init(dest);
  if ((r = mp_div(&a->mp_int, &b->mp_int, &dest->mp_int,
                  NULL)) != MP_OKAY)
    errx(1, "integer_div: %s", mp_error_to_string(r));
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
  if ((r = mp_gcd(&a->mp_int, &b->mp_int, &dest->mp_int)) != MP_OKAY)
    errx(1, "integer_gcd: %s", mp_error_to_string(r));
  return dest;
}

s_integer * integer_init (s_integer *dest)
{
  sw r;
  assert(dest);
  if ((r = mp_init(&dest->mp_int)) != MP_OKAY)
    errx(1, "integer_init: %s", mp_error_to_string(r));
  return dest;
}

s_integer * integer_init_1 (s_integer *i, const s8 *p)
{
  s_buf buf;
  assert(i);
  assert(p);
  buf_init_1(&buf, p);
  buf_parse_integer(&buf, i);
  buf_clean(&buf);
  return i;
}

s_integer * integer_init_copy (s_integer *a, const s_integer *src)
{
  sw r;
  assert(a);
  assert(src);
  if ((r = mp_init_copy(&a->mp_int, &src->mp_int)) != MP_OKAY)
    errx(1, "integer_init_copy: %s", mp_error_to_string(r));
  return a;
}

s_integer * integer_init_f64 (s_integer *a, f64 x)
{
  assert(a);
  integer_init(a);
  return integer_set_double(a, x);
}

s_integer * integer_init_f32 (s_integer *a, f32 x)
{
  assert(a);
  integer_init(a);
  return integer_set_double(a, x);
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

bool integer_is_zero (const s_integer *i)
{
  assert(i);
  return (i->mp_int.used == 0);
}

s_integer * integer_lcm (const s_integer *a, const s_integer *b,
                         s_integer *dest)
{
  sw r;
  assert(a);
  assert(b);
  assert(dest);
  integer_init(dest);
  if ((r = mp_lcm(&a->mp_int, &b->mp_int, &dest->mp_int)) != MP_OKAY)
    errx(1, "integer_lcm: %s", mp_error_to_string(r));
  return dest;
}

s_integer * integer_lshift (const s_integer *a, sw b, s_integer *dest)
{
  sw r;
  assert(a);
  assert(dest);
  integer_init(dest);
  if ((r = mp_mul_2d(&a->mp_int, b, &dest->mp_int)) != MP_OKAY)
    errx(1, "integer_lshift: %s", mp_error_to_string(r));
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
  if ((r = mp_mod(&a->mp_int, &b->mp_int, &dest->mp_int)) != MP_OKAY)
    errx(1, "integer_mod: %s", mp_error_to_string(r));
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
  if ((r = mp_mul(&a->mp_int, &b->mp_int, &dest->mp_int)) != MP_OKAY)
    errx(1, "integer_mul: %s", mp_error_to_string(r));
  return dest;
}

s_integer * integer_neg (const s_integer *a, s_integer *dest)
{
  sw r;
  assert(a);
  assert(dest);
  integer_init(dest);
  if ((r = mp_neg(&a->mp_int, &dest->mp_int)) != MP_OKAY)
    errx(1, "integer_neg: %s", mp_error_to_string(r));
  return dest;
}

s_integer * integer_new (void)
{
  s_integer *i = NULL;
  if (! (i = malloc(sizeof(s_integer)))) {
    err(1, "integer_new: out of memory");
  }
  return integer_init(i);
}

s_integer * integer_new_copy (const s_integer *a)
{
  s_integer *dest;
  assert(a);
  dest = malloc(sizeof(s_integer));
  if (!dest)
    errx(1, "integer_new_copy: out of memory");
  return integer_copy(a, dest);
}

s_integer * integer_pow (const s_integer *a, const s_integer *b,
                         s_integer *dest)
{
  sw r;
  assert(a);
  assert(b);
  assert(dest);
  integer_init(dest);
  if ((r = mp_exptmod(&a->mp_int, &b->mp_int, &dest->mp_int, &dest->mp_int))
      != MP_OKAY)
    errx(1, "integer_pow: %s", mp_error_to_string(r));
  return dest;
}

s_integer * integer_set_double (s_integer *a, double x)
{
  sw r;
  assert(a);
  if ((r = mp_set_double(&a->mp_int, x)) != MP_OKAY)
    errx(1, "integer_set_double: %s", mp_error_to_string(r));
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

s_integer * integer_sqrt (const s_integer *a, s_integer *dest)
{
  sw r;
  assert(a);
  assert(dest);
  integer_init(dest);
  if ((r = mp_sqrt(&a->mp_int, &dest->mp_int)) != MP_OKAY)
    errx(1, "integer_sqrt: %s", mp_error_to_string(r));
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
  if ((r = mp_sub(&a->mp_int, &b->mp_int, &dest->mp_int)) != MP_OKAY)
    errx(1, "integer_sub: %s", mp_error_to_string(r));
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
