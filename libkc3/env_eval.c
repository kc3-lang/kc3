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
#include "alloc.h"
#include "array.h"
#include "assert.h"
#include "block.h"
#include "call.h"
#include "callable.h"
#include "cfn.h"
#include "compare.h"
#include "complex.h"
#include "cow.h"
#include "data.h"
#include "do_block.h"
#include "env.h"
#include "env_eval.h"
#include "env_eval_equal.h"
#include "env_eval_quote.h"
#include "fn.h"
#include "facts.h"
#include "frame.h"
#include "ident.h"
#include "list.h"
#include "map.h"
#include "ops.h"
#include "pcallable.h"
#include "pstruct.h"
#include "pvar.h"
#include "securelevel.h"
#include "struct.h"
#include "sw.h"
#include "tag.h"
#include "time.h"
#include "tuple.h"
#include "var.h"

// TODO fixme unwind protect and ko: data_clean() in reverse order
bool env_eval_array (s_env *env, const s_array *array, s_array *dest)
{
  u8 *data;
  uw i;
  uw item_size;
  s_tag       *tag;
  s_tag        tag_eval = {0};
  s_array tmp = {0};
  assert(env);
  assert(array);
  assert(dest);
  if (securelevel(0) > 2) {
    err_puts("env_eval_array: cannot eval with securelevel > 2");
    abort();
  }
  array_init_copy(&tmp, array);
  if (tmp.dimension_count) {
    item_size = tmp.dimensions[tmp.dimension_count - 1].item_size;
    if (! tmp.data && tmp.tags) {
      tmp.free_data = alloc(tmp.dimensions[0].count *
                            tmp.dimensions[0].item_size);
      if (! tmp.free_data)
        return false;
      tmp.data = tmp.free_data;
      data = tmp.data;
      tag = tmp.tags;
      i = 0;
      while (i < tmp.count) {
        if (! env_eval_tag(env, tag, &tag_eval))
          goto ko;
        if (! data_init_cast(data, &tmp.element_type, &tag_eval)) {
          err_write_1("env_eval_array: cannot cast ");
          err_inspect_tag(&tag_eval);
          err_write_1(" to ");
          err_inspect_sym(tmp.element_type);
          err_puts(".");
          goto ko;
        }
        tag_clean(&tag_eval);
        data += item_size;
        tag++;
        i++;
      }
    }
  }
  *dest = tmp;
  return true;
 ko:
  array_clean(&tmp);
  return false;
}

bool env_eval_array_tag (s_env *env, const s_array *array, s_tag *dest)
{
  s_array tmp = {0};
  if (securelevel(0) > 2) {
    err_puts("env_eval_array_tag: cannot eval with securelevel > 2");
    abort();
  }
  if (! env_eval_array(env, array, &tmp))
    return false;
  dest->type = TAG_ARRAY;
  dest->data.td_array = tmp;
  return true;
}

bool env_eval_call (s_env *env, s_call *call, s_tag *dest)
{
  s_call c = {0};
  bool result;
  s_unwind_protect up = {0};
  assert(env);
  assert(call);
  assert(dest);
  if (securelevel(0) > 2) {
    err_puts("env_eval_call: cannot eval with securelevel > 2");
    abort();
  }
  call_init_copy(&c, call);
  if (c.pcallable)
    pcallable_clean(&c.pcallable);
  if (! env_eval_call_resolve(env, &c)) {
    err_stacktrace();
    err_write_1("env_eval_call: env_eval_call_resolve: ");
    err_inspect_ident(&c.ident);
    err_write_1("\n");
    result = false;
    goto clean;
  }
  if (! c.pcallable || c.pcallable->type == CALLABLE_VOID) {
    err_write_1("env_eval_call: could not resolve call ");
    err_inspect_ident(&c.ident);
    err_write_1("\n");
    result = false;
    goto clean;
  }
  env_unwind_protect_push(env, &up);
  if (setjmp(up.buf)) {
    env_unwind_protect_pop(env, &up);
    call_clean(&c);
    env->stacktrace_depth--;
    longjmp(*up.jmp, 1);
    abort();
  }
  if (env->stacktrace_depth > 256) {
    err_puts("env_eval_call: stacktrace depth > 256");
    err_stacktrace();
    assert(! "env_eval_call: stacktrace depth > 256");
    abort();
  }
  env->stacktrace_depth++;
  result = env_eval_call_callable(env, &c, dest);
  env->stacktrace_depth--;
  env_unwind_protect_pop(env, &up);
 clean:
  call_clean(&c);
  return result;
}

