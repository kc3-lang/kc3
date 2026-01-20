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

#include <dlfcn.h>
#include <unistd.h>
#include "alloc.h"

const char *g_env_argv0_default = "";
const char *g_env_argv0_dir_default = "";

#include "array.h"
#include "assert.h"
#include "binding.h"
#include "do_block.h"
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
#include "counter.h"
#include "cow.h"
#include "data.h"
#include "env.h"
#include "env_eval.h"
#include "env_frame_capture.h"
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
#include "ht.h"
#include "ident.h"
#include "integer.h"
#include "io.h"
#include "kc3_main.h"
#include "list.h"
#include "map.h"
#include "marshall.h"
#include "marshall_read.h"
#include "module.h"
#include "ncpu.h"
#include "op.h"
#include "ops.h"
#include "pcallable.h"
#include "pfacts.h"
#include "plist.h"
#include "pstruct.h"
#include "pstruct_type.h"
#include "rwlock.h"
#include "securelevel.h"
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

static thread_local s_env *g_kc3_env_default = NULL;
static thread_local s_env *g_kc3_env_global = NULL;

static void    env_args_clean (s_env *env);
static s_env * env_args_init (s_env *env, int *argc, char ***argv);
static void    env_globals_clean (s_env *env);
static s_env * env_globals_init (s_env *env);
static void    env_toplevel_clean (s_env *env);
static s_env * env_toplevel_init (s_env *env);

#if defined(WIN32) || defined(WIN64)
static char * realpath (const char *path, char *resolved_path);
#endif

// Special operator.
s_pointer * env_address_of (s_env *env, s_ident *ident, s_pointer *dest)
{
  s_tag *tag = NULL;
  s_pointer tmp = {0};
  assert(env);
  assert(ident);
  assert(dest);
  if (! ident->module)
    tag = env_frames_get(env, ident->sym);
  if (! tag &&
      ! (tag = env_ident_get_address(env, ident))) {
    err_write_1("env_address_of: undeclared ident ");
    err_inspect_ident(ident);
    err_write_1("\n");
    assert(! "env_address_of: undeclared ident");
    return NULL;
  }
  if (! tag_type(tag, &tmp.target_type)) {
    err_puts("env_address_of: invalid tag type");
    assert(! "env_address_of: invalid tag type");
    return NULL;
  }
  if (! (tmp.pointer_type =
         sym_target_to_pointer_type(tmp.target_type))) {
    err_puts("env_address_of: sym_target_to_pointer_type");
    assert(! "env_address_of: sym_target_to_pointer_type");
    return NULL;
  }
  if (! tag_to_pointer(tag, tmp.target_type, &tmp.ptr.p)) {
    err_puts("env_address_of: tag_to_pointer");
    assert(! "env_address_of: tag_to_pointer");
    return NULL;
  }
  *dest = tmp;
  return dest;
}

// TODO: unwind_protect
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
  p_list tmp;
  if (! plist_init_copy(&tmp, &env->args))
    return NULL;
  *dest = tmp;
  return dest;
}

void env_args_clean (s_env *env)
{
  list_delete_all(env->args);
  str_delete(env->argv0);
  str_delete(env->argv0_dir);
}

s_env * env_args_init (s_env *env, int *argc, char ***argv)
{
  s32 argc_prev = 0;
  char argv0_real[PATH_MAX];
  int count;
  char **vector;
  assert(env);
  if (argc && argv && *argc && *argv) {
    env->argc = (*argc)--;
    env->argv = (*argv)++;
    if (realpath(env->argv[0], argv0_real))
      env->argv0 = str_new_1_alloc(argv0_real);
    else
      env->argv0 = str_new_1(NULL, env->argv[0]);
    if (! env->argv0)
      return NULL;
    if (! (env->argv0_dir = alloc(sizeof(s_str))))
      return NULL;
    file_dirname(env->argv0, env->argv0_dir);
    while (*argc > 0 && *argc != argc_prev) {
      argc_prev = *argc;
      if (**argv && ! strcmp(**argv, "--trace")) {
        env->trace = true;
        (*argc)--;
        (*argv)++;
      }
      if (**argv && ! strcmp(**argv, "--copy")) {
        env->pass_by_copy = true;
        (*argc)--;
        (*argv)++;
      }
      if (*argc > 1 && (*argv)[0] && (*argv)[1] &&
          ! strcmp((*argv)[0], "--dump")) {
        if (! str_init_1(&env->dump_path, NULL, (*argv)[1]))
          return NULL;
        (*argc) -= 2;
        (*argv) += 2;
      }
      if (*argc > 1 && (*argv)[0] && (*argv)[1] &&
          ! strcmp((*argv)[0], "--restore")) {
        if (! str_init_1(&env->restore_path, NULL, (*argv)[1]))
          return NULL;
        (*argc) -= 2;
        (*argv) += 2;
      }
      if (**argv && ! strcmp(**argv, "--bool-ptr")) {
        env->bool_ptr = true;
        (*argc)--;
        (*argv)++;
      }
    }
    count = *argc;
    vector = *argv;
    while (count-- > 0) {
      env->args = list_new_str_1(NULL, vector[count], env->args);
    }
    env->args = list_new_str_copy(env->argv0, env->args);
    return env;
  }
  env->argc = 0;
  env->argv = NULL;
  env->argv0 = str_new_1(NULL, g_env_argv0_default);
  // TODO: FIXME: breaks debug
  // env->argv0_dir = str_new_1(NULL, g_env_argv0_dir_default);
  env->argv0_dir = str_new_empty();
  env->args = list_new_str_copy(env->argv0, NULL);
  return env;
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
  s_tag tag_pvar;
  tag_init_ident(&tag_ident, &call->ident);
  tag_init_psym( &tag_is_a, &g_sym_is_a);
  tag_init_psym( &tag_macro, &g_sym_macro);
  tag_init_psym( &tag_module_name, call->ident.module);
  tag_init_psym( &tag_op, &g_sym_op);
  tag_init_psym( &tag_special_operator, &g_sym_special_operator);
  tag_init_psym( &tag_symbol, &g_sym_symbol);
  tag_init_psym( &tag_symbol_value, &g_sym_symbol_value);
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
  tag_init_pvar( &tag_pvar, &g_sym_Callable);
  if (! facts_with_tags (env->facts, &cursor, &tag_ident,
                         &tag_symbol_value, &tag_pvar)) {
    err_puts("env_call_get: facts_with_tags");
    assert(! "env_call_get: facts_with_tags");
    tag_clean(&tag_pvar);
    return false;
  }
  if (! facts_cursor_next(&cursor, &fact)) {
    err_puts("env_call_get: facts_cursor_next");
    assert(! "env_call_get: facts_cursor_next");
    tag_clean(&tag_pvar);
    return false;
  }
  if (! fact) {
    err_write_1("env_call_get: ");
    err_inspect_ident(&call->ident);
    err_puts(" :symbol_value not found");
    tag_clean(&tag_pvar);
    return false;
  }
  if (! pcallable_init_copy(&call->pcallable,
                            &tag_pvar.data.pvar->tag.data.pcallable))
    return false;
  tag_clean(&tag_pvar);
  callable_set_name_if_null(call->pcallable, &call->ident);
  if (! facts_find_fact_by_tags(env->facts, &tag_ident, &tag_is_a,
                                &tag_special_operator, &found)) {
    err_puts("env_call_get: facts_find_fact_by_tags 4");
    assert(! "env_call_get: facts_find_fact_by_tags 4");
    return false;
  }
  if (found) {
    switch (call->pcallable->type) {
    case CALLABLE_CFN:
      call->pcallable->data.cfn.special_operator = true;
      break;
    case CALLABLE_FN:
      call->pcallable->data.fn.special_operator = true;
      break;
    case CALLABLE_VOID:
      err_puts("env_call_get: void callable");
      assert(! "env_call_get: void callable");
      abort();
    }
  }
  facts_cursor_clean(&cursor);
  return true;
}

void env_clean (s_env *env)
{
  if (! env)
    env = g_kc3_env_global;
  if (! env)
    env = g_kc3_env_default;
  if (! env)
    return;
  if (false) {
    uw size;
    sym_list_size(&size);
    err_write_1("env_clean: g_sym_list: ");
    err_inspect_uw_decimal(size);
    err_write_1("\n");
  }
  //facts_save_file(env->facts, "debug.facts"); // debug
  str_clean(&env->restore_path);
  str_clean(&env->dump_path);
  list_delete_all(env->dlopen_list);
  env_globals_clean(env);
  env_toplevel_clean(env);
  error_handler_delete_all(env->error_handler);
  env->error_handler = NULL;
  assert(env->facts->ref_count == 1);
  if (env == env_global())
    env_cleaning(true);
#if LIBKC3_PROFILE
  profile_clean();
#endif
  facts_delete(env->facts);
  env->facts = NULL;
  ops_delete(env->ops);
  env->ops = NULL;
  if (env == env_global()) {
    counter_ht_delete(env->counter_ht);
    env->counter_ht = NULL;
  }
  env_args_clean(env);
  str_delete(env->module_path);
  env->module_path = NULL;
  list_delete_all(env->path);
  env->path = NULL;
  list_delete_all(env->search_modules_default);
  env->search_modules_default = NULL;
  if (g_kc3_env_global == env) {
    if (env->parent_env)
      g_kc3_env_global = env->parent_env;
    else
      g_kc3_env_global = g_kc3_env_default;
  }
  if (g_kc3_env_default == env)
    sym_delete_all();
  buf_file_close(env->in);
  buf_delete(env->in);
  env->in = NULL;
  buf_file_close(env->out);
  buf_delete(env->out);
  env->out = NULL;
  buf_file_close(env->err);
  buf_delete(env->err);
  env->err = NULL;
  if (g_kc3_env_default == env) {
    free(g_kc3_env_default);
    g_kc3_env_default = NULL;
  }
}

bool env_cleaning (bool enable)
{
  static bool cleaning = false;
  if (enable)
    cleaning = true;
  return cleaning;
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
  if (securelevel(0) > 1) {
    err_puts("env_def: cannot use def with securelevel > 1");
    abort();
  }
  tag_ident.type = TAG_IDENT;
  tag_ident.data.ident.sym = ident->sym;
  if (ident->module)
    tag_ident.data.ident.module = ident->module;
  else
    tag_ident.data.ident.module = env->current_defmodule;
  tag_init_psym(&tag_module, tag_ident.data.ident.module);
  tag_init_psym(&tag_symbol, &g_sym_symbol);
  if (! facts_add_tags(env->facts, &tag_module, &tag_symbol,
                       &tag_ident)) {
    err_puts("env_def: facts_add_tags");
    assert(! "env_def: facts_add_tags");
    return false;
  }
  tag_init_psym(&tag_symbol_value, &g_sym_symbol_value);
  if (! facts_replace_tags(env->facts, &tag_ident, &tag_symbol_value,
                           value)) {
    err_puts("env_def: facts_replace_tags");
    assert(! "env_def: facts_replace_tags");
    return false;
  }
  if (tag_ident.data.ident.module == env->current_defmodule &&
      tag_ident.data.ident.sym == &g_sym_clean &&
      ! env_def_clean(env, env->current_defmodule, value)) {
    err_puts("env_def: env_def_clean");
    assert(! "env_def: env_def_clean");
    return false;
  }
  return true;
}

