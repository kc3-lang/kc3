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
#include <assert.h>
#include "env.h"
#include "error_handler.h"
#include "frame.h"

s_env * env_init (s_env *env)
{
  assert(env);
  env->frame = NULL;
  env->error_handler = NULL;
  return env;
}

void env_clean (s_env *env)
{
  assert(env);
  frame_delete_all(env->frame);
  error_handler_delete_all(env->error_handler);
}
