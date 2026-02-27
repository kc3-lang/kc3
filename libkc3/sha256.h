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
#ifndef LIBKC3_SHA256_H
#define LIBKC3_SHA256_H

#include "types.h"

#ifdef __clang__
# define BOUNDED_STRING(a, b) __attribute__((__bounded__(__string__, a, b)))
# define BOUNDED_MINBYTES(a, b) __attribute__((__bounded__(__minbytes__, a, b)))
#else
# define BOUNDED_STRING(a, b)
# define BOUNDED_MINBYTES(a, b)
#endif

char * sha256_end(s_sha2 *ctx, char *buf);
void   sha256_init (s_sha2 *ctx);
void   sha256_transform (u32 state[8],
                        const u8 block[SHA256_BLOCK_LENGTH]);
void   sha256_update (s_sha2 *ctx, const u8 *data, uw size)
  BOUNDED_STRING(2, 3);
void   sha256_pad (s_sha2 *ctx);
void   sha256_final (u8 digest[SHA256_DIGEST_LENGTH], s_sha2 *ctx)
  BOUNDED_MINBYTES(1, SHA256_DIGEST_LENGTH);

/* Observers. */
s_str * sha256_str_to_hex (const s_str *in, s_str *out);

/* HMAC functions. */
void    sha256_hmac (const s_str *k, const s_str *m, u8 *dest);
s_str * sha256_hmac_str (const s_str *k, const s_str *m, s_str *dest);

#endif /* LIBKC3_SHA256_H */
