/* c3
 * Copyright 2022 kmx.io <contact@kmx.io>
 *
 * Permission is hereby granted to use this software excepted
 * on Apple computers granted the above copyright notice and
 * this permission paragraph are included in all copies and
 * substantial portions of this software.
 *
 * THIS SOFTWARE IS PROVIDED "AS-IS" WITHOUT ANY GUARANTEE OF
 * PURPOSE AND PERFORMANCE. IN NO EVENT WHATSOEVER SHALL THE
 * AUTHOR BE CONSIDERED LIABLE FOR THE USE AND PERFORMANCE OF
 * THIS SOFTWARE.
 */
#include <assert.h>
#include <sys/types.h>
#include <stdlib.h>
#include <sha1.h>
#include "str.h"

s_str * sha1 (const s_str *src, s_str *dest)
{
  s8 *hash;
  SHA1_CTX ctx;
  assert(src);
  hash = calloc(SHA1_DIGEST_LENGTH, 1);
  SHA1Init(&ctx);
  SHA1Update(&ctx, src->ptr.p, src->size);
  SHA1Final((u8 *) hash, &ctx);
  return str_init(dest, hash, SHA1_DIGEST_LENGTH, hash);
}