const s_sym * env_def_clean (s_env *env, const s_sym *module,
                             s_tag *clean)
{
  s_struct_type *st;
  if (securelevel(0) > 1) {
    err_puts("env_def_clean: cannot use def clean with"
             " securelevel > 1");
    abort();
  }
  if (! env_pstruct_type_find(env, module, &st))
    return NULL;
  if (! st) {
    err_write_1("env_def_clean: module ");
    err_inspect_sym(module);
    err_write_1(": struct type not found");
    assert(! "env_def_clean: module struct type not found");
    return NULL;
  }
  if (clean->type != TAG_PCALLABLE) {
    err_write_1("env_def_clean: module ");
    err_inspect_sym(module);
    err_write_1(": clean method must be a Callable");
    assert(! "env_def_clean: module clean method must be a Callable");
    return NULL;
  }
  if (! pcallable_init_copy(&st->clean, &clean->data.pcallable)) {
    err_write_1("env_def_clean: module ");
    err_inspect_sym(module);
    err_write_1(": pcallable_init_copy");
    assert(! "env_def_clean: pcallable_init_copy");
    return NULL;
  }
  return module;
}

s_env * env_default (void)
{
  return g_kc3_env_default;
}

void env_default_set (s_env *env)
{
  assert(env);
  g_kc3_env_default = env;
  g_kc3_env_global = env;
}

s_tag * env_defcounter (s_env *env, s_ident *name, s_tag *value,
                        s_tag *dest)
{
  s_counter *counter = NULL;
  s_tag      counter_tag = {0};
  s_ident ident;
  ident = *name;
  if (! tag_is_integer(value)) {
    err_puts("env_defcounter: value is not an integer");
    assert(! "env_defcounter: value is not an integer");
    return NULL;
  }
  if (! ident.module)
    ident.module = env->current_defmodule;
  if (! (counter = counter_new(&ident, value)))
    return NULL;
  if (! tag_init_pstruct_with_data(&counter_tag, &g_sym_Counter,
                                   counter, true)) {
    counter_delete(counter);
    return NULL;
  }
#if HAVE_PTHREAD
  rwlock_w(&env->counter_ht->rwlock);
#endif
  if (ht_has(env->counter_ht, &counter_tag)) {
    err_puts("env_defcounter: counter is already defined");
    assert(! "env_defcounter: counter is already defined");
    goto clean;
  }
  if (! ht_add(env->counter_ht, &counter_tag)) {
    err_puts("env_defcounter: ht_add");
    assert(! "env_defcounter: ht_add");
    tag_clean(&counter_tag);
    return NULL;
  }
#if HAVE_PTHREAD
  rwlock_unlock_w(&env->counter_ht->rwlock);
#endif
  tag_clean(&counter_tag);
  if (false) {
    err_write_1("env_defcounter: ");
    err_inspect_ident(&ident);
    err_write_1(" = ");
    err_inspect_tag(value);
    err_write_1("\n");
  }
  return tag_init_copy(dest, value);
 clean:
#if HAVE_PTHREAD
  rwlock_unlock_w(&env->counter_ht->rwlock);
#endif
  tag_clean(&counter_tag);
  return NULL;
}

// FIXME: transaction ?
// TODO: unwind_protect
s_tag * env_defmodule (s_env *env, const s_sym * const *name,
                       const s_do_block *do_block, s_tag *dest)
{
  bool is_loading;
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
  assert(do_block);
  assert(dest);
  if (securelevel(0) > 1) {
    err_puts("env_defmodule: cannot use defmodule with"
             " securelevel > 1");
    abort();
  }
  prev_defmodule = env->current_defmodule;
  if (! env_module_is_loading(env, *name, &is_loading))
    return NULL;
  if (! is_loading &&
      ! env_module_is_loading_set(env, *name, true))
    return NULL;
  env->current_defmodule = *name;
  search_modules = env->search_modules;
  if (! env_module_search_modules(env, name, &env->search_modules))
    goto clean;
  tag_init_psym(&tag_is_a, &g_sym_is_a);
  tag_init_psym(&tag_module, &g_sym_module);
  tag_init_psym(&tag_module_name, *name);
  if (! facts_add_tags(env->facts, &tag_module_name, &tag_is_a,
                       &tag_module))
    goto clean;
  if (! env_eval_do_block(env, do_block, &tmp))
    goto clean;
  tag_clean(&tmp);
  tag_init_psym(dest, *name);
  result = dest;
 clean:
  list_delete_all(env->search_modules);
  env->search_modules = search_modules;
  env->current_defmodule = prev_defmodule;
  if (! is_loading &&
      ! env_module_is_loading_set(env, *name, false))
    return NULL;
  return result;
}

bool env_defoperator (s_env *env, s_tag *tag_op)
{
  s_op *op;
  s_tag tag_id = {0};
  s_tag tag_is_a = {0};
  s_tag tag_sym = {0};
  s_tag tag_sym_op = {0};
  s_tag tag_sym_op_sym = {0};
  assert(env);
  assert(tag_op);
  assert(env->ops);
  if (securelevel(0) > 1) {
    err_puts("env_defoperator: cannot use defoperator with"
             " securelevel > 1");
    abort();
  }
  if (! tag_op ||
      tag_op->type != TAG_PSTRUCT ||
      ! tag_op->data.pstruct ||
      tag_op->data.pstruct->pstruct_type->module != &g_sym_KC3_Op) {
    err_puts("env_defoperator: invalid operator, expected %KC3.Op{}");
    assert(! "env_defoperator: invalid operator, expected %KC3.Op{}");
  }
  op = tag_op->data.pstruct->data;
  if (! ops_add_tag(env->ops, tag_op))
    return false;
  tag_init_psym_anon(&tag_id, &g_sym_op.str);
  tag_init_psym(&tag_is_a, &g_sym_is_a);
  tag_init_psym(&tag_sym, op->sym);
  tag_init_psym(&tag_sym_op, &g_sym_op);
  tag_init_psym(&tag_sym_op_sym, &g_sym_op_sym);
  if (! facts_add_tags(env->facts, &tag_id, &tag_is_a, &tag_sym_op))
    return false;
  if (! facts_add_tags(env->facts, &tag_id, &tag_sym_op_sym, &tag_sym))
    return false;
  return true;
}

s_tag * env_defspecial_operator (s_env *env, s_tag *tag, s_tag *dest)
{
  s8 arity;
  p_call call;
  s_tag callable_tag = {0};
  s_tag ident_tag = {0};
  p_list second;
  s_tag tag_arity = {0};
  s_tag tag_is_a = {0};
  s_tag tag_module = {0};
  s_tag tag_special_operator = {0};
  s_tag tag_sym_arity = {0};
  s_tag tag_symbol = {0};
  s_tag tag_symbol_value = {0};
  assert(env);
  assert(tag);
  assert(dest);
  if (securelevel(0) > 1) {
    err_puts("env_defspecial_operator: cannot use defspecial_operator"
             " with securelevel > 1");
    abort();
  }
  if (tag->type != TAG_PCALL) {
    err_puts("env_defspecial_operator: expected Call");
    return NULL;
  }
  call = tag->data.pcall;
  if (call->ident.sym != &g_sym__equal ||
      ! call->arguments ||
      ! (second = list_next(call->arguments)) ||
      list_next(second) ||
      call->arguments->tag.type != TAG_IDENT) {
    err_puts("env_defspecial_operator: expected Ident = Callable");
    return NULL;
  }
  tag_init_copy(&ident_tag, &call->arguments->tag);
  if (! ident_tag.data.ident.module)
    ident_tag.data.ident.module = env->current_defmodule;
  if (! env_eval_tag(env, &second->tag, &callable_tag)) {
    tag_clean(&callable_tag);
    return NULL;
  }
  if (callable_tag.type != TAG_PCALLABLE) {
    err_puts("env_defspecial_operator: right operand must be a"
             " Callable");
    assert(!("env_defspecial_operator: right operand must be a"
             " Callable"));
    tag_clean(&callable_tag);
    return NULL;
  }
  arity = callable_arity(callable_tag.data.pcallable);
  if (callable_tag.data.pcallable->type == CALLABLE_CFN &&
      callable_tag.data.pcallable->data.cfn.result_type)
    arity--;
  if (arity < 0) {
    err_puts("env_defspecial_operator: invalid arity");
    tag_clean(&callable_tag);
    return NULL;
  }
  tag_init_psym(&tag_module, ident_tag.data.ident.module);
  tag_init_psym(&tag_is_a, &g_sym_is_a);
  tag_init_psym(&tag_special_operator, &g_sym_special_operator);
  tag_init_psym(&tag_sym_arity, &g_sym_arity);
  tag_init_psym(&tag_symbol, &g_sym_symbol);
  tag_init_psym(&tag_symbol_value, &g_sym_symbol_value);
  tag_init_u8(&tag_arity, arity);
  if (! facts_add_tags(env->facts, &tag_module, &tag_symbol,
                       &ident_tag)) {
    tag_clean(&callable_tag);
    return NULL;
  }
  if (! facts_add_tags(env->facts, &ident_tag, &tag_is_a, 
                       &tag_special_operator)) {
    tag_clean(&callable_tag);
    return NULL;
  }
  if (! facts_add_tags(env->facts, &ident_tag, &tag_sym_arity,
                       &tag_arity)) {
    tag_clean(&callable_tag);
    return NULL;
  }
  callable_set_special(callable_tag.data.pcallable, true);
  if (! facts_add_tags(env->facts, &ident_tag, &tag_symbol_value, 
                       &callable_tag)) {
    tag_clean(&callable_tag);
    return NULL;
  }
  *dest = ident_tag;
  tag_clean(&callable_tag);
  return dest;
}

const s_sym * env_defstruct (s_env *env, s_list *spec)
{
  s_tag tag_module_name;
  s_tag tag_st;
  s_tag tag_struct_type;
  if (securelevel(0) > 1) {
    err_puts("env_defstruct: cannot use defstruct with"
             " securelevel > 1");
    abort();
  }
  tag_init_psym(&tag_module_name, env->current_defmodule);
  tag_init_pstruct_type(&tag_st, env->current_defmodule, spec);
  tag_init_psym(&tag_struct_type, &g_sym_struct_type);
  if (! facts_replace_tags(env->facts, &tag_module_name,
                           &tag_struct_type, &tag_st)) {
    tag_clean(&tag_st);
    return NULL;
  }
  tag_clean(&tag_st);
  return env->current_defmodule;
}

