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
/* Gen from buf_inspect_s_base.c.in BITS=16 bits=16 base=binary */
#include <string.h>
#include "buf.h"
#include "buf_inspect_u16_binary.h"

sw buf_inspect_s16_binary (s_buf *buf, const s16 *s)
{
  sw r;
  sw result = 0;
  u16 u;
  u = *s;
  if (*s < 0) {
    if ((r = buf_write_1(buf, "-")) < 0)
      return r;
    result += r;
    u = -*s;
  }
  if ((r = buf_inspect_u16_binary(buf, &u)) < 0)
    return r;
  result += r;
  return result;
}

sw buf_inspect_s16_binary_size (const s16 *s)
{
  sw result = 0;
  u16 u;
  u = *s;
  if (*s < 0) {
    result += strlen("-");
    u = -*s;
  }
  result += buf_inspect_u16_binary_size(&u);
  return result;
}
