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
#include <err.h>
#include <stdlib.h>
#include "hash.h"
#include "str.h"

t_hash_context * hash_init (t_hash_context *context)
{
  assert(context);
  SHA1Init(context);
  return context;
}

t_hash_context * hash_update (t_hash_context *context,
                              const void *data, uw size)
{
  assert(context);
  assert(data);
  SHA1Update(context, data, size);
  return context;
}

uw hash_result (t_hash_context *context)
{
  u8 digest[SHA1_DIGEST_LENGTH];
  SHA1Final(digest, context);
  return *((uw *) digest);
}

u64 hash_result_u64 (t_hash_context *context)
{
  u8 digest[SHA1_DIGEST_LENGTH];
  SHA1Final(digest, context);
  return *((u64 *) digest);
}
