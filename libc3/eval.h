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
#ifndef LIBC3_EVAL_H
#define LIBC3_EVAL_H

#include "types.h"

bool eval_call_function (const s_call *call,
                         s_tag *dest);
bool eval_call_macro (const s_call *call, s_tag *dest);
bool eval_fn (const s_fn *fn, s_tag *dest);
bool eval_fn_call (const s_fn *fn, const s_list *arguments,
                   s_tag *dest);
bool eval_ident (const s_ident *ident, s_tag *dest);
bool eval_progn (const s_list *program, s_tag *dest);
bool eval_tag (const s_tag *tag, s_tag *dest);

#endif /* LIBC3_EVAL_H */
