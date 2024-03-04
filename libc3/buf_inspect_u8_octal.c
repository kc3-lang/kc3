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
/* Gen from buf_inspect_u_base.c.in BITS=8 bits=8 BASE=octal */
#include "buf_inspect.h"
#include "c3_main.h"

sw buf_inspect_u8_octal (s_buf *buf, const u8 *u)
{
  return buf_inspect_u8_base(buf,
                                  &g_c3_base_octal,
                                  u);
}

sw buf_inspect_u8_octal_size (const u8 *u)
{
  return buf_inspect_u8_base_size(&g_c3_base_octal,
                                       u);
}
