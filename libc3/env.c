/* c3
 * Copyright 2022,2023 kmx.io <contact@kmx.io>
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
#include <assert.h>
#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "binding.h"
#include "c3.h"
#include "error.h"
#include "error_handler.h"
#include "frame.h"

s_env g_c3_env;

void env_clean (s_env *env)
{
  assert(env);
  frame_delete_all(env->frame);
  error_handler_delete_all(env->error_handler);
  facts_clean(&env->facts);
  buf_file_close(&env->in);
  buf_clean(&env->in);
  buf_file_close(&env->out);
  buf_clean(&env->out);
  buf_file_close(&env->err);
  buf_clean(&env->err);
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
    tag_copy(tag, &error_handler->tag);
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

bool env_eval_call (s_env *env, const s_call *call, s_tag *dest)
{
  s_call c;
  bool result = false;
  assert(env);
  assert(call);
  assert(dest);
  call_copy(call, &c);
  env_eval_call_resolve(env, &c);
  if (c.macro)
    result = env_eval_call_macro(env, &c, dest);
  else if (c.special_operator)
    result = env_eval_call_special_operator(env, &c, dest);
  else if (c.cfn)
    result = env_eval_call_cfn(env, &c, dest);
  else if (c.fn)
    result = env_eval_call_fn(env, &c, dest);
  else {
    warnx("env_eval_call: could not resolve call %s.",
          call->ident.sym->str.ptr.ps8);
    result = false;
  }
  call_clean(&c);
  return result;
}

bool env_eval_call_arguments (s_env *env, s_list *args, s_list **dest)
{
  s_list **tail;
  s_list *tmp;
  tail = &tmp;
  while (args) {
    *tail = list_new(NULL, NULL);
    if (! env_eval_tag(env, &args->tag, &(*tail)->tag)) {
      list_delete_all(tmp);
      err_puts("env_eval_call_arguments: invalid argument: ");
      err_inspect(&args->tag);
      err_puts("\n");
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
  s_cfn *cfn;
  s_tag tag;
  assert(env);
  assert(call);
  assert(dest);
  cfn = call->cfn;
  assert(cfn);
  if (call->arguments) {
    if (! env_eval_call_arguments(env, call->arguments, &args)) {
      return false;
    }
  }
  if (! cfn_apply(cfn, args, &tag)) {
    list_delete_all(args);
    return false;
  }
  *dest = tag;
  list_delete_all(args);
  return true;
}

bool env_eval_call_fn (s_env *env, const s_call *call, s_tag *dest)
{
  s_list *args = NULL;
  s_frame frame;
  s_fn *fn;
  s_fn *fn2;
  s_tag tag;
  s_list *tmp = NULL;
  assert(env);
  assert(call);
  assert(dest);
  fn = call->fn;
  assert(fn);
  frame_init(&frame, env->frame);
  env->frame = &frame;
  fn2 = fn;
  if (call->arguments) {
    if (! env_eval_call_arguments(env, call->arguments, &args)) {
      env->frame = frame_clean(&frame);
      return false;
    }
    while (fn2 && ! env_eval_equal_list(env, fn2->pattern, args, &tmp))
      fn2 = fn2->next_clause;
    if (! fn2) {
      err_puts("env_eval_call_fn: no clause matching.\nTried clauses :\n");
      fn2 = fn;
      while (fn2) {
        err_inspect_fn_pattern(fn2->pattern);
        fn2 = fn2->next_clause;
      }
      err_puts("\nArguments :\n");
      err_inspect_fn_pattern(args);
      err_puts("\n");
      list_delete_all(args);
      env->frame = frame_clean(&frame);
      return false;
    }
  }
  if (! env_eval_progn(env, fn2->algo, &tag)) {
    list_delete_all(args);
    list_delete_all(tmp);
    env->frame = frame_clean(&frame);
    return false;
  }
  *dest = tag;
  list_delete_all(args);
  list_delete_all(tmp);
  env->frame = frame_clean(&frame);
  return true;
}

bool env_eval_call_macro (s_env *env, const s_call *call, s_tag *dest)
{
  s_tag *expanded;
  assert(env);
  assert(call);
  assert(dest);
  (void) env;
  (void) call;
  (void) expanded;
  (void) dest;
  return false;
}

bool env_eval_call_resolve (s_env *env, s_call *call)
{
  s_facts_with_cursor cursor;
  s_tag tag_cfn;
  s_tag tag_fn;
  s_tag tag_ident;
  s_tag tag_is_a;
  s_tag tag_macro;
  s_tag tag_module;
  s_tag tag_module_name;
  s_tag tag_special_operator;
  s_tag tag_sym;
  s_tag tag_symbol;
  s_tag tag_var;
  const s_tag *value;
  assert(env);
  assert(call);
  if ((value = frame_get(env->frame, call->ident.sym))) {
    if (value->type == TAG_CFN) {
      call->cfn = cfn_new_copy(&value->data.cfn);
      return true;
    }
    else if (value->type == TAG_FN) {
      call->fn = value->data.fn;
      return true;
    }
  }
  ident_resolve_module(&call->ident, env);
  tag_init_1(    &tag_cfn,      ":cfn");
  tag_init_1(    &tag_fn,       ":fn");
  tag_init_ident(&tag_ident, &call->ident);
  tag_init_1(    &tag_is_a,     ":is_a");
  tag_init_1(    &tag_macro,    ":macro");
  tag_init_1(    &tag_module,   ":module");
  tag_init_sym(  &tag_module_name, call->ident.module_name);
  tag_init_1(    &tag_special_operator, ":special_operator");
  tag_init_sym(  &tag_sym, call->ident.sym);
  tag_init_1(    &tag_symbol,   ":symbol");
  tag_init_var(  &tag_var);
  facts_with(&env->facts, &cursor, (t_facts_spec) {
      &tag_module_name,
      &tag_is_a, &tag_module,     /* module exists */
      &tag_symbol, &tag_ident,    /* module exports symbol */
      NULL, NULL });
  if (! facts_with_cursor_next(&cursor))
    errx(1, "symbol %s not found in module %s",
         call->ident.sym->str.ptr.ps8,
         call->ident.module_name->str.ptr.ps8);
  facts_with_cursor_clean(&cursor);
  facts_with(&env->facts, &cursor, (t_facts_spec) {
      &tag_ident, &tag_fn, &tag_var,
      NULL, NULL });
  if (facts_with_cursor_next(&cursor)) {
    if (tag_var.type != TAG_FN)
      errx(1, "%s.%s is not a function",
           call->ident.module_name->str.ptr.ps8,
           call->ident.sym->str.ptr.ps8);
    call->fn = tag_var.data.fn;
  }
  facts_with_cursor_clean(&cursor);
  facts_with(&env->facts, &cursor, (t_facts_spec) {
      &tag_ident, &tag_cfn, &tag_var,
      NULL, NULL });
  if (facts_with_cursor_next(&cursor)) {
    if (tag_var.type != TAG_CFN)
      errx(1, "%s.%s is not a C function",
           call->ident.module_name->str.ptr.ps8,
           call->ident.sym->str.ptr.ps8);
    call->cfn = cfn_new_copy(&tag_var.data.cfn);
  }
  facts_with_cursor_clean(&cursor);
  facts_with(&env->facts, &cursor, (t_facts_spec) {
      &tag_ident, &tag_is_a, &tag_macro, NULL, NULL });
  if (facts_with_cursor_next(&cursor))
    call->macro = true;
  facts_with_cursor_clean(&cursor);
  facts_with(&env->facts, &cursor, (t_facts_spec) {
      &tag_ident, &tag_is_a, &tag_special_operator, NULL, NULL});
  if (facts_with_cursor_next(&cursor))
    call->special_operator = true;
  facts_with_cursor_clean(&cursor);
  return true;
}

