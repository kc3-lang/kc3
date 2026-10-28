#include <limits.h>
#include "assert.h"
#include "integer.h"
#include "ratio.h"

#define RATIO_POW_TRY(call)                                             \
  do {                                                                 \
    if ((e = (call)) != MP_OKAY)                                       \
      goto clean;                                                      \
  } while (0)

static bool ratio_pow_is_odd (const mp_int *a)
{
  return a->used > 0 && (a->dp[0] & 1);
}

static mp_err ratio_pow_shift (const mp_int *a, int bits, mp_int *dest)
{
  if (bits < 0 || mp_count_bits(a) > INT_MAX - bits)
    return MP_VAL;
  if (MP_IS_ZERO(a)) {
    mp_zero(dest);
    return MP_OKAY;
  }
  return mp_mul_2d(a, bits, dest);
}

static mp_err ratio_pow_product (const mp_int *a, const mp_int *b,
                                 mp_int *dest)
{
  if (mp_count_bits(a) > INT_MAX - mp_count_bits(b))
    return MP_VAL;
  return mp_mul(a, b, dest);
}

static mp_err ratio_pow_div_ceil (const mp_int *a, const mp_int *b,
                                  mp_int *dest)
{
  mp_err e;
  mp_int remainder;
  if ((e = mp_init(&remainder)) != MP_OKAY)
    return e;
  RATIO_POW_TRY(mp_div(a, b, dest, &remainder));
  if (! MP_IS_ZERO(&remainder))
    RATIO_POW_TRY(mp_add_d(dest, 1, dest));
 clean:
  mp_clear(&remainder);
  return e;
}

static mp_err ratio_pow_shr (const mp_int *a, int bits, bool up,
                             mp_int *dest)
{
  mp_err e;
  mp_int remainder;
  if (MP_IS_ZERO(a)) {
    mp_zero(dest);
    return MP_OKAY;
  }
  if ((e = mp_init(&remainder)) != MP_OKAY)
    return e;
  RATIO_POW_TRY(mp_div_2d(a, bits, dest, &remainder));
  if (up && ! MP_IS_ZERO(&remainder))
    RATIO_POW_TRY(mp_add_d(dest, 1, dest));
 clean:
  mp_clear(&remainder);
  return e;
}

static mp_err ratio_pow_mul_scaled (const mp_int *a, const mp_int *b,
                                    int bits, bool up, mp_int *dest)
{
  mp_err e;
  mp_int product;
  if ((e = mp_init(&product)) != MP_OKAY)
    return e;
  RATIO_POW_TRY(ratio_pow_product(a, b, &product));
  RATIO_POW_TRY(ratio_pow_shr(&product, bits, up, dest));
 clean:
  mp_clear(&product);
  return e;
}

static mp_err ratio_pow_integer (const mp_int *a, const mp_int *b,
                                 mp_int *dest)
{
  mp_int base;
  int bit;
  mp_err e;
  if ((e = mp_init_copy(&base, a)) != MP_OKAY)
    return e;
  mp_set(dest, 1);
  if (mp_cmp_d(&base, 1) == MP_EQ)
    goto clean;
  for (bit = mp_count_bits(b) - 1; bit >= 0; bit--) {
    RATIO_POW_TRY(ratio_pow_product(dest, dest, dest));
    if ((b->dp[bit / MP_DIGIT_BIT] >> (bit % MP_DIGIT_BIT)) & 1)
      RATIO_POW_TRY(ratio_pow_product(dest, &base, dest));
  }
 clean:
  mp_clear(&base);
  return e;
}

static mp_err ratio_pow_root (const mp_int *a, const mp_int *degree,
                              mp_int *dest, bool *exact)
{
  mp_int check;
  mp_err e;
  *exact = false;
  if (mp_cmp_d(degree, 1) == MP_EQ || mp_cmp_d(a, 1) != MP_GT) {
    e = mp_copy(a, dest);
    *exact = e == MP_OKAY;
    return e;
  }
  if ((e = mp_init(&check)) != MP_OKAY)
    return e;
  mp_set_u32(&check, mp_count_bits(a));
  if (mp_cmp(degree, &check) == MP_GT)
    goto clean;
  RATIO_POW_TRY(mp_root_u32(a, mp_get_u32(degree), dest));
  RATIO_POW_TRY(mp_expt_u32(dest, mp_get_u32(degree), &check));
  *exact = mp_cmp(a, &check) == MP_EQ;
 clean:
  mp_clear(&check);
  return e;
}

