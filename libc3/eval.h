/* c3
 * Copyright 2022 kmx.io <contact@kmx.io>
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
#ifndef EVAL_H
#define EVAL_H

#include "types.h"

s_tag *       eval_call_function (s_env *env, s_call *call,
                                  s_tag *dest);
s_tag *       eval_call_macro (s_env *env, s_call *call, s_tag *dest);
s_tag *       eval_fn (s_env *env, s_fn *fn, s_tag *dest);
const s_tag * eval_ident (s_env *env, s_ident *ident);
s_tag *       eval_progn (s_env *env, s_list *program, s_tag *dest);
s_tag *       eval_tag (s_env *env, s_tag *tag, s_tag *dest);

#endif /* EVAL_H */
