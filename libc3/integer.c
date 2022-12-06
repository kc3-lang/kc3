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
#include <stdlib.h>
#include "buf.h"
#include "buf_parse.h"
#include "integer.h"

s_integer * integer_abs (const s_integer *a, s_integer *dest)
{
  sw r;
  assert(a);
  assert(dest);
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
  if ((r = mp_xor(&a->mp_int, &b->mp_int, &dest->mp_int)) != MP_OKAY)
    errx(1, "integer_bxor: %s", mp_error_to_string(r));
  return dest;
}

uw integer_bytes (const s_integer *i)
{
  return (integer_bits(i) + 7) / 8;
}

void integer_clean (s_integer *dest)
{
  assert(dest);
  mp_clear(&dest->mp_int);
}

s8 integer_compare (const s_integer *a, const s_integer *b)
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

s8 integer_compare_s64 (const s_integer *a, s64 b)
{
  assert(a);
  if (b >= 0)
    return integer_compare_u64(a, b);
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

s8 integer_compare_u64 (const s_integer *a, u64 b)
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

s_integer * integer_copy (const s_integer *a, s_integer *dest)
{
  sw r;
  assert(a);
  assert(dest);
  if ((r = mp_copy(&a->mp_int, &dest->mp_int)) != MP_OKAY)
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
  if ((r = mp_gcd(&a->mp_int, &b->mp_int, &dest->mp_int)) != MP_OKAY)
    errx(1, "integer_gcd: %s", mp_error_to_string(r));
  return dest;
}

t_hash_context * integer_hash_update (t_hash_context *context,
                                      const s_integer *i)
{
  int j = 0;
  mp_digit *digit;
  assert(i);
  digit = i->mp_int.dp;
  while (j < i->mp_int.used) {
    hash_update(context, digit, sizeof(*digit));
    j++;
  }
  return context;
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

s_integer * integer_init_zero (s_integer *dest)
{
  assert(dest);
  integer_init(dest);
  mp_zero(&dest->mp_int);
  return dest;
}

e_bool integer_is_negative (const s_integer *i)
{
  assert(i);
  return i->mp_int.sign == MP_NEG;
}

e_bool integer_is_zero (const s_integer *i)
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
  if ((r = mp_lcm(&a->mp_int, &b->mp_int, &dest->mp_int)) != MP_OKAY)
    errx(1, "integer_lcm: %s", mp_error_to_string(r));
  return dest;
}

s_integer * integer_lshift (s_integer *a, sw b, s_integer *dest)
{
  sw r;
  assert(a);
  assert(dest);
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
  if ((r = mp_mul(&a->mp_int, &b->mp_int, &dest->mp_int)) != MP_OKAY)
    errx(1, "integer_mul: %s", mp_error_to_string(r));
  return dest;
}

s_integer * integer_neg (const s_integer *a, s_integer *dest)
{
  sw r;
  assert(a);
  assert(dest);
  if ((r = mp_neg(&a->mp_int, &dest->mp_int)) != MP_OKAY)
    errx(1, "integer_neg: %s", mp_error_to_string(r));
  return dest;
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
  if ((r = mp_exptmod(&a->mp_int, &b->mp_int, &dest->mp_int, &dest->mp_int))
      != MP_OKAY)
    errx(1, "integer_pow: %s", mp_error_to_string(r));
  return dest;
}

s_integer * integer_sqrt (const s_integer *a, s_integer *dest)
{
  sw r;
  assert(a);
  assert(dest);
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
  if ((r = mp_sub(&a->mp_int, &b->mp_int, &dest->mp_int)) != MP_OKAY)
    errx(1, "integer_sub: %s", mp_error_to_string(r));
  return dest;
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
