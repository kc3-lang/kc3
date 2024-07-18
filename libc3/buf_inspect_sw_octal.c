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
/* Gen from buf_inspect_s_base.c.in BITS=W bits=w base=octal */
#include <string.h>
#include "buf.h"
#include "buf_inspect_uw_octal.h"

sw buf_inspect_sw_octal (s_buf *buf, const sw *s)
{
  sw r;
  sw result = 0;
  uw u;
  u = *s;
  if (*s < 0) {
    if ((r = buf_write_1(buf, "-")) < 0)
      return r;
    result += r;
    u = -*s;
  }
  if ((r = buf_inspect_uw_octal(buf, &u)) < 0)
    return r;
  result += r;
  return result;
}

sw buf_inspect_sw_octal_size (const sw *s)
{
  sw result = 0;
  uw u;
  u = *s;
  if (*s < 0) {
    result += strlen("-");
    u = -*s;
  }
  result += buf_inspect_uw_octal_size(&u);
  return result;
}
