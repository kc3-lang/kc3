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

#define DEF_PRIMEHASH(type)                             \
  type primehash_ ## type (const s_str *key, type hash) \
  {                                                     \
    u8 *h = (u8 *) &hash;                               \
    uw i;                                               \
    u8 j;                                               \
    const u8 *k = key->ptr.p_pu8;                       \
    i = 0;                                              \
    while (i < key->size) {                             \
      hash *= 17;                                       \
      hash ^= 0x6666666666666666;                       \
      h[0] ^= k[i];                                     \
      j = 1;                                            \
      while (j < sizeof(hash)) {                        \
        h[j] ^= ((k[i] >> j) | (k[i] << (8 - j)));      \
        j++;                                            \
      }                                                 \
      i++;                                              \
    }                                                   \
    return hash;                                        \
  }

DEF_PRIMEHASH(u8)
DEF_PRIMEHASH(u16)
DEF_PRIMEHASH(u32)
DEF_PRIMEHASH(u64)
DEF_PRIMEHASH(uw)
