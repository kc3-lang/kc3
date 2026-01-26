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
/* Gen from buf_inspect_s.c.in BITS=8 bits=8 */
#include <string.h>
#include "buf.h"
#include "buf_inspect.h"
#include "buf_inspect_u8.h"
#include "buf_inspect_u8_decimal.h"
#include "cast.h"
#include "sym.h"

sw buf_inspect_s8 (s_buf *buf, s8 x)
{
  sw r;
  sw result = 0;
  u8 u;
  if (g_buf_inspect_type != &g_sym_S8 &&
      cast_needed_s8(x)) {
    if ((r = buf_inspect_paren_sym(buf, &g_sym_S8)) < 0)
      return r;
    result += r;
    if ((r = buf_write_1(buf, " ")) < 0)
      return r;
    result += r;
  }
  u = x;
  if (x < 0) {
    if ((r = buf_write_1(buf, "-")) < 0)
      return r;
    result += r;
    u = -x;
  }
  if ((r = buf_inspect_u8_decimal(buf, u)) < 0)
    return r;
  result += r;
  return result;
}

sw buf_inspect_s8_size (s_pretty *pretty, s8 x)
{
  sw r;
  sw result = 0;
  u8 u;
  if (g_buf_inspect_type != &g_sym_S8 &&
      cast_needed_s8(x)) {
    if ((r = buf_inspect_paren_sym_size(pretty, &g_sym_S8)) < 0)
      return r;
    result += r;
    if ((r = buf_write_1_size(pretty, " ")) < 0)
      return r;
    result += r;
  }
  u = x;
  if (x < 0) {
    if ((r = buf_write_1_size(pretty, "-")) < 0)
      return r;
    result += r;
    u = -x;
  }
  if ((r = buf_inspect_u8_decimal_size(pretty, u)) < 0)
    return r;
  result += r;
  return result;
}
