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
#ifndef LIBKC3_PRIMEHASH_H
#define LIBKC3_PRIMEHASH_H

#if (defined(__i386__) || defined(__x86_64__)) && \
    (defined(__GNUC__) || defined(__clang__))
# define PRIMEHASH_HAVE_AVX2_DISPATCH 1
#endif
#include <string.h>
#include "types.h"
#include "inline.h"

#define PROTO_PRIMEHASH(type)                                    \
  type primehash_ ## type (const s_str *key, type hash)

#define PRIMEHASH_ROTATE_U8(byte, bits) \
  ((u8) (((byte) >> (bits)) | ((byte) << (8 - (bits)))))

#define PRIMEHASH_ROTATE_U64(word, bits) \
  (((word) << (bits)) | ((word) >> (64 - (bits))))

#define PRIMEHASH_U64_C0 0x243f6a8885a308d3ULL
#define PRIMEHASH_U64_C1 0x13198a2e03707344ULL
#define PRIMEHASH_U64_C2 0xa4093822299f31d0ULL
#define PRIMEHASH_U64_C3 0x082efa98ec4e6c89ULL

extern const u8 g_primehash_mix_u32[256][4];
extern const u8 g_primehash_mix_u64[256][8];

#if defined(PRIMEHASH_HAVE_AVX2_DISPATCH)
t_hash * primehash_u64_update_avx2 (t_hash *hash, const u8 *data,
                                    uw size);
#endif

INLINE u64
primehash_mix_u64_inline (u8 byte)
{
  u64 mix;

  memcpy(&mix, g_primehash_mix_u64[byte], sizeof(mix));
  return mix;
}

INLINE u64
primehash_u64_finalize_inline (const t_hash *state, u64 hash)
{
  if (! state->size)
    return hash;
  hash ^= state->state[0];
  hash ^= PRIMEHASH_ROTATE_U64(state->state[1], 17);
  hash ^= PRIMEHASH_ROTATE_U64(state->state[2], 31);
  hash ^= PRIMEHASH_ROTATE_U64(state->state[3], 47);
  hash ^= state->size * 0x9e3779b97f4a7c15ULL;
  hash ^= hash >> 30;
  hash *= 0xbf58476d1ce4e5b9ULL;
  hash ^= hash >> 27;
  hash *= 0x94d049bb133111ebULL;
  hash ^= hash >> 31;
  return hash;
}

INLINE t_hash *
primehash_u64_init_inline (t_hash *state, u64 hash)
{
  state->state[0] = hash ^ PRIMEHASH_U64_C0;
  state->state[1] = hash ^ PRIMEHASH_U64_C1;
  state->state[2] = hash ^ PRIMEHASH_U64_C2;
  state->state[3] = hash ^ PRIMEHASH_U64_C3;
  state->size = 0;
  return state;
}

INLINE void
primehash_u64_update_byte_inline (t_hash *hash, u8 byte)
{
  u8 lane;

  lane = (u8) (hash->size & 3);
  hash->state[lane] *= 17;
  switch (lane) {
  case 0: hash->state[0] ^= PRIMEHASH_U64_C0; break;
  case 1: hash->state[1] ^= PRIMEHASH_U64_C1; break;
  case 2: hash->state[2] ^= PRIMEHASH_U64_C2; break;
  default: hash->state[3] ^= PRIMEHASH_U64_C3; break;
  }
  hash->state[lane] ^= primehash_mix_u64_inline(byte);
  hash->size++;
}

