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
/* Gen from buf_inspect_u_base.c.in BITS=8 bits=8 BASE=binary */
#include "assert.h"
#include "buf.h"
#include "buf_inspect.h"
#include "kc3_main.h"

sw buf_inspect_u8_binary (s_buf *buf, u8 u)
{
  return buf_inspect_u8_base
    (buf, &g_kc3_base_binary, u);
}

u8 buf_inspect_u8_binary_digits (u8 x)
{
  return buf_inspect_u8_base_digits
    (&g_kc3_base_binary, x);
}

sw buf_inspect_u8_binary_pad (s_buf *buf, u8 size, character pad,
                                   u8 x)
{
  u8 digits;
  u8 i;
  sw r;
  sw result = 0;
  assert(buf);
  assert(size);
  digits = buf_inspect_u8_binary_digits(x);
  i = digits;
  while (i < size) {
    if ((r = buf_write_character_utf8(buf, pad)) <= 0)
      return r;
    result += r;
    i++;
  }
  if ((r = buf_inspect_u8_binary(buf, x)) <= 0)
    return r;
  result += r;
  assert(size == result);
  return result;
}

sw buf_inspect_u8_binary_size (s_pretty *pretty, u8 x)
{
  return buf_inspect_u8_base_size(pretty, &g_kc3_base_binary,
                                       x);
}
