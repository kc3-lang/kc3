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
/* Gen from buf_parse_s.c.in BITS=64 bits=64 */
#include "assert.h"
#include "buf.h"
#include "buf_parse.h"
#include "buf_save.h"
#include "kc3_main.h"
#include "ceiling.h"
#include "str.h"

sw buf_parse_s64 (s_buf *buf, s64 *dest)
{
  s_buf_save digits;
  e_bool negative = false;
  sw r;
  sw r1;
  sw result = 0;
  s_buf_save save;
  s64 tmp = 0;
  s64 tmp1 = 0;
  assert(buf);
  assert(dest);
  buf_save_init(buf, &save);
  if ((r = buf_read_1(buf, "(S64)")) < 0)
    goto restore;
  if (r) {
    result += r;
    if ((r = buf_ignore_spaces(buf)) < 0)
      goto restore;
    result += r;
  }
  if ((r = buf_read_1(buf, "-")) < 0)
    goto restore;
  if (r > 0) {
    result += r;
    negative = true;
  }
  if ((r = buf_read_1(buf, "0b")) < 0)
    goto restore;
  if (r > 0) {
    result += r;
    if ((r = buf_parse_s64_base(buf, &g_kc3_base_binary,
                                    negative, dest)) <= 0)
      goto restore;
    result += r;
    goto ok;
  }
  if ((r = buf_read_1(buf, "0o")) < 0)
    goto restore;
  if (r > 0) {
    result += r;
    if ((r = buf_parse_s64_base(buf, &g_kc3_base_octal,
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
    if ((r = buf_parse_s64_base(buf,
                                    g_kc3_bases_hexadecimal,
                                    negative, &tmp)) < 0) {
      buf_save_clean(buf, &digits);
      goto restore;
    }
    buf_save_restore_rpos(buf, &digits);
    buf_save_clean(buf, &digits);
    if ((r1 = buf_parse_s64_base(buf, g_kc3_bases_hexadecimal + 1,
                                     negative, &tmp1)) < 0) {
      r = r1;
      goto restore;
    }
    if (r == 0 && r1 == 0) {
      if (negative)
        err_puts("invalid number: -0x");
      else
        err_puts("invalid number: 0x");
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
  if ((r = buf_parse_s64_base(buf, &g_kc3_base_decimal,
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

sw buf_parse_s64_base (s_buf *buf, const s_str *base,
                           bool negative, s64 *dest)
{
  u8 digit;
  sw r;
  sw radix;
  sw result = 0;
  s_buf_save save;
  u64 u = 0;
  assert(buf);
  assert(base);
  assert(dest);
  buf_save_init(buf, &save);
  radix = str_length_utf8(base);
  if (radix < 2 || (SW_MAX > S64_MAX && radix > S64_MAX)) {
    buf_save_clean(buf, &save);
    err_write_1("buf_parse_s64_base: invalid radix: ");
    err_inspect_sw(radix);
    err_write_1("\n");
    assert(! "buf_parse_s64_base: invalid radix");
    return -1;
  }
  while ((r = buf_parse_digit(buf, base, &digit)) > 0) {
    result += r;
    if (digit >= radix) {
      buf_save_clean(buf, &save);
      err_write_1("buf_parse_s64_base:"
                  " digit greater than or equal to radix: ");
      err_inspect_u8(digit);
      err_write_1("\n");
      assert(! "buf_parse_s64_base: digit greater than or equal to"
             " radix");
      return -1;
    }
    if (u > (u64) ceiling_s64 (S64_MAX, radix)) {
      err_puts("buf_parse_s64_base: *: integer overflow");
      r = -1;
      goto restore;
    }
    u *= radix;
    if (negative ? (-((s64) u) < S64_MIN + digit) :
        (u > (u64) S64_MAX - digit)) {
      err_puts("buf_parse_s64_base: +: integer overflow");
      assert(! "buf_parse_s64_base: +: integer overflow");
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

sw buf_parse_s64_decimal (s_buf *buf, bool negative,
                              s64 *dest)
{
  return buf_parse_s64_base(buf, &g_kc3_base_decimal,
                                negative, dest);
}

s_tag * buf_parse_tag_s64 (s_buf *buf, s_tag *dest)
{
  s_tag tmp = {0};
  tmp.type = TAG_S64;
  if (buf_parse_s64(buf, &tmp.data.td_s64) <= 0)
    return NULL;
  *dest = tmp;
  return dest;
}

s_tag * buf_parse_tag_s64_base (s_buf *buf, const s_str *base,
                                    bool negative, s_tag *dest)
{
  s_tag tmp = {0};
  tmp.type = TAG_S64;
  if (buf_parse_s64_base(buf, base, negative,
                             &tmp.data.td_s64) <= 0)
    return NULL;
  *dest = tmp;
  return dest;
}