// FIXME: better error message (cite call function name if any)
bool env_eval_call_arguments (s_env *env, s_list *args,
                              s_list **dest)
{
  s_list * volatile a;
  s_list ** volatile tail;
  s_list *tmp = NULL;
  s_unwind_protect up = {0};
  if (securelevel(0) > 2) {
    err_puts("env_eval_call_arguments: cannot eval with"
             " securelevel > 2");
    abort();
  }
  tail = &tmp;
  a = args;
  while (a) {
    *tail = list_new(NULL);
    env_unwind_protect_push(env, &up);
    if (setjmp(up.buf)) {
      env_unwind_protect_pop(env, &up);
      list_delete_all(tmp);
      longjmp(*up.jmp, 1);
    }
    if (! env_eval_tag(env, &a->tag, &(*tail)->tag)) {
      err_write_1("env_eval_call_arguments: invalid argument: ");
      err_inspect(&args->tag);
      err_write_1("\n");
      env_unwind_protect_pop(env, &up);
      list_delete_all(tmp);
      return false;
    }
    env_unwind_protect_pop(env, &up);
    tail = &(*tail)->next.data.td_plist;
    a = list_next(a);
  }
  *dest = tmp;
  return true;
}

bool env_eval_call_callable (s_env *env, s_call *call,
                             s_tag *dest)
{
#if LIBKC3_PROFILE
  s_tag profile_dt = {0};
  s_tag profile_end = {0};
  s_tag profile_predicate = {0};
  s_tag profile_start = {0};
  s_tag profile_subject = {0};
#endif
  bool result;
#if LIBKC3_PROFILE
  if (g_profile_facts)
    tag_init_time_now(&profile_start);
#endif
  if (securelevel(0) > 2) {
    err_puts("env_eval_call_callable: cannot eval with"
             " securelevel > 2");
    abort();
  }
  switch (call->pcallable->type) {
  case CALLABLE_CFN:
    result = env_eval_call_cfn_args(env, &call->pcallable->data.cfn,
                                    call->arguments, dest);
    goto profile;
  case CALLABLE_FN:
    result = env_eval_call_fn_args(env, &call->pcallable->data.fn,
                                   call->arguments, dest);
    goto profile;
  case CALLABLE_VOID:
    err_puts("env_eval_call_callable: CALLABLE_VOID");
    assert(! "env_eval_call_callable: CALLABLE_VOID");
    result = false;
    goto profile;
  }
  err_puts("env_eval_call_callable: unknown callable type");
  assert(! "env_eval_call_callable: unknown callable type");
  result = false;
 profile:
#if LIBKC3_PROFILE
  if (g_profile_facts) {
    tag_init_time_copy(&profile_subject, &profile_start.data.td_time);
    tag_init_pcallable_copy(&profile_predicate, &call->pcallable);
    tag_init_time_now(&profile_end);
    profile_dt.type = TAG_TIME;
    time_sub(&profile_end.data.td_time, &profile_start.data.td_time,
             &profile_dt.data.td_time);
    facts_add_tags(g_profile_facts, &profile_subject,
                   &profile_predicate, &profile_dt);
    tag_clean(&profile_predicate);
  }
#endif
  return result;
}

bool env_eval_call_callable_args (s_env *env,
                                  s_callable *callable,
                                  s_list *arguments,
                                  s_tag *dest)
{
  if (securelevel(0) > 2) {
    err_puts("env_eval_call_callable_args: cannot eval with"
             " securelevel > 2");
    abort();
  }
  switch (callable->type) {
  case CALLABLE_CFN:
    return env_eval_call_cfn_args(env, &callable->data.cfn,
                                  arguments, dest);
  case CALLABLE_FN:
    return env_eval_call_fn_args(env, &callable->data.fn,
                                 arguments, dest);
  case CALLABLE_VOID:
    err_puts("env_eval_call_callable_args: CALLABLE_VOID");
    assert(! "env_eval_call_callable_args: CALLABLE_VOID");
    return false;
  }
  err_puts("env_eval_call_callable_args: unknown callable type");
  assert(! "env_eval_call_callable_args: unknown callable type");
  return false;
}

bool env_eval_call_cfn_args (s_env *env, s_cfn *cfn, s_list *arguments,
                             s_tag *dest)
{
  s_list *args = NULL;
  s_list * volatile args_volatile = NULL;
  s_tag tag;
  s_unwind_protect unwind_protect;
  assert(env);
  assert(cfn);
  assert(dest);
  if (securelevel(0) > 2) {
    err_puts("env_eval_call_cfn_args: cannot eval with"
             " securelevel > 2");
    abort();
  }
  if (arguments && ! (cfn->macro || cfn->special_operator)) {
    if (! env_eval_call_arguments(env, arguments, &args))
      return false;
  }
  args_volatile = args;
  env_unwind_protect_push(env, &unwind_protect);
  if (setjmp(unwind_protect.buf)) {
    env_unwind_protect_pop(env, &unwind_protect);
    list_delete_all(args_volatile);
    longjmp(*unwind_protect.jmp, 1);
    return false;
  }
  if (! cfn_apply(cfn, (cfn->macro || cfn->special_operator) ?
                  arguments : args_volatile, &tag)) {
    env_unwind_protect_pop(env, &unwind_protect);
    list_delete_all(args);
    return false;
  }
  env_unwind_protect_pop(env, &unwind_protect);
  *dest = tag;
  list_delete_all(args);
  return true;
}

