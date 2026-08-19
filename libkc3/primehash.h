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

#include "types.h"
#include "inline.h"

#define PROTO_PRIMEHASH(type)                                    \
  type primehash_ ## type (const s_str *key, type hash)

#define DEF_PRIMEHASH_INLINE(type)                               \
  INLINE type                                                    \
  primehash_ ## type ## _inline (const s_str *key, type hash)    \
  {                                                              \
    u8 *h = (u8 *) &hash;                                        \
    uw i;                                                        \
    u8 j;                                                        \
    const u8 *k = key->ptr.p_pu8;                                \
    i = 0;                                                       \
    while (i < key->size) {                                      \
      hash *= 17;                                                \
      hash ^= (sizeof(hash) == 4) ?                              \
        0x66666666 :                                             \
        0x6666666666666666;                                      \
      h[0] ^= k[i];                                              \
      j = 1;                                                     \
      while (j < sizeof(hash)) {                                 \
        h[j] ^= ((k[i] >> j) | (k[i] << (8 - j)));               \
        if (sizeof(hash) < 8)                                    \
          h[j] ^= ((k[i] >> (j + 4)) | (k[i] << (8 - (j + 4)))); \
        j++;                                                     \
      }                                                          \
      i++;                                                       \
    }                                                            \
    return hash;                                                 \
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
