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
/* Gen from buf_inspect_u_base.c.in BITS=8 bits=8 BASE=decimal */
#include "buf_inspect.h"
#include "kc3_main.h"

sw buf_inspect_u8_decimal (s_pretty *pretty, s_buf *buf, const u8 *u)
{
  return buf_inspect_u8_base(pretty, buf,
                                  &g_kc3_base_decimal,
                                  u);
}

sw buf_inspect_u8_decimal_size (s_pretty *pretty, const u8 *u)
{
  return buf_inspect_u8_base_size(pretty, &g_kc3_base_decimal,
                                       u);
}