INLINE t_hash *
primehash_u64_update_scalar_inline (t_hash *hash, const u8 *data,
                                    uw size)
{
  uw i;

  i = 0;
  while (i < size && (hash->size & 3)) {
    primehash_u64_update_byte_inline(hash, data[i]);
    i++;
  }
  while (i + 4 <= size) {
    hash->state[0] = hash->state[0] * 17 ^ PRIMEHASH_U64_C0 ^
      primehash_mix_u64_inline(data[i]);
    hash->state[1] = hash->state[1] * 17 ^ PRIMEHASH_U64_C1 ^
      primehash_mix_u64_inline(data[i + 1]);
    hash->state[2] = hash->state[2] * 17 ^ PRIMEHASH_U64_C2 ^
      primehash_mix_u64_inline(data[i + 2]);
    hash->state[3] = hash->state[3] * 17 ^ PRIMEHASH_U64_C3 ^
      primehash_mix_u64_inline(data[i + 3]);
    hash->size += 4;
    i += 4;
  }
  while (i < size) {
    primehash_u64_update_byte_inline(hash, data[i]);
    i++;
  }
  return hash;
}

INLINE t_hash *
primehash_u64_update_inline (t_hash *hash, const u8 *data, uw size)
{
#if defined(PRIMEHASH_HAVE_AVX2_DISPATCH)
  if (__builtin_cpu_supports("avx2"))
    return primehash_u64_update_avx2(hash, data, size);
#endif
  return primehash_u64_update_scalar_inline(hash, data, size);
}

#define DEF_PRIMEHASH_INLINE(type)                              \
  INLINE type                                                   \
  primehash_ ## type ## _inline (const s_str *key, type hash)   \
  {                                                             \
    u8 *h = (u8 *) &hash;                                       \
    uw i;                                                       \
    const u8 *k = key->ptr.p_pu8;                               \
    u32 mix_u32;                                                \
    u64 mix_u64;                                                \
    i = 0;                                                      \
    while (i < key->size) {                                     \
      hash *= 17;                                               \
      hash ^= (sizeof(hash) == 4) ?                             \
        0x66666666 :                                            \
        0x6666666666666666;                                     \
      if (sizeof(hash) == 8) {                                  \
        memcpy(&mix_u64, g_primehash_mix_u64[k[i]],             \
               sizeof(mix_u64));                                \
        hash ^= (type) mix_u64;                                 \
      }                                                         \
      else if (sizeof(hash) == 4) {                             \
        memcpy(&mix_u32, g_primehash_mix_u32[k[i]],             \
               sizeof(mix_u32));                                \
        hash ^= (type) mix_u32;                                 \
      }                                                         \
      else {                                                    \
        h[0] ^= k[i];                                           \
        if (sizeof(hash) > 1) {                                 \
          h[1] ^= PRIMEHASH_ROTATE_U8(k[i], 1);                 \
          h[1] ^= PRIMEHASH_ROTATE_U8(k[i], 5);                 \
        }                                                       \
      }                                                         \
      i++;                                                      \
    }                                                           \
    return hash;                                                \
  }

INLINE u64
primehash_u64_scalar_inline (const s_str *key, u64 hash)
{
  t_hash state;

  if (! key->size)
    return hash;
  primehash_u64_init_inline(&state, hash);
  primehash_u64_update_scalar_inline(&state, key->ptr.p_pu8,
                                     key->size);
  return primehash_u64_finalize_inline(&state, hash);
}

INLINE u64
primehash_u64_inline (const s_str *key, u64 hash)
{
  t_hash state;

  if (! key->size)
    return hash;
  primehash_u64_init_inline(&state, hash);
  primehash_u64_update_inline(&state, key->ptr.p_pu8, key->size);
  return primehash_u64_finalize_inline(&state, hash);
}

PROTO_PRIMEHASH(u8);
PROTO_PRIMEHASH(u16);
PROTO_PRIMEHASH(u32);
PROTO_PRIMEHASH(u64);
PROTO_PRIMEHASH(uw);

DEF_PRIMEHASH_INLINE(u8)
DEF_PRIMEHASH_INLINE(u16)
DEF_PRIMEHASH_INLINE(u32)
DEF_PRIMEHASH_INLINE(uw)

#endif /* LIBKC3_PRIMEHASH_H */
