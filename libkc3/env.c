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
#include <string.h>
#include <unistd.h>
#include "alloc.h"
#include "array.h"
#include "assert.h"
#include "binding.h"
#include "block.h"
#include "buf.h"
#include "buf_file.h"
#include "buf_getc.h"
#include "buf_inspect.h"
#include "buf_parse.h"
#include "buf_save.h"
#include "call.h"
#include "cfn.h"
#include "compare.h"
#include "complex.h"
#include "cow.h"
#include "data.h"
#include "env.h"
#include "error.h"
#include "error_handler.h"
#include "facts.h"
#include "facts_cursor.h"
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
#include "str.h"
#include "struct.h"
#include "struct_type.h"
#include "sym.h"
#include "tag.h"
#include "tag_init.h"
#include "tuple.h"

s_env g_kc3_env;

static void env_clean_globals (s_env *env);
static void env_clean_toplevel (s_env *env);
static s_env * env_init_args (s_env *env, int *argc, char ***argv);
static s_env * env_init_globals (s_env *env);
static s_env * env_init_toplevel (s_env *env);

bool env_call_get (s_env *env, s_call *call)
{
  s_facts_cursor cursor;
  const s_fact *fact;
  s_tag tag_ident;
  s_tag tag_is_a;
  s_tag tag_macro;
  s_tag tag_module_name;
  s_tag tag_operator;
  s_tag tag_special_operator;
  s_tag tag_sym;
  s_tag tag_symbol;
  s_tag tag_symbol_value;
  s_tag tag_var;
  tag_init_ident(&tag_ident, &call->ident);
  tag_init_sym(  &tag_is_a, &g_sym_is_a);
  tag_init_sym(  &tag_macro, &g_sym_macro);
  tag_init_sym(  &tag_module_name, call->ident.module);
  tag_init_sym(  &tag_operator, &g_sym_operator);
  tag_init_sym(  &tag_special_operator, &g_sym_special_operator);
  tag_init_sym(  &tag_sym, call->ident.sym);
  tag_init_sym(  &tag_symbol, &g_sym_symbol);
  tag_init_sym(  &tag_symbol_value, &g_sym_symbol_value);
  tag_init_var(  &tag_var, &g_sym_Tag);
  if (! facts_find_fact_by_tags(&env->facts, &tag_module_name,
                                &tag_symbol, &tag_ident, &fact))
    return false;
  if (! fact) {
    if (! facts_find_fact_by_tags(&env->facts, &tag_module_name,
                                  &tag_operator, &tag_ident, &fact))
      return false;
    if (! fact) {
      err_write_1("env_call_get: symbol ");
      err_write_1(call->ident.sym->str.ptr.pchar);
      err_write_1(" not found in module ");
      err_write_1(call->ident.module->str.ptr.pchar);
      err_write_1("\n");
      return false;
    }
  }
  if (! facts_with_tags(&env->facts, &cursor, &tag_ident,
                        &tag_symbol_value, &tag_var))
    return false;
  if (! facts_cursor_next(&cursor, &fact))
    return false;
  if (fact) {
    if (tag_var.type == TAG_FN)
      call->fn = fn_new_copy(&tag_var.data.fn);
    else if (tag_var.type == TAG_CFN)
      call->cfn = cfn_new_copy(&tag_var.data.cfn);
    else {
      err_write_1("env_call_get: ");
      err_inspect_ident(&call->ident);
      err_puts(" is not a function");
      facts_cursor_clean(&cursor);
      return false;
    }
    facts_cursor_clean(&cursor);
  }
  if (! facts_find_fact_by_tags(&env->facts, &tag_ident, &tag_is_a,
                                &tag_macro, &fact))
    return false;
  if (fact) {
    if (call->fn)
      call->fn->macro = true;
    if (call->cfn)
      call->cfn->macro = true;
  }
  if (! facts_find_fact_by_tags(&env->facts, &tag_ident, &tag_is_a,
                                &tag_special_operator, &fact))
    return false;
  if (fact) {
    if (call->fn)
      call->fn->special_operator = true;
    if (call->cfn)
      call->cfn->special_operator = true;
  }
  return true;
}

void env_clean (s_env *env)
{
  assert(env);
  //facts_save_file(&env->facts, "debug.facts"); // debug
  env_clean_globals(env);
  env_clean_toplevel(env);
  error_handler_delete_all(env->error_handler);
  facts_clean(&env->facts);
  buf_file_close(&env->in);
  buf_clean(&env->in);
  buf_file_close(&env->out);
  buf_clean(&env->out);
  buf_file_close(&env->err);
  buf_clean(&env->err);
  str_clean(&env->argv0_dir);
  str_clean(&env->module_path);
  list_delete_all(env->path);
  list_delete_all(env->search_modules_default);
}

void env_clean_globals (s_env *env)
{
  frame_clean(&env->global_frame);
}

void env_clean_toplevel (s_env *env)
{
  frame_delete_all(env->frame);
}

s_tag * env_def (s_env *env, const s_call *call, s_tag *dest)
{
  s_ident *ident;
  s_struct *s;
  s_tag tag_ident;
  s_tag tag_module;
  s_tag tag_symbol;
  s_tag tag_symbol_value;
  s_tag tag_value;
  (void) env;
  assert(env);
  assert(call);
  assert(dest);
  if (call->ident.module != &g_sym_KC3 ||
      call->ident.sym != &g_sym_operator_equal ||
      call->arguments->tag.type != TAG_IDENT ||
      ! list_next(call->arguments) ||
      list_next(list_next(call->arguments))) {
    err_puts("env_def: invalid assignment: expected Ident = value");
    assert(! "env_def: invalid assignment: expected Ident = value");
    return NULL;
  }
  ident = &call->arguments->tag.data.ident;
  tag_ident.type = TAG_IDENT;
  tag_ident.data.ident.sym = ident->sym;
  if (ident->module)
    tag_ident.data.ident.module = ident->module;
  else
    tag_ident.data.ident.module = env->current_defmodule;
  tag_init_sym(&tag_module, tag_ident.data.ident.module);
  tag_init_sym(&tag_symbol, &g_sym_symbol);
  if (! facts_add_tags(&env->facts, &tag_module, &tag_symbol,
                       &tag_ident))
    return NULL;
  if (! env_eval_tag(env, &list_next(call->arguments)->tag,
                     &tag_value)) {
    err_puts("env_def: env_eval_tag");
    assert(! "env_def: env_eval_tag");
    return NULL;
  }
  if (tag_value.type == TAG_STRUCT &&
      (s = &tag_value.data.struct_) &&
      s->type->module == &g_sym_KC3__Operator) {
    if (! env_defoperator(env, &tag_ident.data.ident.sym,
                          struct_get_sym(&tag_value.data.struct_,
                                         &g_sym_sym),
                          struct_get_tag(&tag_value.data.struct_,
                                         &g_sym_symbol_value),
                          struct_get_u8(&tag_value.data.struct_,
                                        &g_sym_operator_precedence),
                          struct_get_sym(&tag_value.data.struct_,
                                         &g_sym_operator_associativity),
                          dest)) {
      tag_clean(&tag_value);
      return NULL;
    }
  }
  else {
    tag_init_sym(&tag_symbol_value, &g_sym_symbol_value);
    if (! facts_replace_tags(&env->facts, &tag_ident, &tag_symbol_value,
                             &tag_value)) {
      tag_clean(&tag_value);
      return NULL;
    }
    if (tag_ident.data.ident.module == env->current_defmodule &&
        tag_ident.data.ident.sym == &g_sym_clean) {
      if (! env_def_clean(env, env->current_defmodule, &tag_value)) {
        tag_clean(&tag_value);
        return NULL;
      }
    }
  }
  tag_clean(&tag_value);
  tag_init_ident(dest, &tag_ident.data.ident);
  return dest;
}

