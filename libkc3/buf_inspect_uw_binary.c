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
/* Gen from buf_inspect_u_base.c.in BITS=W bits=w BASE=binary */
#include "buf_inspect.h"
#include "kc3_main.h"

sw buf_inspect_uw_binary (s_buf *buf, const uw *u)
{
  return buf_inspect_uw_base(buf,
                                  &g_kc3_base_binary,
                                  u);
}

sw buf_inspect_uw_binary_size (s_pretty *pretty, const uw *u)
{
  return buf_inspect_uw_base_size(pretty, &g_kc3_base_binary,
                                       u);
}
