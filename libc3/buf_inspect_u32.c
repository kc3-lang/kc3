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
/* Gen from buf_inspect_u.c.in BITS=32 bits=32 */
#include "alloc.h"
#include "buf.h"
#include "buf_inspect.h"
#include "buf_save.h"
#include "c3_main.h"
#include "str.h"

sw buf_inspect_u32 (s_buf *buf, const u32 *u)
{
  return buf_inspect_u32_base(buf, &g_c3_base_decimal, u);
}

sw buf_inspect_u32_base (s_buf *buf,
                             const s_str *base,
                             const u32 *u)
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
  u32 u_;
  u_ = *u;
  if (u_ == 0) {
    if (str_character(base, 0, &zero) < 0)
      return -1;
    return buf_write_character_utf8(buf, zero);
  }
  size = buf_inspect_u32_base_digits(base, u);
  c = alloc(size * sizeof(character));
  buf_save_init(buf, &save);
  radix = base->size;
  i = 0;
  while (u_ > 0) {
    digit = u_ % radix;
    u_ /= radix;
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

sw buf_inspect_u32_base_digits (const s_str *base,
                                    const u32 *u)
{
  uw radix;
  sw size = 0;
  u32 u_;
  u_ = *u;
  if (u_ == 0)
    return 1;
  radix = base->size;
  while (u_ > 0) {
    u_ /= radix;
    size++;
  }
  return size;
}

sw buf_inspect_u32_base_size (const s_str *base,
                                  const u32 *u)
{
  character c;
  uw digit;
  uw radix;
  sw result = 0;
  u32 u_;
  u_ = *u;
  if (u_ == 0)
    return 1;
  radix = base->size;
  while (u_ > 0) {
    digit = u_ % radix;
    u_ /= radix;
    if (str_character(base, digit, &c) < 0)
      return -1;
    result += buf_inspect_str_character_size(&c);
  }
  return result;
}

sw buf_inspect_u32_size (const u32 *u)
{
  return buf_inspect_u32_base_size(&g_c3_base_decimal, u);
}
