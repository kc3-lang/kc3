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
/* Gen from buf_inspect_u.c.in BITS=16 bits=16 */
#include "buf.h"
#include "buf_inspect.h"
#include "buf_save.h"
#include "c3_main.h"
#include "str.h"

sw buf_inspect_u16 (s_buf *buf, const u16 *u)
{
  return buf_inspect_u16_base(buf, &g_c3_base_decimal, u);
}

sw buf_inspect_u16_base (s_buf *buf,
                             const s_str *base,
                             const u16 *u)
{
  character *c;
  u8 digit;
  sw i;
  sw r;
  uw radix;
  s_buf_save save;
  sw size;
  character zero;
  u16 u_;
  u_ = *u;
  if (u_ == 0) {
    if (str_character(base, 0, &zero) < 0)
      return -1;
    return buf_write_character_utf8(buf, zero);
  }
  size = buf_inspect_u16_base_size(base, u);
  c = calloc(size, sizeof(character));
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
  while (i--)
    if ((r = buf_write_character_utf8(buf, c[i])) < 0)
      goto restore;
  r = size;
  goto clean;
 restore:
  buf_save_restore_wpos(buf, &save);
 clean:
  free(c);
  buf_save_clean(buf, &save);
  return r;
}

sw buf_inspect_u16_base_size (const s_str *base,
                                  const u16 *u)
{
  uw radix;
  sw size = 0;
  u16 u_;
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

sw buf_inspect_u16_size (const u16 *u)
{
  return buf_inspect_u16_base_size(&g_c3_base_decimal, u);
}
