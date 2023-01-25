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
#ifndef ENV_H
#define ENV_H

#include "types.h"

/* stack allocation compatible functions */
void    env_clean (s_env *env);
s_env * env_init (s_env *env);

/* modifiers */
s_tag *       env_eval_call_fn (s_env *env, s_call *call, s_tag *dest);
s_tag *       env_eval_call_macro (s_env *env, s_call *call,
                                   s_tag *dest);
s_tag *       env_eval_fn (s_env *env, s_fn *fn, s_tag *dest);
const s_tag * env_eval_ident (s_env *env, s_ident *ident);
s_tag *       env_eval_progn (s_env *env, s_list *program, s_tag *dest);
s_tag *       env_eval_tag (s_env *env, s_tag *tag, s_tag *dest);

/* control structures */
void env_error_f (s_env *env, const char *fmt, ...);
void env_error_tag (s_env *env, const s_tag *tag);
void env_longjmp (s_env *env, jmp_buf *jmp_buf);

#endif /* ENV_H */
