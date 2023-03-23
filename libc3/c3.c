/* c3
 * Copyright 2022,2023 kmx.io <contact@kmx.io>
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
#include <stdio.h>
#include <stdlib.h>
#include "c3.h"

void c3_init (s_env *env)
{
  if (! env)
    env = &g_c3_env;
  if (! env_init(env))
    exit(1);
}

void c3_clean (s_env *env)
{
  if (! env)
    env = &g_c3_env;
  env_clean(env);
  sym_delete_all();
}
