/* c3
 * Copyright 2022,2023 kmx.io <contact@kmx.io>
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
/* Gen from buf_parse_s.c.in BITS=32 bits=32 */

#include "c3.h"

sw buf_parse_s32 (s_buf *buf, s32 *dest)
{
  s_buf_save digits;
  e_bool negative = false;
  sw r;
  sw r1;
  sw result = 0;
  s_buf_save save;
  s32 tmp;
  s32 tmp1;
  buf_save_init(buf, &save);
  if ((r = buf_read_1(buf, "-")) < 0)
    goto clean;
  if (r > 0) {
    result += r;
    negative = true;
  }
  if ((r = buf_read_1(buf, "0b")) < 0)
    goto restore;
  if (r > 0) {
    result += r;
    if ((r = buf_parse_s32_base(buf, &g_c3_base_binary,
                                    negative, dest)) <= 0)
      goto restore;
    result += r;
    goto ok;
  }
  if ((r = buf_read_1(buf, "0o")) < 0)
    goto restore;
  if (r > 0) {
    result += r;
    if ((r = buf_parse_s32_base(buf, &g_c3_base_octal,
                                    negative, dest)) <= 0)
      goto restore;
    result += r;
    goto ok;
  }
  if ((r = buf_read_1(buf, "0x")) < 0)
    goto restore;
  if (r > 0) {
    result += r;
    buf_save_init(buf, &digits);
    if ((r = buf_parse_s32_base(buf,
                                    g_c3_bases_hexadecimal,
                                    negative, &tmp)) < 0) {
      buf_save_clean(buf, &digits);
      goto restore;
    }
    buf_save_restore_rpos(buf, &digits);
    buf_save_clean(buf, &digits);
    if ((r1 = buf_parse_s32_base(buf, g_c3_bases_hexadecimal + 1,
                                     negative, &tmp1)) < 0) {
      r = r1;
      goto restore;
    }
    if (r == 0 && r1 == 0) {
      if (negative)
        warnx("invalid number: -0x");
      else
        warnx("invalid number: 0x");
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
  if ((r = buf_parse_s32_base(buf, &g_c3_base_decimal,
                                  negative, dest)) <= 0)
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

sw buf_parse_s32_base (s_buf *buf, const s_str *base,
                           bool negative, s32 *dest)
{
  u8 digit;
  sw r;
  sw radix;
  sw result = 0;
  s_buf_save save;
  u32 u = 0;
  assert(buf);
  assert(base);
  assert(dest);
  buf_save_init(buf, &save);
  radix = str_length_utf8(base);
  if (radix < 2 || radix > S32_MAX) {
    buf_save_clean(buf, &save);
    assert(! "buf_parse_s32_base: invalid radix");
    errx(1, "buf_parse_s32_base: invalid radix: %ld",
         radix);
    return -1;
  }
  while ((r = buf_parse_digit(buf, base, &digit)) > 0) {
    result += r;
    if (digit >= radix) {
      buf_save_clean(buf, &save);
      assert(! "buf_parse_s32_base: digit greater than or equal to"
             " radix");
      errx(1, "buf_parse_s32_base: digit greater than or equal to"
           " radix: %u",
           digit);
      return -1;
    }
    if (u > (u32) ceiling_s32 (S32_MAX, radix)) {
      warnx("buf_parse_s32_base: *: integer overflow");
      r = -1;
      goto restore;
    }
    u *= radix;
    if (negative ? -u < (u32) S32_MIN + digit :
        u > (u32) (S32_MAX - digit)) {
      warnx("buf_parse_s32_base: +: integer overflow");
      r = -1;
      goto restore;
    }
    u += digit;
  }
  *dest = negative ? -u : u;
  r = result;
  goto clean;
 restore:
  buf_save_restore_rpos(buf, &save);
 clean:
  buf_save_clean(buf, &save);
  return r;
}