bool env_eval_call_fn (s_env *env, const s_call *call, s_tag *dest)
{
  assert(env);
  assert(call);
  assert(dest);
  if (securelevel(0) > 2) {
    err_puts("env_eval_call_fn: cannot eval with securelevel > 2");
    abort();
  }
  if (! call->pcallable ||
      call->pcallable->type != CALLABLE_FN) {
    err_puts("env_eval_call_fn: not a Fn");
    assert(! "env_eval_call_fn: not a Fn");
    return false;
  }
  return env_eval_call_fn_args(env, &call->pcallable->data.fn,
                               call->arguments, dest);
}

bool env_eval_call_fn_args (s_env *env, const s_fn *fn,
                            s_list *arguments, s_tag *dest)
{
  s_list *args = NULL;
  s_list * volatile args_final = NULL;
  s_fn_clause *clause;
  s_tag * volatile dest_v = dest;
  s_frame *env_frame;
  s_frame frame = {0};
  const s_sym *module;
  s_list *search_modules;
  bool silence_errors;
  s_tag tag;
  s_list *tmp = NULL;
  s_list *trace;
  struct { /* XXX needed to sort unwind protect jumps
              XXX only works if stack grows down */
    s_unwind_protect unwind_macro;
    s_block          block;
    s_unwind_protect unwind_do;
    s_unwind_protect unwind_pattern;
    s_unwind_protect unwind_args;
  } jump = {0};
  assert(env);
  assert(fn);
  assert(dest);
  if (securelevel(0) > 2) {
    err_puts("env_eval_call_fn_args: cannot eval with"
             " securelevel > 2");
    abort();
  }
  search_modules = env->search_modules;
  module = fn->module;
  if (! module)
    module = env->current_defmodule;
  if (! module)
    module = &g_sym_KC3;
  if (! env_module_search_modules(env, &module, &env->search_modules))
    return false;
  env_frame = env->frame;
  clause = fn->clauses;
  silence_errors = env->silence_errors;
  if (arguments) {
    if (fn->macro || fn->special_operator)
      args_final = arguments;
    else {
      env_unwind_protect_push(env, &jump.unwind_args);
      if (setjmp(jump.unwind_args.buf)) {
        env_unwind_protect_pop(env, &jump.unwind_args);
        list_delete_all(env->search_modules);
        env->search_modules = search_modules;
        longjmp(*jump.unwind_args.jmp, 1);
      }
      if (! env_eval_call_arguments(env, arguments, &args)) {
        env_unwind_protect_pop(env, &jump.unwind_args);
        list_delete_all(env->search_modules);
        env->search_modules = search_modules;
        return false;
      }
      env_unwind_protect_pop(env, &jump.unwind_args);
      args_final = args;
    }
    while (clause) {
      if (fn->frame) {
        assert(! fn->frame->next);
        if (! frame_init_copy(&frame, fn->frame)) {
          list_delete_all(args);
          env->silence_errors = silence_errors;
          list_delete_all(env->search_modules);
          env->search_modules = search_modules;
          return false;
        }
        frame.next = env_frame;
      }
      else if (! frame_init(&frame, env_frame)) {
        list_delete_all(args);
        env->silence_errors = silence_errors;
        list_delete_all(env->search_modules);
        env->search_modules = search_modules;
        return false;
      }
      env->frame = &frame;
      env->silence_errors = true;
      env_unwind_protect_push(env, &jump.unwind_pattern);
      if (setjmp(jump.unwind_pattern.buf)) {
        env_unwind_protect_pop(env, &jump.unwind_pattern);
        env->silence_errors = silence_errors;
        assert(env->frame == &frame);
        env->frame = env_frame;
        frame_clean(&frame);
        list_delete_all(args);
        list_delete_all(env->search_modules);
        env->search_modules = search_modules;
        longjmp(*jump.unwind_pattern.jmp, 1);
      }
      if (env_eval_equal_list(env, fn->macro || fn->special_operator,
                              clause->pattern, args_final, &tmp)) {
        env_unwind_protect_pop(env, &jump.unwind_pattern);
        env->silence_errors = silence_errors;
        break;
      }
      env_unwind_protect_pop(env, &jump.unwind_pattern);
      env->silence_errors = silence_errors;
      assert(env->frame == &frame);
      env->frame = env_frame;
      frame_clean(&frame);
      clause = clause->next;
    }
    if (! clause) {
      err_write_1("env_eval_call_fn_args: ");
      err_inspect_ident(&fn->name);
      err_puts(": no clause matching, tried clauses :");
      clause = fn->clauses;
      while (clause) {
        err_inspect_fn_pattern(clause->pattern);
        err_write_1("\n");
        clause = clause->next;
      }
      err_puts("\nArguments :\n");
      err_inspect_fn_pattern(args);
      err_write_1("\n");
      err_puts("stacktrace:");
      err_inspect_stacktrace(env->stacktrace);
      err_write_1("\n");
      list_delete_all(args);
      list_delete_all(env->search_modules);
      env->search_modules = search_modules;
      return false;
    }
  }
  else {
    if (fn->frame) {
      if (! frame_init_copy(&frame, fn->frame)) {
        list_delete_all(args);
        list_delete_all(tmp);
        list_delete_all(env->search_modules);
        env->search_modules = search_modules;
        return false;
      }
      frame.next = env->frame;
    }
    else if (! frame_init(&frame, env_frame)) {
      list_delete_all(args);
      env->silence_errors = silence_errors;
      list_delete_all(env->search_modules);
      env->search_modules = search_modules;
      return false;
    }
    env->frame = &frame;
  }
  if (! (trace = list_new(env->stacktrace))) {
    list_delete_all(args);
    list_delete_all(tmp);
    list_delete_all(env->search_modules);
    env->search_modules = search_modules;
    assert(env->frame == &frame);
    env->frame = env_frame;
    frame_clean(&frame);
    return false;
  }
  {
    s_list *args_copy;
    s_list *trace_plist;
    args_copy = list_new_copy_all(args);
    trace_plist = list_new_ident(&fn->name, args_copy);
    if (! trace_plist) {
      list_delete_all(args_copy);
      list_delete(trace);
      list_delete_all(args);
      list_delete_all(tmp);
      list_delete_all(env->search_modules);
      env->search_modules = search_modules;
      assert(env->frame == &frame);
      env->frame = env_frame;
      frame_clean(&frame);
      return false;
    }
    tag_init_plist(&trace->tag, trace_plist);
  }
  env->stacktrace = trace;
  if (! block_init(&jump.block, fn->name.sym)) {
    env->stacktrace = list_delete(env->stacktrace);
    list_delete_all(args);
    list_delete_all(tmp);
    list_delete_all(env->search_modules);
    env->search_modules = search_modules;
    assert(env->frame == &frame);
    env->frame = env_frame;
    frame_clean(&frame);
    return false;
  }
  env_unwind_protect_push(env, &jump.unwind_do);
  if (setjmp(jump.unwind_do.buf)) {
    env_unwind_protect_pop(env, &jump.unwind_do);
    block_clean(&jump.block);
    assert(env->stacktrace == trace);
    env->stacktrace = list_delete(env->stacktrace);
    list_delete_all(args);
    list_delete_all(tmp);
    list_delete_all(env->search_modules);
    env->search_modules = search_modules;
    assert(env->frame == &frame);
    env->frame = env_frame;
    frame_clean(&frame);
    longjmp(*jump.unwind_do.jmp, 1);
  }
  if (setjmp(jump.block.buf)) {
    tag = jump.block.tag;
    env_unwind_protect_pop(env, &jump.unwind_do);
    assert(env->stacktrace == trace);
    env->stacktrace = list_delete(env->stacktrace);
    list_delete_all(args);
    list_delete_all(tmp);
    list_delete_all(env->search_modules);
    env->search_modules = search_modules;
    assert(env->frame == &frame);
    env->frame = env_frame;
    frame_clean(&frame);
    goto ok;
  }
  if (! env_eval_do_block(env, &clause->algo, &tag)) {
    env_unwind_protect_pop(env, &jump.unwind_do);
    block_clean(&jump.block);
    assert(env->stacktrace == trace);
    env->stacktrace = list_delete(env->stacktrace);
    list_delete_all(args);
    list_delete_all(tmp);
    list_delete_all(env->search_modules);
    env->search_modules = search_modules;
    assert(env->frame == &frame);
    env->frame = env_frame;
    frame_clean(&frame);
    return false;
  }
  env_unwind_protect_pop(env, &jump.unwind_do);
  assert(env->stacktrace == trace);
  env->stacktrace = list_delete(env->stacktrace);
  list_delete_all(args);
  list_delete_all(tmp);
  list_delete_all(env->search_modules);
  env->search_modules = search_modules;
  assert(env->frame == &frame);
  env->frame = env_frame;
  frame_clean(&frame);
 ok:
  block_clean(&jump.block);
  if (fn->macro) {
    env_unwind_protect_push(env, &jump.unwind_macro);
    if (setjmp(jump.unwind_macro.buf)) {
      env_unwind_protect_pop(env, &jump.unwind_macro);
      tag_clean(&tag);
      longjmp(*jump.unwind_macro.jmp, 1);
    }
    if (! env_eval_tag(env, &tag, dest_v)) {
      env_unwind_protect_pop(env, &jump.unwind_macro);
      tag_clean(&tag);
      return false;
    }
    env_unwind_protect_pop(env, &jump.unwind_macro);
    tag_clean(&tag);
  }
  else
    *dest_v = tag;
  return true;
}