bool env_eval_call_special_operator (s_env *env, const s_call *call,
                                     s_tag *dest)
{
  s_frame frame;
  s_fn *fn;
  s_tag tag;
  s_list *tmp = NULL;
  assert(env);
  assert(call);
  assert(dest);
  if (call->cfn)
    return cfn_apply(call->cfn, call->arguments, dest) != NULL;
  fn = call->fn;
  assert(fn);
  frame_init(&frame, env->frame);
  env->frame = &frame;
  if (! env_eval_equal_list(env, fn->pattern, call->arguments, &tmp)) {
    err_puts("env_eval_call_fn: no clause matching.\nTried clauses :\n");
    err_inspect_fn_pattern(fn->pattern);
    err_puts("\nArguments :\n");
    err_inspect_fn_pattern(call->arguments);
    err_puts("\n");
    env->frame = frame_clean(&frame);
    return false;
  }
  if (! env_eval_progn(env, fn->algo, &tag)) {
    list_delete_all(tmp);
    env->frame = frame_clean(&frame);
    return false;
  }
  *dest = tag;
  list_delete_all(tmp);
  env->frame = frame_clean(&frame);
  return true;
}

bool env_eval_equal_list (s_env *env, const s_list *a, const s_list *b,
                          s_list **dest)
{
  s_list *a_next;
  s_list *b_next;
  s_list *tmp = NULL;
  s_list **t;
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
    *t = list_new(NULL, NULL);
    if (! env_eval_equal_tag(env, &a->tag, &b->tag, &(*t)->tag))
      goto ko;
    a_next = list_next(a);
    b_next = list_next(b);
    if (! a_next || ! b_next) {
      if (! env_eval_equal_tag(env, &a->next, &b->next, &(*t)->next))
        goto ko;
      break;
    }
    a = a_next;
    b = b_next;
    t = &(*t)->next.data.list;
  }
 ok:
  *dest = tmp;
  return true;
 ko:
  list_delete_all(tmp);
  return false;
}

