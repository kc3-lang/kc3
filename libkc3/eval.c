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
#include <stdlib.h>
#include "assert.h"
#include "env_eval.h"

bool eval_array (s_array *array, s_array *dest)
{
  s_env *env;
  env = env_global();
  return env_eval_array(env, array, dest);
}

bool eval_call (s_call *call, s_tag *dest)
{
  s_env *env;
  env = env_global();
  return env_eval_call(env, call, dest);
}

bool eval_callable_call (s_callable *callable, s_list *arguments,
                         s_tag *dest)
{
  s_env *env;
  env = env_global();
  assert(callable);
  assert(dest);
  if (! callable) {
    err_puts("eval_callable_call: NULL callable");
    assert(! "eval_callable_call: NULL callable");
    return false;
  }
  switch (callable->type) {
  case CALLABLE_CFN:
    return env_eval_call_cfn_args(env, &callable->data.cfn,
                                  arguments, dest);
  case CALLABLE_FN:
    return env_eval_call_fn_args(env, &callable->data.fn,
                                 arguments, dest);
  case CALLABLE_VOID:
    err_puts("eval_callable_call: CALLABLE_VOID");
    assert(! "eval_callable_call: CALLABLE_VOID");
    return false;
  }
  err_puts("eval_callable_call: unknown callable type");
  assert(! "eval_callable_call: unknown callable type");
  return false;
  
    
}

bool eval_fn_call (const s_fn *fn, s_list *args, s_tag *dest)
{
  s_env *env;
  env = env_global();
  return env_eval_call_fn_args(env, fn, args, dest);
}

bool eval_struct (s_struct *s, s_struct **dest)
{
  s_env *env;
  env = env_global();
  return env_eval_struct(env, s, dest);
}

bool eval_tag (s_tag *tag, s_tag *dest)
{
  s_env *env;
  env = env_global();
  return env_eval_tag(env, tag, dest);
}