void ** env_dlopen (s_env *env, const s_str *so_path, void **dest)
{
  s_list *iter;
  s_list *list = NULL;
  s_str path = {0};
  s_tag tag = {0};
  void *tmp = NULL;
  assert(env);
  assert(so_path);
  assert(dest);
  if (securelevel(0) > 0) {
    err_puts("env_dlopen: cannot dlopen with securelevel > 0");
    abort();
  }
  tag.type = TAG_STR;
  tag.data.str = *so_path;
  iter = env->dlopen_list;
  while (iter) {
    if (iter->tag.type == TAG_STR &&
        ! compare_str(so_path, &iter->tag.data.str)) {
      *dest = NULL;
      goto add_to_list;
    }
    iter = list_next(iter);
  }
  if (! str_init_concatenate(&path, env->module_path, so_path))
    return NULL;
  if (env->trace) {
    err_write_1("kc3_dlopen: ");
    err_inspect_str(&path);
    err_write_1("\n");
  }
  if (! (tmp = dlopen(path.ptr.pchar, RTLD_LAZY | RTLD_GLOBAL))) {
    err_write_1("kc3_dlopen: ");
    err_inspect_str(&path);
    err_write_1(": dlopen: ");
    err_puts(dlerror());
    assert(! "kc3_dlopen: dlopen failed");
    str_clean(&path);
    return NULL;
  }
  str_clean(&path);
  *dest = tmp;
 add_to_list:
  if (! (list = list_new_str_copy(so_path, env->dlopen_list))) {
    if (tmp)
      dlclose(tmp);
    return NULL;
  }
  env->dlopen_list = list;
  return dest;
}

sw env_dump (const s_env *env, const s_str *path)
{
  sw r;
  if (env->trace) {
    err_write_1("env_dump: ");
    err_inspect_str(path);
    err_write_1("\n");
  }
  r = marshall_env_to_file(env, path);
  if (env->trace) {
    err_write_1("env_dump: ");
    err_inspect_str(path);
    err_write_1(": ");
    err_puts(r > 0 ? "OK" : "ERROR");
  }
  return r;
}

sw env_dump_restore (s_env *env, const s_str *path)
{
  sw result = -1;
  if (env->trace) {
    err_write_1("env_dump_restore: ");
    err_inspect_str(path);
    err_write_1("\n");
  }
  result = marshall_read_env_from_file(env, path);
  if (env->trace) {
    err_write_1("env_dump_restore: ");
    err_inspect_str(path);
    if (result <= 0)
      err_puts(": ERROR");
    else
      err_puts(": OK");
  }
  return result;
}