bool env_eval_call_resolve (s_env *env, s_call *call)
{
  sw arity;
  bool b;
  s_env *global_env;
  s_op *op = NULL;
  s_tag op_tag = {0};
  s_ops *ops = NULL;
  s_call tmp = {0};
  s_tag *value;
  assert(env);
  assert(env->ops);
  assert(call);
  if (securelevel(0) > 2) {
    err_puts("env_eval_call_resolve: cannot eval with"
             " securelevel > 2");
    abort();
  }
  tmp = *call;
  if (tmp.ident.module == NULL &&
      (value = env_frames_get(env, tmp.ident.sym))) {
    if (value->type == TAG_PVAR && value->data.td_pvar->bound)
      value = &value->data.td_pvar->tag;
    if (value->type == TAG_PCALLABLE) {
      if (! pcallable_init_copy(&tmp.pcallable, &value->data.td_pcallable))
        return false;
      *call = tmp;
      return true;
    }
    err_puts("env_eval_call_resolve: not a Callable (Cfn or Fn)");
    err_stacktrace();
    assert(! "env_eval_call_resolve: not a Callable (Cfn or Fn)");
    return false;
  }
  arity = call_arity(&tmp);
  if (false) {
    err_write_1("env_eval_call_resolve: arity = ");
    err_inspect_sw(arity);
    err_write_1(" sym = ");
    err_inspect_sym(tmp.ident.sym);
    err_write_1("\n");
  }
  /* FIXME: multiple env and env->ops. See env_defoperator. */
  /* Quickfix is to use env_global() as is done in kc3_defoperator. */
  global_env = env_global();
  if (arity >= 1 && arity <= 2) {
    ops = global_env->ops;
    if (ops_get_tag(ops, tmp.ident.sym, arity, &op_tag)) {
      op = op_tag.data.td_pstruct->data;
      if (! pcallable_init_copy(&tmp.pcallable, &op->pcallable))
        return false;
      callable_set_special(tmp.pcallable, op->special);
      tag_clean(&op_tag);
      *call = tmp;
      return true;
    }
  }
  if (! env_ident_resolve_module(env, &tmp.ident, &tmp.ident)) {
    err_puts("env_eval_call_resolve: env_ident_resolve_module");
    assert(! "env_eval_call_resolve: env_ident_resolve_module");
    return false;
  }
  /* too slow, use require
  if (! env_module_ensure_loaded(env, tmp.ident.module)) {
    err_puts("env_eval_call_resolve: env_module_ensure_loaded");
    assert(! "env_eval_call_resolve: env_module_ensure_loaded");
    return false;
  }
  */
  if (! env_module_has_ident(env, tmp.ident.module, &tmp.ident,
                             &b)) {
    err_puts("env_eval_call_resolve: env_module_has_ident");
    assert(! "env_eval_call_resolve: env_module_has_ident");
    return false;
  }
  if (! b) {
    err_write_1("env_eval_call_resolve: env_module_has_ident(");
    err_inspect_sym(tmp.ident.module);
    err_write_1(", ");
    err_inspect_ident(&tmp.ident);
    err_puts(") -> false");
    assert(! "env_eval_call_resolve: env_module_has_ident -> false");
    return false;
  }
  if (! env_call_get(env, &tmp)) {
    err_write_1("env_eval_call_resolve: env_call_get(");
    err_inspect_call(&tmp);
    err_puts(")");
    assert(! "env_eval_call_resolve: env_call_get");
    return false;
  }
  *call = tmp;
  return true;
}

