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
#include <string.h>
#include <sys/types.h>

#if (defined(__APPLE__) ||      \
     defined(__FreeBSD__) ||    \
     defined(__NetBSD__) ||     \
     defined(__OpenBSD__))
# include <sys/sysctl.h>
#else
# if ! (defined(WIN32) || defined(WIN64))
#  include <sys/sysinfo.h>
# endif
#endif

#include <unistd.h>
#include "alloc.h"
#include "array.h"
#include "assert.h"
#include "binding.h"
#include "block.h"
#include "bool.h"
#include "buf.h"
#include "buf_file.h"
#include "buf_getc.h"
#include "buf_inspect.h"
#include "buf_parse.h"
#include "buf_save.h"
#include "call.h"
#include "callable.h"
#include "cfn.h"
#include "compare.h"
#include "complex.h"
#include "cow.h"
#include "data.h"
#include "env.h"
#include "error.h"
#include "error_handler.h"
#include "fact.h"
#include "facts.h"
#include "facts_cursor.h"
#include "facts_spec.h"
#include "facts_transaction.h"
#include "facts_with.h"
#include "facts_with_cursor.h"
#include "file.h"
#include "fn.h"
#include "fn_clause.h"
#include "frame.h"
#include "ident.h"
#include "integer.h"
#include "io.h"
#include "kc3_main.h"
#include "list.h"
#include "map.h"
#include "module.h"
#include "op.h"
#include "ops.h"
#include "str.h"
#include "struct.h"
#include "struct_type.h"
#include "sw.h"
#include "sym.h"
#include "tag.h"
#include "tag_init.h"
#include "time.h"
#include "tuple.h"
#include "var.h"

static thread_local s_env *g_kc3_env = NULL;

static void    env_clean_globals (s_env *env);
static void    env_clean_toplevel (s_env *env);
static s_env * env_init_args (s_env *env, int *argc, char ***argv);
static s_env * env_init_globals (s_env *env);
static s_env * env_init_toplevel (s_env *env);

s_tag * env_and (s_env *env, s_tag *a, s_tag *b, s_tag *dest)
{
  s_tag eval;
  bool p;
  s_tag tmp;
  const s_sym *sym_Bool = &g_sym_Bool;
  assert(env);
  assert(a);
  assert(b);
  assert(dest);
  if (! env_eval_tag(env, a, &eval))
    return NULL;
  if (! bool_init_cast(&p, &sym_Bool, &eval)) {
    tag_clean(&eval);
    return NULL;
  }
  tag_clean(&eval);
  tag_init_bool(&tmp, false);
  if (p) {
    if (! env_eval_tag(env, b, &eval))
      return NULL;
    if (! bool_init_cast(&p, &sym_Bool, &eval)) {
      tag_clean(&eval);
      return NULL;
    }
    if (p)
      tmp = eval;
  }
  *dest = tmp;
  return dest;
}

s_list ** env_args (s_env *env, s_list **dest)
{
  sw i;
  s_list *tmp;
  s_list **tail;
  tmp = NULL;
  tail = &tmp;
  i = 0;
  while (i < env->argc) {
    if (! (*tail = list_new_str_1(NULL, env->argv[i], NULL)))
      goto clean;
    tail = &(*tail)->next.data.list;
    i++;
  }
  *dest = tmp;
  return dest;
 clean:
  list_delete_all(tmp);
  return NULL;
}

bool env_call_get (s_env *env, s_call *call)
{
  s_facts_cursor cursor;
  s_fact *fact;
  s_fact *found;
  s_tag tag_ident;
  s_tag tag_is_a;
  s_tag tag_macro;
  s_tag tag_module_name;
  s_tag tag_op;
  s_tag tag_special_operator;
  s_tag tag_symbol;
  s_tag tag_symbol_value;
  s_tag tag_var;
  tag_init_ident(&tag_ident, &call->ident);
  tag_init_sym(  &tag_is_a, &g_sym_is_a);
  tag_init_sym(  &tag_macro, &g_sym_macro);
  tag_init_sym(  &tag_module_name, call->ident.module);
  tag_init_sym(  &tag_op, &g_sym_op);
  tag_init_sym(  &tag_special_operator, &g_sym_special_operator);
  tag_init_sym(  &tag_symbol, &g_sym_symbol);
  tag_init_sym(  &tag_symbol_value, &g_sym_symbol_value);
  tag_init_var(  &tag_var, &g_sym_Tag);
  if (! facts_find_fact_by_tags(env->facts, &tag_module_name,
                                &tag_symbol, &tag_ident, &fact)) {
    err_puts("env_call_get: facts_find_fact_by_tags 1");
    assert(! "env_call_get: facts_find_fact_by_tags 1");
    return false;
  }
  if (! fact) {
    if (! facts_find_fact_by_tags(env->facts, &tag_module_name,
                                  &tag_op, &tag_ident, &fact)) {
      err_puts("env_call_get: facts_find_fact_by_tags 2");
      assert(! "env_call_get: facts_find_fact_by_tags 2");
      return false;
    }
    if (! fact) {
      err_write_1("env_call_get: symbol ");
      err_write_1(call->ident.sym->str.ptr.pchar);
      err_write_1(" not found in module ");
      err_write_1(call->ident.module->str.ptr.pchar);
      err_write_1("\n");
      return false;
    }
  }
  if (! facts_with_tags (env->facts, &cursor, &tag_ident,
                        &tag_symbol_value, &tag_var)) {
    err_puts("env_call_get: facts_with_tags");
    assert(! "env_call_get: facts_with_tags");
    return false;
  }
  if (! facts_cursor_next(&cursor, &fact)) {
    err_puts("env_call_get: facts_cursor_next");
    assert(! "env_call_get: facts_cursor_next");
    return false;
  }
  if (! fact) {
    err_write_1("env_call_get: ");
    err_inspect_ident(&call->ident);
    err_puts(" :symbol_value not found");
    return false;
  }
  if (tag_var.type != TAG_CALLABLE) {
    err_write_1("env_call_get: ");
    err_inspect_ident(&call->ident);
    err_puts(" is not a Callable");
    facts_cursor_clean(&cursor);
    return false;
  }
  call->callable = callable_new_ref(tag_var.data.callable);
  if (call->callable->type == CALLABLE_FN) {
    fn_set_name_if_null(&call->callable->data.fn,
                        call->ident.module,
                        call->ident.sym);
  }
  facts_cursor_clean(&cursor);
  if (! facts_find_fact_by_tags(env->facts, &tag_ident, &tag_is_a,
                                &tag_special_operator, &found)) {
    err_puts("env_call_get: facts_find_fact_by_tags 4");
    assert(! "env_call_get: facts_find_fact_by_tags 4");
    return false;
  }
  if (found) {
    switch (call->callable->type) {
    case CALLABLE_CFN:
      call->callable->data.cfn.special_operator = true;
      break;
    case CALLABLE_FN:
      call->callable->data.fn.special_operator = true;
      break;
    case CALLABLE_VOID:
      err_puts("env_call_get: void callable");
      assert(! "env_call_get: void callable");
      abort();
    }
  }
  return true;
}

void env_clean (s_env *env)
{
  assert(env);
  if (false) {
    uw size;
    sym_list_size(&size);
    err_write_1("env_clean: g_sym_list: ");
    err_inspect_uw_decimal(&size);
    err_write_1("\n");
  }
  //facts_save_file(env->facts, "debug.facts"); // debug
  env_clean_globals(env);
  env_clean_toplevel(env);
  error_handler_delete_all(env->error_handler);
  facts_delete(env->facts);
  ops_delete(env->ops);
  buf_file_close(env->in);
  buf_delete(env->in);
  buf_file_close(env->out);
  buf_delete(env->out);
  buf_file_close(env->err);
  buf_delete(env->err);
  str_delete(env->argv0_dir);
  str_delete(env->module_path);
  list_delete_all(env->path);
  list_delete_all(env->search_modules_default);
}

void env_clean_globals (s_env *env)
{
  frame_delete(env->global_frame);
  frame_delete(env->read_time_frame);
}

void env_clean_toplevel (s_env *env)
{
  frame_delete_all(env->frame);
}

bool env_def (s_env *env, const s_ident *ident, s_tag *value)
{
  s_tag tag_ident;
  s_tag tag_module;
  s_tag tag_symbol;
  s_tag tag_symbol_value;
  (void) env;
  assert(env);
  assert(ident);
  assert(value);
  tag_ident.type = TAG_IDENT;
  tag_ident.data.ident.sym = ident->sym;
  if (ident->module)
    tag_ident.data.ident.module = ident->module;
  else
    tag_ident.data.ident.module = env->current_defmodule;
  tag_init_sym(&tag_module, tag_ident.data.ident.module);
  tag_init_sym(&tag_symbol, &g_sym_symbol);
  if (! facts_add_tags(env->facts, &tag_module, &tag_symbol,
                       &tag_ident))
    return false;
  tag_init_sym(&tag_symbol_value, &g_sym_symbol_value);
  if (! facts_replace_tags(env->facts, &tag_ident, &tag_symbol_value,
                           value)) {
    return false;
  }
  if (tag_ident.data.ident.module == env->current_defmodule &&
      tag_ident.data.ident.sym == &g_sym_clean) {
    if (! env_def_clean(env, env->current_defmodule, value)) {
      return false;
    }
  }
  return true;
}

