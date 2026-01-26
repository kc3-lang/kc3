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
#ifndef LIBKC3_SHA512_H
#define LIBKC3_SHA512_H

#include "types.h"

#define SHA512_BLOCK_LENGTH  128
#define SHA512_DIGEST_LENGTH 64

void    sha512_init (s_sha512 *s);
void    sha512_sum (s_sha512 *s, u8 *md);
void    sha512_update (s_sha512 *s, const void *m, u64 len);
void    sha512_hmac (const s_str *k, const s_str *m, u8 *dest);
s_str * sha512_hmac_str (const s_str *k, const s_str *m, s_str *dest);

#endif /* LIBKC3_SHA512_H */