bool env_eval_callable (s_env *env, s_callable *callable,
                        s_tag *dest)
{
  s_callable *tmp = NULL;
  assert(env);
  assert(callable);
  assert(dest);
  if (securelevel(0) > 2) {
    err_puts("env_eval_callable: cannot eval with"
             " securelevel > 2");
    abort();
  }
  switch (callable->type) {
  case CALLABLE_CFN:
    if (callable->data.cfn.cif_ready) {
      if (! pcallable_init_copy(&tmp, &callable))
        return false;
      goto ok;
    }
    if (securelevel(0)) {
      err_puts("env_eval_callable: cannot eval Cfn with securelevel"
               " > 0");
      abort();
    }
    if (! (tmp = callable_new_copy(callable)))
      return false;
    if (! cfn_eval(&tmp->data.cfn)) {
      callable_delete(tmp);
      return false;
    }
    goto ok;
  case CALLABLE_FN:
    if (callable->data.fn.frame) {
      if (! pcallable_init_copy(&tmp, &callable))
        return false;
      goto ok;
    }
    if (securelevel(0) > 2) {
      err_puts("env_eval_callable: cannot eval Fn with securelevel"
               " > 2");
      abort();
    }
    if (! (tmp = callable_new_copy(callable)))
      return false;
    if (! tmp->data.fn.module)
      tmp->data.fn.module = env->current_defmodule;
    if (tmp->data.fn.frame)
      frame_delete(tmp->data.fn.frame);
    tmp->data.fn.frame = env_frame_new_capture(env, &tmp->data.fn);
    goto ok;
  case CALLABLE_VOID:
    err_puts("env_eval_callable: CALLABLE_VOID");
    assert(! "env_eval_callable: CALLABLE_VOID");
    return false;
  }
  goto ko;
 ok:
  dest->type = TAG_PCALLABLE;
  dest->data.td_pcallable = tmp;
  return true;
 ko:
  if (tmp)
    pcallable_clean(&tmp);
  return false;
}

