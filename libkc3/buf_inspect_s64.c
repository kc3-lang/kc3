/* kc3
 * Copyright 2022,2023,2024 kmx.io <contact@kmx.io>
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
/* Gen from buf_inspect_s.c.in BITS=64 bits=64 */
#include <string.h>
#include "buf.h"
#include "buf_inspect.h"
#include "buf_inspect_u64.h"
#include "sym.h"

sw buf_inspect_s64 (s_buf *buf, const s64 *s)
{
  sw r;
  sw result = 0;
  u64 u;
  if ((r = buf_inspect_paren_sym(buf, &g_sym_S64)) < 0)
    return r;
  result += r;
  if ((r = buf_write_1(buf, " ")) < 0)
    return r;
  result += r;
  u = *s;
  if (*s < 0) {
    if ((r = buf_write_1(buf, "-")) < 0)
      return r;
    result += r;
    u = -*s;
  }
  if ((r = buf_inspect_u64(buf, &u)) < 0)
    return r;
  result += r;
  return result;
}

sw buf_inspect_s64_size (const s64 *s)
{
  sw r;
  sw result = 0;
  u64 u;
  if ((r = buf_inspect_paren_sym_size(&g_sym_S64)) < 0)
    return r;
  result += r;
  result += strlen(" ");
  u = *s;
  if (*s < 0) {
    result += strlen("-");
    u = -*s;
  }
  result += buf_inspect_u64_size(&u);
  return result;
}