const s_sym * env_def_clean (s_env *env, const s_sym *module,
                             const s_tag *clean)
{
  s_struct_type *st;
  s_tag tag_module_name;
  s_tag tag_st;
  s_tag tag_struct_type;
  if (! env_struct_type_find(env, module, &st))
    return NULL;
  if (! st) {
    err_write_1("env_def_clean: module ");
    err_inspect_sym(&module);
    err_write_1(": struct type not found");
    assert(! "env_def_clean: module struct type not found");
    return NULL;
  }
  if (clean->type != TAG_CALLABLE ||
      clean->data.callable->type != CALLABLE_CFN) {
    err_write_1("env_def_clean: module ");
    err_inspect_sym(&module);
    err_write_1(": clean method must be a Cfn");
    assert(! "env_def_clean: module clean method must be a Cfn");
    return NULL;
  }
  tag_init_sym(&tag_module_name, module);
  tag_init_struct_type_update_clean(&tag_st, st,
                                    &clean->data.callable->data.cfn);
  tag_init_sym(&tag_struct_type, &g_sym_struct_type);
  if (! facts_replace_tags(env->facts, &tag_module_name,
                           &tag_struct_type, &tag_st)) {
    tag_clean(&tag_st);
    return NULL;
  }
  tag_clean(&tag_st);
  return module;
}

// FIXME: transaction ?
s_tag * env_defmodule (s_env *env, const s_sym * const *name,
                       const s_block *block, s_tag *dest)
{
  const s_sym *prev_defmodule;
  s_tag *result = NULL;
  s_list *search_modules;
  s_tag tag_is_a;
  s_tag tag_module;
  s_tag tag_module_name;
  s_tag tmp = {0};
  assert(env);
  assert(name);
  assert(*name);
  assert(block);
  assert(dest);
  prev_defmodule = env->current_defmodule;
  env_module_is_loading_set(env, *name, true);
  env->current_defmodule = *name;
  search_modules = env->search_modules;
  if (! env_module_search_modules(env, name, &env->search_modules))
    goto clean;
  tag_init_sym(&tag_is_a, &g_sym_is_a);
  tag_init_sym(&tag_module, &g_sym_module);
  tag_init_sym(&tag_module_name, *name);
  if (! facts_add_tags(env->facts, &tag_module_name, &tag_is_a,
                       &tag_module))
    goto clean;
  if (! env_eval_block(env, block, &tmp))
    goto clean;
  tag_clean(&tmp);
  tag_init_sym(dest, *name);
  result = dest;
 clean:
  list_delete_all(env->search_modules);
  env->search_modules = search_modules;
  env->current_defmodule = prev_defmodule;
  env_module_is_loading_set(env, *name, false);
  return result;
}

s_tag * env_defoperator (s_env *env, s_op *op, s_tag *dest)
{
  s_tag tag_sym = {0};
  s_op *tmp;
  assert(env);
  assert(op);
  assert(dest);
  assert(env->ops);
  assert(op->sym);
  assert(op->arity);
  if (! (tmp = op_new_copy(op)))
    return NULL;
  if (! ops_add(env->ops, tmp)) {
    op_delete(tmp);
    return NULL;
  }
  op_delete(tmp);
  tag_init_sym(&tag_sym, op->sym);
  *dest = tag_sym;
  return dest;
}

const s_sym * env_defstruct (s_env *env, s_list *spec)
{
  s_tag tag_module_name;
  s_tag tag_st;
  s_tag tag_struct_type;
  tag_init_sym(&tag_module_name, env->current_defmodule);
  tag_init_struct_type(&tag_st, env->current_defmodule, spec);
  tag_init_sym(&tag_struct_type, &g_sym_struct_type);
  if (! facts_replace_tags(env->facts, &tag_module_name,
                           &tag_struct_type, &tag_st)) {
    tag_clean(&tag_st);
    return NULL;
  }
  tag_clean(&tag_st);
  return env->current_defmodule;
}

void env_error_f (s_env *env, const char *fmt, ...)
{
  va_list ap;
  s_tag tag;
  assert(env);
  assert(fmt);
  va_start(ap, fmt);
  tag.type = TAG_STR;
  str_init_vf(&tag.data.str, fmt, ap);
  va_end(ap);
  env_error_tag(env, &tag);
}

void env_error_tag (s_env *env, s_tag *tag)
{
  s_error_handler *error_handler;
  assert(env);
  assert(tag);
  error_handler = env->error_handler;
  if (error_handler) {
    tag_init_copy(&error_handler->tag, tag);
    error_handler->stacktrace = list_new_copy(env->stacktrace);
    env_longjmp(env, &error_handler->jmp_buf);
    /* never reached */
    return;
  }
  if (buf_file_is_open(env->err)) {
    buf_write_1(env->err, "error: ");
    buf_inspect_tag(env->err, tag);
    buf_write_1(env->err, "\n");
    return;
  }
}

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

