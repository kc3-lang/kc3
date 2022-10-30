/* c3
 * Copyright 2022 kmx.io <contact@kmx.io>
 *
 * Permission is hereby granted to use this software granted
 * the above copyright notice and this permission paragraph
 * are included in all copies and substantial portions of this
 * software.
 *
 * THIS SOFTWARE IS PROVIDED "AS-IS" WITHOUT ANY GUARANTEE OF
 * PURPOSE AND PERFORMANCE. IN NO EVENT WHATSOEVER SHALL THE
 * AUTHOR BE CONSIDERED LIABLE FOR THE USE AND PERFORMANCE OF
 * THIS SOFTWARE.
 */
/**
 * @file buf_parse.h
 * @brief Byte buffer.
 *
 * Structure to manipulate byte buffers.
 */
#ifndef BUF_PARSE_H
#define BUF_PARSE_H

#include "types.h"

/**
 * # buf_parse_
 *
 * Parse UTF-8 bytes from buf and fills dest if successful.
 *
 * Returns number of bytes read in buf, possibly zero in which case
 * dest is untouched.
 */

sw buf_parse_bool (s_buf *buf, bool *dest);
sw buf_parse_character (s_buf *buf, character *dest);
sw buf_parse_digit_bin(s_buf *buf, u8 *dest);
sw buf_parse_digit_hex (s_buf *buf, u8 *dest);
sw buf_parse_digit_oct (s_buf *buf, u8 *dest);
sw buf_parse_digit_dec (s_buf *buf, u8 *dest);
sw buf_parse_f32 (s_buf *buf, f32 *dest);
sw buf_parse_f64 (s_buf *buf, f64 *dest);
sw buf_parse_fact (s_buf *buf, s_fact *dest);
sw buf_parse_call (s_buf *buf, s_call *dest);
sw buf_parse_call_args (s_buf *buf, s_list **dest);
sw buf_parse_comments (s_buf *buf);
sw buf_parse_integer (s_buf *buf, s_integer *dest);
sw buf_parse_integer_unsigned_bin (s_buf *buf, s_integer *dest);
sw buf_parse_integer_unsigned_dec (s_buf *buf, s_integer *dest);
sw buf_parse_integer_unsigned_hex (s_buf *buf, s_integer *dest);
sw buf_parse_integer_unsigned_oct (s_buf *buf, s_integer *dest);
sw buf_parse_ident (s_buf *buf, s_ident *dest);
sw buf_parse_integer (s_buf *buf, s_integer *dest);
sw buf_parse_list (s_buf *buf, s_list **dest);
sw buf_parse_module (s_buf *buf, const s_sym **dest);
sw buf_parse_str (s_buf *buf, s_str *dest);
sw buf_parse_str_character (s_buf *buf, character *dest);
sw buf_parse_str_character_unicode (s_buf *buf, character *dest);
sw buf_parse_str_u8 (s_buf *buf, u8 *dest);
sw buf_parse_sym (s_buf *buf, const s_sym **dest);
sw buf_parse_tag (s_buf *buf, s_tag *dest);
sw buf_parse_tag_bool (s_buf *buf, s_tag *dest);
sw buf_parse_tag_call (s_buf *buf, s_tag *dest);
sw buf_parse_tag_character (s_buf *buf, s_tag *dest);
sw buf_parse_tag_ident (s_buf *buf, s_tag *dest);
sw buf_parse_tag_integer (s_buf *buf, s_tag *dest);
sw buf_parse_tag_list (s_buf *buf, s_tag *dest);
sw buf_parse_tag_str (s_buf *buf, s_tag *dest);
sw buf_parse_tag_str_character (s_buf *buf, s_tag *dest);
sw buf_parse_tag_str_u8 (s_buf *buf, s_tag *dest);
sw buf_parse_tag_sym (s_buf *buf, s_tag *dest);
sw buf_parse_tag_tuple (s_buf *buf, s_tag *dest);
sw buf_parse_tuple (s_buf *buf, s_tuple *dest);
sw buf_parse_u64_hex (s_buf *buf, u64 *dest);

#endif /* BUF_PARSE_H */
