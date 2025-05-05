/* kc3
 * Copyright from 2022 to 2025 kmx.io <contact@kmx.io>
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
#include "fn.h"
#include "frame.h"
#include "ident.h"
#include "list.h"
#include "map.h"
#include "ops.h"
#include "pcallable.h"
#include "pstruct.h"
#include "struct.h"
#include "sw.h"
#include "tag.h"
#include "time.h"
#include "tuple.h"
#include "var.h"

bool env_eval_array (s_env *env, const s_array *array, s_array *dest)
{
  u8 *data;
  uw i;
  uw item_size;
  s_tag       *tag;
  s_tag        tag_eval;
  s_array tmp = {0};
  assert(env);
  assert(array);
  assert(dest);
  array_init_copy(&tmp, array);
  if (tmp.dimension) {
    item_size = tmp.dimensions[tmp.dimension - 1].item_size;
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
          err_inspect_sym(&tmp.element_type);
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
  if (! env_eval_array(env, array, &tmp))
    return false;
  dest->type = TAG_ARRAY;
  dest->data.array = tmp;
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
  if (! do_block->count) {
    tag_init_void(dest);
    return true;
  }
  while (i < do_block->count - 1) {
    if (env_eval_tag(env, do_block->tag + i, &tmp))
      tag_clean(&tmp);
    i++;
  }
  return env_eval_tag(env, do_block->tag + i, dest);
}

bool env_eval_call (s_env *env, s_call *call, s_tag *dest)
{
  s_call c = {0};
  bool result;
  assert(env);
  assert(call);
  assert(dest);
  (void) call;
  call_init_copy(&c, call);
  if (! env_eval_call_resolve(env, &c)) {
    err_write_1("env_eval_call: env_eval_call_resolve: ");
    err_inspect_ident(&c.ident);
    err_write_1("\n");
    return false;
  }
  if (! c.pcallable || c.pcallable->type == CALLABLE_VOID) {
    err_write_1("env_eval_call: could not resolve call ");
    err_inspect_ident(&c.ident);
    err_write_1("\n");
    result = false;
  }
  result = env_eval_call_callable(env, &c, dest);
  call_clean(&c);
  return result;
}

// FIXME: better error message (cite call function name if any)
bool env_eval_call_arguments (s_env *env, s_list *args,
                              s_list **dest)
{
  s_list **tail;
  s_list *tmp = NULL;
  tail = &tmp;
  while (args) {
    *tail = list_new(NULL);
    if (! env_eval_tag(env, &args->tag, &(*tail)->tag)) {
      list_delete_all(tmp);
      err_write_1("env_eval_call_arguments: invalid argument: ");
      err_inspect(&args->tag);
      err_write_1("\n");
      return false;
    }
    tail = &(*tail)->next.data.list;
    args = list_next(args);
  }
  *dest = tmp;
  return true;
}

bool env_eval_call_callable (s_env *env, const s_call *call,
                             s_tag *dest)
{
  switch (call->pcallable->type) {
  case CALLABLE_CFN:
    return env_eval_call_cfn_args(env, &call->pcallable->data.cfn,
                                  call->arguments, dest);
  case CALLABLE_FN:
    return env_eval_call_fn_args(env, &call->pcallable->data.fn,
                                 call->arguments, dest);
  case CALLABLE_VOID:
    err_puts("env_eval_call_callable: CALLABLE_VOID");
    assert(! "env_eval_call_callable: CALLABLE_VOID");
    return false;
  }
  err_puts("env_eval_call_callable: unknown callable type");
  assert(! "env_eval_call_callable: unknown callable type");
  return false;
}

bool env_eval_call_callable_args (s_env *env,
                                  s_callable *callable,
                                  s_list *arguments,
                                  s_tag *dest)
{
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
  s_list *args_final = NULL;
  //s_frame frame;
  s_tag tag;
  assert(env);
  assert(cfn);
  assert(dest);
  //if (! frame_init(&frame, env->frame))
  //  return false;
  //env->frame = &frame;
  if (arguments) {
    if (cfn->macro || cfn->special_operator)
      args_final = arguments;
    else {
      if (! env_eval_call_arguments(env, arguments, &args)) {
        //env->frame = frame_clean(&frame);
        return false;
      }
      args_final = args;
    }
  }
  if (! cfn_apply(cfn, args_final, &tag)) {
    list_delete_all(args);
    //env->frame = frame_clean(&frame);
    return false;
  }
  *dest = tag;
  list_delete_all(args);
  //env->frame = frame_clean(&frame);
  return true;
}

bool env_eval_call_fn (s_env *env, const s_call *call, s_tag *dest)
{
  assert(env);
  assert(call);
  assert(dest);
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
  s_list *args_final = NULL;
  s_block block = {0};
  s_fn_clause *clause;
  s_frame *env_frame;
  s_frame frame;
  const s_sym *module;
  s_list *search_modules;
  bool silence_errors;
  s_tag tag;
  s_list *tmp = NULL;
  s_list *trace;
  assert(env);
  assert(fn);
  assert(dest);
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
      if (! env_eval_call_arguments(env, arguments, &args)) {
        list_delete_all(env->search_modules);
        env->search_modules = search_modules;
        return false;
      }
      args_final = args;
    }
    while (clause) {
      if (! frame_init(&frame, env->frame, fn->frame)) {
        list_delete_all(env->search_modules);
        env->silence_errors = silence_errors;
        env->search_modules = search_modules;
        return false;
      }
      env->frame = &frame;
      env->silence_errors = true;
      if (env_eval_equal_list(env, fn->macro || fn->special_operator,
                              clause->pattern, args_final, &tmp)) {
        env->silence_errors = silence_errors;
        break;
      }
      env->silence_errors = silence_errors;
      env->frame = env_frame;
      frame_clean(&frame);
      clause = clause->next_clause;
    }
    if (! clause) {
      err_write_1("env_eval_call_fn_args: ");
      err_inspect_ident(&fn->ident);
      err_puts(": no clause matching, tried clauses :");
      clause = fn->clauses;
      while (clause) {
        err_inspect_fn_pattern(clause->pattern);
        err_write_1("\n");
        clause = clause->next_clause;
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
    frame_init(&frame, env->frame, fn->frame);
    env->frame = &frame;
  }
  if (! (trace = list_new(env->stacktrace))) {
    list_delete_all(args);
    list_delete_all(tmp);
    list_delete_all(env->search_modules);
    env->search_modules = search_modules;
    env->frame = env_frame;
    frame_clean(&frame);
    return false;
  }
  tag_init_list(&trace->tag, list_new_ident
                (&fn->ident, list_new_copy
                 (args)));
  env->stacktrace = trace;
  if (! block_init(&block, fn->ident.sym)) {
    env->stacktrace = list_delete(env->stacktrace);
    list_delete_all(args);
    list_delete_all(tmp);
    list_delete_all(env->search_modules);
    env->search_modules = search_modules;
    env->frame = env_frame;
    frame_clean(&frame);
    return false;
  }
  if (setjmp(block.buf)) {
    tag_init_copy(&tag, &block.tag);
    goto ok;
  }
  if (! env_eval_do_block(env, &clause->algo, &tag)) {
    block_clean(&block);
    env->stacktrace = list_delete(env->stacktrace);
    list_delete_all(args);
    list_delete_all(tmp);
    list_delete_all(env->search_modules);
    env->search_modules = search_modules;
    env->frame = env_frame;
    frame_clean(&frame);
    return false;
  }
 ok:
  block_clean(&block);
  assert(env->stacktrace == trace);
  env->stacktrace = list_delete(env->stacktrace);
  list_delete_all(args);
  list_delete_all(tmp);
  list_delete_all(env->search_modules);
  env->search_modules = search_modules;
  env->frame = env_frame;
  frame_clean(&frame);
  if (fn->macro) {
    if (! env_eval_tag(env, &tag, dest)) {
      tag_clean(&tag);
      return false;
    }
    tag_clean(&tag);
  }
  else
    *dest = tag;
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
  tmp = *call;
  if (tmp.ident.module == NULL &&
      (value = env_frames_get(env, tmp.ident.sym))) {
    if (value->type == TAG_PCALLABLE) {
      if (! pcallable_init_copy(&tmp.pcallable, &value->data.pcallable))
        return false;
      *call = tmp;
      return true;
    }
  }
  arity = call_arity(&tmp);
  if (false) {
    err_write_1("env_eval_call_resolve: arity = ");
    err_inspect_sw(&arity);
    err_write_1(" sym = ");
    err_inspect_sym(&tmp.ident.sym);
    err_write_1("\n");
  }
  /* FIXME: multiple env and env->ops. See env_defoperator. */
  /* Quickfix is to use env_global() as is done in kc3_defoperator. */
  global_env = env_global();
  if (arity >= 1 && arity <= 2) {
    ops = global_env->ops;
    if (ops_get(ops, tmp.ident.sym, arity, &op_tag)) {
      op = op_tag.data.pstruct->data;
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
  if (! env_module_ensure_loaded(env, tmp.ident.module)) {
    err_puts("env_eval_call_resolve: env_module_ensure_loaded");
    assert(! "env_eval_call_resolve: env_module_ensure_loaded");
    return false;
  }
  if (! env_module_has_ident(env, tmp.ident.module, &tmp.ident,
                             &b)) {
    err_puts("env_eval_call_resolve: env_module_has_ident");
    assert(! "env_eval_call_resolve: env_module_has_ident");
    return false;
  }
  if (! b) {
    err_write_1("env_eval_call_resolve: env_module_has_ident(");
    err_inspect_sym(&tmp.ident.module);
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
  s_callable c = {0};
  s_callable *tmp = NULL;
  assert(env);
  assert(callable);
  assert(dest);
  (void) env;
  switch (callable->type) {
  case CALLABLE_CFN:
    if (! pcallable_init_copy(&tmp, &callable))
      return false;
    if (! cfn_eval(&tmp->data.cfn))
      return false;
    goto ok;
  case CALLABLE_FN:
    c = *callable;
    c.data.fn.frame = env->frame;
    if (! (tmp = callable_new_copy(&c)))
      return false;
    if (! tmp->data.fn.module)
      tmp->data.fn.module = env->current_defmodule;
    goto ok;
  case CALLABLE_VOID:
    err_puts("env_eval_callable: CALLABLE_VOID");
    assert(! "env_eval_callable: CALLABLE_VOID");
    return false;
  }
  goto ko;
 ok:
  dest->type = TAG_PCALLABLE;
  dest->data.pcallable = tmp;
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
  tmp = alloc(sizeof(s_complex));
  if (! tmp)
    return false;
  if (! env_eval_tag(env, &c->x, &tmp->x)) {
    free(tmp);
    return false;
  }
  if (! env_eval_tag(env, &c->y, &tmp->y)) {
    tag_clean(&tmp->x);
    free(tmp);
    return false;
  }
  dest->type = TAG_COMPLEX;
  dest->data.complex = tmp;
  return true;
}

bool env_eval_cow (s_env *env, s_cow *cow, s_tag *dest)
{
  s_cow *tmp = NULL;
  assert(env);
  assert(cow);
  assert(dest);
  tmp = cow_new(cow->type);
  if (! tmp)
    return false;
  if (! env_eval_tag(env, cow_read_only(cow),
                     cow_read_write(tmp))) {
    free(tmp);
    return false;
  }
  cow_freeze(tmp);
  dest->type = TAG_COW;
  dest->data.cow = tmp;
  return true;
}

bool env_eval_equal_cow (s_env *env, s_cow *a,
                         s_cow *b, s_cow **dest)
{
  s8 r;
  s_cow *tmp = {0};
  assert(env);
  assert(a);
  assert(b);
  assert(dest);
  (void) env;
  if ((r = compare_sym(a->type, b->type))) {
    err_puts("env_eval_equal_cow: type mismatch");
    assert(! "env_eval_equal_cow: type mismatch");
    return false;
  }
  if ((r = compare_tag(cow_read_only(a), cow_read_only(b)))) {
    err_puts("env_eval_equal_cow: value mismatch");
    assert(! "env_eval_equal_cow: value mismatch");
    return false;
  }
  tmp = cow_new_tag_copy(a->type, cow_read_only(a));
  if (! tmp)
    return false;
  cow_freeze(tmp);
  *dest = tmp;
  return true;
}

bool env_eval_equal_list (s_env *env, bool macro, s_list *a,
                          s_list *b, s_list **dest)
{
  s_list *a_next;
  s_list *b_next;
  s_list *tmp = NULL;
  s_list **t;
  assert(env);
  assert(dest);
  t = &tmp;
  while (1) {
    if (! a && ! b) {
      *t = NULL;
      goto ok;
    }
    if (! a)
      goto ko;
    if (! b)
      goto ko;
    *t = list_new(NULL);
    if (! env_eval_equal_tag(env, macro, &a->tag, &b->tag,
                             &(*t)->tag))
      goto ko;
    a_next = list_next(a);
    b_next = list_next(b);
    if (! a_next || ! b_next) {
      if (! env_eval_equal_tag(env, macro, &a->next, &b->next,
                               &(*t)->next))
        goto ko;
      goto ok;
    }
    a = a_next;
    b = b_next;
    if (dest)
      t = &(*t)->next.data.list;
  }
 ok:
  *dest = tmp;
  return true;
 ko:
  list_delete_all(tmp);
  return false;
}

bool env_eval_equal_map (s_env *env, bool macro, const s_map *a,
                         const s_map *b, s_map *dest)
{
  const s_map *c;
  uw i;
  uw j;
  s_tag tmp = {0};
  assert(env);
  assert(a);
  assert(b);
  assert(dest);
  if (! a->count) {
    map_init_copy(dest, b);
    return true;
  }
  if (! b->count) {
    map_init_copy(dest, a);
    return true;
  }
  if (a->count > b->count) {
    c = a;
    a = b;
    b = c;
  }
  i = 0;
  while (i < a->count) {
    j = 0;
    while (j < b->count) {
      if (! compare_tag(a->key + i, b->key + j)) {
        if (! env_eval_equal_tag(env, macro, a->value + i, b->value + j,
                                 &tmp)) {
          return false;
        }
        tag_clean(&tmp);
        goto next;
      }
      j++;
    }
    return false;
  next:
    i++;
  }
  map_init_copy(dest, b);
  return true;
}

bool env_eval_equal_tag (s_env *env, bool macro, s_tag *a,
                         s_tag *b, s_tag *dest)
{
  bool is_unbound_a;
  bool is_unbound_b;
  bool is_var_a = false;
  bool is_var_b = false;
  s_tag tmp_a;
  s_tag tmp_b;
  s_tag *var_a;
  s_tag *var_b;
  assert(env);
  assert(a);
  assert(b);
  assert(dest);
  tag_init_void(&tmp_a);
  tag_init_void(&tmp_b);
  if (a->type == TAG_VAR) {
    if (! a->data.var.ptr) {
      if (! (var_a = frame_binding_new_var(env->frame)))
        return false;
      a = var_a;
    }
    if (! tag_is_unbound_var(a, &is_var_a))
      return false;
  }
  if (b->type == TAG_VAR) {
    if (! b->data.var.ptr) {
      if (! (var_b = frame_binding_new_var(env->frame)))
        return false;
      b = var_b;
    }
    if (! tag_is_unbound_var(b, &is_var_b))
      return false;
  }
  is_unbound_a = a->type == TAG_IDENT;
  is_unbound_b = ! macro && (b->type == TAG_IDENT);
  if (is_unbound_a && is_unbound_b) {
    err_write_1("env_eval_equal_tag: unbound equal on both sides: ");
    err_inspect_ident(&a->data.ident);
    err_write_1(" = ");
    err_inspect_ident(&b->data.ident);
    err_write_1(".\nTo match an existing variable please use the"
                " pin operator, e.g.: ");
    err_inspect_ident(&a->data.ident);
    err_write_1(" = ^ ");
    err_inspect_ident(&b->data.ident);
    err_write_1("\n");
    return false;
  }
  if (is_unbound_a || is_var_a) {
    if (macro)
      tag_init_copy(dest, b);
    else
      env_eval_tag(env, b, dest);
    if (is_var_a) {
      if (! var_set(&a->data.var, dest))
        return false;
    }
    else {
      if (! frame_replace(env->frame, a->data.ident.sym, dest))
        return false;
    }
    return true;
  }
  if (is_unbound_b || is_var_b) {
    if (macro)
      tag_init_copy(dest, a);
    else
      env_eval_tag(env, a, dest);
    if (is_var_b) {
      if (! var_set(&b->data.var, dest))
        return false;
    }
    else {
      if (! frame_replace(env->frame, b->data.ident.sym, dest))
        return false;
    }
    return true;
  }
  if (! macro &&
      a->type == TAG_CALL) {
    if (! env_eval_tag(env, a, &tmp_a))
      return false;
    if (! env_eval_equal_tag(env, macro, &tmp_a, b, dest)) {
      tag_clean(&tmp_a);
      return false;
    }
    tag_clean(&tmp_a);
    return true;
  }
  if (! macro &&
      b->type == TAG_CALL) {
    if (! env_eval_tag(env, b, &tmp_b))
      return false;
    if (! env_eval_equal_tag(env, macro, a, &tmp_b, dest)) {
      tag_clean(&tmp_b);
      return false;
    }
    tag_clean(&tmp_b);
    return true;
  }
  a = tag_resolve_cow(a);
  b = tag_resolve_cow(b);
  switch (a->type) {
  case TAG_COMPLEX:
  case TAG_F32:
  case TAG_F64:
  case TAG_F128:
  case TAG_INTEGER:
  case TAG_RATIO:
  case TAG_S8:
  case TAG_S16:
  case TAG_S32:
  case TAG_S64:
  case TAG_SW:
  case TAG_U8:
  case TAG_U16:
  case TAG_U32:
  case TAG_U64:
  case TAG_UW:
    switch (b->type) {
    case TAG_COMPLEX:
    case TAG_F32:
    case TAG_F64:
    case TAG_F128:
    case TAG_INTEGER:
    case TAG_RATIO:
    case TAG_S8:
    case TAG_S16:
    case TAG_S32:
    case TAG_S64:
    case TAG_SW:
    case TAG_U8:
    case TAG_U16:
    case TAG_U32:
    case TAG_U64:
    case TAG_UW:
      if (compare_tag(a, b)) {
        return false;
      }
      tag_init_copy(dest, a);
      return true;
    default:
      break;
    }
  default:
    break;
  }
  if (a->type != b->type) {
    if (! env->silence_errors) {
      err_write_1("env_eval_equal_tag: type mismatch: ");
      err_inspect_tag(a);
      err_write_1(" != ");
      err_inspect_tag(b);
      err_write_1("\n");
      err_stacktrace();
    }
    return false;
  }
  switch (a->type) {
  case TAG_VOID:
    tag_init_void(dest);
    return true;
  case TAG_LIST:
    tag_init_list(dest, NULL);
    return env_eval_equal_list(env, macro, a->data.list, b->data.list,
                               &dest->data.list);
  case TAG_MAP:
    dest->type = TAG_MAP;
    return env_eval_equal_map(env, macro, &a->data.map, &b->data.map,
                              &dest->data.map);
  /*
  case TAG_PSTRUCT:
    dest->type = TAG_PSTRUCT;
    return env_eval_equal_struct(env, macro, a->data.pstruct,
                                 b->data.pstruct, &dest->data.pstruct);
  */
  case TAG_TIME:
    dest->type = TAG_TIME;
    return env_eval_equal_time(env, macro, &a->data.time,
                               &b->data.time, &dest->data.time);
  case TAG_TUPLE:
    dest->type = TAG_TUPLE;
    return env_eval_equal_tuple(env, macro, &a->data.tuple,
                                &b->data.tuple, &dest->data.tuple);
  case TAG_ARRAY:
  case TAG_DO_BLOCK:
  case TAG_BOOL:
  case TAG_CALL:
  case TAG_CHARACTER:
  case TAG_FACT:
  case TAG_IDENT:
  case TAG_PCALLABLE:
  case TAG_PSTRUCT:
  case TAG_PSTRUCT_TYPE:
  case TAG_PTAG:
  case TAG_PTR:
  case TAG_PTR_FREE:
  case TAG_QUOTE:
  case TAG_STR:
  case TAG_SYM:
  case TAG_VAR:
    if (compare_tag(a, b)) {
      err_puts("env_eval_equal_tag: value mismatch");
      err_stacktrace();
      return false;
    }
    tag_init_copy(dest, a);
    return true;
  case TAG_COMPLEX:
  case TAG_COW:
  case TAG_F32:
  case TAG_F64:
  case TAG_F128:
  case TAG_INTEGER:
  case TAG_RATIO:
  case TAG_S8:
  case TAG_S16:
  case TAG_S32:
  case TAG_S64:
  case TAG_SW:
  case TAG_U8:
  case TAG_U16:
  case TAG_U32:
  case TAG_U64:
  case TAG_UNQUOTE:
  case TAG_UW:
    break;
  }
  err_puts("env_eval_equal_tag: invalid tag");
  assert(! "env_eval_equal_tag: invalid tag");
  abort();
  return false;
}

bool env_eval_equal_time (s_env *env, bool macro, s_time *a,
                          s_time *b, s_time *dest)
{
  s_tag *a2;
  s_tag  a_tag[2] = {0};
  s_tag *b2;
  s_tag  b_tag[2] = {0};
  const s_sym *sym_Sw = &g_sym_Sw;
  s_time tmp = {0};
  s_tag  tmp_tag[2] = {0};
  assert(env);
  assert(a);
  assert(b);
  assert(dest);
  if (! (a2 = a->tag)) {
    a2 = a_tag;
    a2[0].type = TAG_SW;
    a2[0].data.sw = a->tv_sec;
    a2[1].type = TAG_SW;
    a2[1].data.sw = a->tv_nsec;
  }
  if (! (b2 = b->tag)) {
    b2 = b_tag;
    b2[0].type = TAG_SW;
    b2[0].data.sw = b->tv_sec;
    b2[1].type = TAG_SW;
    b2[1].data.sw = b->tv_nsec;
  }
  if (! env_eval_equal_tag(env, macro, a2, b2, tmp_tag)) {
    return false;
  }
  if (! env_eval_equal_tag(env, macro, a2 + 1, b2 + 1, tmp_tag + 1)) {
    tag_clean(tmp_tag);
    return false;
  }
  if (! sw_init_cast(&tmp.tv_sec, &sym_Sw, tmp_tag)) {
    err_puts("env_eval_equal_time: sw_init_cast (tv_sec)");
    assert(! "env_eval_equal_time: sw_init_cast (tv_sec)");
    tag_clean(tmp_tag + 1);
    tag_clean(tmp_tag);
    return false;
  }
  if (! sw_init_cast(&tmp.tv_nsec, &sym_Sw, tmp_tag + 1)) {
    err_puts("env_eval_equal_time: sw_init_cast (tv_nsec)");
    assert(! "env_eval_equal_time: sw_init_cast (tv_nsec)");
    tag_clean(tmp_tag + 1);
    tag_clean(tmp_tag);
    return false;
  }
  *dest = tmp;
  return true;
}

bool env_eval_equal_tuple (s_env *env, bool macro, s_tuple *a,
                           s_tuple *b, s_tuple *dest)
{
  uw i;
  s_tuple tmp = {0};
  assert(env);
  assert(a);
  assert(b);
  assert(dest);
  if (a->count != b->count) {
    err_puts("env_eval_equal_tuple: tuple arity mismatch");
    assert(! "env_eval_equal_tuple: tuple arity mismatch");
    return false;
  }
  tuple_init(&tmp, a->count);
  i = 0;
  while (i < a->count) {
    if (! env_eval_equal_tag(env, macro, a->tag + i, b->tag + i,
                             tmp.tag + i)) {
      err_write_1("env_eval_equal_tuple: tuple tag mismatch index ");
      err_inspect_uw(&i);
      err_write_1("\n");
      assert(! "env_eval_equal_tuple: tuple tag mismatch");
      tuple_clean(&tmp);
      return false;
    }
    i++;
  }
  *dest = tmp;
  return true;
}

bool env_eval_ident (s_env *env, const s_ident *ident, s_tag *dest)
{
  s_tag *tag;
  s_tag tmp = {0};
  s_ident tmp_ident;
  assert(env);
  assert(ident);
  assert(dest);
  if ((tag = env_frames_get(env, ident->sym))) {
    tag_init_copy(dest, tag);
    return true;
  }
  if (env_ident_resolve_module(env, ident, &tmp_ident) &&
      ! (tag = env_ident_get(env, &tmp_ident, &tmp))) {
    err_write_1("env_eval_ident: unbound ident: ");
    err_inspect_ident(ident);
    err_write_1("\n");
    if (true) {
      err_puts("env_eval_ident: stacktrace:");
      err_inspect_stacktrace(env->stacktrace);
      err_write_1("\n");
    }
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
  if (env_frames_get(env, ident->sym))
    return true;
  if (env_ident_resolve_module(env, ident, &tmp_ident) &&
      env_ident_get(env, &tmp_ident, &tmp)) {
    tag_clean(&tmp);
    return true;
  }
  return false;
}

bool env_eval_list (s_env *env, s_list *list, s_tag *dest)
{
  s_list *next;
  s_list *tmp = NULL;
  s_list **tail = &tmp;
  assert(env);
  assert(dest);
  while (list) {
    *tail = list_new(NULL);
    if (! env_eval_tag(env, &list->tag, &(*tail)->tag))
      goto ko;
    next = list_next(list);
    if (! next)
      if (! env_eval_tag(env, &list->next, &(*tail)->next))
        goto ko;
    tail = &(*tail)->next.data.list;
    list = next;
  }
  dest->type = TAG_LIST;
  dest->data.list = tmp;
  return true;
 ko:
  list_delete_all(tmp);
  return false;
}

bool env_eval_map (s_env *env, s_map *map, s_tag *dest)
{
  s_map tmp = {0};
  uw i = 0;
  assert(env);
  assert(map);
  assert(dest);
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
  dest->data.map = tmp;
  return true;
 ko:
  map_clean(&tmp);
  return false;
}

bool env_eval_quote (s_env *env, s_quote *quote, s_tag *dest)
{
  bool r;
  assert(env);
  assert(quote);
  assert(dest);
  env->quote_level++;
  r = env_eval_quote_tag(env, quote->tag, dest);
  env->quote_level--;
  return r;
}

bool env_eval_quote_array (s_env *env, s_array *array,
                           s_tag *dest)
{
  uw i;
  s_tag *tag;
  s_array tmp = {0};
  s_tag  *tmp_tag;
  assert(env);
  assert(array);
  assert(dest);
  if (! array->dimension || array->data || ! array->tags) {
    if (! tag_init_array_copy(dest, array))
      return false;
    return true;
  }
  if (! array_init_copy_shallow(&tmp, array))
    return false;
  tag = array->tags;
  tmp.tags = alloc(tmp.count * sizeof(s_tag));
  if (! tmp.tags)
    goto ko;
  tmp_tag = tmp.tags;
  i = 0;
  while (i < array->count) {
    if (! env_eval_quote_tag(env, tag, tmp_tag))
      goto ko;
    tag++;
    tmp_tag++;
    i++;
  }
  dest->type = TAG_ARRAY;
  dest->data.array = tmp;
  return true;
 ko:
  array_clean(&tmp);
  return false;
}

bool env_eval_quote_do_block (s_env *env, s_do_block *do_block,
                              s_tag *dest)
{
  uw i = 0;
  s_do_block tmp = {0};
  assert(env);
  assert(do_block);
  assert(dest);
  do_block_init(&tmp, do_block->count);
  while (i < do_block->count) {
    if (! env_eval_quote_tag(env, do_block->tag + i, tmp.tag + i))
      goto ko;
    i++;
  }
  dest->type = TAG_DO_BLOCK;
  dest->data.do_block = tmp;
  return true;
 ko:
  do_block_clean(&tmp);
  return false;
}

bool env_eval_quote_call (s_env *env, s_call *call, s_tag *dest)
{
  s_list *arg;
  s_call        tmp = {0};
  s_list **tmp_arg_last;
  assert(call);
  assert(dest);
  if (! ident_init_copy(&tmp.ident, &call->ident))
    return false;
  tmp_arg_last = &tmp.arguments;
  arg = call->arguments;
  while (arg) {
    *tmp_arg_last = list_new(NULL);
    if (! env_eval_quote_tag(env, &arg->tag, &(*tmp_arg_last)->tag))
      goto ko;
    tmp_arg_last = &(*tmp_arg_last)->next.data.list;
    arg = list_next(arg);
  }
  if (call->pcallable &&
      ! pcallable_init_copy(&tmp.pcallable, &call->pcallable))
    goto ko;
  dest->type = TAG_CALL;
  dest->data.call = tmp;
  return true;
 ko:
  call_clean(&tmp);
  return false;
}

bool env_eval_quote_complex (s_env *env, s_complex *c,
                             s_tag *dest)
{
  s_tag tmp = {0};
  assert(env);
  assert(c);
  assert(dest);
  tmp.type = TAG_COMPLEX;
  tmp.data.complex = complex_new();
  if (! tmp.data.complex)
    return false;
  if (! env_eval_quote_tag(env, &c->x, &tmp.data.complex->x) ||
      ! env_eval_quote_tag(env, &c->y, &tmp.data.complex->y)) {
    complex_delete(tmp.data.complex);
    return false;
  }
  *dest = tmp;
  return true;
}

bool env_eval_quote_cow (s_env *env, s_cow *cow,
                         s_tag *dest)
{
  s_tag tmp = {0};
  assert(env);
  assert(cow);
  assert(dest);
  tmp.type = TAG_COW;
  tmp.data.cow = cow_new(cow->type);
  if (! tmp.data.cow)
    return false;
  if (! env_eval_quote_tag(env, cow_read_only(cow),
                           cow_read_write(tmp.data.cow))) {
    cow_delete(tmp.data.cow);
    return false;
  }
  cow_freeze(tmp.data.cow);
  *dest = tmp;
  return true;
}

bool env_eval_quote_list (s_env *env, s_list *list, s_tag *dest)
{
  s_list *next = NULL;
  s_list *tmp = NULL;
  s_list **tail = &tmp;
  assert(env);
  assert(dest);
  while (list) {
    *tail = list_new(NULL);
    if (! *tail)
      goto ko;
    if (! env_eval_quote_tag(env, &list->tag, &(*tail)->tag))
      goto ko;
    next = list_next(list);
    if (! next)
      if (! env_eval_quote_tag(env, &list->next, &(*tail)->next))
        goto ko;
    tail = &(*tail)->next.data.list;
    list = next;
  }
  dest->type = TAG_LIST;
  dest->data.list = tmp;
  return true;
 ko:
  list_delete_all(tmp);
  return false;
}

bool env_eval_quote_map (s_env *env, s_map *map, s_tag *dest)
{
  s_map tmp = {0};
  uw i = 0;
  assert(env);
  assert(map);
  assert(dest);
  if (! map_init(&tmp, map->count))
    return false;
  while (i < tmp.count) {
    if (! env_eval_quote_tag(env, map->key + i, tmp.key + i) ||
        ! env_eval_quote_tag(env, map->value + i, tmp.value + i))
      goto ko;
    i++;
  }
  dest->type = TAG_MAP;
  dest->data.map = tmp;
  return true;
 ko:
  map_clean(&tmp);
  return false;
}

bool env_eval_quote_quote (s_env *env, s_quote *quote, s_tag *dest)
{
  bool r;
  s_quote tmp = {0};
  assert(env);
  assert(quote);
  assert(dest);
  tmp.tag = tag_new();
  if (! tmp.tag)
    return false;
  env->quote_level++;
  r = env_eval_quote_tag(env, quote->tag, tmp.tag);
  env->quote_level--;
  if (! r)
    return false;
  dest->type = TAG_QUOTE;
  dest->data.quote = tmp;
  return true;
}

bool env_eval_quote_struct (s_env *env, s_struct *s, s_tag *dest)
{
  uw i;
  p_struct_type st;
  s_tag tmp = {0};
  assert(env);
  assert(s);
  assert(dest);
  tmp.type = TAG_PSTRUCT;
  if (s->data || ! s->tag) {
    if (! pstruct_init_copy(&tmp.data.pstruct, &s))
      return false;
    *dest = tmp;
    return true;
  }
  pstruct_init_with_type(&tmp.data.pstruct, s->pstruct_type);
  st = tmp.data.pstruct->pstruct_type;
  tmp.data.pstruct->tag = alloc(st->map.count * sizeof(s_tag));
  if (! tmp.data.pstruct->tag)
    return false;
  i = 0;
  while (i < st->map.count) {
    if (! env_eval_quote_tag(env, s->tag + i,
                             tmp.data.pstruct->tag + i))
      goto ko;
    i++;
  }
  *dest = tmp;
  return true;
 ko:
  return false;
}

// Like tag_init_copy excepted that the unquote parts get evaluated.
bool env_eval_quote_tag (s_env *env, s_tag *tag, s_tag *dest)
{
  assert(env);
  assert(tag);
  assert(dest);
  switch (tag->type) {
  case TAG_ARRAY:
    return env_eval_quote_array(env, &tag->data.array, dest);
  case TAG_DO_BLOCK:
    return env_eval_quote_do_block(env, &tag->data.do_block, dest);
  case TAG_CALL:
    return env_eval_quote_call(env, &tag->data.call, dest);
  case TAG_COMPLEX:
    return env_eval_quote_complex(env, tag->data.complex, dest);
  case TAG_COW:
    return env_eval_quote_cow(env, tag->data.cow, dest);
  case TAG_LIST:
    return env_eval_quote_list(env, tag->data.list, dest);
  case TAG_MAP:
    return env_eval_quote_map(env, &tag->data.map, dest);
  case TAG_PSTRUCT:
    return env_eval_quote_struct(env, tag->data.pstruct, dest);
  case TAG_QUOTE:
    return env_eval_quote_quote(env, &tag->data.quote, dest);
  case TAG_TIME:
    return env_eval_quote_time(env, &tag->data.time, dest);
  case TAG_TUPLE:
    return env_eval_quote_tuple(env, &tag->data.tuple, dest);
  case TAG_UNQUOTE:
    return env_eval_quote_unquote(env, &tag->data.unquote, dest);
  case TAG_VOID:
  case TAG_BOOL:
  case TAG_CHARACTER:
  case TAG_F32:
  case TAG_F64:
  case TAG_F128:
  case TAG_FACT:
  case TAG_IDENT:
  case TAG_INTEGER:
  case TAG_PCALLABLE:
  case TAG_PSTRUCT_TYPE:
  case TAG_PTAG:
  case TAG_PTR:
  case TAG_PTR_FREE:
  case TAG_RATIO:
  case TAG_S8:
  case TAG_S16:
  case TAG_S32:
  case TAG_S64:
  case TAG_STR:
  case TAG_SW:
  case TAG_SYM:
  case TAG_U8:
  case TAG_U16:
  case TAG_U32:
  case TAG_U64:
  case TAG_UW:
  case TAG_VAR:
    if (! tag_init_copy(dest, tag))
      return false;
    return true;
  }
  err_puts("env_eval_quote_tag: invalid tag type");
  assert(! "env_eval_quote_tag: invalid tag type");
  return false;
}

bool env_eval_quote_time (s_env *env, s_time *time, s_tag *dest)
{
  s_time tmp = {0};
  assert(env);
  assert(time);
  assert(dest);
  if (time->tag) {
    if (! time_allocate(&tmp))
      return false;
    if (! env_eval_quote_tag(env, time->tag, tmp.tag)) {
      err_puts("env_eval_quote_time: env_eval_quote_tag: tv_sec");
      assert(! "env_eval_quote_time: env_eval_quote_tag: tv_sec");
      free(tmp.tag);
      return false;
    }
    if (! env_eval_quote_tag(env, time->tag + 1, tmp.tag + 1)) {
      err_puts("env_eval_quote_time: env_eval_quote_tag: tv_nsec");
      assert(! "env_eval_quote_time: env_eval_quote_tag: tv_nsec");
      tag_clean(tmp.tag);
      free(tmp.tag);
      return false;
    }
  }
  else {
    tmp.tv_sec = time->tv_sec;
    tmp.tv_nsec = time->tv_nsec;
    tmp.tag = NULL;
  }
  dest->type = TAG_TIME;
  dest->data.time = tmp;
  return true;
}

bool env_eval_quote_tuple (s_env *env, s_tuple *tuple, s_tag *dest)
{
  uw i = 0;
  s_tuple tmp = {0};
  assert(env);
  assert(tuple);
  assert(dest);
  tuple_init(&tmp, tuple->count);
  while (i < tuple->count) {
    if (! env_eval_quote_tag(env, tuple->tag + i, tmp.tag + i))
      goto ko;
    i++;
  }
  dest->type = TAG_TUPLE;
  dest->data.tuple = tmp;
  return true;
 ko:
  tuple_clean(&tmp);
  return false;
}

bool env_eval_quote_unquote (s_env *env, s_unquote *unquote,
                             s_tag *dest)
{
  bool r;
  s_tag tmp = {0};
  assert(env);
  assert(unquote);
  assert(dest);
  if (env->unquote_level >= env->quote_level) {
    err_puts("env_eval_quote_unquote: unquote outside of a quote");
    assert(! "env_eval_quote_unquote: unquote outside of a quote");
    return false;
  }
  env->unquote_level++;
  if (env->unquote_level == 1) {
    if (env->quote_level == 1)
      r = env_eval_tag(env, unquote->tag, &tmp);
    else
      r = tag_init_copy(&tmp, unquote->tag) ? true : false;
  }
  else
    r = tag_init_unquote_copy(&tmp, unquote) ? true : false;
  env->unquote_level--;
  if (! r)
    return false;
  *dest = tmp;
  return true;
}

bool env_eval_struct (s_env *env, s_struct *s, p_struct *dest)
{
  void *data = NULL;
  uw i;
  s_tag *key = NULL;
  s_tag tag = {0};
  p_struct tmp = NULL;
  void    *tmp_data = NULL;
  const s_sym *type;
  s_var *var = NULL;
  assert(env);
  assert(s);
  assert(dest);
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
    if (key->type != TAG_SYM) {
      err_puts("env_eval_struct: struct type key is not a Sym");
      assert(! "env_eval_struct: struct type key is not a Sym");
      goto ko;
    }
    if (key->data.sym->str.ptr.pchar[0] != '_') {
      if (tmp->pstruct_type->map.value[i].type == TAG_VAR) {
        var = &tmp->pstruct_type->map.value[i].data.var;
        type = var->type;
      }
      else if (! tag_type(tmp->pstruct_type->map.value + i, &type))
        goto ko;
      if (s->tag) {
        if (! env_eval_tag(env, s->tag + i, &tag))
          goto ko;
        if (! tag_to_const_pointer(&tag, type, &data)) {
          tag_clean(&tag);
          goto ko;
        }
      }
      else {
        if (! tag_to_const_pointer(tmp->pstruct_type->map.value + i,
                                   type, &data))
          goto ko;
      }
      if (false) {
        err_write_1("env_eval_struct: type = ");
        err_inspect_sym(&type);
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
  err_write_1(tmp->pstruct_type->map.key[i].data.sym->str.ptr.pchar);
  err_write_1(", expected ");
  err_puts(tag_type_to_string(tmp->pstruct_type->map.value[i].type));
  tag_clean(&tag);
 ko:
  pstruct_clean(&tmp);
  return false;
}

bool env_eval_struct_tag (s_env *env, s_struct *s, s_tag *dest)
{
  if (! env_eval_struct(env, s, &dest->data.pstruct))
    return false;
  dest->type = TAG_PSTRUCT;
  return true;
}

bool env_eval_tag (s_env *env, s_tag *tag, s_tag *dest)
{
  assert(env);
  assert(tag);
  assert(dest);
  switch (tag->type) {
  case TAG_VOID:
    tag_init_void(dest);
    return true;
  case TAG_ARRAY:
    return env_eval_array_tag(env, &tag->data.array, dest);
  case TAG_DO_BLOCK:
    return env_eval_do_block(env, &tag->data.do_block, dest);
  case TAG_CALL:
    return env_eval_call(env, &tag->data.call, dest);
  case TAG_COMPLEX:
    return env_eval_complex(env, tag->data.complex, dest);
  case TAG_COW:
    return env_eval_cow(env, tag->data.cow, dest);
  case TAG_IDENT:
    return env_eval_ident(env, &tag->data.ident, dest);
  case TAG_LIST:
    return env_eval_list(env, tag->data.list, dest);
  case TAG_MAP:
    return env_eval_map(env, &tag->data.map, dest);
  case TAG_PCALLABLE:
    return env_eval_callable(env, tag->data.pcallable, dest);
  case TAG_PSTRUCT:
    return env_eval_struct_tag(env, tag->data.pstruct, dest);
  case TAG_QUOTE:
    return env_eval_quote(env, &tag->data.quote, dest);
  case TAG_TIME:
    return env_eval_time(env, &tag->data.time, dest);
  case TAG_TUPLE:
    return env_eval_tuple(env, &tag->data.tuple, dest);
  case TAG_VAR:
    return env_eval_var(env, &tag->data.var, dest);
  case TAG_BOOL:
  case TAG_CHARACTER:
  case TAG_F32:
  case TAG_F64:
  case TAG_F128:
  case TAG_FACT:
  case TAG_INTEGER:
  case TAG_PSTRUCT_TYPE:
  case TAG_PTAG:
  case TAG_PTR:
  case TAG_PTR_FREE:
  case TAG_RATIO:
  case TAG_S8:
  case TAG_S16:
  case TAG_S32:
  case TAG_S64:
  case TAG_STR:
  case TAG_SW:
  case TAG_SYM:
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
  tmp.type = TAG_TIME;
  if (time->tag) {
    if (! env_eval_tag(env, time->tag, tag))
      return false;
    if (! env_eval_tag(env, time->tag + 1, tag + 1)) {
      tag_clean(tag);
      return false;
    }
    if (! sw_init_cast(&tmp.data.time.tv_sec, &sym_Sw, tag)) {
      err_write_1("env_eval_time: tv_sec is not a Sw: ");
      err_inspect_tag(tag);
      err_write_1("\n");
      assert(! "env_eval_time: tv_sec is not a Sw");
      tag_clean(tag + 1);
      tag_clean(tag);
      return false;
    }
    if (! sw_init_cast(&tmp.data.time.tv_nsec, &sym_Sw, tag + 1)) {
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
    tmp.data.time.tv_sec = time->tv_sec;
    tmp.data.time.tv_nsec = time->tv_nsec;
  }
  *dest = tmp;
  return true;
}

bool env_eval_tuple (s_env *env, const s_tuple *tuple, s_tag *dest)
{
  uw i = 0;
  s_tuple tmp = {0};
  assert(env);
  assert(tuple);
  assert(dest);
  tuple_init(&tmp, tuple->count);
  while (i < tuple->count) {
    if (! env_eval_tag(env, tuple->tag + i, tmp.tag + i))
      return false;
    i++;
  }
  dest->type = TAG_TUPLE;
  dest->data.tuple = tmp;
  return true;
}

bool env_eval_var (s_env *env, const s_var *var, s_tag *dest)
{
  s_tag tmp = {0};
  assert(env);
  assert(var);
  assert(dest);
  (void) env;
  if (var->ptr && var->ptr->type != TAG_VAR)
    return tag_init_copy(dest, var->ptr) ? true : false;
  tmp.type = TAG_VAR;
  tmp.data.var = *var;
  *dest = tmp;
  return true;
}
