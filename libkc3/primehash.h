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

#include <string.h>
#include "types.h"
#include "inline.h"

#define PROTO_PRIMEHASH(type)                                    \
  type primehash_ ## type (const s_str *key, type hash)

#define PRIMEHASH_ROTATE_U8(byte, bits) \
  ((u8) (((byte) >> (bits)) | ((byte) << (8 - (bits)))))

extern const u8 g_primehash_mix_u32[256][4];
extern const u8 g_primehash_mix_u64[256][8];

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

PROTO_PRIMEHASH(u8);
PROTO_PRIMEHASH(u16);
PROTO_PRIMEHASH(u32);
PROTO_PRIMEHASH(u64);
PROTO_PRIMEHASH(uw);

DEF_PRIMEHASH_INLINE(u8)
DEF_PRIMEHASH_INLINE(u16)
DEF_PRIMEHASH_INLINE(u32)
DEF_PRIMEHASH_INLINE(u64)
DEF_PRIMEHASH_INLINE(uw)

#endif /* LIBKC3_PRIMEHASH_H */
