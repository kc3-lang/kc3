/*
 * public domain sha512 crypt implementation
 *
 * original sha crypt design:
 * http://people.redhat.com/drepper/SHA-crypt.txt
 * in this implementation at least 32bit int is assumed, key length
 * is limited, the $6$ prefix is mandatory, * '\n' and ':' is rejected
 * in the salt and rounds= setting must contain a valid iteration count,
 * on error "*" is returned.
 */
#include <string.h>
#include "assert.h"
#include "explicit_bzero.h"
#include "sha512.h"
#include "str.h"

/* public domain sha512 implementation based on fips180-3 */
/* >=2^64 bits messages are not supported (about 2000 peta bytes) */

#define Ch(x,y,z) (z ^ (x & (y ^ z)))
#define Maj(x,y,z) ((x & y) | (z & (x | y)))
#define S0(x) (ror(x, 28) ^ ror(x, 34) ^ ror(x, 39))
#define S1(x) (ror(x, 14) ^ ror(x, 18) ^ ror(x, 41))
#define R0(x) (ror(x,  1) ^ ror(x,  8) ^ (x >> 7))
#define R1(x) (ror(x, 19) ^ ror(x, 61) ^ (x >> 6))

static const u64 k[80] =
  { 0x428a2f98d728ae22ULL, 0x7137449123ef65cdULL,
    0xb5c0fbcfec4d3b2fULL, 0xe9b5dba58189dbbcULL,
    0x3956c25bf348b538ULL, 0x59f111f1b605d019ULL,
    0x923f82a4af194f9bULL, 0xab1c5ed5da6d8118ULL,
    0xd807aa98a3030242ULL, 0x12835b0145706fbeULL,
    0x243185be4ee4b28cULL, 0x550c7dc3d5ffb4e2ULL,
    0x72be5d74f27b896fULL, 0x80deb1fe3b1696b1ULL,
    0x9bdc06a725c71235ULL, 0xc19bf174cf692694ULL,
    0xe49b69c19ef14ad2ULL, 0xefbe4786384f25e3ULL,
    0x0fc19dc68b8cd5b5ULL, 0x240ca1cc77ac9c65ULL,
    0x2de92c6f592b0275ULL, 0x4a7484aa6ea6e483ULL,
    0x5cb0a9dcbd41fbd4ULL, 0x76f988da831153b5ULL,
    0x983e5152ee66dfabULL, 0xa831c66d2db43210ULL,
    0xb00327c898fb213fULL, 0xbf597fc7beef0ee4ULL,
    0xc6e00bf33da88fc2ULL, 0xd5a79147930aa725ULL,
    0x06ca6351e003826fULL, 0x142929670a0e6e70ULL,
    0x27b70a8546d22ffcULL, 0x2e1b21385c26c926ULL,
    0x4d2c6dfc5ac42aedULL, 0x53380d139d95b3dfULL,
    0x650a73548baf63deULL, 0x766a0abb3c77b2a8ULL,
    0x81c2c92e47edaee6ULL, 0x92722c851482353bULL,
    0xa2bfe8a14cf10364ULL, 0xa81a664bbc423001ULL,
    0xc24b8b70d0f89791ULL, 0xc76c51a30654be30ULL,
    0xd192e819d6ef5218ULL, 0xd69906245565a910ULL,
    0xf40e35855771202aULL, 0x106aa07032bbd1b8ULL,
    0x19a4c116b8d2d0c8ULL, 0x1e376c085141ab53ULL,
    0x2748774cdf8eeb99ULL, 0x34b0bcb5e19b48a8ULL,
    0x391c0cb3c5c95a63ULL, 0x4ed8aa4ae3418acbULL,
    0x5b9cca4f7763e373ULL, 0x682e6ff3d6b2b8a3ULL,
    0x748f82ee5defb2fcULL, 0x78a5636f43172f60ULL,
    0x84c87814a1f0ab72ULL, 0x8cc702081a6439ecULL,
    0x90befffa23631e28ULL, 0xa4506cebde82bde9ULL,
    0xbef9a3f7b2c67915ULL, 0xc67178f2e372532bULL,
    0xca273eceea26619cULL, 0xd186b8c721c0c207ULL,
    0xeada7dd6cde0eb1eULL, 0xf57d4f7fee6ed178ULL,
    0x06f067aa72176fbaULL, 0x0a637dc5a2c898a6ULL,
    0x113f9804bef90daeULL, 0x1b710b35131c471bULL,
    0x28db77f523047d84ULL, 0x32caab7b40c72493ULL,
    0x3c9ebe0a15c9bebcULL, 0x431d67c49c100d4cULL,
    0x4cc5d4becb3e42b6ULL, 0x597f299cfc657e2aULL,
    0x5fcb6fab3ad6faecULL, 0x6c44198c4a475817ULL };

