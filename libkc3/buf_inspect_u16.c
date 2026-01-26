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
/* Gen from buf_inspect_u.c.in BITS=16 bits=16 */
#include <string.h>
#include "alloc.h"
#include "buf.h"
#include "buf_inspect.h"
#include "buf_save.h"
#include "cast.h"
#include "kc3_main.h"
#include "str.h"
#include "sym.h"

sw buf_inspect_u16 (s_buf *buf, u16 x)
{
  sw r;
  sw result = 0;
  if (g_buf_inspect_type != &g_sym_U16 &&
      cast_needed_u16(x)) {
    if ((r = buf_inspect_paren_sym(buf, &g_sym_U16)) < 0)
      return r;
    result += r;
    if ((r = buf_write_1(buf, " ")) < 0)
      return r;
    result += r;
  }
  if ((r = buf_inspect_u16_base(buf, &g_kc3_base_decimal,
                                    x)) < 0)
    return r;
  result += r;
  return result;
}

sw buf_inspect_u16_base (s_buf *buf, const s_str *base, u16 x)
{
  character *c;
  u8 digit;
  sw i;
  sw r;
  sw result = 0;
  uw radix;
  s_buf_save save;
  sw size;
  character zero;
  u16 u;
  if (x == 0) {
    if (str_character(base, 0, &zero) < 0)
      return -1;
    return buf_write_character_utf8(buf, zero);
  }
  u = x;
  size = buf_inspect_u16_base_digits(base, u);
  c = alloc(size * sizeof(character));
  buf_save_init(buf, &save);
  radix = base->size;
  i = 0;
  while (u > 0) {
    digit = u % radix;
    u /= radix;
    if (str_character(base, digit, c + i) < 0) {
      r = -1;
      goto restore;
    }
    i++;
  }
  while (i--) {
    if ((r = buf_write_character_utf8(buf, c[i])) < 0)
      goto restore;
    result += r;
  }
  r = result;
  goto clean;
 restore:
  buf_save_restore_wpos(buf, &save);
 clean:
  free(c);
  buf_save_clean(buf, &save);
  return r;
}

u8 buf_inspect_u16_base_digits (const s_str *base,
                                    u16 x)
{
  uw radix;
  u8 size = 0;
  u16 u;
  u = x;
  if (u == 0)
    return 1;
  radix = base->size;
  while (u > 0) {
    u /= radix;
    size++;
  }
  return size;
}

sw buf_inspect_u16_base_size (s_pretty *pretty, const s_str *base,
                                  u16 x)
{
  character *c;
  u8 digit;
  sw i;
  sw r;
  sw result = 0;
  uw radix;
  sw size;
  character zero;
  u16 u;
  u = x;
  if (u == 0) {
    if (str_character(base, 0, &zero) < 0)
      return -1;
    return buf_write_character_utf8_size(pretty, zero);
  }
  size = buf_inspect_u16_base_digits(base, u);
  c = alloc(size * sizeof(character));
  radix = base->size;
  i = 0;
  while (u > 0) {
    digit = u % radix;
    u /= radix;
    if (str_character(base, digit, c + i) < 0) {
      r = -1;
      goto restore;
    }
    i++;
  }
  while (i--) {
    if ((r = buf_write_character_utf8_size(pretty, c[i])) < 0)
      goto restore;
    result += r;
  }
  r = result;
  goto clean;
 restore:
 clean:
  free(c);
  return r;
}

sw buf_inspect_u16_size (s_pretty *pretty, u16 x)
{
  sw r;
  sw result = 0;
  if (g_buf_inspect_type != &g_sym_U16 &&
      cast_needed_u16(x)) {
    if ((r = buf_inspect_paren_sym_size(pretty, &g_sym_U16)) < 0)
      return r;
    result += r;
    result += strlen(" ");
  }
  if ((r = buf_inspect_u16_base_size(pretty, &g_kc3_base_decimal,
                                         x)) < 0)
    return r;
  result += r;
  return result;
}
