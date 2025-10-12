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
#ifndef LIBKC3_SHA512_H
#define LIBKC3_SHA512_H

#include "types.h"

void sha512_init (s_sha512 *s);
void sha512_sum (s_sha512 *s, u8 *md);
void sha512_update (s_sha512 *s, const void *m, u64 len);

#endif /* LIBKC3_SHA512_H */
