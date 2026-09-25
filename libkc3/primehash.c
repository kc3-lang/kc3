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
#include "primehash.h"

#if defined(PRIMEHASH_HAVE_SSE2_DISPATCH) || \
    defined(PRIMEHASH_HAVE_AVX2_DISPATCH)
# include <immintrin.h>
#endif

#define PRIMEHASH_MIX_U32(byte) {                         \
    (u8) (byte),                                          \
    (u8) (PRIMEHASH_ROTATE_U8(byte, 1) ^                  \
          PRIMEHASH_ROTATE_U8(byte, 5)),                  \
    (u8) (PRIMEHASH_ROTATE_U8(byte, 2) ^                  \
          PRIMEHASH_ROTATE_U8(byte, 6)),                  \
    (u8) (PRIMEHASH_ROTATE_U8(byte, 3) ^                  \
          PRIMEHASH_ROTATE_U8(byte, 7))                   \
  }

#define PRIMEHASH_MIX_U64(byte) {                         \
    (u8) (byte),                                          \
    PRIMEHASH_ROTATE_U8(byte, 1),                         \
    PRIMEHASH_ROTATE_U8(byte, 2),                         \
    PRIMEHASH_ROTATE_U8(byte, 3),                         \
    PRIMEHASH_ROTATE_U8(byte, 4),                         \
    PRIMEHASH_ROTATE_U8(byte, 5),                         \
    PRIMEHASH_ROTATE_U8(byte, 6),                         \
    PRIMEHASH_ROTATE_U8(byte, 7)                          \
  }

#define PRIMEHASH_MIX_ROW(mix, high)                      \
  mix((high) | 0x0), mix((high) | 0x1),                   \
  mix((high) | 0x2), mix((high) | 0x3),                   \
  mix((high) | 0x4), mix((high) | 0x5),                   \
  mix((high) | 0x6), mix((high) | 0x7),                   \
  mix((high) | 0x8), mix((high) | 0x9),                   \
  mix((high) | 0xa), mix((high) | 0xb),                   \
  mix((high) | 0xc), mix((high) | 0xd),                   \
  mix((high) | 0xe), mix((high) | 0xf)

#define PRIMEHASH_MIX_TABLE(mix)                          \
  PRIMEHASH_MIX_ROW(mix, 0x00),                           \
  PRIMEHASH_MIX_ROW(mix, 0x10),                           \
  PRIMEHASH_MIX_ROW(mix, 0x20),                           \
  PRIMEHASH_MIX_ROW(mix, 0x30),                           \
  PRIMEHASH_MIX_ROW(mix, 0x40),                           \
  PRIMEHASH_MIX_ROW(mix, 0x50),                           \
  PRIMEHASH_MIX_ROW(mix, 0x60),                           \
  PRIMEHASH_MIX_ROW(mix, 0x70),                           \
  PRIMEHASH_MIX_ROW(mix, 0x80),                           \
  PRIMEHASH_MIX_ROW(mix, 0x90),                           \
  PRIMEHASH_MIX_ROW(mix, 0xa0),                           \
  PRIMEHASH_MIX_ROW(mix, 0xb0),                           \
  PRIMEHASH_MIX_ROW(mix, 0xc0),                           \
  PRIMEHASH_MIX_ROW(mix, 0xd0),                           \
  PRIMEHASH_MIX_ROW(mix, 0xe0),                           \
  PRIMEHASH_MIX_ROW(mix, 0xf0)

const u8 g_primehash_mix_u32[256][4] = {
  PRIMEHASH_MIX_TABLE(PRIMEHASH_MIX_U32)
};

const u8 g_primehash_mix_u64[256][8] = {
  PRIMEHASH_MIX_TABLE(PRIMEHASH_MIX_U64)
};

#if defined(PRIMEHASH_HAVE_SSE2_DISPATCH)

