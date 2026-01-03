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
#include <string.h>
#include <sys/types.h>

#if HAVE_SHA2_H
# include <sha2.h>
#elif HAVE_SHA256_H
# include <sha256.h>
#endif

#include "assert.h"
#include "io.h"
#include "sha256.h"
#include "str.h"

s_str * sha256_str_to_hex (const s_str *in, s_str *out)
{
#if HAVE_SHA256_CTX
  SHA256_CTX context;
#else
  SHA2_CTX context;
#endif
  s_str tmp = {0};
  if (! str_init_alloc(&tmp, SHA256_DIGEST_LENGTH * 2))
    return NULL;
#if HAVE_SHA256_INIT
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

void sha256_hmac (const s_str *k, const s_str *m,
                  u8 dest[SHA256_DIGEST_SIZE])
{
#if HAVE_SHA256_CTX
  SHA256_CTX h_ctx;
#else
  SHA2_CTX h_ctx;
#endif
  u8       h[2][SHA256_BLOCK_SIZE] = {0};
  u8       i;
  s_str    k_p = {0};
  u8       pad[2][SHA256_BLOCK_SIZE];
  err_puts("sha256_hmac: start");
  assert(SHA256_DIGEST_SIZE <= SHA256_BLOCK_SIZE);
  err_write_1("sha256_hmac: k->size = ");
  err_inspect_uw(k->size);
  err_write_1(", m->size = ");
  err_inspect_uw(m->size);
  err_write_1("\n");
  if (k->size > SHA256_BLOCK_SIZE) {
    err_puts("sha256_hmac: key too large, hashing");
#if HAVE_SHA256_INIT
    SHA256_Init(&h_ctx);
    SHA256_Update(&h_ctx, k->ptr.pu8, k->size);
    SHA256_Final(h[0], &h_ctx);
#else
    SHA256Init(&h_ctx);
    SHA256Update(&h_ctx, k->ptr.pu8, k->size);
    SHA256Final(h[0], &h_ctx);
#endif
    str_init(&k_p, NULL, SHA256_DIGEST_SIZE, (const char *) h[0]);
  }
  else {
    err_puts("sha256_hmac: using key directly");
    str_init(&k_p, NULL, k->size, k->ptr.p);
  }
  err_puts("sha256_hmac: memset pads");
  memset(pad[0], 0x5c, SHA256_BLOCK_SIZE);
  memset(pad[1], 0x36, SHA256_BLOCK_SIZE);
  err_puts("sha256_hmac: xor key into pads");
  i = 0;
  while (i < k_p.size) {
    pad[0][i] ^= k_p.ptr.pu8[i];
    pad[1][i] ^= k_p.ptr.pu8[i];
    i++;
  }
  while (i < SHA256_BLOCK_SIZE) {
    pad[0][i] ^= 0;
    pad[1][i] ^= 0;
    i++;
  }
  err_puts("sha256_hmac: inner hash");
#if HAVE_SHA256_INIT
  SHA256_Init(&h_ctx);
  err_puts("sha256_hmac: inner update pad");
  SHA256_Update(&h_ctx, pad[1], SHA256_BLOCK_SIZE);
  err_puts("sha256_hmac: inner update message");
  SHA256_Update(&h_ctx, m->ptr.p, m->size);
  err_puts("sha256_hmac: inner final");
  SHA256_Final(h[1], &h_ctx);
  err_puts("sha256_hmac: outer hash");
  SHA256_Init(&h_ctx);
  err_puts("sha256_hmac: outer update pad");
  SHA256_Update(&h_ctx, pad[0], SHA256_BLOCK_SIZE);
  err_puts("sha256_hmac: outer update inner hash");
  SHA256_Update(&h_ctx, h[1], SHA256_DIGEST_SIZE);
  err_puts("sha256_hmac: outer final");
  SHA256_Final(dest, &h_ctx);
#else
  SHA256Init(&h_ctx);
  err_puts("sha256_hmac: inner update pad");
  SHA256Update(&h_ctx, pad[1], SHA256_BLOCK_SIZE);
  err_puts("sha256_hmac: inner update message");
  SHA256Update(&h_ctx, m->ptr.p, m->size);
  err_puts("sha256_hmac: inner final");
  SHA256Final(h[1], &h_ctx);
  err_puts("sha256_hmac: outer hash");
  SHA256Init(&h_ctx);
  err_puts("sha256_hmac: outer update pad");
  SHA256Update(&h_ctx, pad[0], SHA256_BLOCK_SIZE);
  err_puts("sha256_hmac: outer update inner hash");
  SHA256Update(&h_ctx, h[1], SHA256_DIGEST_SIZE);
  err_puts("sha256_hmac: outer final");
  SHA256Final(dest, &h_ctx);
#endif
  err_puts("sha256_hmac: done");
}

s_str * sha256_hmac_str (const s_str *k, const s_str *m, s_str *dest)
{
  s_str tmp = {0};
  err_puts("sha256_hmac_str: start");
  if (! str_init_alloc(&tmp, SHA256_DIGEST_SIZE)) {
    err_puts("sha256_hmac_str: str_init_alloc failed");
    return NULL;
  }
  err_puts("sha256_hmac_str: calling sha256_hmac");
  sha256_hmac(k, m, tmp.free.pu8);
  err_puts("sha256_hmac_str: assigning to dest");
  *dest = tmp;
  err_puts("sha256_hmac_str: done");
  return dest;
}