const s_sym * env_def_clean (s_env *env, const s_sym *module,
                             const s_tag *clean)
{
  const s_struct_type *st;
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
  if (clean->type != TAG_CFN) {
    err_write_1("env_def_clean: module ");
    err_inspect_sym(&module);
    err_write_1(": clean method must be a Cfn");
    assert(! "env_def_clean: module clean method must be a Cfn");
    return NULL;
  }
  tag_init_sym(&tag_module_name, module);
  tag_init_struct_type_update_clean(&tag_st, st, &clean->data.cfn);
  tag_init_sym(&tag_struct_type, &g_sym_struct_type);
  if (! facts_replace_tags(&env->facts, &tag_module_name,
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
  tag_init_sym(&tag_is_a, &g_sym_is_a);
  tag_init_sym(&tag_module, &g_sym_module);
  tag_init_sym(&tag_module_name, *name);
  if (! facts_add_tags(&env->facts, &tag_module_name, &tag_is_a,
                       &tag_module))
    goto clean;
  if (! env_module_search_modules(env, name, &env->search_modules))
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

s_tag * env_defoperator (s_env *env, const s_sym * const *name,
                         const s_sym * const *sym,
                         const s_tag *symbol_value,
                         u8 op_precedence,
                         const s_sym * const *op_assoc,
                         s_tag *dest)
{
  s8 arity;
  s_tag tag_module_name;
  s_tag tag_operator;
  s_tag tag_ident;
  s_tag tag_is_a;
  s_tag tag_symbol;
  s_tag tag_sym;
  s_tag tag_arity_sym;
  s_tag tag_arity_u8;
  s_tag tag_symbol_value;
  s_tag tag_op_precedence_sym;
  s_tag tag_op_precedence_u8;
  s_tag tag_op_assoc_rel;
  s_tag tag_op_assoc_value;
  tag_init_sym(&tag_module_name, env->current_defmodule);
  tag_init_sym(&tag_operator, &g_sym_operator);
  tag_ident.type = TAG_IDENT;
  tag_ident.data.ident.module = env->current_defmodule;
  tag_ident.data.ident.sym = *name;
  tag_init_sym(&tag_is_a, &g_sym_is_a);
  tag_init_sym(&tag_symbol, &g_sym_symbol);
  tag_init_sym(&tag_sym, *sym);
  tag_init_sym(&tag_arity_sym, &g_sym_arity);
  arity = tag_arity(symbol_value);
  if (arity < 1) {
    err_write_1("env_defoperator: invalid arity: ");
    err_inspect_s8(&arity);
    err_write_1("\n");
    assert(! "env_defoperator: invalid arity");
  };
  tag_init_u8( &tag_arity_u8, arity);
  tag_init_sym(&tag_symbol_value, &g_sym_symbol_value);
  tag_init_sym(&tag_op_precedence_sym,
               &g_sym_operator_precedence);
  tag_init_u8( &tag_op_precedence_u8, op_precedence);
  tag_init_sym(&tag_op_assoc_rel,
               &g_sym_operator_associativity);
  tag_init_sym(&tag_op_assoc_value,
               *op_assoc);
  facts_add_tags(&env->facts, &tag_module_name, &tag_operator,
                 &tag_ident);
  facts_replace_tags(&env->facts, &tag_ident, &tag_is_a, &tag_operator);
  facts_replace_tags(&env->facts, &tag_ident, &tag_symbol, &tag_sym);
  facts_replace_tags(&env->facts, &tag_ident, &tag_arity_sym,
                     &tag_arity_u8);
  facts_replace_tags(&env->facts, &tag_ident, &tag_symbol_value,
                     symbol_value);
  facts_replace_tags(&env->facts, &tag_ident,
                     &tag_op_precedence_sym,
                     &tag_op_precedence_u8);
  facts_replace_tags(&env->facts, &tag_ident,
                     &tag_op_assoc_rel,
                     &tag_op_assoc_value);
  *dest = tag_ident;
  return dest;
}

const s_sym * env_defstruct (s_env *env, const s_list *spec)
{
  s_tag tag_module_name;
  s_tag tag_st;
  s_tag tag_struct_type;
  tag_init_sym(&tag_module_name, env->current_defmodule);
  tag_init_struct_type(&tag_st, env->current_defmodule, spec);
  tag_init_sym(&tag_struct_type, &g_sym_struct_type);
  if (! facts_replace_tags(&env->facts, &tag_module_name,
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

void env_error_tag (s_env *env, const s_tag *tag)
{
  s_error_handler *error_handler;
  assert(env);
  assert(tag);
  error_handler = env->error_handler;
  if (error_handler) {
    tag_init_copy(&error_handler->tag, tag);
    error_handler->backtrace = env->backtrace;
    env_longjmp(env, &error_handler->jmp_buf);
    /* never reached */
    return;
  }
  if (buf_file_is_open(&env->err)) {
    buf_write_1(&env->err, "error: ");
    buf_inspect_tag(&env->err, tag);
    buf_write_1(&env->err, "\n");
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
  bool r;
  s_tag tmp;
  assert(env);
  assert(block);
  assert(dest);
  if (! block->count) {
    tag_init_void(dest);
    return true;
  }
  while (i < block->count - 1) {
    r = env_eval_tag(env, block->tag + i, &tmp);
    tag_clean(&tmp);
    if (! r)
      return false;
    i++;
  }
  return env_eval_tag(env, block->tag + i, dest);
}

bool env_eval_call (s_env *env, const s_call *call, s_tag *dest)
{
  s_call c;
  bool result;
  assert(env);
  assert(call);
  assert(dest);
  (void) call;
  call_init_copy(&c, call);
  env_eval_call_resolve(env, &c);
  if (c.cfn)
    result = env_eval_call_cfn(env, &c, dest);
  else if (c.fn)
    result = env_eval_call_fn(env, &c, dest);
  else {
    err_write_1("env_eval_call: could not resolve call ");
    err_inspect_ident(&c.ident);
    err_write_1("\n");
    result = false;
  }
  call_clean(&c);
  return result;
}

// FIXME: better error message (cite call function name if any)
bool env_eval_call_arguments (s_env *env, const s_list *args,
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

bool env_eval_call_cfn (s_env *env, const s_call *call, s_tag *dest)
{
  s_list *args = NULL;
  s_list *args_final = NULL;
  s_cfn *cfn;
  s_frame frame;
  s_tag tag;
  assert(env);
  assert(call);
  assert(dest);
  cfn = call->cfn;
  assert(cfn);
  if (! frame_init(&frame, env->frame))
    return false;
  env->frame = &frame;
  if (call->arguments) {
    if (cfn->macro || cfn->special_operator)
      args_final = call->arguments;
    else {
      if (! env_eval_call_arguments(env, call->arguments, &args)) {
        env->frame = frame_clean(&frame);
        return false;
      }
      args_final = args;
    }
  }
  if (! cfn_apply(cfn, args_final, &tag)) {
    list_delete_all(args);
    env->frame = frame_clean(&frame);
    return false;
  }
  *dest = tag;
  list_delete_all(args);
  env->frame = frame_clean(&frame);
  return true;
}

bool env_eval_call_fn (s_env *env, const s_call *call, s_tag *dest)
{
  assert(env);
  assert(call);
  assert(dest);
  return env_eval_call_fn_args(env, call->fn, call->arguments, dest);
}

bool env_eval_call_fn_args (s_env *env, const s_fn *fn,
                            const s_list *arguments, s_tag *dest)
{
  s_list *args = NULL;
  const s_list *args_final = NULL;
  s_fn_clause *clause;
  s_frame frame;
  s_list *search_modules;
  s_tag tag;
  s_list *tmp = NULL;
  assert(env);
  assert(fn);
  assert(dest);
  search_modules = env->search_modules;
  if (! env_module_search_modules(env, &fn->module, &env->search_modules))
    return false;
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
      if (! frame_init(&frame, env->frame)) {
        list_delete_all(env->search_modules);
        env->search_modules = search_modules;
        return false;
      }
      env->frame = &frame;
      if (env_eval_equal_list(env, fn->macro || fn->special_operator,
                              clause->pattern, args_final, &tmp))
        break;
      env->frame = frame_clean(&frame);
      clause = clause->next_clause;
    }
    if (! clause) {
      err_puts("env_eval_call_fn_args: no clause matching.\nTried clauses :\n");
      clause = fn->clauses;
      while (clause) {
        err_inspect_fn_pattern(clause->pattern);
        err_puts("\n");
        clause = clause->next_clause;
      }
      err_puts("\nArguments :\n");
      err_inspect_fn_pattern(args);
      err_puts("\n");
      list_delete_all(args);
      list_delete_all(env->search_modules);
      env->search_modules = search_modules;
      return false;
    }
  }
  else {
    frame_init(&frame, env->frame);
    env->frame = &frame;
  }
  if (! env_eval_block(env, &clause->algo, &tag)) {
    list_delete_all(args);
    list_delete_all(tmp);
    list_delete_all(env->search_modules);
    env->search_modules = search_modules;
    env->frame = frame_clean(&frame);
    return false;
  }
  list_delete_all(args);
  list_delete_all(tmp);
  list_delete_all(env->search_modules);
  env->search_modules = search_modules;
  env->frame = frame_clean(&frame);
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
  bool b;
  s_ident tmp_ident;
  const s_tag *value;
  assert(env);
  assert(call);
  if (call->ident.module == NULL &&
      (value = env_frames_get(env, call->ident.sym))) {
    if (value->type == TAG_CFN) {
      call->cfn = cfn_new_copy(&value->data.cfn);
      return true;
    }
    else if (value->type == TAG_FN) {
      call->fn = fn_new_copy(&value->data.fn);
      return true;
    }
  }
  ident_init_copy(&tmp_ident, &call->ident);
  if (! env_ident_resolve_module(env, &tmp_ident, &call->ident) ||
      ! env_module_ensure_loaded(env, call->ident.module) ||
      ! env_module_has_ident(env, call->ident.module, &call->ident,
                             &b) ||
      ! b ||
      ! env_call_get(env, call)) {
    ident_init_copy(&call->ident, &tmp_ident);
    call->ident.module = &g_sym_KC3;
    if (! env_module_ensure_loaded(env, call->ident.module) ||
        ! env_module_has_ident(env, call->ident.module, &call->ident,
                               &b) ||
        ! b ||
        ! env_call_get(env, call)) {      
      ident_init_copy(&call->ident, &tmp_ident);
      return false;
    }
  }
  return true;
}

bool env_eval_cfn (s_env *env, const s_cfn *cfn, s_tag *dest)
{
  s_cfn tmp;
  assert(cfn);
  assert(dest);
  (void) env;
  if (! cfn_init_copy(&tmp, cfn))
    return false;
  if (! cfn_prep_cif(&tmp))
    return false;
  if (! cfn_link(&tmp))
    return false;
  dest->type = TAG_CFN;
  dest->data.cfn = tmp;
  return true;
}

bool env_eval_complex (s_env *env, const s_complex *c, s_tag *dest)
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

bool env_eval_cow (s_env *env, const s_cow *cow, s_tag *dest)
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

bool env_eval_equal_cow (s_env *env, const s_cow *a,
                         const s_cow *b, s_cow **dest)
{
  s8 r;
  s_cow *tmp;
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

bool env_eval_equal_list (s_env *env, bool macro, const s_list *a,
                          const s_list *b, s_list **dest)
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
  s_tag tmp;
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

bool env_eval_equal_tag (s_env *env, bool macro, const s_tag *a,
                         const s_tag *b, s_tag *dest)
{
  bool is_unbound_a;
  bool is_unbound_b;
  s_tag tmp_a;
  s_tag tmp_b;
  assert(env);
  assert(a);
  assert(b);
  assert(dest);
  tag_init_void(&tmp_a);
  tag_init_void(&tmp_b);
  is_unbound_a = a->type == TAG_IDENT;
  is_unbound_b = ! macro && b->type == TAG_IDENT;
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
  if (is_unbound_a) {
    if (macro)
      tag_init_copy(dest, b);
    else
      env_eval_tag(env, b, dest);
    frame_binding_new_copy(env->frame->next, a->data.ident.sym, dest);
    return true;
  }
  if (is_unbound_b) {
    if (macro)
      tag_init_copy(dest, a);
    else
      env_eval_tag(env, a, dest);
    frame_binding_new_copy(env->frame->next, b->data.ident.sym, dest);
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
  case TAG_TUPLE:
    dest->type = TAG_TUPLE;
    return env_eval_equal_tuple(env, macro, &a->data.tuple,
                                &b->data.tuple, &dest->data.tuple);
  case TAG_ARRAY:
  case TAG_BLOCK:
  case TAG_BOOL:
  case TAG_CALL:
  case TAG_CFN:
  case TAG_CHARACTER:
  case TAG_FACT:
  case TAG_FN:
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

bool env_eval_equal_tuple (s_env *env, bool macro, const s_tuple *a,
                           const s_tuple *b, s_tuple *dest)
{
  uw i;
  s_tuple tmp;
  assert(env);
  assert(a);
  assert(b);
  assert(dest);
  if (a->count != b->count)
    return false;
  tuple_init(&tmp, a->count);
  i = 0;
  while (i < a->count) {
    if (! env_eval_equal_tag(env, macro, a->tag + i, b->tag + i,
                             tmp.tag + i)) {
      tuple_clean(&tmp);
      return false;
    }
    i++;
  }
  *dest = tmp;
  return true;
}

bool env_eval_fn (s_env *env, const s_fn *fn, s_tag *dest)
{
  s_tag tmp = {0};
  assert(env);
  assert(fn);
  assert(dest);
  tmp.type = TAG_FN;
  if (! fn_init_copy(&tmp.data.fn, fn))
    return false;
  if (! tmp.data.fn.module)
    tmp.data.fn.module = env->current_defmodule;
  *dest = tmp;
  return true;
}

bool env_eval_ident (s_env *env, const s_ident *ident, s_tag *dest)
{
  const s_tag *tag;
  s_tag tmp;
  s_ident tmp_ident;
  assert(env);
  assert(ident);
  if ((tag = env_frames_get(env, ident->sym))) {
    tag_init_copy(dest, tag);
    return true;
  }
  if (env_ident_resolve_module(env, ident, &tmp_ident) &&
      ! (tag = env_ident_get(env, &tmp_ident, &tmp))) {
    err_write_1("env_eval_ident: unbound ident: ");
    err_inspect_ident(ident);
    err_write_1("\n");
    assert(! "env_eval_ident: unbound ident");
    return false;
  }
  *dest = *tag;
  return true;
}

bool env_eval_ident_is_bound (s_env *env, const s_ident *ident)
{
  s_ident tmp_ident;
  s_tag tmp;
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

bool env_eval_list (s_env *env, const s_list *list, s_tag *dest)
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

bool env_eval_map (s_env *env, const s_map *map, s_tag *dest)
{
  s_map tmp;
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

bool env_eval_quote (s_env *env, const s_quote *quote, s_tag *dest)
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

bool env_eval_quote_array (s_env *env, const s_array *array,
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

bool env_eval_quote_block (s_env *env, const s_block *block, s_tag *dest)
{
  uw i = 0;
  s_block tmp;
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

bool env_eval_quote_call (s_env *env, const s_call *call, s_tag *dest)
{
  const s_list *arg;
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
  if (call->cfn) {
    tmp.cfn = cfn_new_copy(call->cfn);
    if (! tmp.cfn)
      goto ko;
  }
  if (call->fn) {
    tmp.fn = fn_new_copy(call->fn);
    if (! tmp.fn)
      goto ko;
  }
  dest->type = TAG_CALL;
  dest->data.call = tmp;
  return true;
 ko:
  call_clean(&tmp);
  return false;
}

bool env_eval_quote_complex (s_env *env, const s_complex *c,
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

bool env_eval_quote_cow (s_env *env, const s_cow *cow,
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

bool env_eval_quote_list (s_env *env, const s_list *list, s_tag *dest)
{
  s_list *next;
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

bool env_eval_quote_map (s_env *env, const s_map *map, s_tag *dest)
{
  s_map tmp;
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

bool env_eval_quote_quote (s_env *env, const s_quote *quote, s_tag *dest)
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

bool env_eval_quote_struct (s_env *env, const s_struct *s, s_tag *dest)
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
bool env_eval_quote_tag (s_env *env, const s_tag *tag, s_tag *dest)
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
  case TAG_TUPLE:
    return env_eval_quote_tuple(env, &tag->data.tuple, dest);
  case TAG_UNQUOTE:
    return env_eval_quote_unquote(env, &tag->data.unquote, dest);
  case TAG_VOID:
  case TAG_BOOL:
  case TAG_CFN:
  case TAG_CHARACTER:
  case TAG_F32:
  case TAG_F64:
  case TAG_F128:
  case TAG_FACT:
  case TAG_FN:
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

bool env_eval_quote_tuple (s_env *env, const s_tuple *tuple, s_tag *dest)
{
  uw i = 0;
  s_tuple tmp;
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

bool env_eval_quote_unquote (s_env *env, const s_unquote *unquote, s_tag *dest)
{
  bool r;
  s_tag tmp;
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

bool env_eval_str (s_env *env, const s_str *str, s_tag *dest)
{
  bool r = true;
  s_tag tmp;
  if (! str_parse_eval(str, &tmp)) {
    err_puts("env_eval_str: invalid Str");
    assert(! "env_eval_str: invalid Str");
    return false;
  }
  if (tmp.type == TAG_STR) {
    *dest = tmp;
    return true;
  }
  r = env_eval_tag(env, &tmp, dest);
  tag_clean(&tmp);
  return r;
}

bool env_eval_struct (s_env *env, const s_struct *s, s_tag *dest)
{
  uw i;
  s_struct *t;
  s_tag       tag = {0};
  const void *tag_data;
  s_tag tmp = {0};
  const s_sym *type;
  const void *value;
  assert(env);
  assert(s);
  assert(dest);
  tmp.type = TAG_STRUCT;
  t = &tmp.data.struct_;
  if (s->data) {
    if (! struct_init_copy(t, s))
      return false;
    *dest = tmp;
    return true;
  }
  t->type = s->type;
  if (! struct_allocate(t))
    return false;
  i = 0;
  while (i < t->type->map.count) {
    if (s->tag) {
      if (t->type->map.value[i].type == TAG_VAR)
        type = t->type->map.value[i].data.var.type;
      else {
        if (! tag_type(t->type->map.value + i, &type))
          goto ko;
      }
      if (! env_eval_tag(env, s->tag + i, &tag))
        goto ko;
      if (t->type->map.value[i].type == TAG_VAR) {
        if (! data_init_cast((s8 *) t->data + t->type->offset[i],
                             &type, &tag))
          goto ko_init;
      }
      else {
        if (! tag_to_const_pointer(&tag, type, &tag_data)) {
          tag_clean(&tag);
          goto ko;
        }
        if (! data_init_copy(type, (s8 *) t->data + t->type->offset[i],
                             tag_data))
          goto ko_init;
      }
      tag_clean(&tag);
    }
    else {
      if (! tag_type(t->type->map.value + i, &type))
        goto ko;
      if (! tag_to_const_pointer(t->type->map.value + i, type, &value))
        goto ko;
      if (! data_init_copy(type, (s8 *) t->data + t->type->offset[i],
                           value))
        goto ko;
    }
    i++;
  }
  *dest = tmp;
  return true;
 ko_init:
  err_write_1("env_eval_struct: invalid type ");
  err_write_1(tag_type_to_string(tag.type));
  err_write_1(" for key ");
  err_write_1(t->type->map.key[i].data.sym->str.ptr.pchar);
  err_write_1(", expected ");
  err_puts(tag_type_to_string(t->type->map.value[i].type));
  tag_clean(&tag);
 ko:
  struct_clean(t);
  return false;
}

bool env_eval_tag (s_env *env, const s_tag *tag, s_tag *dest)
{
  assert(env);
  assert(tag);
  assert(dest);
  switch (tag->type) {
  case TAG_VOID:
    return env_eval_void(env, NULL, dest);
  case TAG_ARRAY:
    return env_eval_array_tag(env, &tag->data.array, dest);
  case TAG_BLOCK:
    return env_eval_block(env, &tag->data.block, dest);
  case TAG_CALL:
    return env_eval_call(env, &tag->data.call, dest);
  case TAG_CFN:
    return env_eval_cfn(env, &tag->data.cfn, dest);
  case TAG_COMPLEX:
    return env_eval_complex(env, tag->data.complex, dest);
  case TAG_COW:
    return env_eval_cow(env, tag->data.cow, dest);
  case TAG_FN:
    return env_eval_fn(env, &tag->data.fn, dest);
  case TAG_IDENT:
    return env_eval_ident(env, &tag->data.ident, dest);
  case TAG_LIST:
    return env_eval_list(env, tag->data.list, dest);
  case TAG_MAP:
    return env_eval_map(env, &tag->data.map, dest);
  case TAG_QUOTE:
    return env_eval_quote(env, &tag->data.quote, dest);
  case TAG_STR:
    return env_eval_str(env, &tag->data.str, dest);
  case TAG_STRUCT:
    return env_eval_struct(env, &tag->data.struct_, dest);
  case TAG_TUPLE:
    return env_eval_tuple(env, &tag->data.tuple, dest);
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
  case TAG_SW:
  case TAG_STRUCT_TYPE:
  case TAG_SYM:
  case TAG_U8:
  case TAG_U16:
  case TAG_U32:
  case TAG_U64:
  case TAG_UNQUOTE:
  case TAG_UW:
  case TAG_VAR:
    if (! tag_init_copy(dest, tag))
      return false;
    return true;
  }
  err_puts("env_eval_tag: unknown tag type");
  assert(! "env_eval_tag: unknown tag type");
  return false;
}

bool env_eval_tuple (s_env *env, const s_tuple *tuple, s_tag *dest)
{
  uw i = 0;
  s_tuple tmp;
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

bool env_eval_void (s_env *env, const void *_, s_tag *dest)
{
  assert(env);
  assert(dest);
  (void) env;
  (void) _;
  tag_init_void(dest);
  return true;
}

s_fact_w * env_fact_w_eval (s_env *env, const s_fact_w *fact,
			    s_fact_w *dest)
{
  s_fact_w tmp = {0};
  assert(env);
  assert(fact);
  assert(dest);
  if (fact->subject.type == TAG_CFN) {
    if (! env_eval_tag(env, &fact->subject, &tmp.subject))
      return NULL;
  }
  else {
    if (! tag_init_copy(&tmp.subject, &fact->subject))
      return NULL;
  }
  if (fact->predicate.type == TAG_CFN) {
    if (! env_eval_tag(env, &fact->predicate, &tmp.predicate))
      return NULL;
  }
  else {
    if (! tag_init_copy(&tmp.predicate, &fact->predicate))
      return NULL;
  }
  if (fact->object.type == TAG_CFN) {
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

const s_tag * env_frames_get (const s_env *env, const s_sym *name)
{
  const s_tag *tag;
  if ((tag = frame_get(env->frame, name)) ||
      (tag = frame_get(&env->global_frame, name)))
    return tag;
  return NULL;
}

s_tag * env_ident_get (s_env *env, const s_ident *ident, s_tag *dest)
{
  s_facts_with_cursor cursor;
  const s_fact *fact;
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
  s_tag tmp;
  module = ident->module;
  if (! module) {
    if (! sym_search_modules(ident->sym, &module) ||
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
  if (! facts_find_fact_by_tags(&env->facts, &tag_module, &tag_symbol,
                                &tag_ident, &fact) ||
      ! fact)
    return NULL;
  if (! facts_with(&env->facts, &cursor, (t_facts_spec) {
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
  if (! facts_with(&env->facts, &cursor, (t_facts_spec) {
        &tag_ident, &tag_is_a, &tag_macro, NULL, NULL }))
    return NULL;
  if (! facts_with_cursor_next(&cursor, &fact)) {
    facts_with_cursor_clean(&cursor);
    return NULL;
  }
  if (fact) {
    if (tmp.type == TAG_CFN)
      tmp.data.cfn.macro = true;
    else if (tmp.type == TAG_FN)
      tmp.data.fn.macro = true;
  }
  facts_with_cursor_clean(&cursor);
  if (! facts_with(&env->facts, &cursor, (t_facts_spec) {
        &tag_ident, &tag_is_a, &tag_special_operator, NULL, NULL}))
    return NULL;
  if (! facts_with_cursor_next(&cursor, &fact)) {
    facts_with_cursor_clean(&cursor);
    return NULL;
  }
  if (fact) {
    if (tmp.type == TAG_CFN)
      tmp.data.cfn.special_operator = true;
    else if (tmp.type == TAG_FN)
      tmp.data.fn.special_operator = true;
  }
  facts_with_cursor_clean(&cursor);
  *dest = tmp;
  return dest;
}

bool * env_ident_is_special_operator (s_env *env,
                                      const s_ident *ident,
                                      bool *dest)
{
  const s_fact *fact;
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
  if (! facts_find_fact_by_tags(&env->facts, &tag_ident, &tag_is_a,
                                &tag_special_operator, &fact))
    return NULL;
  *dest = fact ? true : false;
  return dest;
}

s_ident * env_ident_resolve_module (s_env *env,
                                    const s_ident *ident,
                                    s_ident *dest)
{
  s_ident tmp;
  assert(env);
  assert(ident);
  ident_init_copy(&tmp, ident);
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
  assert(env);
  if (! env_init_args(env, argc, argv))
    return NULL;
  sym_init_g_sym();
  env->error_handler = NULL;
  if (! env_init_toplevel(env))
    return NULL;
  if (! env_init_globals(env))
    return NULL;
  buf_init_alloc(&env->in, BUF_SIZE);
  buf_file_open_r(&env->in, stdin);
  buf_init_alloc(&env->out, BUF_SIZE);
  buf_file_open_w(&env->out, stdout);
  buf_init_alloc(&env->err, BUF_SIZE);
  buf_file_open_w(&env->err, stderr);
  facts_init(&env->facts);
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
  if (! file_search(&path, &g_sym_x, &env->module_path)) {
    err_puts("env_init: lib/kc3/0.1 not found in module path");
    assert(! "env_init: lib/kc3/0.1 not found in module path");
    return NULL;
  }
  env->current_defmodule = &g_sym_KC3;
  env->search_modules_default = list_new_sym(&g_sym_KC3, NULL);
  env->search_modules = env->search_modules_default;
  env->quote_level = 0;
  env->unquote_level = 0;
  if (! env_module_load(env, &g_sym_KC3)) {
    env_clean(env);
    return NULL;
  }
  return env;
}

s_env * env_init_args (s_env *env, int *argc, char ***argv)
{
  char a[PATH_MAX];
  s_str argv0;
  s_buf buf;
  s_str dir;
  sw r;
  assert(env);
  if (argc && argv && *argc && *argv) {
    env->argc = (*argc)--;
    env->argv = (*argv)++;
    str_init_1(&argv0, NULL, env->argv[0]);
    file_dirname(&argv0, &dir);
    buf_init(&buf, false, sizeof(a), a);
    if ((r = buf_write_str(&buf, &dir)) < 0 ||
        (r = buf_write_u8(&buf, '/') < 0)) {
      str_clean(&dir);
      goto ko;
    }
    str_clean(&dir);
    buf_read_to_str(&buf, &env->argv0_dir);
    return env;
  }
 ko:
  env->argc = 0;
  env->argv = NULL;
  str_init_1(&env->argv0_dir, NULL, "./");
  return env;
}

s_env * env_init_globals (s_env *env)
{
  s_tag *file_dir;
  s_tag *file_path;
  if (! frame_init(&env->global_frame, NULL))
    return NULL;
  if (! (file_dir = frame_binding_new(&env->global_frame, &g_sym___DIR__)))
    return NULL;
  if (! (file_path = frame_binding_new(&env->global_frame, &g_sym___FILE__)))
    return NULL;
  file_dir->type = TAG_STR;
  if (! file_pwd(&file_dir->data.str))
    return NULL;
  if (! tag_init_str_1(file_path, NULL, "stdin"))
    return NULL;
  return env;
}

s_env * env_init_toplevel (s_env *env)
{
  env->frame = frame_new(NULL);
  return env;
}

s_tag * env_let (s_env *env, const s_tag *tag, const s_block *block,
                 s_tag *dest)
{
  uw i;
  const s_map *map;
  s_tag tmp;
  assert(env);
  assert(tag);
  assert(block);
  assert(dest);
  if (! env_eval_tag(env, tag, &tmp))
    return NULL;
  switch(tag->type) {
  case TAG_MAP:
    map = &tag->data.map;
    break;
  case TAG_STRUCT:
    map = &tag->data.struct_.type->map;
    break;
  default:
    tag_clean(&tmp);
    err_write_1("env_let: unsupported associative tag type: ");
    err_inspect_tag_type(tag->type);
    err_write_1(": ");
    err_inspect_tag(tag);
    err_write_1("\n");
    assert(! "env_let: unsupported associative tag type");
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
      return NULL;
    }
    if (! frame_binding_new_copy(env->frame, map->key[i].data.sym,
                            map->value + i)) {
      tag_clean(&tmp);
      return NULL;
    }
    i++;
  }
  if (! env_eval_block(env, block, dest)) {
    tag_clean(&tmp);
    return NULL;
  }
  tag_clean(&tmp);
  return dest;
}

bool env_load (s_env *env, const s_str *path)
{
  s_buf buf;
  s_tag *file_dir;
  s_tag  file_dir_save;
  s_tag *file_path;
  s_tag  file_path_save;
  sw r;
  s_tag tag = {0};
  s_tag tmp;
  assert(env);
  assert(path);
  if (! buf_init_alloc(&buf, BUF_SIZE))
    return false;
  if (! buf_getc_open_r(&buf, path->ptr.pchar)) {
    buf_clean(&buf);
    return false;
  }
  file_dir = frame_get_w(&env->global_frame, &g_sym___DIR__);
  file_dir_save = *file_dir;
  file_path = frame_get_w(&env->global_frame, &g_sym___FILE__);
  file_path_save = *file_path;
  if (! file_dirname(path, &file_dir->data.str)) {
    buf_clean(&buf);
    return false;
  }
  tag_init_str(file_path, NULL, path->size, path->ptr.pchar);
  while (1) {
    if ((r = buf_parse_tag(&buf, &tag)) < 0) {
      buf_getc_close(&buf);
      buf_clean(&buf);
      return false;
    }
    if (! r)
      break;
    if (! env_eval_tag(env, &tag, &tmp)) {
      tag_clean(&tag);
      buf_getc_close(&buf);
      buf_clean(&buf);
      return false;
    }
    tag_clean(&tmp);
    tag_clean(&tag);
  }
  tag_clean(file_dir);
  *file_dir = file_dir_save;
  *file_path = file_path_save;
  buf_getc_close(&buf);
  buf_clean(&buf);
  return true;
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
  const s_fact *fact;
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
  if (! facts_find_fact_by_tags(&env->facts, &tag_module_name,
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
  const s_fact *fact;
  s_tag tag_ident;
  s_tag tag_module_name;
  s_tag tag_operator;
  s_tag tag_sym;
  s_tag tag_symbol;
  s_tag tag_var;
  tag_init_ident(&tag_ident, ident);
  tag_init_sym(  &tag_module_name, module);
  tag_init_sym(  &tag_operator, &g_sym_operator);
  tag_init_sym(  &tag_symbol, &g_sym_symbol);
  if (! facts_find_fact_by_tags(&env->facts, &tag_module_name,
                                &tag_symbol, &tag_ident, &fact))
    return NULL;
  if (fact) {
    *dest = true;
    return dest;
  }
  tag_init_sym(&tag_sym, &g_sym_sym);
  tag_init_var(&tag_var, &g_sym_Ident);
  if (! facts_with(&env->facts, &cursor, (t_facts_spec) {
        &tag_module_name, &tag_operator, &tag_var, NULL,
        &tag_var, &tag_sym, &tag_ident, NULL, NULL})) {
    err_puts("env_module_has_ident: facts_with");
    assert(! "env_module_has_ident: facts_with");
    return NULL;
  }
  if (! facts_with_cursor_next(&cursor, &fact)) {
    err_puts("env_module_has_ident: facts_with_cursor_next");
    assert(! "env_module_has_ident: facts_with_cursor_next");
    return NULL;
  }
  if (fact) {
    facts_with_cursor_clean(&cursor);
    *dest = true;
    return dest;
  }
  if (! facts_find_fact_by_tags(&env->facts, &tag_module_name,
                                &tag_operator, &tag_ident, &fact)) {
    err_puts("env_module_has_ident: facts_find_fact_by_tags");
    assert(! "env_module_has_ident: facts_find_fact_by_tags");
    return NULL;
  }
  if (fact) {
    *dest = true;
    return dest;
  }
  *dest = false;
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
  const s_fact *fact;
  s_tag tag_module;
  s_tag tag_is_loading;
  s_tag tag_true;
  assert(env);
  assert(module);
  tag_init_sym(&tag_module, module);
  tag_init_sym(&tag_is_loading, &g_sym_is_loading);
  tag_init_bool(&tag_true, true);
  if (! facts_find_fact_by_tags(&env->facts, &tag_module,
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
    if (! facts_replace_tags(&env->facts, &tag_module, &tag_is_loading,
                             &tag_true))
      return false;
  }
  else
    if (! facts_remove_fact_tags(&env->facts, &tag_module,
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
  facts_transaction_start(&env->facts, &transaction);
  if (! env_module_is_loading_set(env, module, true))
    goto rollback;
  if (module_path(module, &env->module_path, KC3_EXT, &path) &&
      file_access(&path, &g_sym_r)) {
    tag_init_time(&tag_time);
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
    if (! module_path(module, &env->module_path, FACTS_EXT, &path)) {
      err_write_1("env_module_load: ");
      err_write_1(module->str.ptr.pchar);
      err_puts(": module_path");
      goto rollback;
    }
    if (! file_access(&path, &g_sym_r))
      goto rollback;
    tag_init_time(&tag_time);
    if (facts_load_file(&env->facts, &path) < 0) {
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
  if (! facts_replace_tags(&env->facts, &tag_module_name,
                           &tag_load_time, &tag_time))
    goto rollback;
  tag_clean(&tag_time);
  env_module_is_loading_set(env, module, false);
  facts_transaction_end(&env->facts, &transaction);
  return true;
 rollback:
  if (! facts_transaction_rollback(&env->facts, &transaction)) {
    abort();
    return false;
  }
  return false;
}

const s_tag ** env_module_load_time (s_env *env, const s_sym *module,
				     const s_tag **dest)
{
  s_facts_with_cursor cursor;
  const s_fact *fact;
  s_tag tag_module_name;
  s_tag tag_load_time;
  s_tag tag_time_var;
  tag_init_sym(&tag_module_name, module);
  tag_init_sym(&tag_load_time, &g_sym_load_time);
  tag_init_var(&tag_time_var, &g_sym_Tag);
  if (! facts_with(&env->facts, &cursor, (t_facts_spec) {
        &tag_module_name, &tag_load_time, &tag_time_var, NULL, NULL }))
    return NULL;
  if (! facts_with_cursor_next(&cursor, &fact) ||
      ! fact) {
    facts_with_cursor_clean(&cursor);
    return NULL;
  }
  *dest = fact->object;
  facts_with_cursor_clean(&cursor);
  return dest;
}

bool env_module_maybe_reload (s_env *env, const s_sym *module)
{
  const s_tag *load_time = {0};
  s_str path;
  bool r = false;
  s_tag tag_mtime;
  if (module_path(module, &env->module_path, KC3_EXT, &path)) {
    if (file_access(&path, &g_sym_r))
      r = true;
    else
      str_clean(&path);
  }
  if (!r && module_path(module, &env->module_path, FACTS_EXT, &path)) {
    if (file_access(&path, &g_sym_r))
      r = true;
    else
      str_clean(&path);
  }
  if (! r)
    return true;
  if (! env_module_load_time(env, module, &load_time)) {
    str_clean(&path);
    return env_module_load(env, module);
  }
  if (! file_mtime(&path, &tag_mtime)) {
    str_clean(&path);
    return false;
  }
  str_clean(&path);
  if (compare_tag(load_time, &tag_mtime) < 0)
    r = env_module_load(env, module);
  tag_clean(&tag_mtime);
  return r;
}

s_list ** env_module_search_modules (s_env *env,
                                     const s_sym * const *module,
                                     s_list **dest)
{
  s_list *tmp;
  s_list *tmp2;
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

s8 env_operator_arity (s_env *env, const s_ident *op)
{
  s_facts_cursor cursor;
  const s_fact *fact;
  s8 r = -1;
  s_tag tag_op;
  s_tag tag_arity;
  s_tag tag_var;
  assert(env);
  assert(op);
  tag_init_ident(&tag_op, op);
  tag_init_sym(  &tag_arity, &g_sym_arity);
  tag_init_var(  &tag_var, &g_sym_U8);
  facts_with_tags(&env->facts, &cursor, &tag_op, &tag_arity, &tag_var);
  if (! facts_cursor_next(&cursor, &fact))
    return -1;
  if (fact)
    r = tag_var.data.u8;
  else {
    err_write_1("env_operator_arity: arity for operator ");
    err_write_1(op->sym->str.ptr.pchar);
    err_write_1(" not found in module ");
    err_puts(op->module->str.ptr.pchar);
    r = -1;
  }
  facts_cursor_clean(&cursor);
  return r;
}

bool * env_operator_find (s_env *env, const s_ident *op, bool *dest)
{
  const s_fact *fact;
  s_tag tag_is_a;
  s_tag tag_op;
  s_tag tag_operator;
  assert(env);
  assert(op);
  tag_init_sym(  &tag_is_a, &g_sym_is_a);
  tag_init_ident(&tag_op, op);
  tag_init_sym(  &tag_operator, &g_sym_operator);
  if (! facts_find_fact_by_tags(&env->facts, &tag_op, &tag_is_a,
                                &tag_operator, &fact))
    return NULL;
  *dest = fact ? true : false;
  return dest;
}

s_ident * env_operator_ident (s_env *env, const s_ident *op,
                              s_ident *dest)
{
  assert(env);
  assert(op);
  assert(dest);
  if (env->current_defmodule == op->module)
    dest->module = NULL;
  else
    dest->module = op->module;
  dest->sym = env_operator_symbol(env, op);
  return dest;
}

bool * env_operator_is_right_associative (s_env *env, const s_ident *op,
                                          bool *dest)
{
  const s_fact *fact;
  s_tag tag_assoc;
  s_tag tag_op;
  s_tag tag_right;
  assert(env);
  assert(op);
  tag_init_sym(  &tag_assoc, &g_sym_operator_associativity);
  tag_init_ident(&tag_op, op);
  tag_init_sym(  &tag_right, &g_sym_right);
  if (! facts_find_fact_by_tags(&env->facts, &tag_op, &tag_assoc,
                                &tag_right, &fact))
    return NULL;
  *dest = fact ? true : false;
  return dest;
}

s8 env_operator_precedence (s_env *env, const s_ident *op)
{
  s_facts_cursor cursor;
  const s_fact *fact;
  s8 r = -1;
  s_tag tag_op;
  s_tag tag_precedence;
  s_tag tag_var;
  assert(env);
  assert(op);
  tag_init_ident(&tag_op, op);
  tag_init_sym(  &tag_precedence, &g_sym_operator_precedence);
  tag_init_var(  &tag_var, &g_sym_U8);
  if (! facts_with_tags(&env->facts, &cursor, &tag_op, &tag_precedence,
                        &tag_var))
    return -1;
  if (! facts_cursor_next(&cursor, &fact))
    return -1;
  if (fact) {
    r = tag_var.data.u8;
  }
  else {
    err_write_1("env_operator_precedence: precedence for operator ");
    err_write_1(op->sym->str.ptr.pchar);
    err_write_1(" not found in module ");
    err_puts(op->module->str.ptr.pchar);
    r = -1;
  }
  facts_cursor_clean(&cursor);
  return r;
}

s_ident * env_operator_resolve (s_env *env, const s_ident *op,
                                u8 arity, s_ident *dest)
{
  s_facts_with_cursor cursor;
  const s_fact *fact;
  s_tag tag_arity;
  s_tag tag_arity_u8;
  s_tag tag_is_a;
  s_tag tag_module;
  s_tag tag_module_name;
  s_tag tag_operator;
  s_tag tag_var;
  s_tag tag_sym;
  s_tag tag_symbol;
  s_ident tmp;
  env_ident_resolve_module(env, op, &tmp);
  tag_init_sym(&tag_arity, &g_sym_arity);
  tag_init_u8( &tag_arity_u8, arity);
  tag_init_sym(&tag_is_a, &g_sym_is_a);
  tag_init_sym(&tag_module, &g_sym_module);
  tag_init_sym(&tag_module_name, tmp.module);
  tag_init_sym(&tag_operator, &g_sym_operator);
  tag_init_var(&tag_var, &g_sym_Ident);
  tag_init_sym(&tag_sym, tmp.sym);
  tag_init_sym(&tag_symbol, &g_sym_symbol);
  if (! facts_with(&env->facts, &cursor, (t_facts_spec) {
        &tag_module_name, &tag_is_a, &tag_module,
        &tag_operator, &tag_var, NULL,
        &tag_var, &tag_symbol, &tag_sym,
        &tag_arity, &tag_arity_u8,
        NULL, NULL })) {
    err_write_1("env_operator_resolve: ");
    err_inspect_ident(op);
    err_puts(": facts_with");
    assert(! "env_operator_resolve: facts_with");
    return NULL;
  }
  if (! facts_with_cursor_next(&cursor, &fact)) {
    err_write_1("env_operator_resolve: ");
    err_inspect_ident(op);
    err_puts(": facts_with_cursor_next");
    assert(! "env_operator_resolve: facts_with_cursor_next");
    return NULL;
  }
  if (! fact) {
    if (false) {
      err_write_1("env_operator_resolve: ");
      err_inspect_ident(op);
      err_puts(": operator not found");
      assert(! "env_operator_resolve: operator not found");
    }
    facts_with_cursor_clean(&cursor);
    return NULL;
  }
  if (tag_var.type != TAG_IDENT) {
    err_write_1("env_operator_resolve: ");
    err_inspect_ident(op);
    err_puts(": operator is not an ident");
    assert(! "env_operator_resolve: operator is not an ident");
    facts_with_cursor_clean(&cursor);
    return NULL;
  }
  *dest = tag_var.data.ident;
  facts_with_cursor_clean(&cursor);
  return dest;
}

const s_sym * env_operator_symbol (s_env *env, const s_ident *op)
{
  s_facts_cursor cursor;
  const s_fact *fact;
  const s_sym *r;
  s_tag tag_op;
  s_tag tag_symbol;
  s_tag tag_var;
  assert(env);
  assert(op);
  tag_init_ident(&tag_op, op);
  tag_init_sym(  &tag_symbol, &g_sym_symbol);
  tag_init_var(  &tag_var, &g_sym_Sym);
  if (! facts_with_tags(&env->facts, &cursor, &tag_op, &tag_symbol,
                        &tag_var))
    return NULL;
  if (! facts_cursor_next(&cursor, &fact))
    return NULL;
  if (fact &&
      tag_var.type == TAG_SYM)
    r = tag_var.data.sym;
  else {
    err_write_1("env_operator_symbol: symbol for operator ");
    err_write_1(op->sym->str.ptr.pchar);
    err_write_1(" not found in module ");
    err_puts(op->module->str.ptr.pchar);
    r = NULL;
  }
  facts_cursor_clean(&cursor);
  return r;
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
  return list_init_copy(dest, (const s_list * const *) &env->search_modules);
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
      err_write_1("env_sym_search_modules: ");
      err_inspect_sym(&sym);
      err_write_1(": search_module: ");
      err_inspect_sym(&module);
      err_write_1(" -> Ok\n");
      return true;
    }
    if (true) {
      err_write_1("env_sym_search_modules: ");
      err_inspect_sym(&sym);
      err_write_1(": search_module: ");
      err_inspect_sym(&env->search_modules->tag.data.sym);
      err_write_1(" -> not found\n");
    }
    search_module = list_next(search_module);
  }
  if (false) {
    err_write_1("env_sym_search_modules: ");
    err_inspect_sym(&sym);
    err_write_1(": search_module: ");
    err_inspect_list((const s_list * const *) &env->search_modules);
    err_write_1(" -> false\n");
  }
  *dest = NULL;
  return true;
}

u8 env_special_operator_arity (s_env *env, const s_ident *ident)
{
  u8 arity;
  s_facts_cursor cursor;
  const s_fact *fact;
  s_tag tag_arity;
  s_tag tag_ident;
  s_tag tag_var;
  assert(env);
  assert(ident);
  tag_ident.type = TAG_IDENT;
  env_ident_resolve_module(env, ident, &tag_ident.data.ident);
  tag_init_sym(  &tag_arity, &g_sym_arity);
  tag_init_var(  &tag_var, &g_sym_U8);
  if (! facts_with_tags(&env->facts, &cursor,
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
  const s_fact *fact;
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
  if (! facts_with_tags(&env->facts, &cursor, &tag_module,
                        &tag_struct_type, &tag_var))
    return NULL;
  if (! facts_cursor_next(&cursor, &fact))
    return NULL;
  *dest = fact ? true : false;
  facts_cursor_clean(&cursor);
  return dest;
}

const s_struct_type ** env_struct_type_find (s_env *env,
                                             const s_sym *module,
                                             const s_struct_type **dest)
{
  s_facts_with_cursor cursor;
  const s_fact *found;
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
  if (! facts_with(&env->facts, &cursor, (t_facts_spec) {
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
    return NULL;
  }
  if (! found) {
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
    return NULL;
  }
  *dest = &found->object->data.struct_type;
  facts_with_cursor_clean(&cursor);
  return dest;
}

f_clean env_struct_type_get_clean (s_env *env, const s_sym *module)
{
  s_facts_with_cursor cursor;
  const s_fact *found;
  s_tag tag_clean;
  s_tag tag_module;
  s_tag tag_var;
  f_clean tmp;
  const s_sym *type;
  tag_init_sym(&tag_module, module);
  tag_init_sym(&tag_clean, &g_sym_clean);
  tag_init_var(&tag_var, &g_sym_Tag);
  facts_with(&env->facts, &cursor, (t_facts_spec) {
      &tag_module, &tag_clean, &tag_var, NULL, NULL });
  if (! facts_with_cursor_next(&cursor, &found))
    return NULL;
  if (! found) {
    facts_with_cursor_clean(&cursor);
    return NULL;
  }
  if (found->object->type != TAG_CFN) {
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
  if (found->object->data.cfn.arity != 1) {
    err_write_1("env_struct_type_get_clean: ");
    err_inspect_sym(&module);
    err_write_1(": clean arity is ");
    err_inspect_u8(&found->object->data.cfn.arity);
    err_write_1(", it should be 1.\n");
    assert(! "env_struct_type_get_clean: invalid arity");
    facts_with_cursor_clean(&cursor);
    return NULL;
  }
  tmp = (f_clean) found->object->data.cfn.ptr.f;
  facts_with_cursor_clean(&cursor);
  return tmp;
}

s_list ** env_struct_type_get_spec (s_env *env,
                                    const s_sym *module,
                                    s_list **dest)
{
  const s_fact *found;
  s_tag tag_defstruct;
  s_tag tag_module;
  s_tag tag_var;
  s_tag tmp;
  assert(env);
  assert(module);
  assert(dest);
  tag_init_sym(&tag_defstruct, &g_sym_defstruct);
  tag_init_sym(&tag_module, module);
  tag_init_var(&tag_var, &g_sym_Tag);
  if (! facts_find_fact_by_tags(&env->facts, &tag_module,
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
  const s_fact *fact;
  s_tag tag_defstruct;
  s_tag tag_module;
  s_tag tag_var;
  assert(env);
  assert(module);
  tag_init_sym(&tag_defstruct, &g_sym_defstruct);
  tag_init_sym(&tag_module, module);
  tag_init_var(&tag_var, &g_sym_Tag);
  if (! facts_with_tags(&env->facts, &cursor, &tag_module,
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
  s_tag tmp;
  assert(tag);
  assert(tag->type == TAG_IDENT);
  return tag->type == TAG_IDENT &&
    (env_frames_get(env, tag->data.ident.sym) ||
     env_ident_get(env, &tag->data.ident, &tmp));
}

s_tag * env_unwind_protect (s_env *env, s_tag *protected, s_block *cleanup,
                            s_tag *dest)
{
  s_tag tmp;
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
