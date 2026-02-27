/*
 * Copyright (c) 2000-2001, Aaron D. Gifford
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the copyright holder nor the names of contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTOR(S) ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTOR(S) BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 */
#include <string.h>
#include "assert.h"
#include "explicit_bzero.h"
#include "sha256.h"
#include "str.h"

#if defined(__amd64__) || defined(__i386__)
#define SHA2_UNROLL_TRANSFORM
#endif

#if !defined(BYTE_ORDER) || (BYTE_ORDER != LITTLE_ENDIAN && BYTE_ORDER != BIG_ENDIAN)
#error Define BYTE_ORDER to be equal to either LITTLE_ENDIAN or BIG_ENDIAN
#endif

/*** SHA-224/256/384/512 Various Length Definitions *******************/
#define SHA224_SHORT_BLOCK_LENGTH (SHA224_BLOCK_LENGTH - 8)
#define SHA256_SHORT_BLOCK_LENGTH (SHA256_BLOCK_LENGTH - 8)
#define SHA384_SHORT_BLOCK_LENGTH (SHA384_BLOCK_LENGTH - 16)
#define SHA512_SHORT_BLOCK_LENGTH (SHA512_BLOCK_LENGTH - 16)

/*** ENDIAN SPECIFIC COPY MACROS **************************************/
#define BE_8_TO_32(dst, cp) do {					\
	(dst) = (u32)(cp)[3] | ((u32)(cp)[2] << 8) |	\
	    ((u32)(cp)[1] << 16) | ((u32)(cp)[0] << 24);	\
} while(0)

#define BE_32_TO_8(cp, src) do {					\
	(cp)[0] = (src) >> 24;						\
	(cp)[1] = (src) >> 16;						\
	(cp)[2] = (src) >> 8;						\
	(cp)[3] = (src);						\
} while (0)

#define BE_64_TO_8(cp, src) do {					\
	(cp)[0] = (src) >> 56;						\
        (cp)[1] = (src) >> 48;						\
	(cp)[2] = (src) >> 40;						\
	(cp)[3] = (src) >> 32;						\
	(cp)[4] = (src) >> 24;						\
	(cp)[5] = (src) >> 16;						\
	(cp)[6] = (src) >> 8;						\
	(cp)[7] = (src);						\
} while (0)

/*** THE SIX LOGICAL FUNCTIONS ****************************************/
/*
 * Bit shifting and rotation (used by the six SHA-XYZ logical functions:
 *
 *   NOTE:  The naming of R and S appears backwards here (R is a SHIFT and
 *   S is a ROTATION) because the SHA-224/256/384/512 description document
 *   (see http://csrc.nist.gov/cryptval/shs/sha256-384-512.pdf) uses this
 *   same "backwards" definition.
 */
/* Shift-right (used in SHA-224, SHA-256, SHA-384, and SHA-512): */
#define R(b,x) 		((x) >> (b))
/* 32-bit Rotate-right (used in SHA-224 and SHA-256): */
#define S32(b,x)	(((x) >> (b)) | ((x) << (32 - (b))))
/* 64-bit Rotate-right (used in SHA-384 and SHA-512): */
#define S64(b,x)	(((x) >> (b)) | ((x) << (64 - (b))))

/* Two of six logical functions used in SHA-224, SHA-256, SHA-384, and SHA-512: */
#define Ch(x,y,z)	(((x) & (y)) ^ ((~(x)) & (z)))
#define Maj(x,y,z)	(((x) & (y)) ^ ((x) & (z)) ^ ((y) & (z)))

/* Four of six logical functions used in SHA-224 and SHA-256: */
#define Sigma0_256(x)	(S32(2,  (x)) ^ S32(13, (x)) ^ S32(22, (x)))
#define Sigma1_256(x)	(S32(6,  (x)) ^ S32(11, (x)) ^ S32(25, (x)))
#define sigma0_256(x)	(S32(7,  (x)) ^ S32(18, (x)) ^ R(3 ,   (x)))
#define sigma1_256(x)	(S32(17, (x)) ^ S32(19, (x)) ^ R(10,   (x)))

