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
#ifndef LIBC3_WEB_EC3_H
#define LIBC3_WEB_EC3_H

#include "types.h"

/* Stack-allocation compatible functions, call ec3_clean after use. */
void    ec3_clean (p_ec3 *ec3);
p_ec3 * ec3_init (p_ec3 *ec3);

/* Operators. */
s_list ** ec3_append_and_empty_buf (s_list **tail, s_buf *buf);
s_list ** ec3_append_block (s_list **tail, s_block *block);
s_list ** ec3_append_silent_block (s_list **tail, s_block *block);
s_list ** ec3_append_str (s_list **tail, s_str *str);
s_list ** ec3_append_sym (s_list **tail, const s_sym *sym);
sw        ec3_buf_parse (s_buf *buf, p_ec3 *dest);
sw        ec3_buf_parse_c3_block (s_buf *buf, s_block *dest);

/* Observers. */
sw     ec3_render (const p_ec3 *ec3, s_buf *buf, s_map *map);
s_fn * ec3_to_render_fn (const p_ec3 *ec3, s_fn *dest);

#endif /* LIBC3_WEB_EC3_H */
