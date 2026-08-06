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
#ifndef LIBKC3_RAPIDHASH_H
#define LIBKC3_RAPIDHASH_H

#include "endian.h"
#include "inline.h"
#include "likely.h"

static const u64 rapidhash_secret[8] = {
  0x2d358dccaa6c78a5ull,
  0x8bb84b93962eacc9ull,
  0x4b33a62ed433d4a3ull,
  0x4d5a2da51de1aa47ull,
  0xa0761d6478bd642full,
  0xe7037ed1a0b428dbull,
  0x90ed1765281c388cull,
  0xaaaaaaaaaaaaaaaaull};

INLINE u64 rapidhash_internal (const void *key, size_t len, u64 seed, const u64 *secret)
{
  const u8 *p = (const u8 *) key;
  seed ^= u64_mix(seed ^ secret[2], secret[1]);
  u64 a=0, b=0;
  uw i = len;
  if (likely(len <= 16)) {
    if (len >= 4) {
      seed ^= len;
      if (len >= 8) {
        const u8 *plast = p + len - 8;
        a = le64(p);
        b = le64(plast);
      } else {
        const u8 *plast = p + len - 4;
        a = le32(p);
        b = le32(plast);
      }
    } else if (len > 0) {
      a = (((u64)p[0])<<45)|p[len-1];
      b = p[len>>1];
    } else
      a = b = 0;
  } else {
    if (len > 112) {
      u64 see1 = seed, see2 = seed;
      u64 see3 = seed, see4 = seed;
      u64 see5 = seed, see6 = seed;
      while (i > 224) {
        seed = u64_mix(le64(p) ^ secret[0], le64(p + 8) ^ seed);
        see1 = u64_mix(le64(p + 16) ^ secret[1], le64(p + 24) ^ see1);
        see2 = u64_mix(le64(p + 32) ^ secret[2], le64(p + 40) ^ see2);
        see3 = u64_mix(le64(p + 48) ^ secret[3], le64(p + 56) ^ see3);
        see4 = u64_mix(le64(p + 64) ^ secret[4], le64(p + 72) ^ see4);
        see5 = u64_mix(le64(p + 80) ^ secret[5], le64(p + 88) ^ see5);
        see6 = u64_mix(le64(p + 96) ^ secret[6], le64(p + 104) ^ see6);
        seed = u64_mix(le64(p + 112) ^ secret[0], le64(p + 120) ^ seed);
        see1 = u64_mix(le64(p + 128) ^ secret[1], le64(p + 136) ^ see1);
        see2 = u64_mix(le64(p + 144) ^ secret[2], le64(p + 152) ^ see2);
        see3 = u64_mix(le64(p + 160) ^ secret[3], le64(p + 168) ^ see3);
        see4 = u64_mix(le64(p + 176) ^ secret[4], le64(p + 184) ^ see4);
        see5 = u64_mix(le64(p + 192) ^ secret[5], le64(p + 200) ^ see5);
        see6 = u64_mix(le64(p + 208) ^ secret[6], le64(p + 216) ^ see6);
        p += 224;
        i -= 224;
      }
      if (i > 112) {
        seed = u64_mix(le64(p) ^ secret[0], le64(p + 8) ^ seed);
        see1 = u64_mix(le64(p + 16) ^ secret[1], le64(p + 24) ^ see1);
        see2 = u64_mix(le64(p + 32) ^ secret[2], le64(p + 40) ^ see2);
        see3 = u64_mix(le64(p + 48) ^ secret[3], le64(p + 56) ^ see3);
        see4 = u64_mix(le64(p + 64) ^ secret[4], le64(p + 72) ^ see4);
        see5 = u64_mix(le64(p + 80) ^ secret[5], le64(p + 88) ^ see5);
        see6 = u64_mix(le64(p + 96) ^ secret[6], le64(p + 104) ^ see6);
        p += 112;
        i -= 112;
      }
      seed ^= see1;
      see2 ^= see3;
      see4 ^= see5;
      seed ^= see6;
      see2 ^= see4;
      seed ^= see2;
    }
    if (i > 16) {
      seed = u64_mix(le64(p) ^ secret[2], le64(p + 8) ^ seed);
      if (i > 32) {
        seed = u64_mix(le64(p + 16) ^ secret[2], le64(p + 24) ^ seed);
        if (i > 48) {
          seed = u64_mix(le64(p + 32) ^ secret[1], le64(p + 40) ^ seed);
          if (i > 64) {
            seed = u64_mix(le64(p + 48) ^ secret[1], le64(p + 56) ^ seed);
            if (i > 80) {
              seed = u64_mix(le64(p + 64) ^ secret[2], le64(p + 72) ^ seed);
              if (i > 96) {
                seed = u64_mix(le64(p + 80) ^ secret[1], le64(p + 88) ^ seed);
              }
            }
          }
        }
      }
    }
    a = le64(p + i - 16) ^ i;
    b = le64(p + i - 8);
  }
  a ^= secret[1];
  b ^= seed;
  u64_mul(&a, &b);
  return u64_mix(a ^ secret[7], b ^ secret[1] ^ i);
}

INLINE u64 rapidhash_update (const void *p, size_t len, u64 *seed)
{
  return *seed = rapidhash_internal(p, len, *seed, rapidhash_secret);
}

INLINE u64 rapidhash (const void *p, uw len)
{
  return rapidhash_update(p, len, 0);
}

#endif /* LIBKC3_RAPIDHASH_H */