/* Four of six logical functions used in SHA-384 and SHA-512: */
#define Sigma0_512(x)	(S64(28, (x)) ^ S64(34, (x)) ^ S64(39, (x)))
#define Sigma1_512(x)	(S64(14, (x)) ^ S64(18, (x)) ^ S64(41, (x)))
#define sigma0_512(x)	(S64( 1, (x)) ^ S64( 8, (x)) ^ R( 7,   (x)))
#define sigma1_512(x)	(S64(19, (x)) ^ S64(61, (x)) ^ R( 6,   (x)))

/*** SHA-XYZ INITIAL HASH VALUES AND CONSTANTS ************************/
/* Hash constant words K for SHA-224 and SHA-256: */
static const u32 K256[64] = {
	0x428a2f98UL, 0x71374491UL, 0xb5c0fbcfUL, 0xe9b5dba5UL,
	0x3956c25bUL, 0x59f111f1UL, 0x923f82a4UL, 0xab1c5ed5UL,
	0xd807aa98UL, 0x12835b01UL, 0x243185beUL, 0x550c7dc3UL,
	0x72be5d74UL, 0x80deb1feUL, 0x9bdc06a7UL, 0xc19bf174UL,
	0xe49b69c1UL, 0xefbe4786UL, 0x0fc19dc6UL, 0x240ca1ccUL,
	0x2de92c6fUL, 0x4a7484aaUL, 0x5cb0a9dcUL, 0x76f988daUL,
	0x983e5152UL, 0xa831c66dUL, 0xb00327c8UL, 0xbf597fc7UL,
	0xc6e00bf3UL, 0xd5a79147UL, 0x06ca6351UL, 0x14292967UL,
	0x27b70a85UL, 0x2e1b2138UL, 0x4d2c6dfcUL, 0x53380d13UL,
	0x650a7354UL, 0x766a0abbUL, 0x81c2c92eUL, 0x92722c85UL,
	0xa2bfe8a1UL, 0xa81a664bUL, 0xc24b8b70UL, 0xc76c51a3UL,
	0xd192e819UL, 0xd6990624UL, 0xf40e3585UL, 0x106aa070UL,
	0x19a4c116UL, 0x1e376c08UL, 0x2748774cUL, 0x34b0bcb5UL,
	0x391c0cb3UL, 0x4ed8aa4aUL, 0x5b9cca4fUL, 0x682e6ff3UL,
	0x748f82eeUL, 0x78a5636fUL, 0x84c87814UL, 0x8cc70208UL,
	0x90befffaUL, 0xa4506cebUL, 0xbef9a3f7UL, 0xc67178f2UL
};

/* Initial hash value H for SHA-256: */
static const u32 sha256_initial_hash_value[8] = {
	0x6a09e667UL,
	0xbb67ae85UL,
	0x3c6ef372UL,
	0xa54ff53aUL,
	0x510e527fUL,
	0x9b05688cUL,
	0x1f83d9abUL,
	0x5be0cd19UL
};

void
sha256_init(s_sha2 *context)
{
  memcpy(context->state.st32, sha256_initial_hash_value,
         sizeof(sha256_initial_hash_value));
  memset(context->buffer, 0, sizeof(context->buffer));
  context->bitcount[0] = 0;
}

#ifdef SHA2_UNROLL_TRANSFORM

/* Unrolled SHA-256 round macros: */

#define ROUND256_0_TO_15(a,b,c,d,e,f,g,h) do {    \
  BE_8_TO_32(W256[j], data);    \
  data += 4;    \
  T1 = (h) + Sigma1_256((e)) + Ch((e), (f), (g)) + K256[j] + W256[j]; \
  (d) += T1;    \
  (h) = T1 + Sigma0_256((a)) + Maj((a), (b), (c));    \
  j++;    \
  } while(0)

