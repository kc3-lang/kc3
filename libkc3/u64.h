/* kc3
 * Copyright from 2022 to 2026 kmx.io <contact@kmx.io>
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
/* Gen from u.h.in BITS=64 bits=64 */
#ifndef LIBKC3_U64_H
#define LIBKC3_U64_H

#include "types.h"
#include "inline.h"

#define u64_clean(u) ((void) u)

u64 * u64_init_1 (u64 *u, const char *p);
u64 * u64_init_cast
(u64 *u, const s_sym * const *type, const s_tag *tag);

u64 * u64_init_copy (u64 *u, u64 src);
u64 * u64_init_str (u64 *u, const s_str *str);
u64 * u64_init_str_hexadecimal (u64 *u, const s_str *str);

uw *      u64_hash_uw (u64 u, uw *dest);
u64 * u64_random (u64 *u);
u64 * u64_random_uniform (u64 *u, u64 max);
s_tag *   u64_sqrt (const u64 x, s_tag *dest);

/* Inline functions. */

INLINE void u64_mul (u64 *a, u64 *b) // rapid_mum
{
#if defined(__SIZEOF_INT128__)
  __uint128_t r = *a; r *= *b;
  *a = (u64) r; *b = (u64) (r >> 64);
#elif defined(_MSC_VER) && (defined(_WIN64) || defined(_M_HYBRID_CHPE_ARM64))
# if defined(_M_X64)
  *a = _umul128(*a, *b, b);
# else
  u64 c;
  c = __umulh(*a, *b);
  *a = *a * *b;
  *b = c;
# endif
#else
  u64 ha;
  u64 hb;
  u64 hi;
  u64 la;
  u64 lb;
  u64 lo;
  u64 rh;
  u64 rl;
  u64 rm0;
  u64 rm1;
  ha = *a >> 32;
  hb = *b >> 32;
  la = (u32) *a;
  lb = (u32) *b;
  rh = ha * hb;
  rm0 = ha * lb;
  rm1 = hb * la;
  rl = la * lb;
  t = rl + (rm0 << 32);
  c = t < rl;
  lo = t + (rm1 << 32);
  c += lo < t;
  hi = rh + (rm0 >> 32) + (rm1 >> 32) + c;
  *a = lo;
  *b = hi;
#endif
}

INLINE u64 u64_mix (u64 a, u64 b) // rapid_mix
{
  u64_mul(&a, &b);
  return a ^ b;
}

#endif /* LIBKC3_U64_H */
