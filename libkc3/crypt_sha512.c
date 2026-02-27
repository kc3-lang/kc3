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
#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "assert.h"
#include "io.h"
#include "sha512.h"
#include "str.h"

#define KEY_MAX 256
#define SALT_MAX 16
#define ROUNDS_DEFAULT 12345
#define ROUNDS_MIN 1000
#define ROUNDS_MAX 999999999

static const u8 b64[] =
  "./0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

static void   hash_md (s_sha512 *s, u32 n, const void *md);
static char * sha512_crypt(const char *key, const char *setting,
                           char *output);
static char * to64 (char *s, u32 u, s32 n);

s_str * crypt_sha512 (const s_str *key, const s_str *setting,
                      s_str *dest)
{
  static const char testkey[] =
    "Xy01@#\x01\x02\x80\x7f\xff\r\n\x81\t !";
  static const char testsetting[] = "$6$rounds=1234$abc0123456789$";
  static const char testhash[] =
    "$6$rounds=1234$abc0123456789$"
    "BCpt8zLrc/RcyuXmCDOE1ALqMXB2MH6n1g891HhFj8.w7LxGv.FTkqq6Vxc/"
    "km3Y0jE0j24jY5PIv/oOu6reg1";
  char testbuf[128];
  char *q;
  char outbuf[128];
  char *p;
  assert(key);
  assert(setting);
  assert(dest);
  p = sha512_crypt(key->ptr.p_pchar, setting->ptr.p_pchar, outbuf);
  q = sha512_crypt(testkey, testsetting, testbuf);
  if (p != outbuf) {
    err_puts("crypt_sha512: sha512_crypt p");
    assert(! "crypt_sha512: sha512_crypt p");
    return NULL;
  }
  if (q != testbuf) {
    err_puts("crypt_sha512: sha512_crypt q");
    assert(! "crypt_sha512: sha512_crypt q");
    return NULL;
  }
  if (memcmp(testbuf, testhash, sizeof(testhash))) {
    err_puts("crypt_sha512: memcmp");
    assert(! "crypt_sha512: memcmp");
    return NULL;
  }
  return str_init_copy_1(dest, outbuf);
}

/* hash n bytes of the repeated md message digest */
static void hash_md (s_sha512 *s, u32 n, const void *md)
{
  unsigned int i;
  for (i = n; i > 64; i -= 64)
    sha512_update(s, md, 64);
  sha512_update(s, md, i);
}