#define ROUND256(a,b,c,d,e,f,g,h) do {    \
    s0 = W256[(j+1)&0x0f];    \
    s0 = sigma0_256(s0);    \
    s1 = W256[(j+14)&0x0f];    \
    s1 = sigma1_256(s1);    \
    T1 = (h) + Sigma1_256((e)) + Ch((e), (f), (g)) + K256[j] +    \
      (W256[j&0x0f] += s1 + W256[(j+9)&0x0f] + s0);    \
    (d) += T1;    \
    (h) = T1 + Sigma0_256((a)) + Maj((a), (b), (c));    \
    j++;    \
  } while(0)

void
sha256_transform(u32 state[8], const u8 data[SHA256_BLOCK_LENGTH])
{
  u32 a, b, c, d, e, f, g, h, s0, s1;
  u32 T1, W256[16];
  int j;

  /* Initialize registers with the prev. intermediate value */
  a = state[0];
  b = state[1];
  c = state[2];
  d = state[3];
  e = state[4];
  f = state[5];
  g = state[6];
  h = state[7];

  j = 0;
  do {
    /* Rounds 0 to 15 (unrolled): */
    ROUND256_0_TO_15(a,b,c,d,e,f,g,h);
    ROUND256_0_TO_15(h,a,b,c,d,e,f,g);
    ROUND256_0_TO_15(g,h,a,b,c,d,e,f);
    ROUND256_0_TO_15(f,g,h,a,b,c,d,e);
    ROUND256_0_TO_15(e,f,g,h,a,b,c,d);
    ROUND256_0_TO_15(d,e,f,g,h,a,b,c);
    ROUND256_0_TO_15(c,d,e,f,g,h,a,b);
    ROUND256_0_TO_15(b,c,d,e,f,g,h,a);
  } while (j < 16);

  /* Now for the remaining rounds up to 63: */
  do {
    ROUND256(a,b,c,d,e,f,g,h);
    ROUND256(h,a,b,c,d,e,f,g);
    ROUND256(g,h,a,b,c,d,e,f);
    ROUND256(f,g,h,a,b,c,d,e);
    ROUND256(e,f,g,h,a,b,c,d);
    ROUND256(d,e,f,g,h,a,b,c);
    ROUND256(c,d,e,f,g,h,a,b);
    ROUND256(b,c,d,e,f,g,h,a);
  } while (j < 64);

  /* Compute the current intermediate hash value */
  state[0] += a;
  state[1] += b;
  state[2] += c;
  state[3] += d;
  state[4] += e;
  state[5] += f;
  state[6] += g;
  state[7] += h;

  /* Clean up */
  a = b = c = d = e = f = g = h = T1 = 0;
}

#else /* SHA2_UNROLL_TRANSFORM */

void
sha256_transform(u32 state[8], const u8 data[SHA256_BLOCK_LENGTH])
{
  u32 a, b, c, d, e, f, g, h, s0, s1;
  u32 T1, T2, W256[16];
  s32 j;

  /* Initialize registers with the prev. intermediate value */
  a = state[0];
  b = state[1];
  c = state[2];
  d = state[3];
  e = state[4];
  f = state[5];
  g = state[6];
  h = state[7];

  j = 0;
  do {
    BE_8_TO_32(W256[j], data);
    data += 4;
    /* Apply the SHA-256 compression function to update a..h */
    T1 = h + Sigma1_256(e) + Ch(e, f, g) + K256[j] + W256[j];
    T2 = Sigma0_256(a) + Maj(a, b, c);
    h = g;
    g = f;
    f = e;
    e = d + T1;
    d = c;
    c = b;
    b = a;
    a = T1 + T2;

    j++;
  } while (j < 16);

  do {
    /* Part of the message block expansion: */
    s0 = W256[(j+1)&0x0f];
    s0 = sigma0_256(s0);
    s1 = W256[(j+14)&0x0f];
    s1 = sigma1_256(s1);

    /* Apply the SHA-256 compression function to update a..h */
    T1 = h + Sigma1_256(e) + Ch(e, f, g) + K256[j] +
      (W256[j&0x0f] += s1 + W256[(j+9)&0x0f] + s0);
    T2 = Sigma0_256(a) + Maj(a, b, c);
    h = g;
    g = f;
    f = e;
    e = d + T1;
    d = c;
    c = b;
    b = a;
    a = T1 + T2;

    j++;
  } while (j < 64);

  /* Compute the current intermediate hash value */
  state[0] += a;
  state[1] += b;
  state[2] += c;
  state[3] += d;
  state[4] += e;
  state[5] += f;
  state[6] += g;
  state[7] += h;

  /* Clean up */
  a = b = c = d = e = f = g = h = T1 = T2 = 0;
}