bool env_eval_complex (s_env *env, s_complex *c, s_tag *dest)
{
  s_complex *tmp = NULL;
  assert(env);
  assert(c);
  assert(dest);
  if (securelevel(0) > 2) {
    err_puts("env_eval_complex: cannot eval with"
             " securelevel > 2");
    abort();
  }
  tmp = alloc(sizeof(s_complex));
  if (! tmp)
    return false;
  if (! env_eval_tag(env, &c->x, &tmp->x)) {
    alloc_free(tmp);
    return false;
  }
  if (! env_eval_tag(env, &c->y, &tmp->y)) {
    tag_clean(&tmp->x);
    alloc_free(tmp);
    return false;
  }
  dest->type = TAG_PCOMPLEX;
  dest->data.td_pcomplex = tmp;
  return true;
}

bool env_eval_do_block (s_env *env, const s_do_block *do_block,
                        s_tag *dest)
{
  uw i = 0;
  s_tag tmp = {0};
  assert(env);
  assert(do_block);
  assert(dest);
  if (securelevel(0) > 2) {
    err_puts("env_eval_do_block: cannot eval with securelevel > 2");
    abort();
  }
  if (! do_block->count) {
    tag_init_void(dest);
    return true;
  }
  // TODO unwind protect
  while (i < do_block->count - 1) {
    if (! env_eval_tag(env, do_block->tag + i, &tmp))
      return false;
    tag_clean(&tmp);
    i++;
  }
  return env_eval_tag(env, do_block->tag + i, dest);
}

bool env_eval_ident (s_env *env, const s_ident *ident, s_tag *dest)
{
  s_tag *tag;
  s_tag tmp = {0};
  s_ident tmp_ident;
  assert(env);
  assert(ident);
  assert(dest);
  if (securelevel(0) > 2) {
    err_puts("env_eval_ident: cannot eval with"
             " securelevel > 2");
    abort();
  }
  if ((tag = env_frames_get(env, ident->sym))) {
    tag_init_copy(dest, tag);
    return true;
  }
  if (env_ident_resolve_module(env, ident, &tmp_ident) &&
      ! (tag = env_ident_get(env, &tmp_ident, &tmp))) {
    if (true) {
      err_puts("env_eval_ident: stacktrace:");
      err_inspect_stacktrace(env->stacktrace);
      err_write_1("\n");
    }
    err_write_1("env_eval_ident: unbound ident: ");
    err_inspect_ident(ident);
    err_write_1("\n");
    if (false) {
      err_write_1("frame: ");
      err_inspect_frame(env->frame);
      err_write_1("\n");
    }
    assert(! "env_eval_ident: unbound ident");
    return false;
  }
  *dest = *tag;
  return true;
}

bool env_eval_ident_is_bound (s_env *env, const s_ident *ident)
{
  s_ident tmp_ident = {0};
  s_tag tmp = {0};
  assert(env);
  assert(ident);
  if (securelevel(0) > 2) {
    err_puts("env_eval_ident_is_bound: cannot eval with"
             " securelevel > 2");
    abort();
  }
  if (env_frames_get(env, ident->sym))
    return true;
  if (env_ident_resolve_module(env, ident, &tmp_ident) &&
      env_ident_get(env, &tmp_ident, &tmp)) {
    tag_clean(&tmp);
    return true;
  }
  return false;
}

// TODO: unwind_protect
bool env_eval_list (s_env *env, s_list *list, s_tag *dest)
{
  s_list *next;
  s_list *tmp = NULL;
  s_list **tail = &tmp;
  assert(env);
  assert(dest);
  if (securelevel(0) > 2) {
    err_puts("env_eval_list: cannot eval with securelevel > 2");
    abort();
  }
  while (list) {
    *tail = list_new(NULL);
    if (! env_eval_tag(env, &list->tag, &(*tail)->tag))
      goto ko;
    next = list_next(list);
    if (! next)
      if (! env_eval_tag(env, &list->next, &(*tail)->next))
        goto ko;
    tail = &(*tail)->next.data.td_plist;
    list = next;
  }
  dest->type = TAG_PLIST;
  dest->data.td_plist = tmp;
  return true;
 ko:
  list_delete_all(tmp);
  return false;
}

// TODO: unwind_protect
bool env_eval_map (s_env *env, s_map *map, s_tag *dest)
{
  s_map tmp = {0};
  uw i = 0;
  assert(env);
  assert(map);
  assert(dest);
  if (securelevel(0) > 2) {
    err_puts("env_eval_map: cannot eval with securelevel > 2");
    abort();
  }
  if (! map_init(&tmp, map->count))
    return false;
  while (i < tmp.count) {
    if (! env_eval_tag(env, map->key + i, tmp.key + i) ||
        ! env_eval_tag(env, map->value + i, tmp.value + i))
      goto ko;
    i++;
  }
  map_sort(&tmp);
  dest->type = TAG_MAP;
  dest->data.td_map = tmp;
  return true;
 ko:
  map_clean(&tmp);
  return false;
}

