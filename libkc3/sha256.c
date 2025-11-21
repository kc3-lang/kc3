/* kc3
 * Copyright from 2022 to 2025 kmx.io <contact@kmx.io>
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
#include <sys/types.h>

#ifdef HAVE_SHA2_H
# include <sha2.h>
# ifndef SHA256_CTX
#  define SHA256_CTX SHA2_CTX
# endif
#elif HAVE_SHA256_H
# include <sha256.h>
#endif

#include "sha256.h"
#include "str.h"

s_str * sha256_str_to_hex (const s_str *in, s_str *out)
{
  SHA256_CTX context;
  s_str tmp = {0};
  if (! str_init_alloc(&tmp, SHA256_DIGEST_LENGTH * 2))
    return NULL;
#ifdef HAVE_SHA256_INIT
  SHA256_Init(&context);
  SHA256_Update(&context, in->ptr.pu8, in->size);
  SHA256_End(&context, tmp.free.pchar);
#else
  SHA256Init(&context);
  SHA256Update(&context, in->ptr.pu8, in->size);
  SHA256End(&context, tmp.free.pchar);
#endif
  *out = tmp;
  return out;
}