static mp_err ratio_pow_log (const mp_int *a, const mp_int *b, int bits,
                             mp_int *lo, mp_int *hi)
{
  mp_int denominator;
  mp_err e;
  mp_int n;
  mp_int power_lo;
  mp_int power_hi;
  mp_int tmp;
  mp_int z_lo;
  mp_int z_hi;
  mp_int z2_lo;
  mp_int z2_hi;
  e = mp_init_multi(&denominator, &n, &power_lo, &power_hi, &tmp,
                    &z_lo, &z_hi, &z2_lo, &z2_hi, NULL);
  if (e != MP_OKAY)
    return e;
  RATIO_POW_TRY(mp_sub(a, b, &tmp));
  RATIO_POW_TRY(ratio_pow_shift(&tmp, bits, &tmp));
  RATIO_POW_TRY(mp_add(a, b, &denominator));
  RATIO_POW_TRY(mp_div(&tmp, &denominator, &z_lo, NULL));
  RATIO_POW_TRY(ratio_pow_div_ceil(&tmp, &denominator, &z_hi));
  RATIO_POW_TRY(ratio_pow_mul_scaled(&z_lo, &z_lo, bits, false,
                                    &z2_lo));
  RATIO_POW_TRY(ratio_pow_mul_scaled(&z_hi, &z_hi, bits, true,
                                    &z2_hi));
  RATIO_POW_TRY(mp_copy(&z_lo, &power_lo));
  RATIO_POW_TRY(mp_copy(&z_hi, &power_hi));
  mp_zero(lo);
  mp_zero(hi);
  mp_set(&n, 1);
  while (mp_cmp_d(&power_hi, 1) == MP_GT) {
    RATIO_POW_TRY(mp_div(&power_lo, &n, &tmp, NULL));
    RATIO_POW_TRY(mp_mul_d(&tmp, 2, &tmp));
    RATIO_POW_TRY(mp_add(lo, &tmp, lo));
    RATIO_POW_TRY(ratio_pow_div_ceil(&power_hi, &n, &tmp));
    RATIO_POW_TRY(mp_mul_d(&tmp, 2, &tmp));
    RATIO_POW_TRY(mp_add(hi, &tmp, hi));
    RATIO_POW_TRY(ratio_pow_mul_scaled(&power_lo, &z2_lo, bits,
                                      false, &power_lo));
    RATIO_POW_TRY(ratio_pow_mul_scaled(&power_hi, &z2_hi, bits,
                                      true, &power_hi));
    RATIO_POW_TRY(mp_add_d(&n, 2, &n));
  }
  RATIO_POW_TRY(mp_mul_d(&power_hi, 3, &tmp));
  RATIO_POW_TRY(mp_add(hi, &tmp, hi));
 clean:
  mp_clear_multi(&denominator, &n, &power_lo, &power_hi, &tmp,
                 &z_lo, &z_hi, &z2_lo, &z2_hi, NULL);
  return e;
}

static mp_err ratio_pow_exp (const mp_int *a_lo, const mp_int *a_hi,
                             int bits, mp_int *lo, mp_int *hi)
{
  mp_int argument_lo;
  mp_int argument_hi;
  mp_err e;
  mp_int n;
  int squarings;
  mp_int term_lo;
  mp_int term_hi;
  mp_int tmp;
  e = mp_init_multi(&argument_lo, &argument_hi, &n, &term_lo, &term_hi,
                    &tmp, NULL);
  if (e != MP_OKAY)
    return e;
  squarings = mp_count_bits(a_hi) - bits + 1;
  if (squarings < 0)
    squarings = 0;
  RATIO_POW_TRY(ratio_pow_shr(a_lo, squarings, false, &argument_lo));
  RATIO_POW_TRY(ratio_pow_shr(a_hi, squarings, true, &argument_hi));
  RATIO_POW_TRY(mp_2expt(lo, bits));
  RATIO_POW_TRY(mp_copy(lo, hi));
  RATIO_POW_TRY(mp_copy(lo, &term_lo));
  RATIO_POW_TRY(mp_copy(lo, &term_hi));
  mp_set(&n, 1);
  while (true) {
    RATIO_POW_TRY(ratio_pow_mul_scaled(&term_lo, &argument_lo, bits,
                                      false, &term_lo));
    RATIO_POW_TRY(mp_div(&term_lo, &n, &term_lo, NULL));
    RATIO_POW_TRY(ratio_pow_mul_scaled(&term_hi, &argument_hi, bits,
                                      true, &term_hi));
    RATIO_POW_TRY(ratio_pow_div_ceil(&term_hi, &n, &term_hi));
    if (mp_cmp_d(&term_hi, 1) != MP_GT) {
      RATIO_POW_TRY(mp_mul_d(&term_hi, 2, &tmp));
      RATIO_POW_TRY(mp_add(hi, &tmp, hi));
      break;
    }
    RATIO_POW_TRY(mp_add(lo, &term_lo, lo));
    RATIO_POW_TRY(mp_add(hi, &term_hi, hi));
    RATIO_POW_TRY(mp_add_d(&n, 1, &n));
  }
  while (squarings--) {
    RATIO_POW_TRY(ratio_pow_mul_scaled(lo, lo, bits, false, lo));
    RATIO_POW_TRY(ratio_pow_mul_scaled(hi, hi, bits, true, hi));
  }
 clean:
  mp_clear_multi(&argument_lo, &argument_hi, &n, &term_lo, &term_hi,
                 &tmp, NULL);
  return e;
}