bool env_eval_equal_tag (s_env *env, const s_tag *a, const s_tag *b,
                         s_tag *dest)
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
  is_unbound_a = a->type == TAG_IDENT && ! tag_ident_is_bound(a);
  is_unbound_b = b->type == TAG_IDENT && ! tag_ident_is_bound(b);
  if (is_unbound_a && is_unbound_b) {
    warnx("unbound equal on both sides: %s = %s",
          a->data.ident.sym->str.ptr.ps8,
          b->data.ident.sym->str.ptr.ps8);
    return false;
  }
  if (a->type == TAG_IDENT && tag_ident_is_bound(a)) {
    if (! env_eval_ident(env, &a->data.ident, &tmp_a))
      return false;
    a = &tmp_a;
  }
  if (b->type == TAG_IDENT && tag_ident_is_bound(b)) {
    if (! env_eval_ident(env, &b->data.ident, &tmp_b))
      return false;
    b = &tmp_b;
  }
  if (a->type == TAG_CALL) {
    if (! env_eval_call(env, &a->data.call, &tmp_a))
      return false;
    a = &tmp_a;
  }
  if (b->type == TAG_CALL) {
    if (! env_eval_call(env, &b->data.call, &tmp_b))
      return false;
    b = &tmp_b;
  }
  if (is_unbound_a) {
    tag_copy(b, dest);
    frame_binding_new(env->frame, a->data.ident.sym, b);
    return true;
  }
  if (is_unbound_b) {
    tag_copy(a, dest);
    frame_binding_new(env->frame, b->data.ident.sym, a);
    return true;
  }
  if (a->type != b->type) {
    warnx("env_eval_equal_tag: type mismatch");
    return false;
  }
  switch (a->type) {
  case TAG_VOID:
    tag_init_void(dest);
    return true;
  case TAG_LIST:
    tag_init_list(dest, NULL);
    return env_eval_equal_list(env, a->data.list, b->data.list,
                               &dest->data.list);
  case TAG_TUPLE:
    dest->type = TAG_TUPLE;
    return env_eval_equal_tuple(env, &a->data.tuple, &b->data.tuple,
                                &dest->data.tuple);
  case TAG_CALL:
  case TAG_CALL_FN:
  case TAG_CALL_MACRO:
  case TAG_QUOTE:
  case TAG_ARRAY:
  case TAG_BOOL:
  case TAG_CFN:
  case TAG_CHARACTER:
  case TAG_F32:
  case TAG_F64:
  case TAG_FN:
  case TAG_IDENT:
  case TAG_INTEGER:
  case TAG_PTAG:
  case TAG_S16:
  case TAG_S32:
  case TAG_S64:
  case TAG_S8:
  case TAG_STR:
  case TAG_SYM:
  case TAG_U16:
  case TAG_U32:
  case TAG_U64:
  case TAG_U8:
  case TAG_VAR:
    if (compare_tag(a, b)) {
      warnx("env_eval_compare_tag: value mismatch");
      return false;
    }
    tag_copy(a, dest);
    return true;
  }
  error("env_eval_equal_tag: invalid tag");
  return false;
}

