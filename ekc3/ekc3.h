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
#ifndef EKC3_H
#define EKC3_H

#include "types.h"

/* Stack-allocation compatible functions, call ekc3_clean after use. */
void     ekc3_clean (p_ekc3 *ekc3);
p_ekc3 * ekc3_init (p_ekc3 *ekc3);

/* Operators. */
s_list *** ekc3_append_and_empty_buf (s_list ***tail, s_buf *buf);
s_list *** ekc3_append_block (s_list ***tail, s_block *block);
s_list *** ekc3_append_silent_block (s_list ***tail, s_block *block);
s_list *** ekc3_append_str (s_list ***tail, s_str *str);
s_list *** ekc3_append_sym (s_list ***tail, const s_sym *sym);
sw         ekc3_buf_parse (s_buf *buf, p_ekc3 *dest);
sw         ekc3_buf_parse_kc3_block (s_buf *buf, s_block *dest);
sw         ekc3_buf_parse_kc3_silent_block (s_buf *buf, s_block *dest);
sw         ekc3_buf_peek_kc3_silent_block (s_buf *buf);

/* Observers. */
s_str * ekc3_escape_html (const s_str *html, s_str *dest);
sw      ekc3_render (const p_ekc3 *ekc3);
sw      ekc3_render_block_to_str (const s_block *block, s_str *dest);
sw      ekc3_render_buf (s_buf *in);
sw      ekc3_render_file (const s_str *path);
sw      ekc3_render_tag (const s_tag *tag);

#endif /* EKC3_H */