bool env_eval_block (s_env *env, const s_block *block, s_tag *dest)
{
  uw i = 0;
  s_tag tmp = {0};
  assert(env);
  assert(block);
  assert(dest);
  if (! block->count) {
    tag_init_void(dest);
    return true;
  }
  while (i < block->count - 1) {
    if (env_eval_tag(env, block->tag + i, &tmp))
      tag_clean(&tmp);
    i++;
  }
  return env_eval_tag(env, block->tag + i, dest);
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
  if (! c.callable || c.callable->type == CALLABLE_VOID) {
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
  switch (call->callable->type) {
  case CALLABLE_CFN:
    return env_eval_call_cfn_args(env, &call->callable->data.cfn,
                                  call->arguments, dest);
  case CALLABLE_FN:
    return env_eval_call_fn_args(env, &call->callable->data.fn,
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
  if (! call->callable ||
      call->callable->type != CALLABLE_FN) {
    err_puts("env_eval_call_fn: not a Fn");
    assert(! "env_eval_call_fn: not a Fn");
    return false;
  }
  return env_eval_call_fn_args(env, &call->callable->data.fn,
                               call->arguments, dest);
}

bool env_eval_call_fn_args (s_env *env, const s_fn *fn,
                            s_list *arguments, s_tag *dest)
{
  s_list *args = NULL;
  s_list *args_final = NULL;
  s_fn_clause *clause;
  s_frame *env_frame;
  s_frame frame;
  const s_sym *module;
  s_list *search_modules;
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
        env->search_modules = search_modules;
        return false;
      }
      env->frame = &frame;
      if (env_eval_equal_list(env, fn->macro || fn->special_operator,
                              clause->pattern, args_final, &tmp))
        break;
      env->frame = env_frame;
      frame_clean(&frame);
      clause = clause->next_clause;
    }
    if (! clause) {
      err_puts("env_eval_call_fn_args: no clause matching.\nTried clauses :\n");
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
  if (! env_eval_block(env, &clause->algo, &tag)) {
    list_delete_all(args);
    list_delete_all(tmp);
    list_delete_all(env->search_modules);
    env->search_modules = search_modules;
    env->frame = env_frame;
    frame_clean(&frame);
    return false;
  }
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
  s_op *op;
  s_call tmp = {0};
  const s_tag *value;
  assert(env);
  assert(env->ops);
  assert(call);
  tmp = *call;
  if (tmp.ident.module == NULL &&
      (value = env_frames_get(env, tmp.ident.sym))) {
    if (value->type == TAG_CALLABLE) {
      tmp.callable = callable_new_ref(value->data.callable);
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
  if (arity && arity <= U8_MAX &&
      (op = ops_get(env_global()->ops, tmp.ident.sym, arity))) {
    tmp.callable = callable_new_ref(op->callable);
    *call = tmp;
    return true;
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
    if (! (tmp = callable_new_ref(callable)))
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
  dest->type = TAG_CALLABLE;
  dest->data.callable = tmp;
  return true;
 ko:
  if (tmp)
    callable_delete(tmp);
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
  case TAG_STRUCT:
    dest->type = TAG_STRUCT;
    return env_eval_equal_struct(env, macro, &a->data.struct_,
                                 &b->data.struct_, &dest->data.struct_);
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
  case TAG_BLOCK:
  case TAG_BOOL:
  case TAG_CALL:
  case TAG_CALLABLE:
  case TAG_CHARACTER:
  case TAG_FACT:
  case TAG_IDENT:
  case TAG_PTAG:
  case TAG_PTR:
  case TAG_PTR_FREE:
  case TAG_QUOTE:
  case TAG_STR:
  case TAG_STRUCT:
  case TAG_STRUCT_TYPE:
  case TAG_SYM:
  case TAG_VAR:
    if (compare_tag(a, b)) {
      err_puts("env_eval_compare_tag: value mismatch");
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
  error("env_eval_equal_tag: invalid tag");
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

bool env_eval_quote_block (s_env *env, s_block *block, s_tag *dest)
{
  uw i = 0;
  s_block tmp = {0};
  assert(env);
  assert(block);
  assert(dest);
  block_init(&tmp, block->count);
  while (i < block->count) {
    if (! env_eval_quote_tag(env, block->tag + i, tmp.tag + i))
      goto ko;
    i++;
  }
  dest->type = TAG_BLOCK;
  dest->data.block = tmp;
  return true;
 ko:
  block_clean(&tmp);
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
  if (call->callable)
    tmp.callable = callable_new_ref(call->callable);
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
  s_struct *t;
  s_tag tmp = {0};
  assert(env);
  assert(s);
  assert(dest);
  tmp.type = TAG_STRUCT;
  t = &tmp.data.struct_;
  if (s->data || ! s->tag) {
    if (! struct_init_copy(t, s))
      return false;
    *dest = tmp;
    return true;
  }
  t->type = s->type;
  t->tag = alloc(t->type->map.count * sizeof(s_tag));
  if (! t->tag)
    return false;
  i = 0;
  while (i < t->type->map.count) {
    if (! env_eval_quote_tag(env, s->tag + i, t->tag + i))
      goto ko;
    i++;
  }
  *dest = tmp;
  return true;
 ko:
  struct_clean(t);
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
  case TAG_BLOCK:
    return env_eval_quote_block(env, &tag->data.block, dest);
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
  case TAG_QUOTE:
    return env_eval_quote_quote(env, &tag->data.quote, dest);
  case TAG_STRUCT:
    return env_eval_quote_struct(env, &tag->data.struct_, dest);
  case TAG_TIME:
    return env_eval_quote_time(env, &tag->data.time, dest);
  case TAG_TUPLE:
    return env_eval_quote_tuple(env, &tag->data.tuple, dest);
  case TAG_UNQUOTE:
    return env_eval_quote_unquote(env, &tag->data.unquote, dest);
  case TAG_VOID:
  case TAG_BOOL:
  case TAG_CALLABLE:
  case TAG_CHARACTER:
  case TAG_F32:
  case TAG_F64:
  case TAG_F128:
  case TAG_FACT:
  case TAG_IDENT:
  case TAG_INTEGER:
  case TAG_PTAG:
  case TAG_PTR:
  case TAG_PTR_FREE:
  case TAG_RATIO:
  case TAG_S8:
  case TAG_S16:
  case TAG_S32:
  case TAG_S64:
  case TAG_STR:
  case TAG_STRUCT_TYPE:
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

bool env_eval_struct (s_env *env, const s_struct *s, s_struct *dest)
{
  void *data = NULL;
  uw i;
  s_tag tag = {0};
  s_struct tmp = {0};
  void    *tmp_data = NULL;
  const s_sym *type;
  s_var *var = NULL;
  assert(env);
  assert(s);
  assert(dest);
  if (s->data) {
    if (! struct_init_copy(&tmp, s))
      return false;
    *dest = tmp;
    return true;
  }
  tmp.type = s->type;
  if (! struct_allocate(&tmp))
    return false;
  i = 0;
  while (i < tmp.type->map.count) {
    if (tmp.type->map.value[i].type == TAG_VAR) {
      var = &tmp.type->map.value[i].data.var;
      type = var->type;
    }
    else if (! tag_type(tmp.type->map.value + i, &type))
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
      if (! tag_to_const_pointer(tmp.type->map.value + i, type, &data))
        goto ko;
    }
    if (false) {
      err_write_1("env_eval_struct: type = ");
      err_inspect_sym(&type);
      err_write_1("\n");
    }
    tmp_data = (s8 *) tmp.data + tmp.type->offset[i];
    if (! data_init_copy(type, tmp_data, data))
      goto ko_init;
    if (s->tag)
      tag_clean(&tag);
    i++;
  }
  *dest = tmp;
  return true;
 ko_init:
  err_write_1("env_eval_struct: invalid type ");
  err_write_1(tag_type_to_string(tag.type));
  err_write_1(" for key ");
  err_write_1(tmp.type->map.key[i].data.sym->str.ptr.pchar);
  err_write_1(", expected ");
  err_puts(tag_type_to_string(tmp.type->map.value[i].type));
  tag_clean(&tag);
 ko:
  struct_clean(&tmp);
  return false;
}

bool env_eval_struct_tag (s_env *env, const s_struct *s, s_tag *dest)
{
  if (! env_eval_struct(env, s, &dest->data.struct_))
    return false;
  dest->type = TAG_STRUCT;
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
  case TAG_BLOCK:
    return env_eval_block(env, &tag->data.block, dest);
  case TAG_CALL:
    return env_eval_call(env, &tag->data.call, dest);
  case TAG_CALLABLE:
    return env_eval_callable(env, tag->data.callable, dest);
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
  case TAG_QUOTE:
    return env_eval_quote(env, &tag->data.quote, dest);
  case TAG_STRUCT:
    return env_eval_struct_tag(env, &tag->data.struct_, dest);
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
  case TAG_RATIO:
  case TAG_PTAG:
  case TAG_PTR:
  case TAG_PTR_FREE:
  case TAG_S8:
  case TAG_S16:
  case TAG_S32:
  case TAG_S64:
  case TAG_STR:
  case TAG_STRUCT_TYPE:
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

s_fact_w * env_fact_w_eval (s_env *env, s_fact_w *fact,
			    s_fact_w *dest)
{
  s_fact_w tmp = {0};
  assert(env);
  assert(fact);
  assert(dest);
  if (fact->subject.type == TAG_CALLABLE) {
    if (! env_eval_tag(env, &fact->subject, &tmp.subject))
      return NULL;
  }
  else {
    if (! tag_init_copy(&tmp.subject, &fact->subject))
      return NULL;
  }
  if (fact->predicate.type == TAG_CALLABLE) {
    if (! env_eval_tag(env, &fact->predicate, &tmp.predicate))
      return NULL;
  }
  else {
    if (! tag_init_copy(&tmp.predicate, &fact->predicate))
      return NULL;
  }
  if (fact->object.type == TAG_CALLABLE) {
    if (! env_eval_tag(env, &fact->object, &tmp.object))
      return NULL;
  }
  else {
    if (! tag_init_copy(&tmp.object, &fact->object))
      return NULL;
  }
  *dest = tmp;
  return dest;
}

s_tag * env_facts_collect_with (s_env *env, s_facts *facts,
                                s_list **spec,
                                s_callable *callback, s_tag *dest)
{
  s_list *arguments;
  s_facts_with_cursor cursor = {0};
  s_fact *fact = NULL;
  s_fact_w *fact_w = NULL;
  s_list **l;
  s_list  *list;
  s_tag tmp = {0};
  assert(env);
  assert(facts);
  assert(spec);
  assert(callback);
  assert(dest);
  if (! (arguments = list_new_struct(&g_sym_FactW, NULL)))
    return NULL;
  if (! struct_allocate(&arguments->tag.data.struct_)) {
    list_delete_all(arguments);
    return NULL;
  }
  fact_w = arguments->tag.data.struct_.data;
  if (! facts_with_list(facts, &cursor, *spec))
    return NULL;
  list = NULL;
  l = &list;
  while (1) {
    if (! facts_with_cursor_next(&cursor, &fact))
      goto clean;
    if (! fact)
      goto ok;
    fact_w_clean(fact_w);
    if (! fact_w_init_fact(fact_w, fact))
      goto clean;
    *l = list_new(NULL);
    if (! env_eval_call_callable_args(env, callback, arguments,
                                      &(*l)->tag)) {
      goto clean;
    }
    l = &(*l)->next.data.list;
  }
 ok:
  list_delete_all(arguments);
  tmp.type = TAG_LIST;
  tmp.data.list = list;
  if (false) {
    err_write_1("env_facts_collect_with: ");
    err_inspect_tag(&tmp);
    err_write_1("\n");
  }
  *dest = tmp;
  return dest;
 clean:
  facts_with_cursor_clean(&cursor);
  tag_clean(&tmp);
  list_delete_all(arguments);
  return NULL;
}

s_tag * env_facts_collect_with_tags (s_env *env, s_facts *facts,
                                     s_tag *subject,
                                     s_tag *predicate,
                                     s_tag *object,
                                     s_callable *callback,
                                     s_tag *dest)
{
  s_list *arguments;
  s_facts_cursor cursor = {0};
  s_fact *fact = NULL;
  s_fact_w *fact_w = NULL;
  s_list **l;
  s_list  *list;
  s_tag tmp = {0};
  if (! (arguments = list_new_struct(&g_sym_FactW, NULL)))
    return NULL;
  if (! struct_allocate(&arguments->tag.data.struct_))
    return NULL;
  fact_w = arguments->tag.data.struct_.data;
  if (! facts_with_tags(facts, &cursor, subject, predicate, object))
    return NULL;
  list = NULL;
  l = &list;
  while (1) {
    if (! facts_cursor_next(&cursor, &fact))
      goto clean;
    if (! fact) {
      goto ok;
    }
    fact_w_clean(fact_w);
    if (! fact_w_init_fact(fact_w, fact))
      goto clean;
    *l = list_new(NULL);
    if (! env_eval_call_callable_args(env, callback, arguments,
                                      &(*l)->tag))
      goto clean;
    l = &(*l)->next.data.list;
  }
 ok:
  list_delete_all(arguments);
  tmp.type = TAG_LIST;
  tmp.data.list = list;
  *dest = tmp;
  return dest;
 clean:
  facts_cursor_clean(&cursor);
  fact_w_clean(fact_w);
  list_delete_all(list);
  list_delete_all(arguments);
  return NULL;
}

s_tag * env_facts_first_with (s_env *env, s_facts *facts,
                              s_list **spec,
                              s_callable *callback, s_tag *dest)
{
  s_list *arguments;
  s_facts_with_cursor cursor = {0};
  s_fact *fact = NULL;
  s_fact_w *fact_w = NULL;
  s_tag tmp = {0};
  assert(env);
  assert(facts);
  assert(spec);
  assert(callback);
  assert(dest);
  if (! (arguments = list_new_struct(&g_sym_FactW, NULL)))
    return NULL;
  if (! struct_allocate(&arguments->tag.data.struct_)) {
    list_delete_all(arguments);
    return NULL;
  }
  fact_w = arguments->tag.data.struct_.data;
  if (! facts_with_list(facts, &cursor, *spec))
    return NULL;
  if (! facts_with_cursor_next(&cursor, &fact))
    goto clean;
  if (! fact)
    goto ok;
  if (! fact_w_init_fact(fact_w, fact))
    goto clean;
  if (! env_eval_call_callable_args(env, callback, arguments, &tmp)) {
    goto clean;
  }
  facts_with_cursor_clean(&cursor);
 ok:
  list_delete_all(arguments);
  if (false) {
    err_write_1("env_facts_first_with: ");
    err_inspect_tag(&tmp);
    err_write_1("\n");
  }
  *dest = tmp;
  return dest;
 clean:
  facts_with_cursor_clean(&cursor);
  tag_clean(&tmp);
  list_delete_all(arguments);
  return NULL;
}

s_tag * env_facts_first_with_tags (s_env *env, s_facts *facts,
                                   s_tag *subject, s_tag *predicate,
                                   s_tag *object, s_callable *callback,
                                   s_tag *dest)
{
  s_list *arguments;
  s_facts_cursor cursor = {0};
  s_fact *fact = NULL;
  s_fact_w *fact_w = NULL;
  s_tag tmp = {0};
  assert(env);
  assert(facts);
  assert(subject);
  assert(predicate);
  assert(object);
  assert(callback);
  assert(dest);
  if (! (arguments = list_new_struct(&g_sym_FactW, NULL)))
    return NULL;
  if (! struct_allocate(&arguments->tag.data.struct_)) {
    list_delete_all(arguments);
    return NULL;
  }
  fact_w = arguments->tag.data.struct_.data;
  if (! facts_with_tags(facts, &cursor, subject, predicate, object))
    return NULL;
  if (! facts_cursor_next(&cursor, &fact))
    goto clean;
  if (! fact)
    goto ok;
  if (! fact_w_init_fact(fact_w, fact))
    goto clean;
  if (! env_eval_call_callable_args(env, callback, arguments, &tmp)) {
    goto clean;
  }
  facts_cursor_clean(&cursor);
 ok:
  list_delete_all(arguments);
  *dest = tmp;
  return dest;
 clean:
  facts_cursor_clean(&cursor);
  tag_clean(&tmp);
  list_delete_all(arguments);
  return NULL;
}

s_tag * env_facts_with (s_env *env, s_facts *facts, s_list **spec,
                        s_callable *callback, s_tag *dest)
{
  s_list *arguments;
  s_facts_with_cursor cursor = {0};
  s_fact *fact = NULL;
  s_fact_w *fact_w = NULL;
  s_tag tmp = {0};
  if (! (arguments = list_new_struct(&g_sym_FactW, NULL)))
    return NULL;
  if (! struct_allocate(&arguments->tag.data.struct_))
    return NULL;
  fact_w = arguments->tag.data.struct_.data;
  if (! facts_with_list(facts, &cursor, *spec))
    return NULL;
  while (1) {
    if (! facts_with_cursor_next(&cursor, &fact))
      goto clean;
    if (! fact)
      goto ok;
    tag_clean(&tmp);
    fact_w_init_fact(fact_w, fact);
    if (! env_eval_call_callable_args(env, callback, arguments, &tmp)) {
      fact_w_clean(fact_w);
      goto clean;
    }
    fact_w_clean(fact_w);
    fact_w_init(fact_w);
  }
  facts_with_cursor_clean(&cursor);
 ok:
  list_delete_all(arguments);
  *dest = tmp;
  return dest;
 clean:
  err_puts("env_facts_with: error");
  assert(! "env_facts_with: error");
  tag_clean(&tmp);
  fact_w_clean(fact_w);
  list_delete_all(arguments);
  return NULL;
}

s_tag * env_facts_with_macro (s_env *env, s_tag *facts_tag, s_tag *spec_tag,
                              s_tag *block_tag, s_tag *dest)
{
  s_facts_with_cursor cursor = {0};
  s_fact *fact = NULL;
  s_facts *facts;
  s_tag    facts_eval;
  s_list *spec = NULL;
  s_tag   spec_eval = {0};
  s_tag tmp = {0};
  if (! env_eval_tag(env, facts_tag, &facts_eval))
    return NULL;
  if (facts_eval.type != TAG_PTR) {
    err_puts("env_facts_with_macro: facts is not a Ptr");
    assert(! "env_facts_with_macro: facts is not a Ptr");
    return NULL;
  }
  facts = facts_eval.data.ptr.p;
  if (! env_eval_tag(env, spec_tag, &spec_eval))
    return NULL;
  if (spec_eval.type != TAG_LIST) {
    err_puts("env_facts_with_macro: spec is not a List");
    assert(! "env_facts_with_macro: spec is not a List");
    return NULL;
  }
  spec = spec_eval.data.list;
  if (! facts_with_list(facts, &cursor, spec))
    return NULL;
  while (1) {
    if (! facts_with_cursor_next(&cursor, &fact))
      goto clean;
    if (! fact)
      goto ok;
    tag_clean(&tmp);
    if (! env_eval_tag(env, block_tag, &tmp)) {
      goto clean;
    }
  }
  facts_with_cursor_clean(&cursor);
 ok:
  *dest = tmp;
  return dest;
 clean:
  err_puts("env_facts_with_macro: error");
  assert(! "env_facts_with_macro: error");
  tag_clean(&tmp);
  return NULL;
}

s_tag * env_facts_with_tags (s_env *env, s_facts *facts, s_tag *subject,
                             s_tag *predicate, s_tag *object,
                             s_callable *callback, s_tag *dest)
{
  s_list *arguments;
  s_facts_cursor cursor = {0};
  s_fact *fact = NULL;
  s_fact_w *fact_w = NULL;
  s_tag tmp = {0};
  if (! (arguments = list_new_struct(&g_sym_FactW, NULL)))
    return NULL;
  if (! struct_allocate(&arguments->tag.data.struct_))
    return NULL;
  fact_w = arguments->tag.data.struct_.data;
  if (! facts_with_tags(facts, &cursor, subject, predicate, object))
    return NULL;
  while (1) {
    if (! facts_cursor_next(&cursor, &fact))
      goto clean;
    if (! fact) {
      goto ok;
    }
    tag_clean(&tmp);
    fact_w_clean(fact_w);
    if (! fact_w_init_fact(fact_w, fact))
      goto clean;
    if (! env_eval_call_callable_args(env, callback, arguments, &tmp)) {
      fact_w_clean(fact_w);
      goto clean;
    }
  }
  facts_cursor_clean(&cursor);
 ok:
  list_delete_all(arguments);
  *dest = tmp;
  return dest;
 clean:
  facts_cursor_clean(&cursor);
  tag_clean(&tmp);
  fact_w_clean(fact_w);
  list_delete_all(arguments);
  return NULL;
}

s_tag * env_frames_get (s_env *env, const s_sym *name)
{
  s_tag *tag;
  if ((tag = frame_get(env->frame, name)) ||
      (tag = frame_get(env->global_frame, name)))
    return tag;
  return NULL;
}

s_env * env_global (void)
{
  return g_kc3_env;
}

void env_global_set (s_env *env)
{
  g_kc3_env = env;
}

s_tag * env_ident_get (s_env *env, const s_ident *ident, s_tag *dest)
{
  s_facts_with_cursor cursor;
  s_fact *fact;
  const s_sym *module;
  s_tag tag_ident;
  s_tag tag_is_a;
  s_tag tag_macro;
  s_tag tag_module;
  s_tag tag_special_operator;
  s_tag tag_sym;
  s_tag tag_symbol;
  s_tag tag_symbol_value;
  s_tag tag_var;
  s_tag tmp = {0};
  module = ident->module;
  if (! module) {
    if (! env_sym_search_modules(env, ident->sym, &module) ||
        ! module) {
      err_write_1("env_ident_get: symbol not found: ");
      err_inspect_sym(&ident->sym);
      err_write_1("\n");
      assert(! "env_ident_get: symbol not found");
      return NULL;
    }
  }
  if (! env_module_ensure_loaded(env, module))
    return NULL;
  tag_init_ident(&tag_ident, ident);
  tag_init_sym(  &tag_is_a, &g_sym_is_a);
  tag_init_sym(  &tag_macro, &g_sym_macro);
  tag_init_sym(  &tag_module, module);
  tag_init_sym(  &tag_special_operator, &g_sym_special_operator);
  tag_init_sym(  &tag_sym, ident->sym);
  tag_init_sym(  &tag_symbol, &g_sym_symbol);
  tag_init_sym(  &tag_symbol_value, &g_sym_symbol_value);
  tag_init_var(  &tag_var, &g_sym_Tag);
  if (! facts_find_fact_by_tags(env->facts, &tag_module, &tag_symbol,
                                &tag_ident, &fact) ||
      ! fact)
    return NULL;
  if (! facts_with(env->facts, &cursor, (t_facts_spec) {
        &tag_ident, &tag_symbol_value, &tag_var,
        NULL, NULL }))
    return NULL;
  if (! facts_with_cursor_next(&cursor, &fact)) {
    facts_with_cursor_clean(&cursor);
    return NULL;
  }
  if (! fact)
    return NULL;
  if (! tag_init_copy(&tmp, &tag_var)) {
    facts_with_cursor_clean(&cursor);
    return NULL;
  }
  facts_with_cursor_clean(&cursor);
  if (! facts_with(env->facts, &cursor, (t_facts_spec) {
        &tag_ident, &tag_is_a, &tag_special_operator, NULL, NULL}))
    return NULL;
  if (! facts_with_cursor_next(&cursor, &fact)) {
    facts_with_cursor_clean(&cursor);
    return NULL;
  }
  if (fact) {
    if (tmp.type == TAG_CALLABLE) {
      switch (tmp.data.callable->type) {
      case CALLABLE_CFN:
        tmp.data.callable->data.cfn.special_operator = true;
        break;
      case CALLABLE_FN:
        tmp.data.callable->data.fn.special_operator = true;
        break;
      case CALLABLE_VOID:
        err_puts("env_ident_get: CALLABLE_VOID");
        assert(! "env_ident_get: CALLABLE_VOID");
        facts_with_cursor_clean(&cursor);
        return NULL;
      }
    }
  }
  facts_with_cursor_clean(&cursor);
  *dest = tmp;
  return dest;
}

bool * env_ident_is_special_operator (s_env *env,
                                      const s_ident *ident,
                                      bool *dest)
{
  s_fact *fact;
  s_tag tag_ident;
  s_tag tag_is_a;
  s_tag tag_special_operator;
  assert(env);
  assert(ident);
  tag_ident.type = TAG_IDENT;
  if (! env_ident_resolve_module(env, ident, &tag_ident.data.ident))
    return NULL;
  tag_init_sym(&tag_is_a, &g_sym_is_a);
  tag_init_sym(&tag_special_operator, &g_sym_special_operator);
  if (! facts_find_fact_by_tags(env->facts, &tag_ident, &tag_is_a,
                                &tag_special_operator, &fact))
    return NULL;
  *dest = fact ? true : false;
  return dest;
}

s_ident * env_ident_resolve_module (s_env *env,
                                    const s_ident *ident,
                                    s_ident *dest)
{
  s_ident tmp = {0};
  assert(env);
  assert(ident);
  tmp = *ident;
  if (! tmp.module) {
    if (! env_sym_search_modules(env, tmp.sym, &tmp.module)) {
      err_puts("env_ident_resolve_module: env_sym_search_modules");
      assert(! "env_ident_resolve_module: env_sym_search_modules");
      return NULL;
    }
    if (! tmp.module) {
      if (false) {
        err_puts("env_ident_resolve_module: env_sym_search_modules"
                 " -> NULL");
        assert(!("env_ident_resolve_module: env_sym_search_modules"
                 " -> NULL"));
      }
      if (! env->current_defmodule) {
        err_puts("env_ident_resolve_module: env current defmodule is"
                 " NULL");
        assert(!("env_ident_resolve_module: env current defmodule is"
                 " NULL"));
        return NULL;
      }
      tmp.module = env->current_defmodule;
    }
  }
  assert(tmp.module);
  *dest = tmp;
  return dest;
}

s_env * env_init (s_env *env, int *argc, char ***argv)
{
  s_str path;
  if (! env)
    env = g_kc3_env;
  if (! env)
    env = g_kc3_env = alloc(sizeof(s_env));
  if (! env)
    return NULL;
  env->trace = false;
  if (! env_init_args(env, argc, argv))
    return NULL;
  sym_init_g_sym();
  env->error_handler = NULL;
  if (! env_init_toplevel(env))
    return NULL;
  if (! env_init_globals(env))
    return NULL;
  if (! (env->in = buf_new_alloc(BUF_SIZE)))
    return NULL;
  buf_file_open_r(env->in, stdin);
  if (! (env->out = buf_new_alloc(BUF_SIZE)))
    return NULL;
  buf_file_open_w(env->out, stdout);
  if (! (env->err = buf_new_alloc(BUF_SIZE)))
    return NULL;
  buf_file_open_w(env->err, stderr);
  env->facts = facts_new();
  env->path = list_new_str_1
    (NULL, "./", list_new_str_1
     (NULL, "../", list_new_str_1
      (NULL, "../Resources/", list_new_str_1
       (NULL, "../../", list_new_str_1
        (NULL, "../../../", list_new_str_1
         (NULL, "../../../../", list_new_str_1
          (NULL, "../../../../../", list_new_str_1
           (NULL, "../../../../../../", NULL))))))));
  str_init_1(&path, NULL, "lib/kc3/0.1/");
  if (! (env->module_path = alloc(sizeof(s_str))))
    return NULL;
  if (! file_search(&path, &g_sym_x, env->module_path)) {
    err_puts("env_init: lib/kc3/0.1 not found in module path");
    assert(! "env_init: lib/kc3/0.1 not found in module path");
    return NULL;
  }
  env->current_defmodule = &g_sym_KC3;
  env->search_modules_default = list_new_sym(&g_sym_KC3, NULL);
  env->search_modules = env->search_modules_default;
  env->quote_level = 0;
  env->unquote_level = 0;
  env->ops = ops_new();
  if (! env_module_load(env, &g_sym_KC3)) {
    env_clean(env);
    return NULL;
  }
  if (! env_module_load(env, sym_1("Init"))) {
    env_clean(env);
    return NULL;
  }
  return env;
}

s_env * env_init_args (s_env *env, int *argc, char ***argv)
{
  s32 argc_prev = 0;
  s_str argv0;
  assert(env);
  if (argc && argv && *argc && *argv) {
    env->argc = (*argc)--;
    env->argv = (*argv)++;
    str_init_1(&argv0, NULL, env->argv[0]);
    if (! (env->argv0_dir = alloc(sizeof(s_str))))
      return NULL;
    file_dirname(&argv0, env->argv0_dir);
    while (*argc > 0 && *argc != argc_prev) {
      argc_prev = *argc;
      if (**argv && ! strcmp(**argv, "--trace")) {
        env->trace = true;
        (*argc)--;
        (*argv)++;
      }
    }
    return env;
  }
  env->argc = 0;
  env->argv = NULL;
  env->argv0_dir = str_new_1(NULL, "./");
  return env;
}

s_env * env_init_globals (s_env *env)
{
  s_tag *file_dir;
  s_tag *file_path;
  s_tag *ncpu;
  if (! (env->read_time_frame = frame_new(NULL, NULL)))
    return NULL;
  if (! (file_dir = frame_binding_new(env->read_time_frame,
                                      &g_sym___DIR__)))
    return NULL;
  if (! (file_path = frame_binding_new(env->read_time_frame,
                                       &g_sym___FILE__)))
    return NULL;
  file_dir->type = TAG_STR;
  if (! file_pwd(&file_dir->data.str))
    return NULL;
  if (! tag_init_str_1(file_path, NULL, "stdin"))
    return NULL;
  if (! (env->global_frame = frame_new(env->read_time_frame, NULL)))
    return NULL;
  if (! (ncpu = frame_binding_new(env->read_time_frame,
                                  &g_sym_ncpu)))
    return NULL;
#if HAVE_PTHREAD
# if (defined(__OpenBSD__))
  {
    s32 mib[2];
    s32 hw_ncpu;
    uw len;
    mib[0] = CTL_HW;
    mib[1] = HW_NCPU;
    len = sizeof(hw_ncpu);
    if (sysctl(mib, 2, &hw_ncpu, &len, NULL, 0) == -1){
      err_puts("env_init_globals: sysctl(hw.ncpu)");
      assert(! "env_init_globals: sysctl(hw.ncpu)");
      return NULL;
    }
    tag_init_u8(ncpu, hw_ncpu);
  }
# else
  tag_init_u8(ncpu, get_nprocs());
# endif
#else
  tag_init_u8(ncpu, 1);
#endif
  return env;

}

s_env * env_init_toplevel (s_env *env)
{
  env->frame = frame_new(NULL, NULL);
  return env;
}

s_tag * env_kc3_def (s_env *env, const s_call *call, s_tag *dest)
{
  s_ident *ident;
  s_tag tag_ident;
  s_tag tag_value;
  (void) env;
  assert(env);
  assert(call);
  assert(dest);
  if (call->ident.sym != &g_sym__equal ||
      call->arguments->tag.type != TAG_IDENT ||
      ! list_next(call->arguments) ||
      list_next(list_next(call->arguments))) {
    err_puts("env_kc3_def: invalid assignment: expected Ident = value");
    assert(! "env_kc3_def: invalid assignment: expected Ident = value");
    return NULL;
  }
  ident = &call->arguments->tag.data.ident;
  tag_ident.type = TAG_IDENT;
  tag_ident.data.ident.sym = ident->sym;
  if (ident->module)
    tag_ident.data.ident.module = ident->module;
  else
    tag_ident.data.ident.module = env->current_defmodule;
  if (! env_eval_tag(env, &list_next(call->arguments)->tag,
                     &tag_value)) {
    err_puts("env_kc3_def: env_eval_tag");
    assert(! "env_kc3_def: env_eval_tag");
    return NULL;
  }
  if (! env_def(env, ident, &tag_value)) {
    tag_clean(&tag_value);
    return NULL;
  }
  tag_clean(&tag_value);
  tag_init_ident(dest, &tag_ident.data.ident);
  return dest;
}

s_tag * env_let (s_env *env, s_tag *vars, s_tag *tag,
                 s_tag *dest)
{
  s_frame frame;
  uw i;
  s_map *map;
  s_tag tmp = {0};
  assert(env);
  assert(vars);
  assert(tag);
  assert(dest);
  if (! frame_init(&frame, env->frame, NULL))
    return NULL;
  env->frame = &frame;
  if (! env_eval_tag(env, vars, &tmp)) {
    env->frame = frame_clean(&frame);
    return NULL;
  }
  switch(tmp.type) {
  case TAG_MAP:
    map = &tmp.data.map;
    break;
  case TAG_STRUCT:
    map = &tmp.data.struct_.type->map;
    // FIXME
    break;
  default:
    tag_clean(&tmp);
    err_write_1("env_let: unsupported associative tag type: ");
    err_inspect_tag_type(tag->type);
    err_write_1(": ");
    err_inspect_tag(tag);
    err_write_1("\n");
    assert(! "env_let: unsupported associative tag type");
    env->frame = frame_clean(&frame);
    return NULL;
  }
  i = 0;
  while (i < map->count) {
    if (map->key[i].type != TAG_SYM) {
      tag_clean(&tmp);
      err_write_1("env_let: binding key is not a symbol: ");
      err_inspect_tag(map->key + i);
      err_write_1("\n");
      assert(! "env_let: binding key is not a symbol");
      env->frame = frame_clean(&frame);
      return NULL;
    }
    if (! frame_binding_new_copy(&frame,
                                 map->key[i].data.sym,
                                 map->value + i)) {
      tag_clean(&tmp);
      env->frame = frame_clean(&frame);
      return NULL;
    }
    i++;
  }
  if (! env_eval_tag(env, tag, dest)) {
    tag_clean(&tmp);
    env->frame = frame_clean(&frame);
    return NULL;
  }
  tag_clean(&tmp);
  env->frame = frame_clean(&frame);
  return dest;
}

bool env_load (s_env *env, const s_str *path)
{
  s_buf buf;
  s_tag *file_dir;
  s_tag  file_dir_save;
  s_tag *file_path;
  s_tag  file_path_save;
  s_tag load_time = {0};
  s_tag now = {0};
  sw r;
  s_tag tag = {0};
  s_tag tmp = {0};
  assert(env);
  assert(path);
  if (env->trace) {
    err_write_1("env_load: ");
    err_inspect_str(path);
    err_write_1("\n");
  }
  if (! buf_init_alloc(&buf, BUF_SIZE))
    return false;
  if (! buf_getc_open_r(&buf, path->ptr.pchar)) {
    buf_clean(&buf);
    return false;
  }
  file_dir = frame_get_w(env->global_frame, &g_sym___DIR__);
  file_dir_save = *file_dir;
  file_path = frame_get_w(env->global_frame, &g_sym___FILE__);
  file_path_save = *file_path;
  if (! file_dirname(path, &file_dir->data.str))
    goto ko;
  tag_init_str(file_path, NULL, path->size, path->ptr.pchar);
  if (false) {
    err_write_1("env_load: __DIR__ = ");
    err_inspect_tag(file_dir);
    err_write_1("\n");
  }
  if (false) {
    err_write_1("env_load: __FILE__ = ");
    err_inspect_tag(file_path);
    err_write_1("\n");
  }
  while (1) {
    if ((r = buf_parse_comments(&buf)) < 0)
      break;
    if ((r = buf_ignore_spaces(&buf)) < 0)
      break;
    if ((r = buf_parse_tag(&buf, &tag)) < 0)
      break;
    if (! r)
      continue;
    if (! env_eval_tag(env, &tag, &tmp)) {
      err_write_1("env_load: env_eval_tag: ");
      err_inspect_tag(&tag);
      err_write_1("\n");
      tag_clean(&tag);
      goto ko;
    }
    tag_clean(&tmp);
    tag_clean(&tag);
  }
  tag_clean(file_dir);
  *file_dir = file_dir_save;
  *file_path = file_path_save;
  buf_getc_close(&buf);
  buf_clean(&buf);
  tag = (s_tag) {0};
  tag.type = TAG_STR;
  tag.data.str = *path;
  tag_init_time_now(&now);
  tag_init_sym(&load_time, &g_sym_load_time);
  facts_replace_tags(env->facts, &tag, &load_time, &now);
  return true;
 ko:
  tag_clean(file_dir);
  *file_dir = file_dir_save;
  *file_path = file_path_save;
  buf_getc_close(&buf);
  buf_clean(&buf);
  return false;
}

bool env_maybe_reload (s_env *env, const s_str *path)
{
  s_facts_cursor cursor;
  s_fact *fact = NULL;
  s_tag load_time = {0};
  s_tag load_time_sym = {0};
  s_tag mtime = {0};
  s_tag path_tag = {0};
  bool r;
  path_tag.type = TAG_STR;
  path_tag.data.str = *path;
  tag_init_sym(&load_time_sym, &g_sym_load_time);
  tag_init_var(&load_time, &g_sym_Time);
  if (! facts_with_tags(env->facts, &cursor, &path_tag, &load_time_sym,
                        &load_time))
    return false;
  if (! facts_cursor_next(&cursor, &fact))
    return false;
  if (! fact) {
    err_write_1("env_maybe_reload: no load time for ");
    err_inspect_str(path);
    err_write_1("\n");
    assert(! "env_maybe_reload: no load time");
    return false;
  }
  if (load_time.type != TAG_TIME)
    abort();
  mtime.type = TAG_TIME;
  if (! file_mtime(path, &mtime.data.time)) {
    facts_cursor_clean(&cursor);
    return false;
  }
  r = true;
  if (compare_tag(&load_time, &mtime) == COMPARE_LT)
    r = env_load(env, path);
  facts_cursor_clean(&cursor);
  return r;
}

void env_longjmp (s_env *env, jmp_buf *jmp_buf)
{
  if (env->unwind_protect && *jmp_buf > env->unwind_protect->buf) {
    s_unwind_protect *unwind_protect = env->unwind_protect;
    while (unwind_protect->next && *jmp_buf > unwind_protect->next->buf) {
      unwind_protect->jmp = &unwind_protect->next->buf;
      unwind_protect = unwind_protect->next;
    }
    unwind_protect->jmp = jmp_buf;
    longjmp(env->unwind_protect->buf, 1);
  }
  longjmp(*jmp_buf, 1);
}

const s_sym ** env_module (s_env *env, const s_sym **dest)
{
  assert(env);
  assert(dest);
  assert(env->current_defmodule);
  *dest = env->current_defmodule;
  return dest;
}

bool env_module_ensure_loaded (s_env *env, const s_sym *module)
{
  bool b;
  s_fact *fact;
  s_tag tag_module_name;
  s_tag tag_is_a;
  s_tag tag_module;
  if (! env_module_is_loading(env, module, &b))
    return false;
  if (b)
    return true;
  tag_init_sym(&tag_is_a, &g_sym_is_a);
  tag_init_sym(&tag_module, &g_sym_module);
  tag_init_sym(&tag_module_name, module);
  if (! facts_find_fact_by_tags(env->facts, &tag_module_name,
                                &tag_is_a, &tag_module, &fact)) {
    err_puts("env_module_ensure_loaded: facts_find_fact_by_tags");
    assert(! "env_module_ensure_loaded: facts_find_fact_by_tags");
    return false;
  }
  if (! fact) {
    if (! env_module_load(env, module)) {
      err_write_1("env_module_ensure_loaded: module not found: ");
      err_puts(module->str.ptr.pchar);
      assert(! "env_module_ensure_loaded: module not found");
      return false;
    }
    return true;
  }
  if (! env_module_maybe_reload(env, module)) {
    err_puts("env_module_ensure_loaded: env_module_maybe_reload");
    assert(! "env_module_ensure_loaded: env_module_maybe_reload");
  }
  return true;
}

bool * env_module_has_ident (s_env *env, const s_sym *module,
                             const s_ident *ident, bool *dest)
{
  s_facts_with_cursor cursor;
  s_fact *fact = NULL;
  s_tag tag_ident;
  s_tag tag_module_name;
  s_tag tag_op;
  s_tag tag_sym_value;
  s_tag tag_sym_sym;
  s_tag tag_symbol;
  s_tag tag_var;
  tag_init_ident(&tag_ident, ident);
  tag_init_sym(  &tag_module_name, module);
  tag_init_sym(  &tag_op, &g_sym_op);
  tag_init_sym(  &tag_sym_value, ident->sym);
  tag_init_sym(  &tag_symbol, &g_sym_symbol);
  if (! facts_find_fact_by_tags(env->facts, &tag_module_name,
                                &tag_symbol, &tag_ident, &fact)) {
    err_puts("env_module_has_ident: facts_find_fact_by_tags 1");
    assert(! "env_module_has_ident: facts_find_fact_by_tags 1");
    return NULL;
  }
  if (fact) {
    *dest = true;
    return dest;
  }
  if (! facts_find_fact_by_tags(env->facts, &tag_module_name,
                                &tag_op, &tag_ident, &fact)) {
    err_puts("env_module_has_ident: facts_find_fact_by_tags 2");
    assert(! "env_module_has_ident: facts_find_fact_by_tags 2");
    return NULL;
  }
  if (fact) {
    *dest = true;
    return dest;
  }
  tag_init_sym(&tag_sym_sym, &g_sym_sym);
  tag_init_var(&tag_var, &g_sym_Ident);
  if (! facts_with(env->facts, &cursor, (t_facts_spec) {
        &tag_module_name, &tag_op, &tag_var, NULL,
        &tag_var, &tag_sym_sym, &tag_sym_value, NULL, NULL})) {
    err_puts("env_module_has_ident: facts_with");
    assert(! "env_module_has_ident: facts_with");
    return NULL;
  }
  if (! facts_with_cursor_next(&cursor, &fact)) {
    err_puts("env_module_has_ident: facts_with_cursor_next");
    assert(! "env_module_has_ident: facts_with_cursor_next");
    return NULL;
  }
  *dest = fact ? true : false;
  facts_with_cursor_clean(&cursor);
  return dest;
}

bool * env_module_has_symbol (s_env *env, const s_sym *module,
                              const s_sym *sym, bool *dest)
{
  s_ident ident;
  ident.module = module;
  ident.sym = sym;
  return env_module_has_ident(env, module, &ident, dest);
}

bool * env_module_is_loading (s_env *env, const s_sym *module,
                              bool *dest)
{
  s_fact *fact;
  s_tag tag_module;
  s_tag tag_is_loading;
  s_tag tag_true;
  assert(env);
  assert(module);
  tag_init_sym(&tag_module, module);
  tag_init_sym(&tag_is_loading, &g_sym_is_loading);
  tag_init_bool(&tag_true, true);
  if (! facts_find_fact_by_tags(env->facts, &tag_module,
                                &tag_is_loading, &tag_true,
                                &fact))
    return NULL;
  *dest = fact ? true : false;
  return dest;
}

bool env_module_is_loading_set (s_env *env, const s_sym *module,
                                bool is_loading)
{
  bool b;
  s_tag tag_module;
  s_tag tag_is_loading;
  s_tag tag_true;
  assert(env);
  assert(module);
  tag_init_sym(&tag_module, module);
  tag_init_sym(&tag_is_loading, &g_sym_is_loading);
  tag_init_bool(&tag_true, true);
  if (is_loading) {
    if (! facts_replace_tags(env->facts, &tag_module, &tag_is_loading,
                             &tag_true))
      return false;
  }
  else
    if (! facts_remove_fact_tags(env->facts, &tag_module,
                                 &tag_is_loading, &tag_true, &b) ||
        ! b)
      return false;
  return true;
}

bool env_module_load (s_env *env, const s_sym *module)
{
  bool b;
  s_str path = {0};
  s_tag tag_module_name;
  s_tag tag_load_time;
  s_tag tag_time;
  s_facts_transaction transaction;
  assert(env);
  assert(module);
  if (! env_module_is_loading(env, module, &b))
    return false;
  if (b)
    return true;
  facts_transaction_start(env->facts, &transaction);
  if (! env_module_is_loading_set(env, module, true))
    goto rollback;
  if (module_path(module, env->module_path, KC3_EXT, &path) &&
      file_access(&path, &g_sym_r)) {
    tag_init_time_now(&tag_time);
    if (! env_load(env, &path)) {
      err_write_1("env_module_load: ");
      err_inspect_sym(&module);
      err_puts(": env_load");
      str_clean(&path);
      goto rollback;
    }
  }
  else {
    str_clean(&path);
    if (! module_path(module, env->module_path, FACTS_EXT, &path)) {
      err_write_1("env_module_load: ");
      err_write_1(module->str.ptr.pchar);
      err_puts(": module_path");
      goto rollback;
    }
    if (! file_access(&path, &g_sym_r))
      goto rollback;
    tag_init_time_now(&tag_time);
    if (facts_load_file(env->facts, &path) < 0) {
      err_write_1("env_module_load: ");
      err_write_1(module->str.ptr.pchar);
      err_puts(": facts_load_file");
      str_clean(&path);
      tag_clean(&tag_time);
      goto rollback;
    }
  }
  str_clean(&path);
  tag_init_sym(&tag_module_name, module);
  tag_init_sym(&tag_load_time, &g_sym_load_time);
  if (! facts_replace_tags(env->facts, &tag_module_name,
                           &tag_load_time, &tag_time))
    goto rollback;
  tag_clean(&tag_time);
  env_module_is_loading_set(env, module, false);
  facts_transaction_end(env->facts, &transaction);
  return true;
 rollback:
  if (! facts_transaction_rollback(env->facts, &transaction)) {
    abort();
    return false;
  }
  return false;
}

const s_time ** env_module_load_time (s_env *env, const s_sym *module,
                                      const s_time **dest)
{
  s_facts_with_cursor cursor;
  s_fact *fact;
  s_tag tag_module_name;
  s_tag tag_load_time;
  s_tag tag_time_var;
  tag_init_sym(&tag_module_name, module);
  tag_init_sym(&tag_load_time, &g_sym_load_time);
  tag_init_var(&tag_time_var, &g_sym_Time);
  if (! facts_with(env->facts, &cursor, (t_facts_spec) {
        &tag_module_name, &tag_load_time, &tag_time_var, NULL, NULL }))
    return NULL;
  if (! facts_with_cursor_next(&cursor, &fact)) {
    facts_with_cursor_clean(&cursor);
    return NULL;
  }
  if (! fact) {
    *dest = NULL;
    facts_with_cursor_clean(&cursor);
    return dest;
  }
  if (fact->object->type != TAG_TIME) {
    err_write_1("env_module_load_time: module ");
    err_inspect_sym(&module);
    err_puts(" load time is not a %Time{}");
    assert(! "env_module_load_time: module load time is not a %Time{}");
    facts_with_cursor_clean(&cursor);
    return NULL;
  }
  *dest = &fact->object->data.time;
  facts_with_cursor_clean(&cursor);
  return dest;
}

bool env_module_maybe_reload (s_env *env, const s_sym *module)
{
  const s_time *load_time = NULL;
  s_str path;
  bool r = false;
  s_time mtime;
  if (module_path(module, env->module_path, KC3_EXT, &path)) {
    if (file_access(&path, &g_sym_r))
      r = true;
    else
      str_clean(&path);
  }
  if (!r && module_path(module, env->module_path, FACTS_EXT, &path)) {
    if (file_access(&path, &g_sym_r))
      r = true;
    else
      str_clean(&path);
  }
  if (! r)
    return true;
  if (! env_module_load_time(env, module, &load_time) ||
      ! load_time) {
    str_clean(&path);
    return env_module_load(env, module);
  }
  if (! file_mtime(&path, &mtime)) {
    str_clean(&path);
    return false;
  }
  str_clean(&path);
  if (compare_time(load_time, &mtime) < 0)
    r = env_module_load(env, module);
  return r;
}

s_list ** env_module_search_modules (s_env *env,
                                     const s_sym * const *module,
                                     s_list **dest)
{
  s_list *tmp = {0};
  s_list *tmp2 = {0};
  assert(env);
  (void) env;
  if (! module || ! *module) {
    err_puts("env_module_search_modules: NULL module");
    assert(! "env_module_search_modules: NULL module");
    return NULL;
  }
  if (! (tmp = list_new_sym(&g_sym_KC3, NULL)))
    return NULL;
  if (*module == &g_sym_KC3) {
    *dest = tmp;
    return dest;
  }
  tmp2 = list_new_sym(*module, tmp);
  if (! tmp2) {
    list_delete(tmp);
    return NULL;
  }
  *dest = tmp2;
  return dest;
}

s_tag * env_or (s_env *env, s_tag *a, s_tag *b, s_tag *dest)
{
  s_tag eval;
  bool p;
  const s_sym *sym_Bool = &g_sym_Bool;
  assert(env);
  assert(a);
  assert(b);
  assert(dest);
  if (! env_eval_tag(env, a, &eval))
    return NULL;
  if (! bool_init_cast(&p, &sym_Bool, &eval)) {
    tag_clean(&eval);
    return NULL;
  }
  if (p) {
    *dest = eval;
    return dest;
  }
  tag_clean(&eval);
  if (! env_eval_tag(env, b, &eval))
    return NULL;
  if (! bool_init_cast(&p, &sym_Bool, &eval)) {
    tag_clean(&eval);
    return NULL;
  }
  if (p) {
    *dest = eval;
    return dest;
  }
  tag_clean(&eval);
  return tag_init_bool(dest, false);
}

void env_pop_error_handler (s_env *env)
{
  if (env->error_handler)
    env->error_handler = error_handler_delete(env->error_handler);
}

void env_pop_unwind_protect (s_env *env)
{
  if (env->unwind_protect)
    env->unwind_protect = env->unwind_protect->next;
}

void env_push_error_handler (s_env *env, s_error_handler *error_handler)
{
  tag_init_void(&error_handler->tag);
  error_handler->next = env->error_handler;
  env->error_handler = error_handler;
}

void env_push_unwind_protect (s_env *env,
                              s_unwind_protect *unwind_protect)
{
  unwind_protect->next = env->unwind_protect;
  env->unwind_protect = unwind_protect;
}

s_list ** env_search_modules (s_env *env, s_list **dest)
{
  assert(env);
  assert(dest);
  assert(env->search_modules);
  assert(env->search_modules->tag.type == TAG_SYM);
  return list_init_copy(dest, &env->search_modules);
}

s_list ** env_stacktrace (s_env *env, s_list **dest)
{
  assert(env);
  assert(dest);
  *dest = list_new_copy(env->stacktrace);
  return dest;
}

bool env_sym_search_modules (s_env *env, const s_sym *sym,
                             const s_sym **dest)
{
  bool b;
  const s_sym *module;
  s_list *search_module;
  assert(env);
  assert(sym);
  assert(dest);
  search_module = env->search_modules;
  if (! search_module) {
    err_puts("env_sym_search_modules: env->search_modules = NULL");
    assert(! "env_sym_search_modules: env->search_modules = NULL");
    return false;
  }
  while (search_module) {
    if (search_module->tag.type != TAG_SYM ||
        ! search_module->tag.data.sym) {
      err_write_1("env_sym_search_modules: ");
      err_inspect_sym(&sym);
      err_puts(": invalid env->search_modules");
      assert(! "env_sym_search_modules: invalid env->search_modules");
      return false;
    }
    module = search_module->tag.data.sym;
    if (! env_module_has_symbol(env, module, sym, &b)) {
      err_puts("env_sym_search_modules: env_module_has_symbol");
      assert(! "env_sym_search_modules: env_module_has_symbol");
      return false;
    }
    if (b) {
      *dest = module;
      if (false) {
        err_write_1("env_sym_search_modules: ");
        err_inspect_sym(&sym);
        err_write_1(": search_module: ");
        err_inspect_sym(&module);
        err_write_1(" -> Ok\n");
      }
      return true;
    }
    if (false) {
      err_write_1("env_sym_search_modules: ");
      err_inspect_sym(&sym);
      err_write_1(": search_module: ");
      err_inspect_sym(&module);
      err_write_1(" ");
      err_inspect_list(search_module);
      err_puts(" -> not found");
    }
    search_module = list_next(search_module);
  }
  if (false) {
    err_write_1("env_sym_search_modules: ");
    err_inspect_sym(&sym);
    err_write_1(": search_module: ");
    err_inspect_list(env->search_modules);
    err_write_1(" -> false\n");
  }
  *dest = NULL;
  return true;
}

u8 env_special_operator_arity (s_env *env, const s_ident *ident)
{
  u8 arity;
  s_facts_cursor cursor;
  s_fact *fact;
  s_tag tag_arity;
  s_tag tag_ident;
  s_tag tag_var;
  assert(env);
  assert(ident);
  tag_ident.type = TAG_IDENT;
  env_ident_resolve_module(env, ident, &tag_ident.data.ident);
  tag_init_sym(  &tag_arity, &g_sym_arity);
  tag_init_var(  &tag_var, &g_sym_U8);
  if (! facts_with_tags(env->facts, &cursor,
                        &tag_ident, &tag_arity, &tag_var))
    return 0;
  if (! facts_cursor_next(&cursor, &fact))
    return 0;
  if (fact) {
    if (tag_var.type != TAG_U8 || ! tag_var.data.u8) {
      err_write_1("env_special_operator_arity: "
                  "invalid arity for special operator ");
      err_inspect_ident(&tag_ident.data.ident);
      err_write_1("\n");
      facts_cursor_clean(&cursor);
      return 0;
    }
    arity = tag_var.data.u8;
    facts_cursor_clean(&cursor);
    return arity;
  }
  facts_cursor_clean(&cursor);
  err_write_1("env_special_operator_arity: "
              "arity not found for special operator ");
  err_inspect_ident(&tag_ident.data.ident);
  err_write_1("\n");
  return 0;
}

bool * env_struct_type_exists (s_env *env, const s_sym *module,
                               bool *dest)
{
  s_facts_cursor cursor;
  s_fact *fact;
  s_tag tag_struct_type;
  s_tag tag_module;
  s_tag tag_var;
  assert(env);
  assert(module);
  assert(dest);
  tag_init_sym(&tag_module, module);
  tag_init_sym(&tag_struct_type, &g_sym_struct_type);
  tag_init_var(&tag_var, &g_sym_Tag);
  if (! env_module_maybe_reload(env, module))
    return NULL;
  if (! facts_with_tags(env->facts, &cursor, &tag_module,
                        &tag_struct_type, &tag_var))
    return NULL;
  if (! facts_cursor_next(&cursor, &fact))
    return NULL;
  *dest = fact ? true : false;
  facts_cursor_clean(&cursor);
  return dest;
}

s_struct_type ** env_struct_type_find (s_env *env,
                                       const s_sym *module,
                                       s_struct_type **dest)
{
  s_facts_with_cursor cursor;
  s_fact *found;
  s_tag tag_struct_type;
  s_tag tag_module;
  s_tag tag_var;
  const s_sym *type;
  assert(env);
  assert(module);
  tag_init_sym(&tag_module, module);
  tag_init_sym(&tag_struct_type, &g_sym_struct_type);
  tag_init_var(&tag_var, &g_sym_Tag);
  if (! env_module_maybe_reload(env, module)) {
    err_write_1("env_struct_type_find: env_module_maybe_reload(");
    err_inspect_sym(&module);
    err_puts(")");
    assert(! "env_struct_type_find: env_module_maybe_reload");
    return NULL;
  }
  if (! facts_with(env->facts, &cursor, (t_facts_spec) {
                   &tag_module, &tag_struct_type, &tag_var,
                   NULL, NULL })) {
    err_write_1("env_struct_type_find: facts_with(");
    err_inspect_sym(&module);
    err_puts(", :struct_type, ?)");
    assert(! "env_struct_type_find: facts_with");
    return NULL;
  }
  if (! facts_with_cursor_next(&cursor, &found)) {
    err_puts("env_struct_type_find: facts_with_cursor_next");
    assert(! "env_struct_type_find: facts_with_cursor_next");
    facts_with_cursor_clean(&cursor);
    return NULL;
  }
  if (! found) {
    facts_with_cursor_clean(&cursor);
    *dest = NULL;
    return dest;
  }
  if (found->object->type != TAG_STRUCT_TYPE) {
    tag_type(found->object, &type);
    err_write_1("env_struct_type_find: module ");
    err_inspect_sym(&module);
    err_write_1(" :struct_type is actually a ");
    err_inspect_sym(&type);
    err_write_1("\n");
    assert(! "env_struct_type_find: invalid struct_type");
    facts_with_cursor_clean(&cursor);
    return NULL;
  }
  *dest = &found->object->data.struct_type;
  facts_with_cursor_clean(&cursor);
  return dest;
}

f_clean env_struct_type_get_clean (s_env *env, const s_sym *module)
{
  s_facts_with_cursor cursor;
  s_fact *found;
  s_tag tag_clean;
  s_tag tag_module;
  s_tag tag_var;
  f_clean tmp = {0};
  const s_sym *type;
  tag_init_sym(&tag_module, module);
  tag_init_sym(&tag_clean, &g_sym_clean);
  tag_init_var(&tag_var, &g_sym_Tag);
  facts_with(env->facts, &cursor, (t_facts_spec) {
      &tag_module, &tag_clean, &tag_var, NULL, NULL });
  if (! facts_with_cursor_next(&cursor, &found))
    return NULL;
  if (! found) {
    facts_with_cursor_clean(&cursor);
    return NULL;
  }
  if (found->object->type != TAG_CALLABLE ||
      found->object->data.callable->type != CALLABLE_CFN) {
    tag_type(found->object, &type);
    err_write_1("env_struct_type_get_clean: ");
    err_inspect_sym(&module);
    err_write_1(": clean is actually a ");
    err_inspect_sym(&type);
    err_write_1(", it should be a Cfn.\n");
    assert(! "env_struct_type_get_clean: invalid object");
    facts_with_cursor_clean(&cursor);
    return NULL;
  }
  if (found->object->data.callable->data.cfn.arity != 1) {
    err_write_1("env_struct_type_get_clean: ");
    err_inspect_sym(&module);
    err_write_1(": clean arity is ");
    err_inspect_u8(&found->object->data.callable->data.cfn.arity);
    err_write_1(", it should be 1.\n");
    assert(! "env_struct_type_get_clean: invalid arity");
    facts_with_cursor_clean(&cursor);
    return NULL;
  }
  tmp = (f_clean) found->object->data.callable->data.cfn.ptr.f;
  facts_with_cursor_clean(&cursor);
  return tmp;
}

s_list ** env_struct_type_get_spec (s_env *env,
                                    const s_sym *module,
                                    s_list **dest)
{
  s_fact *found;
  s_tag tag_defstruct;
  s_tag tag_module;
  s_tag tag_var;
  s_tag tmp = {0};
  assert(env);
  assert(module);
  assert(dest);
  tag_init_sym(&tag_defstruct, &g_sym_defstruct);
  tag_init_sym(&tag_module, module);
  tag_init_var(&tag_var, &g_sym_Tag);
  if (! facts_find_fact_by_tags(env->facts, &tag_module,
                                &tag_defstruct, &tag_var, &found))
    return NULL;
  if (! found) {
    err_write_1("env_struct_type_get_spec: ");
    err_inspect_sym(&module);
    err_puts(": defstruct not found");
    assert(! "env_struct_type_get_spec: defstruct not found");
    return NULL;
  }
  if (! env_eval_tag(env, &tag_var, &tmp))
    return NULL;
  if (tmp.type != TAG_LIST ||
      ! list_is_plist(tmp.data.list)) {
    err_write_1("env_get_struct_type_spec: module ");
    err_write_1(module->str.ptr.pchar);
    err_puts(" has a defstruct that is not a property list");
    tag_clean(&tmp);
    return NULL;
  }
  *dest = tmp.data.list;
  return dest;
}

bool * env_struct_type_has_spec (s_env *env, const s_sym *module,
                                 bool *dest)
{
  s_facts_cursor cursor;
  s_fact *fact;
  s_tag tag_defstruct;
  s_tag tag_module;
  s_tag tag_var;
  assert(env);
  assert(module);
  tag_init_sym(&tag_defstruct, &g_sym_defstruct);
  tag_init_sym(&tag_module, module);
  tag_init_var(&tag_var, &g_sym_Tag);
  if (! facts_with_tags(env->facts, &cursor, &tag_module,
                        &tag_defstruct, &tag_var))
    return NULL;
  if (! facts_cursor_next(&cursor, &fact))
    return NULL;
  facts_cursor_clean(&cursor);
  *dest = fact ? true : false;
  return dest;
}

bool env_tag_ident_is_bound (s_env *env, const s_tag *tag)
{
  s_tag tmp = {0};
  assert(tag);
  assert(tag->type == TAG_IDENT);
  return tag->type == TAG_IDENT &&
    (env_frames_get(env, tag->data.ident.sym) ||
     env_ident_get(env, &tag->data.ident, &tmp));
}

s_tag * env_unwind_protect (s_env *env, s_tag *protected, s_block *cleanup,
                            s_tag *dest)
{
  s_tag tmp = {0};
  s_unwind_protect unwind_protect;
  assert(env);
  assert(protected);
  if (setjmp(unwind_protect.buf)) {
    env_pop_unwind_protect(env);
    env_eval_block(env, cleanup, &tmp);
    longjmp(*unwind_protect.jmp, 1);
  }
  env_eval_tag(env, protected, dest);
  env_pop_unwind_protect(env);
  env_eval_block(env, cleanup, &tmp);
  return dest;
}

s_tag * env_while (s_env *env, s_tag *cond, s_tag *body,
                   s_tag *dest)
{
  s_tag  cond_bool = {0};
  s_call cond_cast = {0};
  s_tag tmp = {0};
  call_init_call_cast(&cond_cast, &g_sym_Bool);
  if (! tag_init_copy(&list_next(cond_cast.arguments)->tag, cond))
    goto ko;
  while (1) {
    tag_clean(&tmp);
    if (! env_eval_call(env, &cond_cast, &cond_bool))
      goto ko;
    if (cond_bool.type != TAG_BOOL)
      goto ko;
    if (! cond_bool.data.bool_)
      break;
    if (! env_eval_tag(env, body, &tmp))
      goto ko;
  }
  call_clean(&cond_cast);
  *dest = tmp;
  return dest;
 ko:
  tag_clean(&tmp);
  call_clean(&cond_cast);
  return NULL;
}
