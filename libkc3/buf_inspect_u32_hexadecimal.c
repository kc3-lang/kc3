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
/* Gen from buf_inspect_u_base.c.in BITS=32 bits=32 BASE=hexadecimal */
#include "assert.h"
#include "buf.h"
#include "buf_inspect.h"
#include "kc3_main.h"

sw buf_inspect_u32_hexadecimal (s_buf *buf, const u32 *u)
{
  return buf_inspect_u32_base(buf,
                                  &g_kc3_base_hexadecimal,
                                  u);
}

u8 buf_inspect_u32_hexadecimal_digits (const u32 *x)
{
  return buf_inspect_u32_base_digits(&g_kc3_base_hexadecimal, x);
}

sw buf_inspect_u32_hexadecimal_pad (s_buf *buf, u8 size, character pad,
                                   const u32 *x)
{
  u8 digits;
  u8 i;
  sw r;
  sw result = 0;
  assert(buf);
  assert(size);
  assert(x);
  digits = buf_inspect_u32_hexadecimal_digits(x);
  i = digits;
  while (i < size) {
    if ((r = buf_write_character_utf8(buf, pad)) <= 0)
      return r;
    result += r;
    i++;
  }
  if ((r = buf_inspect_u32_hexadecimal(buf, x)) <= 0)
    return r;
  result += r;
  assert(size == result);
  return result;
}

sw buf_inspect_u32_hexadecimal_size (s_pretty *pretty, const u32 *u)
{
  return buf_inspect_u32_base_size(pretty, &g_kc3_base_hexadecimal,
                                       u);
}
