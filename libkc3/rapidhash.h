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

INLINE u64 rapidhash_internal (const void *key, uw len, u64 seed, const u64 *secret)
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

INLINE u64 rapidhash_seeded (const void *p, uw len, u64 seed)
{
  return rapidhash_internal(p, len, seed, rapidhash_secret);
}

INLINE u64 rapidhash (const void *p, uw len)
{
  return rapidhash_seeded(p, len, 0);
}

INLINE uw rapidhash_uw (const void *p, uw len)
{
  return rapidhash_seeded(p, len, 0);
}

INLINE s_hash_rapid * hash_rapid_init_seeded (s_hash_rapid *hash,
                                               u64 seed)
{
  uw i;

  seed ^= u64_mix(seed ^ rapidhash_secret[2], rapidhash_secret[1]);
  hash->seed = seed;
  i = 0;
  while (i < 6) {
    hash->see[i] = seed;
    i++;
  }
  hash->pending_size = 0;
  hash->tail_size = 0;
  hash->wide = false;
  return hash;
}

INLINE s_hash_rapid * hash_rapid_init (s_hash_rapid *hash)
{
  return hash_rapid_init_seeded(hash, 0);
}

INLINE void hash_rapid_clean (s_hash_rapid *hash)
{
  memset(hash, 0, sizeof(*hash));
}

INLINE void hash_rapid_block (s_hash_rapid *hash, const u8 *p)
{
  hash->seed = u64_mix(le64(p) ^ rapidhash_secret[0],
                       le64(p + 8) ^ hash->seed);
  hash->see[0] = u64_mix(le64(p + 16) ^ rapidhash_secret[1],
                         le64(p + 24) ^ hash->see[0]);
  hash->see[1] = u64_mix(le64(p + 32) ^ rapidhash_secret[2],
                         le64(p + 40) ^ hash->see[1]);
  hash->see[2] = u64_mix(le64(p + 48) ^ rapidhash_secret[3],
                         le64(p + 56) ^ hash->see[2]);
  hash->see[3] = u64_mix(le64(p + 64) ^ rapidhash_secret[4],
                         le64(p + 72) ^ hash->see[3]);
  hash->see[4] = u64_mix(le64(p + 80) ^ rapidhash_secret[5],
                         le64(p + 88) ^ hash->see[4]);
  hash->see[5] = u64_mix(le64(p + 96) ^ rapidhash_secret[6],
                         le64(p + 104) ^ hash->see[5]);
  hash->wide = true;
}

INLINE void hash_rapid_tail_update (s_hash_rapid *hash, const u8 *p,
                                    uw size)
{
  uw keep;

  if (! size)
    return;
  if (size >= sizeof(hash->tail)) {
    memcpy(hash->tail, p + size - sizeof(hash->tail),
           sizeof(hash->tail));
    hash->tail_size = sizeof(hash->tail);
    return;
  }
  keep = hash->tail_size;
  if (keep > sizeof(hash->tail) - size)
    keep = sizeof(hash->tail) - size;
  if (keep)
    memmove(hash->tail, hash->tail + hash->tail_size - keep, keep);
  memcpy(hash->tail + keep, p, size);
  hash->tail_size = keep + size;
}

INLINE bool hash_rapid_update (s_hash_rapid *hash, const void *data,
                               uw size)
{
  const u8 *p = (u8 *) data;
  uw n;
  if (! size)
    return true;
  hash_rapid_tail_update(hash, p, size);
  if (hash->pending_size) {
    n = sizeof(hash->pending) - hash->pending_size;
    if (n > size)
      n = size;
    memcpy(hash->pending + hash->pending_size, p, n);
    hash->pending_size += n;
    p += n;
    size -= n;
    if (! size)
      return true;
    hash_rapid_block(hash, hash->pending);
    hash->pending_size = 0;
  }
  while (size > sizeof(hash->pending)) {
    hash_rapid_block(hash, p);
    p += sizeof(hash->pending);
    size -= sizeof(hash->pending);
  }
  if (size) {
    memcpy(hash->pending, p, size);
    hash->pending_size = size;
  }
  return true;
}

