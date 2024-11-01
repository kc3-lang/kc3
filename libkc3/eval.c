/* kc3
 * Copyright 2022,2023,2024 kmx.io <contact@kmx.io>
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
#include <stdlib.h>
#include "types.h"
#include "env.h"


bool eval_call (const s_call *call, s_tag *dest)
{
  return env_eval_call(&g_kc3_env, call, dest);
}

bool eval_fn_call (const s_fn *fn, const s_list *args, s_tag *dest)
{
  return env_eval_call_fn_args(&g_kc3_env, fn, args, dest);
}

bool eval_tag (const s_tag *tag, s_tag *dest)
{
  return env_eval_tag(&g_kc3_env, tag, dest);
}