bool env_dump_restore_path_resolve (s_env *env)
{
  static const s_str ext = STR(".dump");
  static const s_str kc3_dump = STR("kc3.dump");
  s_str path = {0};
  s_str progname = {0};
  static const s_str slash = STR("/");
  if (! file_name(env->argv0, &progname))
    return false;
  if (! str_init_concatenate_v(&path, 4, (const s_str*[])
                               {env->module_path, &slash,
                                &progname, &ext})) {
    str_clean(&progname);
    return false;
  }
  str_clean(&progname);
  if (false) {
    err_write_1("env_dump_restore_path_resolve: path: ");
    err_inspect_str(&path);
    err_write_1("\n");
  }
  if (file_access(&path, &g_sym_r)) {
    env->restore_path = path;
    return true;
  }
  str_clean(&path);
  if (file_access(&kc3_dump, &g_sym_r)) {
    env->restore_path = kc3_dump;
    return true;
  }
  if (! str_init_concatenate(&path, env->module_path, &kc3_dump))
    return false;
  if (file_access(&path, &g_sym_r)) {
    env->restore_path = path;
    return true;
  }
  str_clean(&path);
  return true;
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

void env_error_handler_pop (s_env *env, s_error_handler *eh)
{
  assert(env);
  assert(eh);
  if (env->error_handler != eh) {
    err_puts("env_error_handler_pop: mismatch");
    assert(! "env_error_handler_pop: mismatch");
    abort();
  }
  env->error_handler = error_handler_delete(env->error_handler);
}

void env_error_handler_push (s_env *env, s_error_handler *eh)
{
  tag_init_void(&eh->tag);
  eh->next = env->error_handler;
  env->error_handler = eh;
}

void env_error_tag (s_env *env, s_tag *tag)
{
  s_error_handler *error_handler;
  assert(env);
  assert(tag);
  error_handler = env->error_handler;
  if (error_handler) {
    tag_init_copy(&error_handler->tag, tag);
    error_handler->stacktrace = list_new_copy_all(env->stacktrace);
    env_longjmp(env, &error_handler->jmp);
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

// TODO: unwind_protect
s_fact_w * env_fact_w_eval (s_env *env, s_fact_w *fact,
			    s_fact_w *dest)
{
  s_fact_w tmp = {0};
  assert(env);
  assert(fact);
  assert(dest);
  if (fact->subject.type == TAG_PCALLABLE) {
    if (! env_eval_tag(env, &fact->subject, &tmp.subject))
      return NULL;
  }
  else {
    if (! tag_init_copy(&tmp.subject, &fact->subject))
      return NULL;
  }
  if (fact->predicate.type == TAG_PCALLABLE) {
    if (! env_eval_tag(env, &fact->predicate, &tmp.predicate))
      return NULL;
  }
  else {
    if (! tag_init_copy(&tmp.predicate, &fact->predicate))
      return NULL;
  }
  if (fact->object.type == TAG_PCALLABLE) {
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

// TODO: unwind_protect
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
  if (! (arguments = list_new_pstruct(&g_sym_FactW, NULL)))
    return NULL;
  if (! struct_allocate(arguments->tag.data.pstruct)) {
    list_delete_all(arguments);
    return NULL;
  }
  fact_w = arguments->tag.data.pstruct->data;
  if (! facts_with_list(facts, &cursor, *spec)) {
    list_delete_all(arguments);
    return NULL;
  }
  list = NULL;
  l = &list;
  while (1) {
    if (! facts_with_cursor_next(&cursor, &fact))
      goto clean;
    if (! fact)
      goto ok;
    if (! fact_w_init_fact(fact_w, fact))
      goto clean;
    *l = list_new(NULL);
    if (! env_eval_call_callable_args(env, callback, arguments,
                                      &(*l)->tag))
      goto clean;
    fact_w_clean(fact_w);
    fact_w_init(fact_w);
    l = &(*l)->next.data.plist;
  }
 ok:
  list_delete_all(arguments);
  tmp.type = TAG_PLIST;
  tmp.data.plist = list;
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

// TODO: unwind_protect
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
  if (! (arguments = list_new_pstruct(&g_sym_FactW, NULL)))
    return NULL;
  if (! struct_allocate(arguments->tag.data.pstruct)) {
    list_delete_all(arguments);
    return NULL;
  }
  fact_w = arguments->tag.data.pstruct->data;
  if (! facts_with_tags(facts, &cursor, subject, predicate, object)) {
    list_delete_all(arguments);
    return NULL;
  }
  list = NULL;
  l = &list;
  while (1) {
    if (! facts_cursor_next(&cursor, &fact))
      goto clean;
    if (! fact) {
      goto ok;
    }
    if (! fact_w_init_fact(fact_w, fact))
      goto clean;
    *l = list_new(NULL);
    if (! env_eval_call_callable_args(env, callback, arguments,
                                      &(*l)->tag))
      goto clean;
    fact_w_clean(fact_w);
    fact_w_init(fact_w);
    l = &(*l)->next.data.plist;
  }
 ok:
  list_delete_all(arguments);
  tmp.type = TAG_PLIST;
  tmp.data.plist = list;
  *dest = tmp;
  return dest;
 clean:
  facts_cursor_clean(&cursor);
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
  s_tag * volatile dest_v = dest;
  s_fact *fact = NULL;
  s_fact_w *fact_w = NULL;
  s_tag tmp = {0};
  s_unwind_protect unwind_protect;
  assert(env);
  assert(facts);
  assert(spec);
  assert(callback);
  assert(dest_v);
  if (! (arguments = list_new_pstruct(&g_sym_FactW, NULL)))
    return NULL;
  if (! struct_allocate(arguments->tag.data.pstruct)) {
    list_delete_all(arguments);
    return NULL;
  }
  fact_w = arguments->tag.data.pstruct->data;
  if (! facts_with_list(facts, &cursor, *spec)) {
    list_delete_all(arguments);
    return NULL;
  }
  if (! facts_with_cursor_next(&cursor, &fact))
    goto clean;
  if (! fact)
    goto ok;
  if (! fact_w_init_fact(fact_w, fact))
    goto clean;
  env_unwind_protect_push(env, &unwind_protect);
  if (setjmp(unwind_protect.buf)) {
    env_unwind_protect_pop(env, &unwind_protect);
    list_delete_all(arguments);
    facts_with_cursor_clean(&cursor);
    longjmp(*unwind_protect.jmp, 1);
  }
  if (! env_eval_call_callable_args(env, callback, arguments, &tmp)) {
    env_unwind_protect_pop(env, &unwind_protect);
    goto clean;
  }
  env_unwind_protect_pop(env, &unwind_protect);
  facts_with_cursor_clean(&cursor);
 ok:
  list_delete_all(arguments);
  if (false) {
    err_write_1("env_facts_first_with: ");
    err_inspect_tag(&tmp);
    err_write_1("\n");
  }
  *dest_v = tmp;
  return dest_v;
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
  s_tag * volatile dest_v = dest;
  s_fact *fact = NULL;
  s_fact_w *fact_w = NULL;
  s_tag tmp = {0};
  s_unwind_protect unwind_protect;
  assert(env);
  assert(facts);
  assert(subject);
  assert(predicate);
  assert(object);
  assert(callback);
  assert(dest_v);
  if (! (arguments = list_new_pstruct(&g_sym_FactW, NULL)))
    return NULL;
  if (! struct_allocate(arguments->tag.data.pstruct)) {
    list_delete_all(arguments);
    return NULL;
  }
  fact_w = arguments->tag.data.pstruct->data;
  if (! facts_with_tags(facts, &cursor, subject, predicate, object)) {
    list_delete_all(arguments);
    return NULL;
  }
  if (! facts_cursor_next(&cursor, &fact))
    goto clean;
  if (! fact)
    goto ok;
  if (! fact_w_init_fact(fact_w, fact)) {
    facts_cursor_clean(&cursor);
    goto clean;
  }
  env_unwind_protect_push(env, &unwind_protect);
  if (setjmp(unwind_protect.buf)) {
    env_unwind_protect_pop(env, &unwind_protect);
    facts_cursor_clean(&cursor);
    list_delete_all(arguments);
    longjmp(*unwind_protect.jmp, 1);
  }
  if (! env_eval_call_callable_args(env, callback, arguments, &tmp)) {
    env_unwind_protect_pop(env, &unwind_protect);
    list_delete_all(arguments);
    facts_cursor_clean(&cursor);
    goto clean;
  }
  env_unwind_protect_pop(env, &unwind_protect);
  facts_cursor_clean(&cursor);
 ok:
  list_delete_all(arguments);
  *dest_v = tmp;
  return dest_v;
 clean:
  tag_clean(&tmp);
  list_delete_all(arguments);
  return NULL;
}

s_tag * env_facts_with (s_env *env, s_facts *facts, s_list **spec,
                        s_callable *callback, s_tag *dest)
{
  s_list *arguments;
  s_facts_with_cursor cursor = {0};
  s_tag * volatile dest_v = dest;
  s_fact *fact = NULL;
  s_fact_w *fact_w = NULL;
  s_tag tmp = {0};
  s_unwind_protect unwind_protect;
  if (! (arguments = list_new_pstruct(&g_sym_FactW, NULL)))
    return NULL;
  if (! struct_allocate(arguments->tag.data.pstruct)) {
    list_delete_all(arguments);
    return NULL;
  }
  fact_w = arguments->tag.data.pstruct->data;
  if (! facts_with_list(facts, &cursor, *spec)) {
    list_delete_all(arguments);
    return NULL;
  }
  env_unwind_protect_push(env, &unwind_protect);
  if (setjmp(unwind_protect.buf)) {
    env_unwind_protect_pop(env, &unwind_protect);
    list_delete_all(arguments);
    facts_with_cursor_clean(&cursor);
    longjmp(*unwind_protect.jmp, 1);
  }
  while (1) {
    if (! facts_with_cursor_next(&cursor, &fact))
      goto clean;
    if (! fact) {
      env_unwind_protect_pop(env, &unwind_protect);
      goto ok;
    }
    tag_clean(&tmp);
    fact_w_init_fact(fact_w, fact);
    if (! env_eval_call_callable_args(env, callback, arguments, &tmp))
      goto clean;
    fact_w_clean(fact_w);
    fact_w_init(fact_w);
  }
  env_unwind_protect_pop(env, &unwind_protect);
  facts_with_cursor_clean(&cursor);
 ok:
  list_delete_all(arguments);
  *dest_v = tmp;
  return dest_v;
 clean:
  err_puts("env_facts_with: error");
  assert(! "env_facts_with: error");
  env_unwind_protect_pop(env, &unwind_protect);
  tag_clean(&tmp);
  list_delete_all(arguments);
  facts_with_cursor_clean(&cursor);
  return NULL;
}

s_tag * env_facts_with_macro (s_env *env, s_tag *facts_tag,
                              s_tag *spec_tag, s_tag *tag,
                              s_tag *dest)
{
  s_facts_with_cursor cursor = {0};
  s_tag * volatile dest_v = dest;
  s_fact *fact = NULL;
  s_facts *facts;
  s_tag    facts_eval;
  s_list *spec = NULL;
  s_tag   spec_eval = {0};
  const s_sym *sym_Facts_star = &g_sym_Facts_star;
  s_tag tmp = {0};
  s_unwind_protect unwind_protect;
  if (! env_eval_tag(env, facts_tag, &facts_eval)) {
    err_puts("env_facts_with_macro: env_eval_tag Facts*");
    assert(! "env_facts_with_macro: env_eval_tag Facts*");
    return NULL;
  }
  if (! pfacts_init_cast(&facts, &sym_Facts_star, &facts_eval)) {
    err_puts("env_facts_with_macro: pfacts_init_cast Facts*");
    assert(! "env_facts_with_macro: pfacts_init_cast Facts*");
    tag_clean(&facts_eval);
    return NULL;
  }
  tag_clean(&facts_eval);
  if (! env_eval_tag(env, spec_tag, &spec_eval)) {
    err_puts("env_facts_with_macro: env_eval_tag spec");
    assert(! "env_facts_with_macro: env_eval_tag spec");
    return NULL;
  }
  if (spec_eval.type != TAG_PLIST) {
    err_puts("env_facts_with_macro: spec is not a List");
    assert(! "env_facts_with_macro: spec is not a List");
    return NULL;
  }
  spec = spec_eval.data.plist;
  if (! facts_with_list(facts, &cursor, spec))
    return NULL;
  while (1) {
    if (! facts_with_cursor_next(&cursor, &fact))
      goto clean;
    if (! fact)
      goto ok;
    tag_clean(&tmp);
    env_unwind_protect_push(env, &unwind_protect);
    if (setjmp(unwind_protect.buf)) {
      env_unwind_protect_pop(env, &unwind_protect);
      tag_clean(&tmp);
      facts_with_cursor_clean(&cursor);
      longjmp(*unwind_protect.jmp, 1);
    }
    if (! env_eval_tag(env, tag, &tmp)) {
      env_unwind_protect_pop(env, &unwind_protect);
      goto clean;
    }
    env_unwind_protect_pop(env, &unwind_protect);
  }
  facts_with_cursor_clean(&cursor);
 ok:
  *dest_v = tmp;
  return dest_v;
 clean:
  err_puts("env_facts_with_macro: error");
  assert(! "env_facts_with_macro: error");
  facts_with_cursor_clean(&cursor);
  tag_clean(&tmp);
  return NULL;
}

s_tag * env_facts_with_tags (s_env *env, s_facts *facts, s_tag *subject,
                             s_tag *predicate, s_tag *object,
                             s_callable *callback,
                             s_tag *dest)
{
  s_list *arguments;
  s_facts_cursor cursor = {0};
  s_tag * volatile dest_v = dest;
  s_fact *fact = NULL;
  s_fact_w *fact_w = NULL;
  s_tag tmp = {0};
  s_unwind_protect unwind_protect;
  if (! (arguments = list_new_pstruct(&g_sym_FactW, NULL)))
    return NULL;
  if (! struct_allocate(arguments->tag.data.pstruct)) {
    list_delete_all(arguments);
    return NULL;
  }
  fact_w = arguments->tag.data.pstruct->data;
  if (! facts_with_tags(facts, &cursor, subject, predicate, object)) {
    list_delete_all(arguments);
    return NULL;
  }
  while (1) {
    if (! facts_cursor_next(&cursor, &fact))
      goto clean;
    if (! fact) {
      goto ok;
    }
    tag_void(&tmp);
    if (! fact_w_init_fact(fact_w, fact)) {
      facts_cursor_clean(&cursor);
      goto clean;
    }
    env_unwind_protect_push(env, &unwind_protect);
    if (setjmp(unwind_protect.buf)) {
      env_unwind_protect_pop(env, &unwind_protect);
      facts_cursor_clean(&cursor);
      list_delete_all(arguments);
      longjmp(*unwind_protect.jmp, 1);
    }
    if (! env_eval_call_callable_args(env, callback, arguments, &tmp)) {
      env_unwind_protect_pop(env, &unwind_protect);
      facts_cursor_clean(&cursor);
      goto clean;
    }
    env_unwind_protect_pop(env, &unwind_protect);
    fact_w_clean(fact_w);
    fact_w_init(fact_w);
  }
  facts_cursor_clean(&cursor);
 ok:
  list_delete_all(arguments);
  *dest_v = tmp;
  return dest_v;
 clean:
  tag_clean(&tmp);
  list_delete_all(arguments);
  return NULL;
}

s_tag * env_facts_with_transaction (s_env *env, s_tag *facts_arg,
                                    s_tag *tag_arg, s_tag *dest)
{
  s_tag * volatile dest_v = dest;
  s_tag facts_tag = {0};
  s_facts *facts;
  const s_sym *sym_Facts_star = &g_sym_Facts_star;
  s_tag tmp = {0};
  s_facts_transaction transaction = {0};
  s_unwind_protect unwind_protect;
  if (! facts_transaction_init(&transaction))
    return NULL;
  if (! env_eval_tag(env, facts_arg, &facts_tag)) {
    facts_transaction_clean(&transaction);
    return NULL;
  }
  if (! pfacts_init_cast(&facts, &sym_Facts_star, &facts_tag)) {
    err_puts("env_facts_with_transaction: expected Facts*");
    assert(!("env_facts_with_transaction: expected Facts*"));
    tag_clean(&facts_tag);
    facts_transaction_clean(&transaction);
    return NULL;
  }
  tag_clean(&facts_tag);
  if (! facts_transaction_start(facts, &transaction)) {
    facts_transaction_clean(&transaction);
    pfacts_clean(&facts);
    return NULL;
  }
  env_unwind_protect_push(env, &unwind_protect);
  if (setjmp(unwind_protect.buf)) {
    env_unwind_protect_pop(env, &unwind_protect);
    facts_transaction_end(facts, &transaction);
    facts_transaction_clean(&transaction);
    pfacts_clean(&facts);
    longjmp(*unwind_protect.jmp, 1);
  }
  if (! env_eval_tag(env, tag_arg, &tmp)) {
    env_unwind_protect_pop(env, &unwind_protect);
    facts_transaction_end(facts, &transaction);
    facts_transaction_clean(&transaction);
    pfacts_clean(&facts);
    return NULL;
  }
  env_unwind_protect_pop(env, &unwind_protect);
  facts_transaction_end(facts, &transaction);
  facts_transaction_clean(&transaction);
  pfacts_clean(&facts);
  *dest_v = tmp;
  return dest_v;
}

s_frame * env_frame_new_capture (s_env *env, s_fn *fn)
{
  s_fn_clause *clause;
  s_frame *frame;
  uw i;
  s_list *pattern;
  assert(env);
  assert(fn);
  frame = frame_new(NULL);
  clause = fn->clauses;
  while (clause) {
    pattern = clause->pattern;
    while (pattern) {
      if (! env_frame_capture_tag(env, frame, &pattern->tag)) {
        frame_delete(frame);
        return NULL;
      }
      pattern = list_next(pattern);
    }
    i = 0;
    while (i < clause->algo.count) {
      if (! env_frame_capture_tag(env, frame,
                                  clause->algo.tag + i)) {
        frame_delete(frame);
        return NULL;
      }
      i++;
    }
    clause = clause->next;
  }
  return frame;
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
  return g_kc3_env_global;
}

bool env_global_set (s_env *env)
{
  assert(env);
  if (env->parent_env) {
    err_puts("env_global_set: argument already has a parent env");
    assert(! "env_global_set: argument already has a parent env");
    return false;
  }
  env->parent_env = g_kc3_env_global;
  g_kc3_env_global = env;
  return true;
}

void env_globals_clean (s_env *env)
{
  frame_delete(env->global_frame);
  frame_delete(env->read_time_frame);
}

s_env * env_globals_init (s_env *env)
{
  s_tag *file_dir;
  s_tag *file_path;
  s_tag *ncpu;
  if (! (env->read_time_frame = frame_new(NULL)))
    return NULL;
  if (! (file_dir = frame_binding_new_void(env->read_time_frame,
                                           &g_sym___DIR__)))
    return NULL;
  if (! (file_path = frame_binding_new_void(env->read_time_frame,
                                            &g_sym___FILE__)))
    return NULL;
  file_dir->type = TAG_STR;
  if (! file_pwd(&file_dir->data.str))
    return NULL;
  if (! tag_init_str_1(file_path, NULL, "stdin"))
    return NULL;
  if (! (env->global_frame = frame_new(env->read_time_frame)))
    return NULL;
  if (! (ncpu = frame_binding_new_void(env->read_time_frame,
                                       &g_sym_ncpu)))
    return NULL;
  ncpu_tag(ncpu);
  return env;
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
  s_tag tag_pvar;
  s_tag tag_special_operator;
  s_tag tag_sym;
  s_tag tag_symbol;
  s_tag tag_symbol_value;
  s_tag tmp = {0};
  module = ident->module;
  if (! module) {
    if (! env_sym_search_modules(env, ident->sym, &module) ||
        ! module) {
      err_write_1("env_ident_get: symbol not found: ");
      err_inspect_sym(ident->sym);
      err_write_1("\n");
      assert(! "env_ident_get: symbol not found");
      return NULL;
    }
  }
  // too slow, use require
  // if (! env_module_ensure_loaded(env, module))
  //   return NULL;
  tag_init_ident(&tag_ident, ident);
  tag_init_psym(  &tag_is_a, &g_sym_is_a);
  tag_init_psym(  &tag_macro, &g_sym_macro);
  tag_init_psym(  &tag_module, module);
  tag_init_psym(  &tag_special_operator, &g_sym_special_operator);
  tag_init_psym(  &tag_sym, ident->sym);
  tag_init_psym(  &tag_symbol, &g_sym_symbol);
  tag_init_psym(  &tag_symbol_value, &g_sym_symbol_value);
  tag_init_pvar( &tag_pvar, &g_sym_Tag);
  if (! facts_find_fact_by_tags(env->facts, &tag_module, &tag_symbol,
                                &tag_ident, &fact) ||
      ! fact) {
    tag_clean(&tag_pvar);
    return NULL;
  }
  if (! facts_with(env->facts, &cursor, (t_facts_spec) {
        &tag_ident, &tag_symbol_value, &tag_pvar,
        NULL, NULL })) {
    tag_clean(&tag_pvar);
    return NULL;
  }
  if (! facts_with_cursor_next(&cursor, &fact)) {
    facts_with_cursor_clean(&cursor);
    tag_clean(&tag_pvar);
    return NULL;
  }
  if (! fact)
    return NULL;
  if (! tag_init_copy(&tmp, &tag_pvar.data.pvar->tag)) {
    facts_with_cursor_clean(&cursor);
    tag_clean(&tag_pvar);
    return NULL;
  }
  facts_with_cursor_clean(&cursor);
  tag_clean(&tag_pvar);
  if (! facts_with(env->facts, &cursor, (t_facts_spec) {
        &tag_ident, &tag_is_a, &tag_special_operator, NULL, NULL}))
    return NULL;
  if (! facts_with_cursor_next(&cursor, &fact)) {
    facts_with_cursor_clean(&cursor);
    return NULL;
  }
  if (fact) {
    if (tmp.type == TAG_PCALLABLE) {
      switch (tmp.data.pcallable->type) {
      case CALLABLE_CFN:
        tmp.data.pcallable->data.cfn.special_operator = true;
        break;
      case CALLABLE_FN:
        tmp.data.pcallable->data.fn.special_operator = true;
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

s_tag * env_ident_get_address (s_env *env, const s_ident *ident)
{
  s_facts_cursor cursor;
  s_fact *fact;
  const s_sym *module;
  s_tag tag_ident;
  s_tag tag_is_a;
  s_tag tag_macro;
  s_tag tag_module;
  s_tag tag_pvar;
  s_tag tag_special_operator;
  s_tag tag_sym;
  s_tag tag_symbol;
  s_tag tag_symbol_value;
  s_tag *tmp;
  module = ident->module;
  if (! module) {
    if (! env_sym_search_modules(env, ident->sym, &module) ||
        ! module) {
      err_write_1("env_ident_get_address: symbol not found: ");
      err_inspect_sym(ident->sym);
      err_write_1("\n");
      assert(! "env_ident_get_address: symbol not found");
      return NULL;
    }
  }
  // too slow, use require
  // if (! env_module_ensure_loaded(env, module))
  //   return NULL;
  tag_init_ident(&tag_ident, ident);
  tag_init_psym(  &tag_is_a, &g_sym_is_a);
  tag_init_psym(  &tag_macro, &g_sym_macro);
  tag_init_psym(  &tag_module, module);
  tag_init_psym(  &tag_special_operator, &g_sym_special_operator);
  tag_init_psym(  &tag_sym, ident->sym);
  tag_init_psym(  &tag_symbol, &g_sym_symbol);
  tag_init_psym(  &tag_symbol_value, &g_sym_symbol_value);
  tag_init_pvar( &tag_pvar, &g_sym_Tag);
  if (! facts_find_fact_by_tags(env->facts, &tag_module, &tag_symbol,
                                &tag_ident, &fact) ||
      ! fact) {
    tag_clean(&tag_pvar);
    return NULL;
  }
  if (! facts_with_tags(env->facts, &cursor, &tag_ident,
                        &tag_symbol_value, &tag_pvar)) {
    tag_clean(&tag_pvar);
    return NULL;
  }
  if (! facts_cursor_next(&cursor, &fact)) {
    facts_cursor_clean(&cursor);
    tag_clean(&tag_pvar);
    return NULL;
  }
  if (! fact)
    return NULL;
  tmp = fact->object;
  facts_cursor_clean(&cursor);
  tag_clean(&tag_pvar);
  return tmp;
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
  if (! env_ident_resolve_module(env, ident, &tag_ident.data.ident)) {
    err_puts("env_ident_is_special_operator: env_ident_resolve_module");
    assert(! "env_ident_is_special_operator: env_ident_resolve_module");
    return NULL;
  }
  tag_init_psym(&tag_is_a, &g_sym_is_a);
  tag_init_psym(&tag_special_operator, &g_sym_special_operator);
  if (! facts_find_fact_by_tags(env->facts, &tag_ident, &tag_is_a,
                                &tag_special_operator, &fact)) {
    err_puts("env_ident_is_special_operator: facts_find_fact_by_tag");
    assert(! "env_ident_is_special_operator: facts_find_fact_by_tag");
    return NULL;
  }
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

/* Special operator. */
s_tag * env_if_then_else (s_env *env, s_tag *cond, s_tag *then,
                          s_tag *else_, s_tag *dest)
{
  bool  cond_bool = false;
  s_tag cond_eval = {0};
  bool silence_errors;
  const s_sym *type;
  silence_errors = env->silence_errors;
  env->silence_errors = true;
  if (! env_eval_tag(env, cond, &cond_eval)) {
    env->silence_errors = silence_errors;
    return NULL;
  }
  env->silence_errors = silence_errors;
  if (cond_eval.type == TAG_BOOL)
    cond_bool = cond_eval.data.bool_;
  else {
    type = &g_sym_Bool;
    if (! bool_init_cast(&cond_bool, &type, &cond_eval)) {
      tag_clean(&cond_eval);
      return NULL;
    }
  }
  tag_clean(&cond_eval);
  if (cond_bool) {
    if (! env_eval_tag(env, then, dest))
      return NULL;
    return dest;
  }
  if (! env_eval_tag(env, else_, dest))
    return NULL;
  return dest;
}

s_env * env_init (s_env *env, int *argc, char ***argv)
{
  s_str path;
  if (! env)
    env = g_kc3_env_default;
  if (! env)
    env = g_kc3_env_default = alloc(sizeof(s_env));
  if (! env)
    return NULL;
  env->parent_env = g_kc3_env_global;
  g_kc3_env_global = env;
  *env = (s_env) {0};
  if (! env_args_init(env, argc, argv))
    return NULL;
  sym_init_g_sym();
  if (! env_toplevel_init(env))
    return NULL;
  if (! env_globals_init(env))
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
#if LIBKC3_PROFILE
  profile_init();
#endif
  env->path = list_new_str_1
    (NULL, "./", list_new_str_1
     (NULL, "./share/kc3/", list_new_str_1
      (NULL, "../", list_new_str_1
       (NULL, "../share/kc3/", list_new_str_1
        (NULL, "../Resources/", list_new_str_1
         (NULL, "../../", list_new_str_1
          (NULL, "../../../", list_new_str_1
           (NULL, "../../../../", list_new_str_1
            (NULL, "../../../../../", list_new_str_1
             (NULL, "../../../../../../", list_new_str_1
              (NULL, "/usr/", list_new_str_1
               (NULL, "/usr/local/", list_new_str_1
                (NULL, "/usr/share/kc3", list_new_str_1
                 (NULL, "/usr/local/share/kc3", NULL))))))))))))));
  const char *kc3_dir = getenv("KC3_DIR");
  if (kc3_dir)
    env->path = list_new_str_1(NULL, kc3_dir, env->path);
  if (! (env->module_path = alloc(sizeof(s_str))))
    return NULL;
  str_init_1(&path, NULL, "lib/kc3/0.1/");
  if (! file_search(&path, &g_sym_x, env->module_path)) {
    if (! strncmp(LIBDIR, "lib", 3) &&
        (! LIBDIR[3] ||
         (LIBDIR[3] == '/' &&
          ! LIBDIR[4]))) {
      err_puts("env_init: lib/kc3/0.1 not found in module path");
      assert(! "env_init: lib/kc3/0.1 not found in module path");
      return NULL;
    }
    str_init_1(&path, NULL, LIBDIR "/kc3/0.1/");
    if (! file_search(&path, &g_sym_x, env->module_path)) {
      err_puts("env_init: " LIBDIR "/kc3/0.1 and lib/kc3/0.1 not "
               "found in module path");
      assert(! "env_init: " LIBDIR "/kc3/0.1 and lib/kc3/0.1 not "
             "found in module path");
      return NULL;
    }
  }
  env->current_defmodule = &g_sym_KC3;
  env->search_modules_default = list_new_psym(&g_sym_KC3, NULL);
  env->search_modules = env->search_modules_default;
  if (! (env->ops = ops_new())) {
    env_clean(env);
    return NULL;
  }
  if (! (env->counter_ht = counter_ht_new())) {
    env_clean(env);
    return NULL;
  }
  if (! env->restore_path.size &&
      ! env_dump_restore_path_resolve(env)) {
    env_clean(env);
    return NULL;
  }
  if (env->restore_path.size) {
    if (env_dump_restore(env, &env->restore_path) <= 0) {
      env_clean(env);
      return NULL;
    }
    env->loaded = true;
  }
  else {
    if (! env_module_load(env, &g_sym_KC3)) {
      env_clean(env);
      return NULL;
    }
    env->loaded = true;
    if (! env_module_load(env, sym_1("Init"))) {
      env_clean(env);
      return NULL;
    }
  }
  if (env->dump_path.size &&
      env_dump(env, &env->dump_path) <= 0) {
    env_clean(env);
    return NULL;
  }
  if (! time_init_now(&env->boot_time)) {
    env_clean(env);
    return NULL;
  }
  return env;
}

// TODO: unwind_protect
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
    err_puts("env_kc3_def: env_def");
    assert(! "env_kc3_def: env_def");
    tag_clean(&tag_value);
    return NULL;
  }
  tag_clean(&tag_value);
  tag_init_ident(dest, &tag_ident.data.ident);
  return dest;
}

s_tag * env_kc3_defcounter (s_env *env, s_call *call, s_tag *dest)
{
  s_ident *ident;
  s_tag *value;
  assert(env);
  assert(call);
  assert(dest);
  if ((call->ident.module &&
       call->ident.module != &g_sym_KC3) ||
      call->ident.sym != &g_sym__equal ||
      call->arguments->tag.type != TAG_IDENT) {
    err_puts("env_kc3_defcounter: expected Ident = value");
    assert(! "env_kc3_defcounter: expected Ident = value");
    return NULL;
  }
  ident = &call->arguments->tag.data.ident;
  value = &list_next(call->arguments)->tag;
  return env_defcounter(env, ident, value, dest);
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
  if (! frame_init(&frame, env->frame))
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
    if (map->key[i].type != TAG_PSYM) {
      tag_clean(&tmp);
      err_write_1("env_let: binding key is not a symbol: ");
      err_inspect_tag(map->key + i);
      err_write_1("\n");
      assert(! "env_let: binding key is not a symbol");
      env->frame = frame_clean(&frame);
      return NULL;
    }
    if (! frame_binding_new(&frame,
                            map->key[i].data.psym,
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

// TODO: unwind_protect
bool env_load (s_env *env, const s_str *path)
{
  s_buf buf;
  bool buf_opened = false;
  s_str cache_path = {0};
  s_time cache_mtime = {0};
  const s_str cache_suffix = STR("c");
  p_list dlopen_list_save = NULL;
  s_tag *file_dir;
  s_tag  file_dir_save;
  s_tag *file_path;
  s_tag  file_path_save;
  s_list **last;
  s_list **last_dlopen;
  p_list list = NULL;
  p_list new_dlopens = NULL;
  s_tag load_time = {0};
  s_tag now = {0};
  sw r;
  s_time src_mtime = {0};
  s_tag tag = {0};
  s_tag tmp = {0};
  p_list tmp_list = NULL;
  bool use_cache = false;
  assert(env);
  assert(path);
  if (! str_init_concatenate(&cache_path, path, &cache_suffix))
    return false;
  if (file_mtime(path, &src_mtime) &&
      file_mtime(&cache_path, &cache_mtime) &&
      compare_time(&cache_mtime, &src_mtime) > 0) {
    use_cache = true;
  }
  file_dir = frame_get_w(env->global_frame, &g_sym___DIR__);
  file_dir_save = *file_dir;
  file_path = frame_get_w(env->global_frame, &g_sym___FILE__);
  file_path_save = *file_path;
  if (! file_dirname(path, &file_dir->data.str))
    goto ko;
  tag_init_str(file_path, NULL, path->size, path->ptr.pchar);
  if (use_cache) {
    if (env->trace) {
      err_write_1("env_load: ");
      err_inspect_str(&cache_path);
      err_write_1("\n");
    }
    if (env_load_kc3c(env, &cache_path) <= 0) {
      use_cache = false;
      if (env->trace)
        err_puts("env_load: cache load failed, falling back to parse");
    }
  }
  if (! use_cache) {
    if (env->trace) {
      err_write_1("env_load: ");
      err_inspect_str(path);
      err_write_1("\n");
    }
    if (! buf_init_alloc(&buf, BUF_SIZE))
      goto ko;
    if (! buf_getc_open_r(&buf, path)) {
      buf_clean(&buf);
      goto ko;
    }
    buf_opened = true;
    dlopen_list_save = env->dlopen_list;
    last = &list;
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
      *last = list_new(NULL);
      if (! *last) {
        tag_clean(&tag);
        goto ko;
      }
      (*last)->tag = tag;
      tag = (s_tag) {0};
      last = &(*last)->next.data.plist;
    }
    buf_getc_close(&buf);
    buf_clean(&buf);
    buf_opened = false;
    last_dlopen = &new_dlopens;
    tmp_list = env->dlopen_list;
    while (tmp_list != dlopen_list_save) {
      *last_dlopen = list_new_str_copy(&tmp_list->tag.data.str, NULL);
      if (! *last_dlopen)
        goto ko;
      last_dlopen = &(*last_dlopen)->next.data.plist;
      tmp_list = list_next(tmp_list);
    }
    if (env->trace) {
      err_write_1("env_load: writing ");
      err_inspect_str(&cache_path);
      err_write_1("\n");
    }
    marshall_kc3c_file(new_dlopens, list, &cache_path);
    list_delete_all(new_dlopens);
    new_dlopens = NULL;
    list_delete_all(list);
    list = NULL;
  }
  tag_clean(file_dir);
  *file_dir = file_dir_save;
  *file_path = file_path_save;
  str_clean(&cache_path);
  tag = (s_tag) {0};
  tag.type = TAG_STR;
  tag.data.str = *path;
  tag_init_time_now(&now);
  tag_init_psym(&load_time, &g_sym_load_time);
  facts_replace_tags(env->facts, &tag, &load_time, &now);
  if (env->trace) {
    err_write_1("env_load: ");
    err_inspect_str(path);
    err_write_1(": OK\n");
  }
  return true;
 ko:
  err_write_1("env_load: ");
  err_inspect_str(path);
  err_puts(": KO");
  if (new_dlopens)
    list_delete_all(new_dlopens);
  if (list)
    list_delete_all(list);
  tag_clean(file_dir);
  *file_dir = file_dir_save;
  *file_path = file_path_save;
  if (buf_opened) {
    buf_getc_close(&buf);
    buf_clean(&buf);
  }
  str_clean(&cache_path);
  return false;
}

sw env_load_kc3c (s_env *env, const s_str *path)
{
  uw count = 0;
  s_str dlopen_str = {0};
  void *dlopen_tmp = NULL;
  uw i;
  s_marshall_read mr = {0};
  sw result = -1;
  s_tag tag = {0};
  s_tag tmp = {0};
  if (! path || ! path->size) {
    err_puts("env_load_kc3c: invalid argument");
    assert(! "env_load_kc3c: invalid argument");
    return -1;
  }
  if (! marshall_read_init_file(&mr, path)) {
    err_puts("env_load_kc3c: marshall_read_init_file");
    assert(! "env_load_kc3c: marshall_read_init_file");
    return -1;
  }
  if ((result = marshall_read_size(&mr)) <= 0) {
    err_puts("env_load_kc3c: marshall_read_size");
    assert(! "env_load_kc3c: marshall_read_size");
    goto clean;
  }
  if (! marshall_read_uw(&mr, false, &count)) {
    err_puts("env_load_kc3c: marshall_read_uw 1");
    assert(! "env_load_kc3c: marshall_read_uw 1");
    goto clean;
  }
  i = 0;
  while (i < count) {
    if (! marshall_read_str(&mr, false, &dlopen_str)) {
      err_puts("env_load_kc3c: marshall_read_str");
      assert(! "env_load_kc3c: marshall_read_str");
      goto clean;
    }
    if (! env_dlopen(env, &dlopen_str, &dlopen_tmp))
      goto clean;
    i++;
  }
  if (! marshall_read_uw(&mr, false, &count)) {
    err_puts("env_load_kc3c: marshall_read_uw 2");
    assert(! "env_load_kc3c: marshall_read_uw 2");
    goto clean;
  }
  i = 0;
  while (i < count) {
    if (! marshall_read_tag(&mr, false, &tag)) {
      err_puts("env_load_kc3c: marshall_read_tag");
      assert(! "env_load_kc3c: marshall_read_tag");
      goto clean;
    }
    if (! env_eval_tag(env, &tag, &tmp)) {
      err_puts("env_load_kc3c: env_eval_tag");
      assert(! "env_load_kc3c: env_eval_tag");
      goto clean;
    }
    tag_clean(&tmp);
    tag_clean(&tag);
    i++;
  }
  marshall_read_clean(&mr);
  return result;
 clean:
  marshall_read_clean(&mr);
  return -1;
}

void env_loop_context_pop (s_env *env, s_loop_context *lc)
{
  assert(env);
  assert(lc);
  if (env->loop_context != lc) {
    err_puts("env_loop_context_pop: mismatch");
    assert(! "env_loop_context_pop: mismatch");
    abort();
  }
  env->loop_context = lc->next;
}

void env_loop_context_push (s_env *env, s_loop_context *lc)
{
  assert(env);
  assert(lc);
  if (env->loop_context == lc) {
    err_puts("env_loop_context_push: double push");
    assert(! "env_loop_context_push: double push");
    abort();
  }
  lc->next = env->loop_context;
  env->loop_context = lc;
}

// TODO: unwind_protect (env_load)
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
  tag_init_psym(&load_time_sym, &g_sym_load_time);
  tag_init_pvar(&load_time, &g_sym_Time);
  if (! facts_with_tags(env->facts, &cursor, &path_tag, &load_time_sym,
                        &load_time)) {
    tag_clean(&load_time);
    return false;
  }
  if (! facts_cursor_next(&cursor, &fact)) {
    tag_clean(&load_time);
    return false;
  }
  if (! fact) {
    err_write_1("env_maybe_reload: no load time for ");
    err_inspect_str(path);
    err_write_1("\n");
    assert(! "env_maybe_reload: no load time");
    tag_clean(&load_time);
    return false;
  }
  if (load_time.data.pvar->tag.type != TAG_TIME)
    abort();
  mtime.type = TAG_TIME;
  if (! file_mtime(path, &mtime.data.time)) {
    facts_cursor_clean(&cursor);
    return false;
  }
  r = true;
  if (compare_tag(&load_time.data.pvar->tag, &mtime) == COMPARE_LT)
    r = env_load(env, path);
  facts_cursor_clean(&cursor);
  tag_clean(&load_time);
  return r;
}

void env_longjmp (s_env *env, jmp_buf *jmp_buf)
{
  s_unwind_protect *up;
  up = env->unwind_protect;
  if (up && up->buf < *jmp_buf) {
    while (up->next && up->next->buf < *jmp_buf) {
      up->jmp = &up->next->buf;
      up = up->next;
    }
    up->jmp = jmp_buf;
    longjmp(env->unwind_protect->buf, 1);
    abort();
  }
  longjmp(*jmp_buf, 1);
  abort();
}

const s_sym ** env_module (s_env *env, const s_sym **dest)
{
  assert(env);
  assert(dest);
  assert(env->current_defmodule);
  *dest = env->current_defmodule;
  return dest;
}

// TODO: unwind_protect
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
  tag_init_psym(&tag_is_a, &g_sym_is_a);
  tag_init_psym(&tag_module, &g_sym_module);
  tag_init_psym(&tag_module_name, module);
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
  s_tag tag_pvar;
  s_tag tag_sym_value;
  s_tag tag_sym_sym;
  s_tag tag_symbol;
  tag_init_ident(&tag_ident, ident);
  tag_init_psym( &tag_module_name, module);
  tag_init_psym( &tag_op, &g_sym_op);
  tag_init_psym( &tag_sym_value, ident->sym);
  tag_init_psym( &tag_symbol, &g_sym_symbol);
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
  tag_init_psym(&tag_sym_sym, &g_sym_sym);
  tag_init_pvar(&tag_pvar, &g_sym_Ident);
  if (! facts_with(env->facts, &cursor, (t_facts_spec) {
        &tag_module_name, &tag_op, &tag_pvar, NULL,
        &tag_pvar, &tag_sym_sym, &tag_sym_value, NULL, NULL})) {
    err_puts("env_module_has_ident: facts_with");
    assert(! "env_module_has_ident: facts_with");
    tag_clean(&tag_pvar);
    return NULL;
  }
  if (! facts_with_cursor_next(&cursor, &fact)) {
    err_puts("env_module_has_ident: facts_with_cursor_next");
    assert(! "env_module_has_ident: facts_with_cursor_next");
    tag_clean(&tag_pvar);
    return NULL;
  }
  *dest = fact ? true : false;
  facts_with_cursor_clean(&cursor);
  tag_clean(&tag_pvar);
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
  tag_init_psym(&tag_module, module);
  tag_init_psym(&tag_is_loading, &g_sym_is_loading);
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
  tag_init_psym(&tag_module, module);
  tag_init_psym(&tag_is_loading, &g_sym_is_loading);
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

// TODO: unwind_protect
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
  if (env->trace) {
    err_write_1("env_module_load: ");
    err_inspect_sym(module);
    err_write_1(":\n");
  }
  if (! env_module_is_loading(env, module, &b))
    return false;
  if (b)
    return true;
  if (! facts_transaction_start(env->facts, &transaction))
    return false;
  if (! env_module_is_loading_set(env, module, true))
    goto rollback;
  if (module_path(module, env->module_path, KC3_EXT, &path) &&
      file_access(&path, &g_sym_r)) {
    tag_init_time_now(&tag_time);
    if (! env_load(env, &path)) {
      err_write_1("env_module_load: ");
      err_inspect_sym(module);
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
    if (! file_access(&path, &g_sym_r)) {
      goto rollback;
    }
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
  tag_init_psym(&tag_module_name, module);
  tag_init_psym(&tag_load_time, &g_sym_load_time);
  if (! facts_replace_tags(env->facts, &tag_module_name,
                           &tag_load_time, &tag_time)) {
    goto rollback;
  }
  tag_clean(&tag_time);
  env_module_is_loading_set(env, module, false);
  facts_transaction_end(env->facts, &transaction);
  if (env->trace) {
    err_write_1("env_module_load: ");
    err_inspect_sym(module);
    err_write_1(": OK\n");
  }
  return true;
 rollback:
  if (env->trace) {
    err_write_1("env_module_load: ");
    err_inspect_sym(module);
    err_write_1(": KO\n");
  }
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
  s_tag tag_time_pvar;
  tag_init_psym(&tag_module_name, module);
  tag_init_psym(&tag_load_time, &g_sym_load_time);
  tag_init_pvar(&tag_time_pvar, &g_sym_Time);
  if (! facts_with(env->facts, &cursor, (t_facts_spec) {
        &tag_module_name, &tag_load_time, &tag_time_pvar,
        NULL, NULL })) {
    tag_clean(&tag_time_pvar);
    return NULL;
  }
  if (! facts_with_cursor_next(&cursor, &fact)) {
    facts_with_cursor_clean(&cursor);
    tag_clean(&tag_time_pvar);
    return NULL;
  }
  if (! fact) {
    *dest = NULL;
    facts_with_cursor_clean(&cursor);
    tag_clean(&tag_time_pvar);
    return dest;
  }
  if (fact->object->type != TAG_TIME) {
    err_write_1("env_module_load_time: module ");
    err_inspect_sym(module);
    err_puts(" load time is not a %Time{}");
    assert(! "env_module_load_time: module load time is not a %Time{}");
    facts_with_cursor_clean(&cursor);
    tag_clean(&tag_time_pvar);
    return NULL;
  }
  *dest = &fact->object->data.time;
  facts_with_cursor_clean(&cursor);
  tag_clean(&tag_time_pvar);
  return dest;
}

// TODO: unwind_protect
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
  s_list *tmp = NULL;
  s_list *tmp2 = NULL;
  assert(env);
  (void) env;
  if (! module || ! *module) {
    err_puts("env_module_search_modules: NULL module");
    assert(! "env_module_search_modules: NULL module");
    return NULL;
  }
  if (! (tmp = list_new_psym(&g_sym_KC3, NULL)))
    return NULL;
  if (*module == &g_sym_KC3) {
    *dest = tmp;
    return dest;
  }
  tmp2 = list_new_psym(*module, tmp);
  if (! tmp2) {
    list_delete(tmp);
    return NULL;
  }
  *dest = tmp2;
  return dest;
}

// TODO: unwind_protect
s_tag * env_or (s_env *env, s_tag *a, s_tag *b, s_tag *dest)
{
  s_tag eval = {0};
  bool p = false;
  bool silence_errors;
  const s_sym *sym_Bool = &g_sym_Bool;
  assert(env);
  assert(a);
  assert(b);
  assert(dest);
  silence_errors = env->silence_errors;
  env->silence_errors = true;
  if (env_eval_tag(env, a, &eval)) {
    bool_init_cast(&p, &sym_Bool, &eval);
  }
  env->silence_errors = silence_errors;
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

s_list ** env_search_modules (s_env *env, s_list **dest)
{
  assert(env);
  assert(dest);
  assert(env->search_modules);
  assert(env->search_modules->tag.type == TAG_PSYM);
  return plist_init_copy(dest, &env->search_modules);
}

s_list ** env_stacktrace (s_env *env, s_list **dest)
{
  assert(env);
  assert(dest);
  *dest = list_new_copy_all(env->stacktrace);
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
    if (search_module->tag.type != TAG_PSYM ||
        ! search_module->tag.data.psym) {
      err_write_1("env_sym_search_modules: ");
      err_inspect_sym(sym);
      err_puts(": invalid env->search_modules");
      assert(! "env_sym_search_modules: invalid env->search_modules");
      return false;
    }
    module = search_module->tag.data.psym;
    if (! env_module_has_symbol(env, module, sym, &b)) {
      err_puts("env_sym_search_modules: env_module_has_symbol");
      assert(! "env_sym_search_modules: env_module_has_symbol");
      return false;
    }
    if (b) {
      *dest = module;
      if (false) {
        err_write_1("env_sym_search_modules: ");
        err_inspect_sym(sym);
        err_write_1(": search_module: ");
        err_inspect_sym(module);
        err_write_1(" -> Ok\n");
      }
      return true;
    }
    if (false) {
      err_write_1("env_sym_search_modules: ");
      err_inspect_sym(sym);
      err_write_1(": search_module: ");
      err_inspect_sym(module);
      err_write_1(" ");
      err_inspect_list(search_module);
      err_puts(" -> not found");
    }
    search_module = list_next(search_module);
  }
  if (false) {
    err_write_1("env_sym_search_modules: ");
    err_inspect_sym(sym);
    err_write_1(": search_module: ");
    err_inspect_list(env->search_modules);
    err_write_1(" -> false\n");
  }
  *dest = NULL;
  return true;
}

s8 env_special_operator_arity (s_env *env, const s_ident *ident)
{
  s8 arity;
  s_facts_cursor cursor;
  s_fact *fact;
  s_tag tag_arity;
  s_tag tag_ident;
  s_tag tag_pvar;
  assert(env);
  assert(ident);
  tag_ident.type = TAG_IDENT;
  env_ident_resolve_module(env, ident, &tag_ident.data.ident);
  tag_init_psym(&tag_arity, &g_sym_arity);
  tag_init_pvar(&tag_pvar, &g_sym_U8);
  if (! facts_with_tags(env->facts, &cursor,
                        &tag_ident, &tag_arity, &tag_pvar)) {
    tag_clean(&tag_pvar);
    return -1;
  }
  if (! facts_cursor_next(&cursor, &fact)) {
    tag_clean(&tag_pvar);
    return -1;
  }
  if (fact) {
    if (tag_pvar.data.pvar->tag.type != TAG_U8 ||
        tag_pvar.data.pvar->tag.data.u8 > S8_MAX) {
      err_write_1("env_special_operator_arity: "
                  "invalid arity for special operator ");
      err_inspect_ident(&tag_ident.data.ident);
      err_write_1("\n");
      facts_cursor_clean(&cursor);
      tag_clean(&tag_pvar);
      return -1;
    }
    arity = (s8) tag_pvar.data.pvar->tag.data.u8;
    facts_cursor_clean(&cursor);
    tag_clean(&tag_pvar);
    return arity;
  }
  facts_cursor_clean(&cursor);
  tag_clean(&tag_pvar);
  err_write_1("env_special_operator_arity: "
              "arity not found for special operator ");
  err_inspect_ident(&tag_ident.data.ident);
  err_write_1("\n");
  return -1;
}

bool * env_struct_type_exists (s_env *env, const s_sym *module,
                               bool *dest)
{
  s_facts_cursor cursor;
  s_fact *fact;
  s_tag tag_struct_type;
  s_tag tag_module;
  s_tag tag_pvar;
  assert(env);
  assert(module);
  assert(dest);
  /* too slow, use require
  if (! env_module_maybe_reload(env, module))
    return NULL;
  */
  tag_init_psym(&tag_module, module);
  tag_init_psym(&tag_struct_type, &g_sym_struct_type);
  tag_init_pvar(&tag_pvar, &g_sym_Tag);
  if (! facts_with_tags(env->facts, &cursor, &tag_module,
                        &tag_struct_type, &tag_pvar)) {
    tag_clean(&tag_pvar);
    return NULL;
  }
  if (! facts_cursor_next(&cursor, &fact)) {
    tag_clean(&tag_pvar);
    return NULL;
  }
  *dest = fact ? true : false;
  facts_cursor_clean(&cursor);
  tag_clean(&tag_pvar);
  return dest;
}

p_struct_type * env_pstruct_type_find (s_env *env,
                                       const s_sym *module,
                                       p_struct_type *dest)
{
  s_facts_cursor cursor;
  s_fact *found;
  s_tag tag_struct_type;
  s_tag tag_module;
  s_tag tag_pvar;
  const s_sym *type;
  assert(env);
  assert(module);
  /* too slow
  if (! env_module_maybe_reload(env, module)) {
    err_write_1("env_pstruct_type_find: env_module_maybe_reload(");
    err_inspect_sym(module);
    err_puts(")");
    assert(! "env_pstruct_type_find: env_module_maybe_reload");
    return NULL;
  }
  */
  tag_init_psym(&tag_module, module);
  tag_init_psym(&tag_struct_type, &g_sym_struct_type);
  tag_init_pvar(&tag_pvar, &g_sym_StructType);
  if (! facts_with_tags(env->facts, &cursor,
                        &tag_module, &tag_struct_type, &tag_pvar)) {
    err_write_1("env_pstruct_type_find: facts_with_tags(");
    err_inspect_sym(module);
    err_puts(", :struct_type, ?)");
    assert(! "env_pstruct_type_find: facts_with");
    tag_clean(&tag_pvar);
    return NULL;
  }
  if (! facts_cursor_next(&cursor, &found)) {
    err_puts("env_pstruct_type_find: facts_with_cursor_next");
    assert(! "env_pstruct_type_find: facts_with_cursor_next");
    tag_clean(&tag_pvar);
    return NULL;
  }
  if (! found) {
    facts_cursor_clean(&cursor);
    tag_clean(&tag_pvar);
    *dest = NULL;
    return dest;
  }
  if (found->object->type != TAG_PSTRUCT_TYPE) {
    tag_type(found->object, &type);
    err_write_1("env_pstruct_type_find: module ");
    err_inspect_sym(module);
    err_write_1(" :struct_type is actually a ");
    err_inspect_sym(type);
    err_write_1("\n");
    assert(! "env_pstruct_type_find: invalid struct_type");
    facts_cursor_clean(&cursor);
    tag_clean(&tag_pvar);
    return NULL;
  }
  *dest = found->object->data.pstruct_type;
  facts_cursor_clean(&cursor);
  tag_clean(&tag_pvar);
  return dest;
}

p_callable env_struct_type_get_clean (s_env *env, const s_sym *module)
{
  s_facts_with_cursor cursor;
  s_fact *found;
  s_tag clean;
  s_tag tag_module;
  s_tag tag_pvar;
  p_callable tmp = NULL;
  const s_sym *type;
  tag_init_psym(&tag_module, module);
  tag_init_psym(&clean, &g_sym_clean);
  tag_init_pvar(&tag_pvar, &g_sym_Tag);
  facts_with(env->facts, &cursor, (t_facts_spec) {
      &tag_module, &clean, &tag_pvar, NULL, NULL });
  if (! facts_with_cursor_next(&cursor, &found)) {
    tag_clean(&tag_pvar);
    return NULL;
  }
  if (! found) {
    facts_with_cursor_clean(&cursor);
    tag_clean(&tag_pvar);
    return NULL;
  }
  if (found->object->type != TAG_PCALLABLE) {
    tag_type(found->object, &type);
    err_write_1("env_struct_type_get_clean: ");
    err_inspect_sym(module);
    err_write_1(": clean is actually a ");
    err_inspect_sym(type);
    err_write_1(", it should be a Callable.\n");
    assert(! "env_struct_type_get_clean: clean should be a Callable");
    facts_with_cursor_clean(&cursor);
    tag_clean(&tag_pvar);
    return NULL;
  }
  if (callable_arity(found->object->data.pcallable) != 1) {
    err_write_1("env_struct_type_get_clean: ");
    err_inspect_sym(module);
    err_write_1(": clean arity is ");
    err_inspect_u8(callable_arity(found->object->data.pcallable));
    err_write_1(", it should be 1.\n");
    assert(! "env_struct_type_get_clean: invalid arity");
    facts_with_cursor_clean(&cursor);
    tag_clean(&tag_pvar);
    return NULL;
  }
  if (! pcallable_init_copy(&tmp, &found->object->data.pcallable)) {
    err_write_1("env_struct_type_get_clean: ");
    err_inspect_sym(module);
    err_puts(": pcallable_init_copy");
    assert(! "env_struct_type_get_clean: pcallable_init_copy");
    facts_with_cursor_clean(&cursor);
    tag_clean(&tag_pvar);
    return NULL;
  }
  facts_with_cursor_clean(&cursor);
  tag_clean(&tag_pvar);
  return tmp;
}

s_list ** env_struct_type_get_spec (s_env *env,
                                    const s_sym *module,
                                    s_list **dest)
{
  s_fact *found;
  s_tag tag_defstruct;
  s_tag tag_module;
  s_tag tag_pvar;
  s_tag tmp = {0};
  assert(env);
  assert(module);
  assert(dest);
  tag_init_psym(&tag_defstruct, &g_sym_defstruct);
  tag_init_psym(&tag_module, module);
  tag_init_pvar(&tag_pvar, &g_sym_Tag);
  if (! facts_find_fact_by_tags(env->facts, &tag_module,
                                &tag_defstruct, &tag_pvar, &found)) {
    tag_clean(&tag_pvar);
    return NULL;
  }
  if (! found) {
    err_write_1("env_struct_type_get_spec: ");
    err_inspect_sym(module);
    err_puts(": defstruct not found");
    assert(! "env_struct_type_get_spec: defstruct not found");
    tag_clean(&tag_pvar);
    return NULL;
  }
  if (! env_eval_tag(env, &tag_pvar, &tmp)) {
    tag_clean(&tag_pvar);
    return NULL;
  }
  if (tmp.type != TAG_PLIST ||
      ! list_is_plist(tmp.data.plist)) {
    err_write_1("env_struct_type_get_spec: module ");
    err_write_1(module->str.ptr.pchar);
    err_puts(" has a defstruct that is not a property list");
    tag_clean(&tmp);
    tag_clean(&tag_pvar);
    return NULL;
  }
  *dest = tmp.data.plist;
  tag_clean(&tag_pvar);
  return dest;
}

bool * env_struct_type_has_spec (s_env *env, const s_sym *module,
                                 bool *dest)
{
  s_facts_cursor cursor;
  s_fact *fact;
  s_tag tag_defstruct;
  s_tag tag_module;
  s_tag tag_pvar;
  assert(env);
  assert(module);
  tag_init_psym(&tag_defstruct, &g_sym_defstruct);
  tag_init_psym(&tag_module, module);
  tag_init_pvar(&tag_pvar, &g_sym_Tag);
  if (! facts_with_tags(env->facts, &cursor, &tag_module,
                        &tag_defstruct, &tag_pvar)) {
    tag_clean(&tag_pvar);
    return NULL;
  }
  if (! facts_cursor_next(&cursor, &fact)) {
    tag_clean(&tag_pvar);
    return NULL;
  }
  facts_cursor_clean(&cursor);
  *dest = fact ? true : false;
  tag_clean(&tag_pvar);
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

void env_toplevel_clean (s_env *env)
{
  frame_clean(&env->toplevel_frame);
}

s_env * env_toplevel_init (s_env *env)
{
  assert(! env->frame);
  if (! frame_init(&env->toplevel_frame, NULL))
    return NULL;
  env->frame = &env->toplevel_frame;
  return env;
}

s_tag * env_unwind_protect (s_env *env, s_tag *protected_,
                            s_tag *cleanup,
                            s_tag *dest)
{
  s_tag * volatile dest_v = dest;
  s_tag discard = {0};
  s_tag tmp = {0};
  s_unwind_protect up;
  assert(env);
  assert(protected_);
  env_unwind_protect_push(env, &up);
  if (setjmp(up.buf)) {
    env_unwind_protect_pop(env, &up);
    env_eval_tag(env, cleanup, &discard);
    tag_clean(&discard);
    longjmp(*up.jmp, 1);
  }
  if (! env_eval_tag(env, protected_, &tmp)) {
    env_unwind_protect_pop(env, &up);
    env_eval_tag(env, cleanup, &discard);
    tag_clean(&discard);
    return NULL;
  }
  env_unwind_protect_pop(env, &up);
  env_eval_tag(env, cleanup, &discard);
  tag_clean(&discard);
  *dest_v = tmp;
  return dest_v;
}

void env_unwind_protect_pop (s_env *env, s_unwind_protect *up)
{
  assert(env);
  assert(up);
  if (env->unwind_protect != up) {
    err_puts("env_unwind_protect_pop: mismatch");
    assert(! "env_unwind_protect_pop: mismatch");
    abort();
  }
  env->unwind_protect = up->next;
}

void env_unwind_protect_push (s_env *env,
                              s_unwind_protect *up)
{
  assert(env);
  assert(up);
  if (env->unwind_protect == up) {
    err_puts("env_unwind_protect_push: deadlock");
    assert(! "env_unwind_protect_push: deadlock");
    abort();
  }
  up->jmp = NULL;
  up->next = env->unwind_protect;
  env->unwind_protect = up;
}

s_time * env_uptime (s_env *env, s_time *dest)
{
  s_time now = {0};
  assert(env);
  assert(dest);
  if (! time_init_now(&now))
    return NULL;
  return time_sub(&now, &env->boot_time, dest);
}

s_tag * env_while (s_env *env, s_tag *cond, s_tag *body,
                   s_tag *dest)
{
  s_tag  cond_bool = {0};
  s_call cond_cast = {0};
  s_tag * volatile dest_v = dest;
  s_loop_context loop_context = {0};
  s_tag tmp = {0};
  assert(env);
  assert(cond);
  assert(body);
  assert(dest_v);
  call_init_call_cast(&cond_cast, &g_sym_Bool);
  if (! tag_init_copy(&list_next(cond_cast.arguments)->tag, cond))
    goto ko;
  env_loop_context_push(env, &loop_context);
  env_unwind_protect_push(env, &loop_context.up);
  if (setjmp(loop_context.up.buf)) {
    tag_clean(&tmp);
    env_unwind_protect_pop(env, &loop_context.up);
    env_loop_context_pop(env, &loop_context);
    call_clean(&cond_cast);
    longjmp(*loop_context.up.jmp, 1);
  }
  if (setjmp(loop_context.break_buf))
    goto ok;
  while (1) {
    if (! env_eval_call(env, &cond_cast, &cond_bool))
      goto ko;
    if (cond_bool.type != TAG_BOOL)
      goto ko;
    if (! cond_bool.data.bool_)
      break;
    tag_clean(&tmp);
    tag_init(&tmp);
    if (setjmp(loop_context.continue_buf))
      continue;
    if (! env_eval_tag(env, body, &tmp))
      goto ko;
  }
  ok:
  env_unwind_protect_pop(env, &loop_context.up);
  env_loop_context_pop(env, &loop_context);  
  call_clean(&cond_cast);
  *dest_v = tmp;
  return dest_v;
 ko:
  env_unwind_protect_pop(env, &loop_context.up);
  tag_clean(&tmp);
  env_loop_context_pop(env, &loop_context);  
  call_clean(&cond_cast);
  return NULL;
}

#if defined(WIN32) || defined(WIN64)

# include <stdlib.h>
# include <limits.h>

char * realpath(const char *path, char *resolved_path) {
  char *result;
  if (resolved_path == NULL) {
    resolved_path = malloc(PATH_MAX);
    if (!resolved_path) return NULL;
  }
  result = _fullpath(resolved_path, path, PATH_MAX);
  if (! result && resolved_path) {
    free(resolved_path);
  }
  return result;
}

#endif