#endif /* SHA2_UNROLL_TRANSFORM */

void
sha256_update(s_sha2 *context, const u8 *data, size_t len)
{
  u64 freespace, usedspace;
  /* Calling with no data is valid (we do nothing) */
  if (len == 0)
    return;
  usedspace = (context->bitcount[0] >> 3) % SHA256_BLOCK_LENGTH;
  if (usedspace > 0) {
    /* Calculate how much free space is available in the buffer */
    freespace = SHA256_BLOCK_LENGTH - usedspace;
    if (len >= freespace) {
      /* Fill the buffer completely and process it */
      memcpy(&context->buffer[usedspace], data, freespace);
      context->bitcount[0] += freespace << 3;
      len -= freespace;
      data += freespace;
      sha256_transform(context->state.st32, context->buffer);
    } else {
      /* The buffer is not yet full */
      memcpy(&context->buffer[usedspace], data, len);
      context->bitcount[0] += (u_int64_t)len << 3;
      /* Clean up: */
      usedspace = freespace = 0;
      return;
    }
  }
  while (len >= SHA256_BLOCK_LENGTH) {
    /* Process as many complete blocks as we can */
    sha256_transform(context->state.st32, data);
    context->bitcount[0] += SHA256_BLOCK_LENGTH << 3;
    len -= SHA256_BLOCK_LENGTH;
    data += SHA256_BLOCK_LENGTH;
  }
  if (len > 0) {
    /* There's left-overs, so save 'em */
    memcpy(context->buffer, data, len);
    context->bitcount[0] += len << 3;
  }
  /* Clean up: */
  usedspace = freespace = 0;
}

void
sha256_pad(s_sha2 *context)
{
  unsigned int usedspace;
  usedspace = (context->bitcount[0] >> 3) % SHA256_BLOCK_LENGTH;
  if (usedspace > 0) {
    /* Begin padding with a 1 bit: */
    context->buffer[usedspace++] = 0x80;

    if (usedspace <= SHA256_SHORT_BLOCK_LENGTH) {
      /* Set-up for the last transform: */
      memset(&context->buffer[usedspace], 0,
             SHA256_SHORT_BLOCK_LENGTH - usedspace);
    } else {
      if (usedspace < SHA256_BLOCK_LENGTH) {
        memset(&context->buffer[usedspace], 0,
               SHA256_BLOCK_LENGTH - usedspace);
      }
      /* Do second-to-last transform: */
      sha256_transform(context->state.st32, context->buffer);

      /* Prepare for last transform: */
      memset(context->buffer, 0, SHA256_SHORT_BLOCK_LENGTH);
    }
  } else {
    /* Set-up for the last transform: */
    memset(context->buffer, 0, SHA256_SHORT_BLOCK_LENGTH);

    /* Begin padding with a 1 bit: */
    *context->buffer = 0x80;
  }
  /* Store the length of input data (in bits) in big endian format: */
  BE_64_TO_8(&context->buffer[SHA256_SHORT_BLOCK_LENGTH],
             context->bitcount[0]);

  /* Final transform: */
  sha256_transform(context->state.st32, context->buffer);

  /* Clean up: */
  usedspace = 0;
}