INLINE u64 hash_rapid_final (const s_hash_rapid *hash)
{
  u64 a = 0;
  u64 b = 0;
  uw i = hash->pending_size;
  const u8 *p = hash->pending;
  u64 seed = hash->seed;

  if (likely(! hash->wide && i <= 16)) {
    if (i >= 4) {
      seed ^= i;
      if (i >= 8) {
        a = le64(p);
        b = le64(p + i - 8);
      }
      else {
        a = le32(p);
        b = le32(p + i - 4);
      }
    }
    else if (i > 0) {
      a = ((u64) p[0] << 45) | p[i - 1];
      b = p[i >> 1];
    }
  }
  else {
    if (hash->wide) {
      seed ^= hash->see[0];
      seed ^= hash->see[5];
      seed ^= hash->see[1] ^ hash->see[2] ^
        hash->see[3] ^ hash->see[4];
    }
    if (i > 16) {
      seed = u64_mix(le64(p) ^ rapidhash_secret[2],
                     le64(p + 8) ^ seed);
      if (i > 32) {
        seed = u64_mix(le64(p + 16) ^ rapidhash_secret[2],
                       le64(p + 24) ^ seed);
        if (i > 48) {
          seed = u64_mix(le64(p + 32) ^ rapidhash_secret[1],
                         le64(p + 40) ^ seed);
          if (i > 64) {
            seed = u64_mix(le64(p + 48) ^ rapidhash_secret[1],
                           le64(p + 56) ^ seed);
            if (i > 80) {
              seed = u64_mix(le64(p + 64) ^ rapidhash_secret[2],
                             le64(p + 72) ^ seed);
              if (i > 96)
                seed = u64_mix(le64(p + 80) ^ rapidhash_secret[1],
                               le64(p + 88) ^ seed);
            }
          }
        }
      }
    }
    a = le64(hash->tail) ^ i;
    b = le64(hash->tail + 8);
  }
  a ^= rapidhash_secret[1];
  b ^= seed;
  u64_mul(&a, &b);
  return u64_mix(a ^ rapidhash_secret[7],
                 b ^ rapidhash_secret[1] ^ i);
}

INLINE u64 rapidhash_micro_internal (const void *key, uw len,
                                     u64 seed, const u64 *secret) {
  const u8 *p = (const u8 *) key;
  seed ^= u64_mix(seed ^ secret[2], secret[1]);
  u64 a = 0;
  u64 b = 0;
  uw i = len;
  if (likely(len <= 16)) {
    if (len >= 4) {
      seed ^= len;
      if (len >= 8) {
        const u8* plast = p + len - 8;
        a = le64(p);
        b = le64(plast);
      } else {
        const u8* plast = p + len - 4;
        a = le32(p);
        b = le32(plast);
      }
    } else if (len > 0) {
      a = (((u64)p[0])<<45)|p[len-1];
      b = p[len>>1];
    } else
      a = b = 0;
  } else {
    if (i > 80) {
      u64 see1 = seed, see2 = seed;
      u64 see3 = seed, see4 = seed;
      do {
        seed = u64_mix(le64(p) ^ secret[0], le64(p + 8) ^ seed);
        see1 = u64_mix(le64(p + 16) ^ secret[1], le64(p + 24) ^ see1);
        see2 = u64_mix(le64(p + 32) ^ secret[2], le64(p + 40) ^ see2);
        see3 = u64_mix(le64(p + 48) ^ secret[3], le64(p + 56) ^ see3);
        see4 = u64_mix(le64(p + 64) ^ secret[4], le64(p + 72) ^ see4);
        p += 80;
        i -= 80;
      } while(i > 80);
      seed ^= see1;
      see2 ^= see3;
      seed ^= see4;
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
          }
        }
      }
    }
    a=le64(p+i-16) ^ i;  b=le64(p+i-8);
  }
  a ^= secret[1];
  b ^= seed;
  u64_mul(&a, &b);
  return u64_mix(a ^ secret[7], b ^ secret[1] ^ i);
}

INLINE u64 rapidhash_micro_seeded (const void *key, uw len, u64 seed)
{
  return rapidhash_micro_internal(key, len, seed, rapidhash_secret);
}

INLINE u64 rapidhash_micro (const void *key, uw len)
{
  return rapidhash_micro_seeded(key, len, 0);
}

INLINE uw rapidhash_micro_uw (const void *key, uw len)
{
  return rapidhash_micro_seeded(key, len, 0);
}

#endif /* LIBKC3_RAPIDHASH_H */
