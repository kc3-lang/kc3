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
#include <stdio.h>
#include <stdlib.h>
#include "c3.h"
#include "sym.h"

s_env g_c3_env = {0};

void c3_init (s_env *env)
{
  if (! env)
    env = &g_c3_env;
  env_init(env);
#ifdef DEBUG
  buf_init_alloc(&g_debug_buf, 1024);
  buf_file_open_w(&g_debug_buf, stderr);
#endif
}

void c3_clean (s_env *env)
{
  if (! env)
    env = &g_c3_env;
#ifdef DEBUG
  buf_file_close(&g_debug_buf);
  buf_clean(&g_debug_buf);
#endif
  sym_delete_all();
  env_clean(env);
}
