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
/* Gen from buf_inspect_s_base.c.in BITS=W bits=w base=binary */
#include <string.h>
#include "buf.h"
#include "buf_inspect_uw_binary.h"

sw buf_inspect_sw_binary (s_buf *buf, sw x)
{
  sw r;
  sw result = 0;
  uw u;
  u = x;
  if (x < 0) {
    if ((r = buf_write_1(buf, "-")) < 0)
      return r;
    result += r;
    u = -x;
  }
  if ((r = buf_inspect_uw_binary(buf, u)) < 0)
    return r;
  result += r;
  return result;
}

sw buf_inspect_sw_binary_size (s_pretty *pretty, sw s)
{
  sw result = 0;
  uw u;
  u = s;
  if (s < 0) {
    result += strlen("-");
    u = -s;
  }
  result += buf_inspect_uw_binary_size(pretty, u);
  return result;
}
