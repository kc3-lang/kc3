/*
 * SHA-1 in C
 * By Steve Reid <steve@edmweb.com>
 * 100% Public Domain
 */
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

#ifndef LIBKC3_SHA1_H
#define LIBKC3_SHA1_H

#include "endian.h"
#include "types.h"

#define HTONDIGEST(x) do {                                     \
        x[0] = htonl(x[0]);                                    \
        x[1] = htonl(x[1]);                                    \
        x[2] = htonl(x[2]);                                    \
        x[3] = htonl(x[3]);                                    \
        x[4] = htonl(x[4]); } while (0)

#define NTOHDIGEST(x) do {                                     \
        x[0] = ntohl(x[0]);                                    \
        x[1] = ntohl(x[1]);                                    \
        x[2] = ntohl(x[2]);                                    \
        x[3] = ntohl(x[3]);                                    \
        x[4] = ntohl(x[4]); } while (0)

void  SHA1Init(s_sha1 *);
void  SHA1Pad(s_sha1 *);
void  SHA1Transform(uint32_t [5], const uint8_t [SHA1_BLOCK_LENGTH]);
void  SHA1Update(s_sha1 *, const uint8_t *, size_t);
void  SHA1Final(uint8_t [SHA1_DIGEST_LENGTH], s_sha1 *);
char *SHA1End(s_sha1 *, char *);
char *SHA1File(const char *, char *);
char *SHA1FileChunk(const char *, char *, off_t, off_t);
char *SHA1Data(const uint8_t *, size_t, char *);

#include "types.h"

/* HMAC functions. */
void    sha1_hmac (const s_str *k, const s_str *m,
                   u8 dest[SHA1_DIGEST_LENGTH]);
s_str * sha1_hmac_str (const s_str *k, const s_str *m, s_str *dest);

#endif /* LIBKC3_SHA1_H */
