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
#ifndef LIBKC3_EVAL_H
#define LIBKC3_EVAL_H

#include "types.h"

bool eval_array (s_array *array, s_array *dest);
bool eval_call (s_call *call, s_tag *dest);
bool eval_call_function (s_call *call,
                         s_tag *dest);
bool eval_call_macro (s_call *call, s_tag *dest);
bool eval_callable_call (s_callable *callable, s_list *arguments,
                         s_tag *dest);
bool eval_fn (const s_fn *fn, s_tag *dest);
bool eval_fn_call (const s_fn *fn, s_list *arguments,
                   s_tag *dest);
bool eval_ident (const s_ident *ident, s_tag *dest);
bool eval_progn (s_list *program, s_tag *dest);
bool eval_struct (s_struct *s, p_struct *dest);
bool eval_tag (s_tag *tag, s_tag *dest);

#endif /* LIBKC3_EVAL_H */