static mp_err ratio_pow_approx (const s_ratio *a, const s_ratio *b,
                                int precision, s_ratio *dest)
{
  int bits;
  mp_int denominator;
  mp_err e;
  mp_int exponent_hi;
  mp_int exponent_lo;
  int extra = mp_count_bits(&b->numerator.in_int);
  int k;
  mp_int log_hi;
  mp_int log_lo;
  mp_int log2_hi;
  mp_int log2_lo;
  mp_int tmp;
  if (extra > INT_MAX - 64 || precision > INT_MAX - extra - 64)
    return MP_VAL;
  bits = precision + extra + 64;
  e = mp_init_multi(&denominator, &exponent_hi, &exponent_lo, &log_hi,
                    &log_lo, &log2_hi, &log2_lo, &tmp, NULL);
  if (e != MP_OKAY)
    return e;
  k = mp_count_bits(&a->numerator.in_int) -
    mp_count_bits(&a->denominator.in_int);
  RATIO_POW_TRY(ratio_pow_shift(&a->denominator.in_int, k,
                                &denominator));
  if (mp_cmp(&a->numerator.in_int, &denominator) == MP_LT) {
    k--;
    RATIO_POW_TRY(ratio_pow_shr(&denominator, 1, false,
                                &denominator));
  }
  while (true) {
    RATIO_POW_TRY(ratio_pow_log(&a->numerator.in_int, &denominator,
                                bits, &log_lo, &log_hi));
    if (k) {
      mp_set(&tmp, 2);
      mp_set(&log2_lo, 1);
      RATIO_POW_TRY(ratio_pow_log(&tmp, &log2_lo, bits, &log2_lo,
                                  &log2_hi));
      mp_set_u32(&tmp, k);
      RATIO_POW_TRY(ratio_pow_product(&log2_lo, &tmp, &log2_lo));
      RATIO_POW_TRY(ratio_pow_product(&log2_hi, &tmp, &log2_hi));
      RATIO_POW_TRY(mp_add(&log_lo, &log2_lo, &log_lo));
      RATIO_POW_TRY(mp_add(&log_hi, &log2_hi, &log_hi));
    }
    RATIO_POW_TRY(ratio_pow_product(&log_lo, &b->numerator.in_int,
                                    &tmp));
    RATIO_POW_TRY(mp_div(&tmp, &b->denominator.in_int, &exponent_lo,
                         NULL));
    RATIO_POW_TRY(ratio_pow_product(&log_hi, &b->numerator.in_int,
                                    &tmp));
    RATIO_POW_TRY(ratio_pow_div_ceil(&tmp, &b->denominator.in_int,
                                    &exponent_hi));
    RATIO_POW_TRY(ratio_pow_exp(&exponent_lo, &exponent_hi, bits,
                                &log_lo, &log_hi));
    RATIO_POW_TRY(mp_sub(&log_hi, &log_lo, &tmp));
    RATIO_POW_TRY(ratio_pow_shift(&tmp, precision, &tmp));
    if (mp_cmp(&tmp, &log_lo) != MP_GT)
      break;
    if (bits > INT_MAX / 2) {
      e = MP_VAL;
      goto clean;
    }
    bits *= 2;
  }
  RATIO_POW_TRY(mp_copy(&log_lo, &dest->numerator.in_int));
  RATIO_POW_TRY(mp_2expt(&dest->denominator.in_int, bits));
 clean:
  mp_clear_multi(&denominator, &exponent_hi, &exponent_lo, &log_hi,
                 &log_lo, &log2_hi, &log2_lo, &tmp, NULL);
  return e;
}

