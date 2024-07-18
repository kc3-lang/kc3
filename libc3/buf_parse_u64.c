/* c3
 * Copyright 2022-2024 kmx.io <contact@kmx.io>
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
/* Gen from buf_parse_u.c.in BITS=64 bits=64 */
#include "assert.h"
#include "buf.h"
#include "buf_parse.h"
#include "buf_save.h"
#include "c3_main.h"
#include "ceiling.h"
#include "io.h"
#include "str.h"

sw buf_parse_u64 (s_buf *buf, u64 *dest)
{
  s_buf_save digits;
  sw r;
  sw r1;
  sw result = 0;
  s_buf_save save;
  u64 tmp;
  u64 tmp1;
  assert(buf);
  assert(dest);
  buf_save_init(buf, &save);
  if ((r = buf_read_1(buf, "0b")) < 0)
    goto restore;
  if (r > 0) {
    result += r;
    if ((r = buf_parse_u64_base(buf, &g_c3_base_binary, dest)) <= 0)
      goto restore;
    result += r;
    goto ok;
  }
  if ((r = buf_read_1(buf, "0o")) < 0)
    goto restore;
  if (r > 0) {
    result += r;
    if ((r = buf_parse_u64_base(buf, &g_c3_base_octal, dest)) <= 0)
      goto restore;
    result += r;
    goto ok;
  }
  if ((r = buf_read_1(buf, "0x")) < 0)
    goto restore;
  if (r > 0) {
    result += r;
    buf_save_init(buf, &digits);
    if ((r = buf_parse_u64_base(buf, g_c3_bases_hexadecimal,
                                    &tmp)) < 0) {
      buf_save_clean(buf, &digits);
      goto restore;
    }
    buf_save_restore_rpos(buf, &digits);
    buf_save_clean(buf, &digits);
    if ((r1 = buf_parse_u64_base(buf, g_c3_bases_hexadecimal + 1,
                                     &tmp1)) < 0) {
      r = r1;
      goto restore;
    }
    if (r == 0 && r1 == 0) {
      err_write_1("invalid number: 0x");
      goto restore;
    }
    if (r > r1) {
      result += r;
      *dest = tmp;
    }
    else {
      result += r1;
      *dest = tmp1;
    }
    goto ok;
  }
  if ((r = buf_parse_u64_base(buf, &g_c3_base_decimal, dest)) <= 0)
    goto restore;
  result += r;
 ok:
  r = result;
  goto clean;
 restore:
  buf_save_restore_rpos(buf, &save);
 clean:
  buf_save_clean(buf, &save);
  return r;
}

sw buf_parse_u64_base (s_buf *buf, const s_str *base,
                           u64 *dest)
{
  u8 digit;
  sw r;
  sw radix;
  sw result = 0;
  s_buf_save save;
  u64 u = 0;
  u64 u2 = 0;
  assert(buf);
  assert(base);
  assert(dest);
  buf_save_init(buf, &save);
  radix = str_length_utf8(base);
  if (radix < 2 || (uw) radix > U64_MAX) {
    buf_save_clean(buf, &save);
    err_write_1("buf_parse_u64_base: invalid radix: ");
    err_inspect_sw(&radix);
    assert(! "buf_parse_u64_base: invalid radix");
    return -1;
  }
  while ((r = buf_parse_digit(buf, base, &digit)) > 0) {
    result += r;
    if (digit >= radix) {
      buf_save_clean(buf, &save);
      err_write_1("buf_parse_u64_base:"
                  " digit greater than or equal to radix: ");
      err_inspect_u8(&digit);
      assert(! "buf_parse_u64_base: digit greater than or equal to"
             " radix");
      return -1;
    }
    if (u > ceiling_u64(U64_MAX, radix)) {
      err_write_1("buf_parse_u64_base: ");
      err_inspect_u64(&u);
      err_write_1(" * ");
      err_inspect_sw(&radix);
      err_write_1(": integer overflow");
      r = -1;
      goto restore;
    }
    u *= radix;
    u2 = U64_MAX - digit;
    if (u > u2) {
      err_write_1("buf_parse_u64_base: +: integer overflow");
      r = -1;
      goto restore;
    }
    u += digit;
  }
  *dest = u;
  r = result;
  goto clean;
 restore:
  buf_save_restore_rpos(buf, &save);
 clean:
  buf_save_clean(buf, &save);
  return r;
}
