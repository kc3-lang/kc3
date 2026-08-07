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

uw primehash_uw (const s_str *key, uw hash)
{
  u8 *h = (u8 *) &hash;
  uw i;
  u8 j;
  const u8 *k = key->ptr.p_pu8;
  i = 0;
  while (i < key->size) {
    j = 0;
    while (j < sizeof(hash)) {
      h[j] ^= ((k[i] >> j) | (k[i] << (8 - j))) ^ 0xf0;
      j++;
    }
    i++;
  }
  return hash;
}