// TODO unwind_protect
bool env_eval_pcow (s_env *env, p_cow *pcow, p_cow *dest)
{
  p_cow cow = *pcow;
  p_cow tmp = NULL;
  assert(env);
  assert(pcow);
  assert(cow);
  assert(dest);
  if (securelevel(0) > 2) {
    err_puts("env_eval_pcow: cannot eval with securelevel > 2");
    abort();
  }
  tmp = cow_new(cow->type);
  if (! tmp)
    return false;
  if (! env_eval_tag(env, cow_read_only(cow),
                     cow_read_write(tmp))) {
    alloc_free(tmp);
    return false;
  }
  cow_freeze(tmp);
  *dest = tmp;
  return true;
}

bool env_eval_pcow_tag (s_env *env, p_cow *cow, s_tag *dest)
{
  s_tag tmp = {0};
  assert(env);
  assert(cow);
  assert(dest);
  if (securelevel(0) > 2) {
    err_puts("env_eval_pcow_tag: cannot eval with securelevel > 2");
    abort();
  }
  if (! env_eval_pcow(env, cow, &tmp.data.td_pcow))
    return false;
  tmp.type = TAG_PCOW;
  *dest = tmp;
  return true;
}

// TODO: unwind_protect
bool env_eval_struct (s_env *env, s_struct *s, p_struct *dest)
{
  void *data = NULL;
  uw i;
  s_tag *key = NULL;
  s_tag tag = {0};
  p_struct tmp = NULL;
  void    *tmp_data = NULL;
  const s_sym *type;
  assert(env);
  assert(s);
  assert(dest);
  if (securelevel(0) > 2) {
    err_puts("env_eval_struct: cannot eval with securelevel > 2");
    abort();
  }
  if (s->data) {
    if (! pstruct_init_copy(&tmp, &s))
      return false;
    *dest = tmp;
    return true;
  }
  if (! pstruct_init_with_type(&tmp, s->pstruct_type))
    return false;
  if (! struct_allocate(tmp))
    return false;
  i = 0;
  while (i < tmp->pstruct_type->map.count) {
    key = tmp->pstruct_type->map.key + i;
    if (key->type != TAG_PSYM) {
      err_puts("env_eval_struct: struct type key is not a Sym");
      assert(! "env_eval_struct: struct type key is not a Sym");
      goto ko;
    }
    if (key->data.td_psym->str.ptr.p_pchar[0] != '_') {
      if (tmp->pstruct_type->map.value[i].type == TAG_PVAR) {
        type = tmp->pstruct_type->map.value[i].data.td_pvar->type;
      }
      else if (! tag_type(tmp->pstruct_type->map.value + i, &type))
        goto ko;
      if (s->tag) {
        if (! env_eval_tag(env, s->tag + i, &tag))
          goto ko;
        if (! tag_to_pointer(&tag, type, &data)) {
          tag_clean(&tag);
          goto ko;
        }
      }
      else {
        if (! tag_to_pointer(tmp->pstruct_type->map.value + i,
                             type, &data))
          goto ko;
      }
      if (false) {
        err_write_1("env_eval_struct: type = ");
        err_inspect_sym(type);
        err_write_1("\n");
      }
      tmp_data = (s8 *) tmp->data + tmp->pstruct_type->offset[i];
      if (! data_init_copy(type, tmp_data, data))
        goto ko_init;
      if (s->tag)
        tag_clean(&tag);
    }
    i++;
  }
  *dest = tmp;
  return true;
 ko_init:
  err_write_1("env_eval_struct: invalid type ");
  err_write_1(tag_type_to_string(tag.type));
  err_write_1(" for key ");
  err_write_1(tmp->pstruct_type->map.key[i].data.td_psym->str.ptr.p_pchar);
  err_write_1(", expected ");
  err_puts(tag_type_to_string(tmp->pstruct_type->map.value[i].type));
  tag_clean(&tag);
 ko:
  pstruct_clean(&tmp);
  return false;
}

bool env_eval_struct_tag (s_env *env, s_struct *s, s_tag *dest)
{
  assert(env);
  assert(s);
  assert(dest);
  if (securelevel(0) > 2) {
    err_puts("env_eval_struct_tag: cannot eval with securelevel > 2");
    abort();
  }
  if (! env_eval_struct(env, s, &dest->data.td_pstruct))
    return false;
  dest->type = TAG_PSTRUCT;
  return true;
}

