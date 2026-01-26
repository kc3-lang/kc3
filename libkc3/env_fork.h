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
#ifndef LIBKC3_ENV_FORK_H
#define LIBKC3_ENV_FORK_H

#include "types.h"

/* Stack-allocation compatible functions, call env_fork_clean
   after use. */
void    env_fork_clean (s_env *env);
s_env * env_fork_init (s_env *env, s_env *src);

/* Heap-allocation functions, call env_fork_delete after use. */
void    env_fork_delete (s_env *env);
s_env * env_fork_new (s_env *src);

#endif /* LIBKC3_ENV_FORK_H */