static void process_block (s_sha512 *s, const u8 *buf);
static u64  ror (u64 n, s32 k);

static void pad (s_sha512 *s)
{
  u32 r = s->len % 128;
  s->buf[r++] = 0x80;
  if (r > 112) {
    explicit_bzero(s->buf + r, 128 - r);
    r = 0;
    process_block(s, s->buf);
  }
  explicit_bzero(s->buf + r, 120 - r);
  s->len *= 8;
  s->buf[120] = s->len >> 56;
  s->buf[121] = s->len >> 48;
  s->buf[122] = s->len >> 40;
  s->buf[123] = s->len >> 32;
  s->buf[124] = s->len >> 24;
  s->buf[125] = s->len >> 16;
  s->buf[126] = s->len >> 8;
  s->buf[127] = s->len;
  process_block(s, s->buf);
}

static void process_block (s_sha512 *s, const u8 *buf)
{
  u64 a;
  u64 b;
  u64 c;
  u64 d;
  u64 e;
  u64 f;
  u64 g;
  u64 h;
  s32 i;
  u64 t1;
  u64 t2;
  u64 w[80];
  i = 0;
  while (i < 16) {
    w[i]  = (uint64_t) buf[8 * i]     << 56;
    w[i] |= (uint64_t) buf[8 * i + 1] << 48;
    w[i] |= (uint64_t) buf[8 * i + 2] << 40;
    w[i] |= (uint64_t) buf[8 * i + 3] << 32;
    w[i] |= (uint64_t) buf[8 * i + 4] << 24;
    w[i] |= (uint64_t) buf[8 * i + 5] << 16;
    w[i] |= (uint64_t) buf[8 * i + 6] << 8;
    w[i] |=            buf[8 * i + 7];
    i++;
  }
  while (i < 80) {
    w[i] = R1(w[i - 2]) + w[i - 7] + R0(w[i - 15]) + w[i - 16];
    i++;
  }
  a = s->h[0];
  b = s->h[1];
  c = s->h[2];
  d = s->h[3];
  e = s->h[4];
  f = s->h[5];
  g = s->h[6];
  h = s->h[7];
  i = 0;
  while (i < 80) {
    t1 = h + S1(e) + Ch(e, f, g) + k[i] + w[i];
    t2 = S0(a) + Maj(a, b, c);
    h = g;
    g = f;
    f = e;
    e = d + t1;
    d = c;
    c = b;
    b = a;
    a = t1 + t2;
    i++;
  }
  s->h[0] += a;
  s->h[1] += b;
  s->h[2] += c;
  s->h[3] += d;
  s->h[4] += e;
  s->h[5] += f;
  s->h[6] += g;
  s->h[7] += h;
}

static u64 ror (u64 n, s32 k)
{
  return (n >> k) | (n << (64 - k));
}