static char * sha512_crypt(const char *key, const char *setting,
                           char *output)
{
  struct sha512 ctx;
  u8 md[64];
  u32 i;
  u32 r;
  u32 klen;
  u8  kmd[64];
  u32 slen;
  u8  smd[64];
  char rounds[20] = "";
  const char *salt;
  char *p;
  /* reject large keys */
  i = 0;
  while (i <= KEY_MAX && key[i])
    i++;
  if (i > KEY_MAX)
    return 0;
  klen = i;
  /* setting: $6$rounds=n$salt$ (rounds=n$ and closing $ are
   * optional) */
  if (strncmp(setting, "$6$", 3) != 0)
    return 0;
  salt = setting + 3;
  r = ROUNDS_DEFAULT;
  if (strncmp(salt, "rounds=", sizeof("rounds=") - 1) == 0) {
    unsigned long u;
    char *end;
    /*
     * this is a deviation from the reference:
     * bad rounds setting is rejected if it is
     * - empty
     * - unterminated (missing '$')
     * - begins with anything but a decimal digit
     * the reference implementation treats these bad
     * rounds as part of the salt or parse them with
     * strtoul semantics which may cause problems
     * including non-portable hashes that depend on
     * the host's value of ULONG_MAX.
     */
    salt += sizeof("rounds=") - 1;
    if (! isdigit((int) *salt))
      return 0;
    u = strtoul(salt, &end, 10);
    if (*end != '$')
      return 0;
    salt = end + 1;
    if (u < ROUNDS_MIN)
      r = ROUNDS_MIN;
    else if (u > ROUNDS_MAX)
      return 0;
    else
      r = u;
    /* needed when rounds is zero prefixed or out of bounds */
    snprintf(rounds, sizeof(rounds), "rounds=%u$", r);
  }

  for (i = 0; i < SALT_MAX && salt[i] && salt[i] != '$'; i++)
    /* reject characters that interfere with /etc/shadow parsing */
    if (salt[i] == '\n' || salt[i] == ':')
      return 0;
  slen = i;

  /* B = sha(key salt key) */
  sha512_init(&ctx);
  sha512_update(&ctx, key, klen);
  sha512_update(&ctx, salt, slen);
  sha512_update(&ctx, key, klen);
  sha512_sum(&ctx, md);

  /* A = sha(key salt repeat-B alternate-B-key) */
  sha512_init(&ctx);
  sha512_update(&ctx, key, klen);
  sha512_update(&ctx, salt, slen);
  hash_md(&ctx, klen, md);
  for (i = klen; i > 0; i >>= 1)
    if (i & 1)
      sha512_update(&ctx, md, sizeof md);
    else
      sha512_update(&ctx, key, klen);
  sha512_sum(&ctx, md);

  /* DP = sha(repeat-key), this step takes O(klen^2) time */
  sha512_init(&ctx);
  for (i = 0; i < klen; i++)
    sha512_update(&ctx, key, klen);
  sha512_sum(&ctx, kmd);

  /* DS = sha(repeat-salt) */
  sha512_init(&ctx);
  for (i = 0; (s32) i < 16 + md[0]; i++)
    sha512_update(&ctx, salt, slen);
  sha512_sum(&ctx, smd);

  /* iterate A = f(A, DP, DS), this step takes O(rounds*klen) time */
  for (i = 0; i < r; i++) {
    sha512_init(&ctx);
    if (i % 2)
      hash_md(&ctx, klen, kmd);
    else
      sha512_update(&ctx, md, sizeof(md));
    if (i % 3)
      sha512_update(&ctx, smd, slen);
    if (i % 7)
      hash_md(&ctx, klen, kmd);
    if (i % 2)
      sha512_update(&ctx, md, sizeof(md));
    else
      hash_md(&ctx, klen, kmd);
    sha512_sum(&ctx, md);
  }

  /* output is $6$rounds=n$salt$hash */
  p = output;
  p += snprintf(p, 128, "$6$%s%.*s$", rounds, slen, salt);
  #if 1
  static const u8 perm[][3] = {
    {  0, 21, 42 }, { 22, 43,  1 }, { 44,  2, 23 }, {  3, 24, 45 },
    { 25, 46,  4 }, { 47,  5, 26 }, {  6, 27, 48 }, { 28, 49,  7 },
    { 50,  8, 29 }, {  9, 30, 51 }, { 31, 52, 10 }, { 53, 11, 32 },
    { 12, 33, 54 }, { 34, 55, 13 }, { 56, 14, 35 }, { 15, 36, 57 },
    { 37, 58, 16 }, { 59, 17, 38 }, { 18, 39, 60 }, { 40, 61, 19 },
    { 62, 20, 41 } };
  for (i=0; i<21; i++) p = to64(p,
                                (md[perm[i][0]]<<16) |
                                (md[perm[i][1]]<<8) |
                                md[perm[i][2]], 4);
  #else
  p = to64(p, (md[0]<<16)|(md[21]<<8)|md[42], 4);
  p = to64(p, (md[22]<<16)|(md[43]<<8)|md[1], 4);
  p = to64(p, (md[44]<<16)|(md[2]<<8)|md[23], 4);
  p = to64(p, (md[3]<<16)|(md[24]<<8)|md[45], 4);
  p = to64(p, (md[25]<<16)|(md[46]<<8)|md[4], 4);
  p = to64(p, (md[47]<<16)|(md[5]<<8)|md[26], 4);
  p = to64(p, (md[6]<<16)|(md[27]<<8)|md[48], 4);
  p = to64(p, (md[28]<<16)|(md[49]<<8)|md[7], 4);
  p = to64(p, (md[50]<<16)|(md[8]<<8)|md[29], 4);
  p = to64(p, (md[9]<<16)|(md[30]<<8)|md[51], 4);
  p = to64(p, (md[31]<<16)|(md[52]<<8)|md[10], 4);
  p = to64(p, (md[53]<<16)|(md[11]<<8)|md[32], 4);
  p = to64(p, (md[12]<<16)|(md[33]<<8)|md[54], 4);
  p = to64(p, (md[34]<<16)|(md[55]<<8)|md[13], 4);
  p = to64(p, (md[56]<<16)|(md[14]<<8)|md[35], 4);
  p = to64(p, (md[15]<<16)|(md[36]<<8)|md[57], 4);
  p = to64(p, (md[37]<<16)|(md[58]<<8)|md[16], 4);
  p = to64(p, (md[59]<<16)|(md[17]<<8)|md[38], 4);
  p = to64(p, (md[18]<<16)|(md[39]<<8)|md[60], 4);
  p = to64(p, (md[40]<<16)|(md[61]<<8)|md[19], 4);
  p = to64(p, (md[62]<<16)|(md[20]<<8)|md[41], 4);
  #endif
  p = to64(p, md[63], 2);
  *p = 0;
  return output;
}

static char * to64 (char *s, u32 u, s32 n)
{
  while (--n >= 0) {
    *s++ = b64[u % 64];
    u /= 64;
  }
  return s;
}