__attribute__((target("sse2")))
t_hash *
primehash_u32_update_sse2 (t_hash *hash, const u8 *data, uw size)
{
  uw block_size;
  __m128i constant;
  __m128i hash_4;

  if (! size)
    return hash;
  while (size && (hash->size & 3)) {
    primehash_u32_update_byte_inline(hash, *data);
    data++;
    size--;
  }
  constant = _mm_set_epi32(PRIMEHASH_U32_C3, PRIMEHASH_U32_C2,
                           PRIMEHASH_U32_C1, PRIMEHASH_U32_C0);
  hash_4 = _mm_loadu_si128((const __m128i *) hash->state);
  block_size = size & ~((uw) 3);
  hash->size += block_size;
  size -= block_size;
  while (block_size) {
    __m128i mix;

    mix = _mm_set_epi32(primehash_mix_u32_inline(data[3]),
                        primehash_mix_u32_inline(data[2]),
                        primehash_mix_u32_inline(data[1]),
                        primehash_mix_u32_inline(data[0]));
    hash_4 = _mm_add_epi32(hash_4, _mm_slli_epi32(hash_4, 4));
    hash_4 = _mm_xor_si128(hash_4, constant);
    hash_4 = _mm_xor_si128(hash_4, mix);
    data += 4;
    block_size -= 4;
  }
  _mm_storeu_si128((__m128i *) hash->state, hash_4);
  while (size) {
    primehash_u32_update_byte_inline(hash, *data);
    data++;
    size--;
  }
  return hash;
}

#endif

#if defined(PRIMEHASH_HAVE_AVX2_DISPATCH)

__attribute__((target("avx2")))
t_hash *
primehash_u64_update_avx2 (t_hash *hash, const u8 *data, uw size)
{
  uw block_size;
  __m256i constant;
  __m256i hash_4;

  if (! size)
    return hash;
  while (size && (hash->size & 3)) {
    primehash_u64_update_byte_inline(hash, *data);
    data++;
    size--;
  }
  constant = _mm256_set_epi64x(PRIMEHASH_U64_C3,
                               PRIMEHASH_U64_C2,
                               PRIMEHASH_U64_C1,
                               PRIMEHASH_U64_C0);
  hash_4 = _mm256_loadu_si256((const __m256i *) hash->state);
  block_size = size & ~((uw) 3);
  hash->size += block_size;
  size -= block_size;
  while (block_size) {
    __m256i mix;

    mix = _mm256_set_epi64x(primehash_mix_u64_inline(data[3]),
                            primehash_mix_u64_inline(data[2]),
                            primehash_mix_u64_inline(data[1]),
                            primehash_mix_u64_inline(data[0]));
    hash_4 = _mm256_add_epi64(hash_4,
                             _mm256_slli_epi64(hash_4, 4));
    hash_4 = _mm256_xor_si256(hash_4, constant);
    hash_4 = _mm256_xor_si256(hash_4, mix);
    data += 4;
    block_size -= 4;
  }
  _mm256_storeu_si256((__m256i *) hash->state, hash_4);
  while (size) {
    primehash_u64_update_byte_inline(hash, *data);
    data++;
    size--;
  }
  return hash;
}

#endif

#define DEF_PRIMEHASH(type)                              \
  type primehash_ ## type (const s_str *key, type hash)  \
  {                                                      \
    u8 *h = (u8 *) &hash;                                \
    uw i;                                                \
    u8 j;                                                \
    const u8 *k = key->ptr.p_pu8;                        \
    u64 mix_u64;                                         \
    i = 0;                                               \
    while (i < key->size) {                              \
      hash *= 17;                                        \
      hash ^= 0x6666666666666666;                        \
      if (sizeof(hash) == 8) {                           \
        memcpy(&mix_u64, g_primehash_mix_u64[k[i]],      \
               sizeof(mix_u64));                         \
        hash ^= (type) mix_u64;                          \
      }                                                  \
      else {                                             \
        h[0] ^= k[i];                                    \
        j = 1;                                           \
        while (j < sizeof(hash)) {                       \
          h[j] ^= PRIMEHASH_ROTATE_U8(k[i], j);          \
          j++;                                           \
        }                                                \
      }                                                  \
      i++;                                               \
    }                                                    \
    return hash;                                         \
  }

DEF_PRIMEHASH(u8)
DEF_PRIMEHASH(u16)
DEF_PRIMEHASH(uw)

u32
primehash_u32 (const s_str *key, u32 hash)
{
  return primehash_u32_inline(key, hash);
}

u64
primehash_u64 (const s_str *key, u64 hash)
{
  return primehash_u64_inline(key, hash);
}
