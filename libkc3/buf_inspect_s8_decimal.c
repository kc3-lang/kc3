/* kc3
 * Copyright from 2022 to 2025 kmx.io <contact@kmx.io>
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
/* Gen from buf_inspect_s_base.c.in BITS=8 bits=8 base=decimal */
#include <string.h>
#include "buf.h"
#include "buf_inspect_u8_decimal.h"

sw buf_inspect_s8_decimal (s_buf *buf, const s8 *s)
{
  sw r;
  sw result = 0;
  u8 u;
  u = *s;
  if (*s < 0) {
    if ((r = buf_write_1(buf, "-")) < 0)
      return r;
    result += r;
    u = -*s;
  }
  if ((r = buf_inspect_u8_decimal(buf, &u)) < 0)
    return r;
  result += r;
  return result;
}

sw buf_inspect_s8_decimal_size (s_pretty *pretty, const s8 *s)
{
  sw result = 0;
  u8 u;
  u = *s;
  if (*s < 0) {
    result += strlen("-");
    u = -*s;
  }
  result += buf_inspect_u8_decimal_size(pretty, &u);
  return result;
}
