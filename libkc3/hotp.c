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
#include <string.h>
#include "hotp.h"
#include "io.h"
#include "sha1.h"
#include "str.h"
#include "sym.h"
#include "u64.h"

void hotp_value (const s_str *k, u64 c, char dest[7])
{
  char c_pchar[8];
  s_str c_str = {0};
  int code;
  u8  hash[SHA1_DIGEST_LENGTH];
  int offset;
  s_str str;
  *(u64 *) c_pchar = htobe64(c);
  str_init(&c_str, NULL, 8, c_pchar);
  sha1_hmac(k, &c_str, hash);
  offset = hash[19] & 0xf;
  code = ((hash[offset]     & 0x7f) << 24 |
          (hash[offset + 1] & 0xff) << 16 |
          (hash[offset + 2] & 0xff) << 8 |
          (hash[offset + 3] & 0xff)) % 1000000;
  if (false) {
    err_write_1("hotp_value: c_str=");
    err_inspect_str(&c_str);
    err_write_1(" hash=");
    str_init(&str, NULL, SHA1_DIGEST_LENGTH, (const char *) hash);
    err_inspect_str_hex(&str);
    err_write_1(" code=");
    err_inspect_s32_hexadecimal(code);
    err_write_1("\n");
  }
  dest[0] = '0' + code / 100000;
  dest[1] = '0' + code / 10000 % 10;
  dest[2] = '0' + code / 1000 % 10;
  dest[3] = '0' + code / 100 % 10;
  dest[4] = '0' + code / 10 % 10;
  dest[5] = '0' + code % 10;
  dest[6] = 0;
}

s_str * hotp_value_str (const s_str *k, const s_tag *c, s_str *dest)
{
  u64 c_u64;
  const s_sym *sym_U64 = &g_sym_U64;
  s_str tmp;
  if (! u64_init_cast(&c_u64, &sym_U64, c))
    return NULL;
  if (! str_init_alloc(&tmp, 6))
    return NULL;
  hotp_value(k, c_u64, tmp.free.pchar);
  *dest = tmp;
  return dest;
}