bool env_eval_tag (s_env *env, s_tag *tag, s_tag *dest)
{
  assert(env);
  assert(tag);
  assert(dest);
  if (securelevel(0) > 2) {
    err_puts("env_eval_tag: cannot eval with securelevel > 2");
    abort();
  }
  switch (tag->type) {
  case TAG_VOID:
    tag_init_void(dest);
    return true;
  case TAG_ARRAY:
    return env_eval_array_tag(env, &tag->data.td_array, dest);
  case TAG_DO_BLOCK:
    return env_eval_do_block(env, &tag->data.td_do_block, dest);
  case TAG_IDENT:
    return env_eval_ident(env, &tag->data.td_ident, dest);
  case TAG_MAP:
    return env_eval_map(env, &tag->data.td_map, dest);
  case TAG_PCALL:
    return env_eval_call(env, tag->data.td_pcall, dest);
  case TAG_PCALLABLE:
    return env_eval_callable(env, tag->data.td_pcallable, dest);
  case TAG_PCOMPLEX:
    return env_eval_complex(env, tag->data.td_pcomplex, dest);
  case TAG_PCOW:
    return env_eval_pcow_tag(env, &tag->data.td_pcow, dest);
  case TAG_PLIST:
    return env_eval_list(env, tag->data.td_plist, dest);
  case TAG_PSTRUCT:
    return env_eval_struct_tag(env, tag->data.td_pstruct, dest);
  case TAG_PVAR:
    return env_eval_var(env, tag->data.td_pvar, dest);
  case TAG_QUOTE:
    return env_eval_quote(env, &tag->data.td_quote, dest);
  case TAG_TIME:
    return env_eval_time(env, &tag->data.td_time, dest);
  case TAG_PTUPLE:
    return env_eval_tuple(env, tag->data.td_ptuple, dest);
  case TAG_BOOL:
  case TAG_CHARACTER:
  case TAG_F32:
  case TAG_F64:
#if HAVE_F80
  case TAG_F80:
#endif
#if HAVE_F128
  case TAG_F128:
#endif
  case TAG_FACT:
  case TAG_INTEGER:
  case TAG_PFACTS:
  case TAG_POINTER:
  case TAG_PSTRUCT_TYPE:
  case TAG_PSYM:
  case TAG_PTR:
  case TAG_PTR_FREE:
  case TAG_RATIO:
  case TAG_S8:
  case TAG_S16:
  case TAG_S32:
  case TAG_S64:
  case TAG_STR:
  case TAG_SW:
  case TAG_U8:
  case TAG_U16:
  case TAG_U32:
  case TAG_U64:
  case TAG_UNQUOTE:
  case TAG_UW:
    if (! tag_init_copy(dest, tag))
      return false;
    return true;
  }
  err_puts("env_eval_tag: unknown tag type");
  assert(! "env_eval_tag: unknown tag type");
  return false;
}

bool env_eval_time (s_env *env, const s_time *time, s_tag *dest)
{
  const s_sym *sym_Sw = &g_sym_Sw;
  s_tag tag[2] = {0};
  s_tag tmp = {0};
  if (securelevel(0) > 2) {
    err_puts("env_eval_time: cannot eval with securelevel > 2");
    abort();
  }
  tmp.type = TAG_TIME;
  if (time->tag) {
    if (! env_eval_tag(env, time->tag, tag))
      return false;
    if (! env_eval_tag(env, time->tag + 1, tag + 1)) {
      tag_clean(tag);
      return false;
    }
    if (! sw_init_cast(&tmp.data.td_time.tv_sec, &sym_Sw, tag)) {
      err_write_1("env_eval_time: tv_sec is not a Sw: ");
      err_inspect_tag(tag);
      err_write_1("\n");
      assert(! "env_eval_time: tv_sec is not a Sw");
      tag_clean(tag + 1);
      tag_clean(tag);
      return false;
    }
    if (! sw_init_cast(&tmp.data.td_time.tv_nsec, &sym_Sw, tag + 1)) {
      err_write_1("env_eval_time: tv_nsec is not a Sw: ");
      err_inspect_tag(tag + 1);
      err_write_1("\n");
      assert(! "env_eval_time: tv_nsec is not a Sw");
      tag_clean(tag + 1);
      tag_clean(tag);
      return false;
    }
  }
  else {
    tmp.data.td_time.tv_sec = time->tv_sec;
    tmp.data.td_time.tv_nsec = time->tv_nsec;
  }
  *dest = tmp;
  return true;
}

// TODO: unwind_protect
bool env_eval_tuple (s_env *env, const s_tuple *tuple, s_tag *dest)
{
  uw i = 0;
  p_tuple tmp;
  assert(env);
  assert(tuple);
  assert(dest);
  if (securelevel(0) > 2) {
    err_puts("env_eval_tuple: cannot eval with securelevel > 2");
    abort();
  }
  tmp = tuple_new(tuple->count);
  if (! tmp)
    return false;
  while (i < tuple->count) {
    if (! env_eval_tag(env, tuple->tag + i, tmp->tag + i)) {
      tuple_delete(tmp);
      return false;
    }
    i++;
  }
  dest->type = TAG_PTUPLE;
  dest->data.td_ptuple = tmp;
  return true;
}

bool env_eval_var (s_env *env, s_var *var, s_tag *dest)
{
  s_tag tmp = {0};
  assert(env);
  assert(dest);
  (void) env;
  if (securelevel(0) > 2) {
    err_puts("env_eval_var: cannot eval with securelevel > 2");
    abort();
  }
  if (var && var->bound) {
    if (! tag_init_copy(dest, &var->tag))
      return false;
    return true;
  }
  tmp.type = TAG_PVAR;
  tmp.data.td_pvar = var_new_copy(var);
  *dest = tmp;
  return true;
}