s_ratio * ratio_pow (const s_ratio *a, const s_ratio *b,
                     uw precision_bits, s_ratio *dest)
{
  s_ratio base = {0};
  mp_err e;
  s_ratio exponent = {0};
  mp_int gcd = {0};
  const char *message = NULL;
  bool negative;
  bool numerator_exact;
  bool denominator_exact;
  bool reciprocal = false;
  s_ratio *result = NULL;
  s_ratio tmp = {0};
  assert(a);
  assert(b);
  assert(dest);
  if (! precision_bits || precision_bits > INT_MAX) {
    err_puts("ratio_pow: invalid precision_bits");
    return NULL;
  }
  if (! integer_is_positive(&a->denominator) ||
      ! integer_is_positive(&b->denominator)) {
    err_puts("ratio_pow: nonpositive denominator");
    return NULL;
  }
  e = mp_init_multi(&base.numerator.in_int, &base.denominator.in_int,
                    &exponent.numerator.in_int,
                    &exponent.denominator.in_int,
                    &tmp.numerator.in_int, &tmp.denominator.in_int,
                    &gcd, NULL);
  if (e != MP_OKAY)
    goto clean;
  RATIO_POW_TRY(mp_abs(&b->numerator.in_int, &exponent.numerator.in_int));
  RATIO_POW_TRY(mp_gcd(&exponent.numerator.in_int,
                       &b->denominator.in_int, &gcd));
  RATIO_POW_TRY(mp_div(&exponent.numerator.in_int, &gcd,
                       &exponent.numerator.in_int, NULL));
  RATIO_POW_TRY(mp_div(&b->denominator.in_int, &gcd,
                       &exponent.denominator.in_int, NULL));
  negative = integer_is_negative(&a->numerator) &&
    ratio_pow_is_odd(&exponent.numerator.in_int);
  if (integer_is_zero(&exponent.numerator)) {
    mp_set(&tmp.numerator.in_int, 1);
    mp_set(&tmp.denominator.in_int, 1);
    goto done;
  }
  if (integer_is_zero(&a->numerator)) {
    if (integer_is_negative(&b->numerator)) {
      message = "zero base with negative exponent";
      e = MP_VAL;
      goto clean;
    }
    mp_set(&tmp.denominator.in_int, 1);
    goto done;
  }
  if (integer_is_negative(&a->numerator) &&
      ! ratio_pow_is_odd(&exponent.denominator.in_int)) {
    message = "non-real result";
    e = MP_VAL;
    goto clean;
  }
  RATIO_POW_TRY(mp_abs(&a->numerator.in_int, &base.numerator.in_int));
  RATIO_POW_TRY(mp_gcd(&base.numerator.in_int, &a->denominator.in_int,
                       &gcd));
  RATIO_POW_TRY(mp_div(&base.numerator.in_int, &gcd,
                       &base.numerator.in_int, NULL));
  RATIO_POW_TRY(mp_div(&a->denominator.in_int, &gcd,
                       &base.denominator.in_int, NULL));
  if (integer_is_negative(&b->numerator))
    mp_exch(&base.numerator.in_int, &base.denominator.in_int);
  RATIO_POW_TRY(ratio_pow_root(&base.numerator.in_int,
                               &exponent.denominator.in_int,
                               &tmp.numerator.in_int,
                               &numerator_exact));
  RATIO_POW_TRY(ratio_pow_root(&base.denominator.in_int,
                               &exponent.denominator.in_int,
                               &tmp.denominator.in_int,
                               &denominator_exact));
  if (numerator_exact && denominator_exact) {
    RATIO_POW_TRY(ratio_pow_integer(&tmp.numerator.in_int,
                                    &exponent.numerator.in_int,
                                    &tmp.numerator.in_int));
    RATIO_POW_TRY(ratio_pow_integer(&tmp.denominator.in_int,
                                    &exponent.numerator.in_int,
                                    &tmp.denominator.in_int));
  }
  else {
    if (mp_cmp(&base.numerator.in_int, &base.denominator.in_int) ==
        MP_LT) {
      mp_exch(&base.numerator.in_int, &base.denominator.in_int);
      reciprocal = true;
    }
    RATIO_POW_TRY(ratio_pow_approx(&base, &exponent, precision_bits,
                                  &tmp));
    if (reciprocal)
      mp_exch(&tmp.numerator.in_int, &tmp.denominator.in_int);
  }
 done:
  if (negative)
    RATIO_POW_TRY(mp_neg(&tmp.numerator.in_int,
                        &tmp.numerator.in_int));
  RATIO_POW_TRY(mp_gcd(&tmp.numerator.in_int, &tmp.denominator.in_int,
                       &gcd));
  RATIO_POW_TRY(mp_div(&tmp.numerator.in_int, &gcd,
                       &tmp.numerator.in_int, NULL));
  RATIO_POW_TRY(mp_div(&tmp.denominator.in_int, &gcd,
                       &tmp.denominator.in_int, NULL));
  *dest = tmp;
  tmp = (s_ratio) {0};
  result = dest;
 clean:
  if (e != MP_OKAY) {
    err_write_1("ratio_pow: ");
    err_puts(message ? message : mp_error_to_string(e));
  }
  ratio_clean(&base);
  ratio_clean(&exponent);
  ratio_clean(&tmp);
  mp_clear(&gcd);
  return result;
}

#undef RATIO_POW_TRY