void sha512_init (s_sha512 *s)
{
  s->len = 0;
  s->h[0] = 0x6a09e667f3bcc908ULL;
  s->h[1] = 0xbb67ae8584caa73bULL;
  s->h[2] = 0x3c6ef372fe94f82bULL;
  s->h[3] = 0xa54ff53a5f1d36f1ULL;
  s->h[4] = 0x510e527fade682d1ULL;
  s->h[5] = 0x9b05688c2b3e6c1fULL;
  s->h[6] = 0x1f83d9abfb41bd6bULL;
  s->h[7] = 0x5be0cd19137e2179ULL;
}

void sha512_sum (s_sha512 *s, u8 *md)
{
  s32 i;
  pad(s);
  i = 0;
  while (i < 8) {
    md[8 * i]     = s->h[i] >> 56;
    md[8 * i + 1] = s->h[i] >> 48;
    md[8 * i + 2] = s->h[i] >> 40;
    md[8 * i + 3] = s->h[i] >> 32;
    md[8 * i + 4] = s->h[i] >> 24;
    md[8 * i + 5] = s->h[i] >> 16;
    md[8 * i + 6] = s->h[i] >> 8;
    md[8 * i + 7] = s->h[i];
    i++;
  }
}

void sha512_update (s_sha512 *s, const void *m, u64 len)
{
  const u8 *p;
  u32 r;
  p = m;
  r = s->len % 128;
  s->len += len;
  if (r) {
    if (len < 128 - r) {
      memcpy(s->buf + r, p, len);
      return;
    }
    memcpy(s->buf + r, p, 128 - r);
    len -= 128 - r;
    p += 128 - r;
    process_block(s, s->buf);
  }
  while (len >= 128) {
    process_block(s, p);
    len -= 128;
    p += 128;
  }
  memcpy(s->buf, p, len);
}

void sha512_hmac (const s_str *k, const s_str *m, u8 *dest)
{
  s_sha512 h_ctx;
  u8       h[2][SHA512_DIGEST_LENGTH] = {0};
  u8       i;
  s_str    k_p = {0};
  u8       pad[2][SHA512_BLOCK_LENGTH];
  assert(SHA512_DIGEST_LENGTH <= SHA512_BLOCK_LENGTH);
  if (k->size > SHA512_BLOCK_LENGTH) {
    sha512_init(&h_ctx);
    sha512_update(&h_ctx, k->ptr.p_pu8, k->size);
    sha512_sum(&h_ctx, h[0]);
    str_init(&k_p, NULL, SHA512_DIGEST_LENGTH, (const char *) h[0]);
  }
  else
    str_init(&k_p, NULL, k->size, k->ptr.p_pvoid);
  memset(pad[0], 0x5c, SHA512_BLOCK_LENGTH);
  memset(pad[1], 0x36, SHA512_BLOCK_LENGTH);
  i = 0;
  while (i < k_p.size) {
    pad[0][i] ^= k_p.ptr.p_pu8[i];
    pad[1][i] ^= k_p.ptr.p_pu8[i];
    i++;
  }
  while (i < SHA512_BLOCK_LENGTH) {
    pad[0][i] ^= 0;
    pad[1][i] ^= 0;
    i++;
  }
  sha512_init(&h_ctx);
  sha512_update(&h_ctx, pad[1], SHA512_BLOCK_LENGTH);
  sha512_update(&h_ctx, m->ptr.p_pvoid, m->size);
  sha512_sum(&h_ctx, h[1]);
  sha512_init(&h_ctx);
  sha512_update(&h_ctx, pad[0], SHA512_BLOCK_LENGTH);
  sha512_update(&h_ctx, h[1], SHA512_DIGEST_LENGTH);
  sha512_sum(&h_ctx, dest);
}

s_str * sha512_hmac_str (const s_str *k, const s_str *m, s_str *dest)
{
  s_str tmp = {0};
  assert(k);
  assert(m);
  assert(dest);
  if (! str_init_alloc(&tmp, SHA512_DIGEST_LENGTH))
    return NULL;
  sha512_hmac(k, m, tmp.free.p_pu8);
  *dest = tmp;
  return dest;
}
