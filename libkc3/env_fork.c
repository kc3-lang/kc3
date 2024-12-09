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
#include "alloc.h"
#include "assert.h"
#include "env.h"
#include "env_fork.h"
#include "frame.h"
#include "sym.h"

void env_fork_clean (s_env *env)
{
  assert(env);
  //env->stacktrace
  //env->error_handler
  frame_delete_all(env->frame);
  frame_delete_all(env->read_time_frame);
  //tmp.unwind_protect = NULL;
}

s_env * env_fork_init (s_env *env, s_env *src)
{
  s_env tmp = {0};
  assert(env);
  assert(src);
  tmp.argc = src->argc;
  tmp.argv = src->argv;
  tmp.argv0_dir = src->argv0_dir;
  //tmp.stacktrace = NULL;
  tmp.current_defmodule = &g_sym_KC3;
  tmp.err = src->err;
  //tmp.error_handler = NULL;
  tmp.facts = src->facts;
  tmp.frame = frame_new_copy(src->frame);
  tmp.global_frame = src->global_frame;
  tmp.in = src->in;
  tmp.module_path = src->module_path;
  tmp.out = src->out;
  tmp.path = src->path;
  tmp.quote_level = src->quote_level;
  if (! (tmp.read_time_frame = frame_new(NULL, NULL)))
    return NULL;
  tmp.search_modules = src->search_modules_default;
  tmp.search_modules_default = src->search_modules_default;
  tmp.trace = src->trace;
  //tmp.unquote_level = 0;
  //tmp.unwind_protect = NULL;
  *env = tmp;
  return env;
}

s_env * env_fork_new (s_env *src)
{
  s_env *env;
  if (! (env = alloc(sizeof(s_env))))
    return NULL;
  if (! env_fork_init(env, src)) {
    free(env);
    return NULL;
  }
  return env;
}
