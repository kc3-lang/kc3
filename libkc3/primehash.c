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
DEF_PRIMEHASH(u32)
DEF_PRIMEHASH(u64)
DEF_PRIMEHASH(uw)
