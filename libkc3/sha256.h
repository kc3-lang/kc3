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
#ifndef LIBKC3_SHA256
#define LIBKC3_SHA256

#include "types.h"

#define SHA256_BLOCK_SIZE   64
#define SHA256_DIGEST_SIZE  32

s_str * sha256_str_to_hex (const s_str *in, s_str *out);

/* HMAC functions. */
void    sha256_hmac (const s_str *k, const s_str *m,
                     u8 dest[SHA256_DIGEST_SIZE]);
s_str * sha256_hmac_str (const s_str *k, const s_str *m, s_str *dest);

#endif /* LIBKC3_SHA256 */