char *
sha256_end(s_sha2 *ctx, char *buf)
{
  int i;
  u_int8_t digest[SHA256_DIGEST_LENGTH];
  static const char hex[] = "0123456789abcdef";

  if (buf == NULL && (buf = malloc(SHA256_DIGEST_STRING_LENGTH)) == NULL)
    return (NULL);

  sha256_final(digest, ctx);
  for (i = 0; i < SHA256_DIGEST_LENGTH; i++) {
    buf[i + i] = hex[digest[i] >> 4];
    buf[i + i + 1] = hex[digest[i] & 0x0f];
  }
  buf[i + i] = '\0';
  explicit_bzero(digest, sizeof(digest));
  return (buf);
}

void
sha256_final(u8 digest[SHA256_DIGEST_LENGTH], s_sha2 *context)
{
  sha256_pad(context);

  #if BYTE_ORDER == LITTLE_ENDIAN
  int i;

  /* Convert TO host byte order */
  for (i = 0; i < 8; i++)
    BE_32_TO_8(digest + i * 4, context->state.st32[i]);
  #else
  memcpy(digest, context->state.st32, SHA256_DIGEST_LENGTH);
  #endif
  explicit_bzero(context, sizeof(*context));
}

s_str * sha256_str_to_hex (const s_str *in, s_str *out)
{
  s_sha2 context;
  s_str tmp = {0};
  if (! str_init_alloc(&tmp, SHA256_DIGEST_LENGTH * 2))
    return NULL;
  sha256_init(&context);
  sha256_update(&context, in->ptr.p_pu8, in->size);
  sha256_end(&context, tmp.free.p_pchar);
  *out = tmp;
  return out;
}

void sha256_hmac (const s_str *k, const s_str *m, u8 *dest)
{
  s_sha2 h_ctx;
  u8       h[2][SHA256_DIGEST_LENGTH] = {0};
  u8       i;
  s_str    k_p = {0};
  u8       pad[2][SHA256_BLOCK_LENGTH];
  assert(SHA256_DIGEST_LENGTH <= SHA256_BLOCK_LENGTH);
  if (k->size > SHA256_BLOCK_LENGTH) {
    sha256_init(&h_ctx);
    sha256_update(&h_ctx, k->ptr.p_pu8, k->size);
    sha256_final(h[0], &h_ctx);
    str_init(&k_p, NULL, SHA256_DIGEST_LENGTH, (const char *) h[0]);
  }
  else
    str_init(&k_p, NULL, k->size, k->ptr.p_pvoid);
  memset(pad[0], 0x5c, SHA256_BLOCK_LENGTH);
  memset(pad[1], 0x36, SHA256_BLOCK_LENGTH);
  i = 0;
  while (i < k_p.size) {
    pad[0][i] ^= k_p.ptr.p_pu8[i];
    pad[1][i] ^= k_p.ptr.p_pu8[i];
    i++;
  }
  while (i < SHA256_BLOCK_LENGTH) {
    pad[0][i] ^= 0;
    pad[1][i] ^= 0;
    i++;
  }
  sha256_init(&h_ctx);
  sha256_update(&h_ctx, pad[1], SHA256_BLOCK_LENGTH);
  sha256_update(&h_ctx, m->ptr.p_pvoid, m->size);
  sha256_final(h[1], &h_ctx);
  sha256_init(&h_ctx);
  sha256_update(&h_ctx, pad[0], SHA256_BLOCK_LENGTH);
  sha256_update(&h_ctx, h[1], SHA256_DIGEST_LENGTH);
  sha256_final(dest, &h_ctx);
}

s_str * sha256_hmac_str (const s_str *k, const s_str *m, s_str *dest)
{
  s_str tmp = {0};
  assert(k);
  assert(m);
  assert(dest);
  if (! str_init_alloc(&tmp, SHA256_DIGEST_LENGTH))
    return NULL;
  sha256_hmac(k, m, tmp.free.p_pu8);
  *dest = tmp;
  return dest;
}