bool env_eval_equal_tuple (s_env *env, const s_tuple *a,
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
    if (! env_eval_equal_tag(env, a->tag + i, b->tag + i, tmp.tag + i)) {
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
  const s_tag *tag;
  s_tag tmp;
  assert(env);
  assert(ident);
  if (! ((tag = frame_get(env->frame, ident->sym)) ||
         (tag = module_get(env->current_module, ident->sym, &tmp)))) {
    warnx("unbound ident: %s", ident->sym->str.ptr.ps8);
    return false;
  }
  tag_copy(tag, dest);
  return true;
}

bool env_eval_progn (s_env *env, const s_list *program, s_tag *dest)
{
  const s_list *next;
  s_tag tmp;
  assert(env);
  assert(program);
  assert(dest);
  while (program) {
    next = list_next(program);
    if (! env_eval_tag(env, &program->tag, &tmp))
      return false;
    if (next)
      tag_clean(&tmp);
    program = next;
  }
  *dest = tmp;
  return true;
}

bool env_eval_tag (s_env *env, const s_tag *tag, s_tag *dest)
{
  switch (tag->type) {
  case TAG_VOID:
    tag_init_void(dest);
    return true;
  case TAG_CALL:
    return env_eval_call(env, &tag->data.call, dest);
  case TAG_CALL_FN:
    return env_eval_call_fn(env, &tag->data.call, dest);
  case TAG_CALL_MACRO:
    return env_eval_call_macro(env, &tag->data.call, dest);
  case TAG_IDENT:
    return env_eval_ident(env, &tag->data.ident, dest);
  case TAG_ARRAY:
  case TAG_BOOL:
  case TAG_CFN:
  case TAG_CHARACTER:
  case TAG_F32:
  case TAG_F64:
  case TAG_FN:
  case TAG_INTEGER:
  case TAG_LIST:
  case TAG_PTAG:
  case TAG_QUOTE:
  case TAG_S16:
  case TAG_S32:
  case TAG_S64:
  case TAG_S8:
  case TAG_STR:
  case TAG_SYM:
  case TAG_TUPLE:
  case TAG_U16:
  case TAG_U32:
  case TAG_U64:
  case TAG_U8:
  case TAG_VAR:
    tag_copy(tag, dest);
    return true;
  }
  assert(! "env_eval_tag: invalid tag");
  errx(1, "env_eval_tag: invalid tag");
  return false;
}

s_env * env_init (s_env *env)
{
  assert(env);
  env->error_handler = NULL;
  env->frame = frame_new(NULL);
  buf_init_alloc(&env->in, BUF_SIZE);
  buf_file_open_r(&env->in, stdin);
  buf_init_alloc(&env->out, BUF_SIZE);
  buf_file_open_w(&env->out, stdout);
  buf_init_alloc(&env->err, BUF_SIZE);
  buf_file_open_w(&env->err, stderr);
  facts_init(&env->facts);
  /* TODO: module path */
  if (! access("lib/c3/0.1", X_OK))
    str_init_1(&env->module_path, NULL, "lib/c3/0.1");
  else if (! access("../lib/c3/0.1", X_OK))
    str_init_1(&env->module_path, NULL, "../lib/c3/0.1");
  else if (! access("../../lib/c3/0.1", X_OK))
    str_init_1(&env->module_path, NULL, "../../lib/c3/0.1");
  else if (! access(PREFIX "/lib/c3/0.1", X_OK))
    str_init_1(&env->module_path, NULL, PREFIX "/lib/c3/0.1");
  else {
    assert(! "env_init: module path not found");
    err(1, "env_init: module_path not found");
  }
  env->current_module = &env->c3_module;
  env->c3_module.name = sym_1("C3");
  if (! module_load(&env->c3_module, sym_1("C3"), &env->facts)) {
    return NULL;
  }
  return env;
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

s_module * env_module_load (s_env *env, s_module *module,
                            const s_sym *name, s_facts *facts)
{
  s_str path;
  assert(env);
  assert(module);
  assert(name);
  assert(facts);
  module->name = name;
  module->facts = facts;
  if (! module_name_path(&env->module_path, name, &path))
    return 0;
  /*
  buf_write_1(&env->out, "module_load ");
  buf_write_str(&env->out, &name->str);
  buf_write_1(&env->out, " -> ");
  buf_write_str(&env->out, &path);
  buf_write_s8(&env->out, '\n');
  buf_flush(&env->out);
  */
  if (facts_load_file(facts, &path) < 0) {
    str_clean(&path);
    return 0;
  }
  str_clean(&path);
  return module;
}

bool env_operator_is_binary (s_env *env, const s_ident *op)
{
  s_facts_with_cursor cursor;
  s8 r;
  s_tag tag_ident;
  s_tag tag_is_a;
  s_tag tag_arity;
  s_tag tag_operator;
  s_tag tag_two;
  s_ident tmp;
  assert(env);
  assert(op);
  tmp = *op;
  ident_resolve_module(&tmp, env);
  tag_init_ident(&tag_ident, &tmp);
  tag_init_1(    &tag_is_a, ":is_a");
  tag_init_1(    &tag_operator, ":operator");
  tag_init_1(    &tag_arity, ":arity");
  tag_init_1(    &tag_two, "2");
  facts_with(&env->facts, &cursor, (t_facts_spec) {
      &tag_ident, &tag_is_a, &tag_operator,
                  &tag_arity, &tag_two,
                  NULL, NULL });
  r = facts_with_cursor_next(&cursor) ? true : false;
  facts_with_cursor_clean(&cursor);
  return r;
}

bool env_operator_is_right_associative (s_env *env, const s_ident *op)
{
  s_facts_with_cursor cursor;
  s8 r;
  s_tag tag_ident;
  s_tag tag_operator_assoc;
  s_tag tag_right;
  s_ident tmp;
  assert(env);
  assert(op);
  tmp = *op;
  ident_resolve_module(&tmp, env);
  tag_init_ident(&tag_ident, &tmp);
  tag_init_1(    &tag_operator_assoc, ":operator_associativity");
  tag_init_1(    &tag_right, ":right");
  facts_with(&env->facts, &cursor, (t_facts_spec) {
      &tag_ident, &tag_operator_assoc, &tag_right,
      NULL, NULL });
  r = facts_with_cursor_next(&cursor) ? true : false;
  facts_with_cursor_clean(&cursor);
  return r;
}

bool env_operator_is_unary (s_env *env, const s_ident *op)
{
  s_facts_with_cursor cursor;
  s8 r;
  s_tag tag_ident;
  s_tag tag_is_a;
  s_tag tag_arity;
  s_tag tag_one;
  s_tag tag_operator;
  s_ident tmp;
  assert(env);
  assert(op);
  tmp = *op;
  ident_resolve_module(&tmp, env);
  tag_init_ident(&tag_ident, &tmp);
  tag_init_1(    &tag_is_a, ":is_a");
  tag_init_1(    &tag_operator, ":operator");
  tag_init_1(    &tag_arity, ":arity");
  tag_init_1(    &tag_one, "1");
  facts_with(&env->facts, &cursor, (t_facts_spec) {
      &tag_ident, &tag_is_a, &tag_operator,
                  &tag_arity, &tag_one,
                  NULL, NULL });
  r = facts_with_cursor_next(&cursor) ? true : false;
  facts_with_cursor_clean(&cursor);
  return r;
}

s8 env_operator_precedence (s_env *env, const s_ident *op)
{
  s_facts_with_cursor cursor;
  s_tag tag_ident;
  s_tag tag_operator_precedence;
  s_tag tag_var;
  s_ident tmp;
  assert(env);
  assert(op);
  tmp = *op;
  ident_resolve_module(&tmp, env);
  tag_init_ident(&tag_ident, &tmp);
  tag_init_1(    &tag_operator_precedence, ":operator_precedence");
  tag_init_var(  &tag_var);
  facts_with(&env->facts, &cursor, (t_facts_spec) {
      &tag_ident, &tag_operator_precedence, &tag_var,
      NULL, NULL });
  if (! facts_with_cursor_next(&cursor))
    goto ko;
  if (tag_var.type != TAG_U8) {
    warnx("%s.%s: invalid operator_precedence type",
          tmp.module_name->str.ptr.ps8,
          tmp.sym->str.ptr.ps8);
    goto ko;
  }
  facts_with_cursor_clean(&cursor);
  return tag_var.data.u8;
 ko:
  facts_with_cursor_clean(&cursor);
  return -1;
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

s_tag * env_unwind_protect (s_env *env, s_tag *protected, s_list *cleanup,
                            s_tag *dest)
{
  s_tag tmp;
  s_unwind_protect unwind_protect;
  assert(env);
  assert(protected);
  if (setjmp(unwind_protect.buf)) {
    env_pop_unwind_protect(env);
    env_eval_progn(env, cleanup, &tmp);
    longjmp(*unwind_protect.jmp, 1);
  }
  env_eval_tag(env, protected, dest);
  env_pop_unwind_protect(env);
  env_eval_progn(env, cleanup, &tmp);
  return dest;
}
