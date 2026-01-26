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
/**
 * @file to_lisp.h
 * @brief Convert Tag to Lisp notation
 *
 * Convert any Tag to Lisp notation, that is List for code.
 */
#ifndef LIBKC3_TO_LISP_H
#define LIBKC3_TO_LISP_H

#include "types.h"

s_tag * to_lisp (s_tag *tag, s_tag *dest);
s_tag * to_lisp_call (s_call *call, s_tag *dest);
s_tag * to_lisp_list (s_list *list, s_tag *dest);
s_tag * to_lisp_tuple (s_tuple *call, s_tag *dest);

#endif /* LIBKC3_TO_LISP_H */
