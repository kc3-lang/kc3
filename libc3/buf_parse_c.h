/* c3
 * Copyright 2022,2023 kmx.io <contact@kmx.io>
 *
 * Permission is hereby granted to use this software excepted
 * on Apple computers granted the above copyright notice and
 * this permission paragraph are included in all copies and
 * substantial portions of this software.
 *
 * THIS SOFTWARE IS PROVIDED "AS-IS" WITHOUT ANY GUARANTEE OF
 * PURPOSE AND PERFORMANCE. IN NO EVENT WHATSOEVER SHALL THE
 * AUTHOR BE CONSIDERED LIABLE FOR THE USE AND PERFORMANCE OF
 * THIS SOFTWARE.
 */
/**
 * @file buf_parse_c.h
 * @brief Parse C.
 *
 * Functions to parse C from buf.
 */
#ifndef BUF_PARSE_C_H
#define BUF_PARSE_C_H

#include "c_types.h"

sw buf_parse_c_ident (s_buf *buf, const s_sym **dest);
sw buf_parse_c_prototype (s_buf *buf, s_tuple *dest);
sw buf_parse_c_type (s_buf *buf, s_sym_list **dest);
sw buf_parse_c_typedef (s_buf *buf, s_c_typedef *dest);

#endif /* BUF_PARSE_C_H */
