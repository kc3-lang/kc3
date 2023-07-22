/* c3
 * Copyright 2022,2023 kmx.io <contact@kmx.io>
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
/* Gen from buf_inspect_u.h.in BITS=64 bits=64 */

sw buf_inspect_u64 (s_buf *buf, const u64 *u);
sw buf_inspect_u64_base (s_buf *buf,
                             const s_str *base,
                             const u64 *u);
sw buf_inspect_u64_base_size (const s_str *base,
                                  const u64 *u);
sw buf_inspect_u64_size (const u64 *u);
