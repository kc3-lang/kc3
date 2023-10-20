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
#include <string.h>
#include <time.h>
#include <unistd.h>
#include "array.h"
#include "binding.h"
#include "buf.h"
#include "buf_file.h"
#include "buf_inspect.h"
#include "buf_save.h"
#include "call.h"
#include "cfn.h"
#include "compare.h"
#include "env.h"
#include "error.h"
#include "error_handler.h"
#include "facts.h"
#include "facts_cursor.h"
#include "facts_with.h"
#include "facts_with_cursor.h"
#include "file.h"
#include "fn.h"
#include "frame.h"
#include "ident.h"
#include "io.h"
#include "list.h"
#include "module.h"
#include "str.h"
#include "tag.h"
#include "tuple.h"

s_env g_c3_env;

bool env_eval_array_cast (s_env *env, s_array *tmp, const s_tag *tag,
                          u8 *data, uw size);

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

bool env_eval_array (s_env *env, const s_array *array, s_array *dest)
{
  u8 *data;
  uw i;
  uw item_size;
  s_tag *tag;
  s_array tmp;
  assert(env);
  assert(array);
  assert(dest);
  array_copy(array, &tmp);
  item_size = tmp.dimensions[tmp.dimension - 1].item_size;
  if (tmp.data) {
    *dest = tmp;
    return true;
  }
  assert(array->tags);
  if (! tmp.data &&
      ! (tmp.data = calloc(tmp.dimensions[0].count,
                           tmp.dimensions[0].item_size))) {
    assert(! "env_eval_array: out of memory: data");
    errx(1, "env_eval_array: out of memory: data");
    return false;
  }
  data = tmp.data;
  tag = tmp.tags;
  i = 0;
  while (i < tmp.count) {
    if (! env_eval_array_cast(env, &tmp, tag, data, item_size)) {
      array_clean(&tmp);
      return false;
    }
    data += item_size;
    tag++;
    i++;
  }
  *dest = tmp;
  return true;
}

bool env_eval_array_cast (s_env *env, s_array *array, const s_tag *tag,
                          u8 *data, uw size)
{
  s_call call;
  s_tag tag_eval;
  e_tag_type tag_type;
  void *data_eval;
  assert(env);
  assert(array);
  assert(tag);
  assert(data);
  assert(size);
  if (! call_init_cast(&call, array->type, tag))
    return false;
  if (! env_eval_call(env, &call, &tag_eval)) {
    call_clean(&call);
    return false;
  }
  tag_type = array_type_to_tag_type(array->type);
  data_eval = tag_to_pointer(&tag_eval, tag_type);
  memcpy(data, data_eval, size);
  call_clean(&call);
  return true;
}

bool env_eval_array_tag (s_env *env, const s_array *array, s_tag *dest)
{
  s_array tmp;
  if (! env_eval_array(env, array, &tmp))
    return false;
  dest->type = TAG_ARRAY;
  dest->data.array = tmp;
  return true;
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
  if (c.cfn)
    result = env_eval_call_cfn(env, &c, dest);
  else if (c.fn)
    result = env_eval_call_fn(env, &c, dest);
  else {
    warnx("env_eval_call: could not resolve call %s.%s.",
          c.ident.module->str.ptr.ps8,
          c.ident.sym->str.ptr.ps8);
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
  s_list *args_final = NULL;
  s_cfn *cfn;
  s_tag tag;
  assert(env);
  assert(call);
  assert(dest);
  cfn = call->cfn;
  assert(cfn);
  if (call->arguments) {
    if (cfn->macro || cfn->special_operator)
      args_final = call->arguments;
    else {
      if (! env_eval_call_arguments(env, call->arguments, &args))
        return false;
      args_final = args;
    }
  }
  if (! cfn_apply(cfn, args_final, &tag)) {
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
  s_list *args_final = NULL;
  s_frame frame;
  s_fn *fn;
  s_fn_clause *clause;
  s_tag tag;
  s_list *tmp = NULL;
  assert(env);
  assert(call);
  assert(dest);
  fn = call->fn;
  assert(fn);
  frame_init(&frame, env->frame);
  env->frame = &frame;
  clause = fn->clauses;
  if (call->arguments) {
    if (fn->macro || fn->special_operator)
      args_final = call->arguments;
    else {
      if (! env_eval_call_arguments(env, call->arguments, &args)) {
        env->frame = frame_clean(&frame);
        return false;
      }
      args_final = args;
    }
    /* FIXME: bindings go through clauses */
    while (clause && ! env_eval_equal_list(env, clause->pattern,
                                           args_final, &tmp))
      clause = clause->next_clause;
    if (! clause) {
      err_puts("env_eval_call_fn: no clause matching.\nTried clauses :\n");
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
      env->frame = frame_clean(&frame);
      return false;
    }
  }
  if (! env_eval_progn(env, clause->algo, &tag)) {
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

bool env_eval_call_resolve (s_env *env, s_call *call)
{
  const s_tag *value;
  assert(env);
  assert(call);
  if ((value = frame_get(env->frame, call->ident.sym))) {
    if (value->type == TAG_CFN) {
      call->cfn = cfn_new_copy(&value->data.cfn);
      return true;
    }
    else if (value->type == TAG_FN) {
      call->fn = fn_new_copy(&value->data.fn);
      return true;
    }
  }
  ident_resolve_module(&call->ident, env);
  if (! module_ensure_loaded(call->ident.module, &env->facts))
    return false;
  return call_get(call, &env->facts);
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
      goto ok;
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
  is_unbound_a = a->type == TAG_IDENT;
  is_unbound_b = b->type == TAG_IDENT;
  if (is_unbound_a && is_unbound_b) {
    warnx("unbound equal on both sides: %s = %s",
          a->data.ident.sym->str.ptr.ps8,
          b->data.ident.sym->str.ptr.ps8);
    return false;
  }
  if (is_unbound_a) {
    env_eval_tag(env, b, dest);
    frame_binding_new(env->frame, a->data.ident.sym, dest);
    return true;
  }
  if (is_unbound_b) {
    env_eval_tag(env, a, dest);
    frame_binding_new(env->frame, b->data.ident.sym, dest);
    return true;
  }
  switch (a->type) {
  case TAG_F32:
  case TAG_F64:
  case TAG_INTEGER:
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
    case TAG_F32:
    case TAG_F64:
    case TAG_INTEGER:
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
        warnx("env_eval_equal_tag: numeric value mismatch");
        return false;
      }
      tag_copy(a, dest);
      return true;
    default:
      break;
    }
  default:
    break;
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
  case TAG_QUOTE:
  case TAG_ARRAY:
  case TAG_BOOL:
  case TAG_CFN:
  case TAG_CHARACTER:
  case TAG_FACT:
  case TAG_FN:
  case TAG_IDENT:
  case TAG_INTEGER:
  case TAG_PTAG:
  case TAG_STR:
  case TAG_SYM:
  case TAG_VAR:
    if (compare_tag(a, b)) {
      warnx("env_eval_compare_tag: value mismatch");
      return false;
    }
    tag_copy(a, dest);
    return true;
  default:
    break;
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
  s_ident tmp_ident;
  assert(env);
  assert(ident);
  ident_copy(ident, &tmp_ident);
  ident_resolve_module(&tmp_ident, env);
  if (! ((tag = frame_get(env->frame, tmp_ident.sym)) ||
         (tag = ident_get(&tmp_ident, &env->facts, &tmp)))) {
    warnx("unbound ident: %s.%s",
          tmp_ident.module->str.ptr.ps8,
          tmp_ident.sym->str.ptr.ps8);
    return false;
  }
  tag_copy(tag, dest);
  return true;
}

bool env_eval_list (s_env *env, const s_list *list, s_tag *dest)
{
  s_list *next;
  s_list *tmp = NULL;
  s_list **tail = &tmp;
  assert(env);
  assert(dest);
  while (list) {
    *tail = list_new(NULL, NULL);
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

bool env_eval_quote(s_env *env, const s_quote *quote, s_tag *dest)
{
  assert(env);
  assert(quote);
  assert(dest);
  (void) env;
  if (! tag_copy(quote->tag, dest))
    return false;
  return true;
}

bool env_eval_tag (s_env *env, const s_tag *tag, s_tag *dest)
{
  switch (tag->type) {
  case TAG_VOID:
    tag_init_void(dest);
    return true;
  case TAG_ARRAY:
    return env_eval_array_tag(env, &tag->data.array, dest);
  case TAG_CALL:
    return env_eval_call(env, &tag->data.call, dest);
  case TAG_IDENT:
    return env_eval_ident(env, &tag->data.ident, dest);
  case TAG_LIST:
    return env_eval_list(env, tag->data.list, dest);
  case TAG_QUOTE:
    return env_eval_quote(env, &tag->data.quote, dest);
  case TAG_TUPLE:
    return env_eval_tuple(env, &tag->data.tuple, dest);
  case TAG_BOOL:
  case TAG_CFN:
  case TAG_CHARACTER:
  case TAG_F32:
  case TAG_F64:
  case TAG_FACT:
  case TAG_FN:
  case TAG_INTEGER:
  case TAG_PTAG:
  case TAG_S8:
  case TAG_S16:
  case TAG_S32:
  case TAG_S64:
  case TAG_SW:
  case TAG_STR:
  case TAG_SYM:
  case TAG_U8:
  case TAG_U16:
  case TAG_U32:
  case TAG_U64:
  case TAG_UW:
  case TAG_VAR:
    tag_copy(tag, dest);
    return true;
  }
  assert(! "env_eval_tag: unknown tag type");
  errx(1, "env_eval_tag: unknown tag type: %d", tag->type);
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
    if (! env_eval_tag(env, &tuple->tag[i], &tmp.tag[i]))
      return false;
    i++;
  }
  dest->type = TAG_TUPLE;
  dest->data.tuple = tmp;
  return true;
}

s64 env_gettime ()
{
  struct timespec time;
  clock_gettime(CLOCK_REALTIME, &time);
  return time.tv_sec;
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
  env->current_module = sym_1("C3");
  if (! module_load(sym_1("C3"), &env->facts)) {
    env_clean(env);
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

bool env_module_load (const s_sym *module, s_env *env, s_facts *facts)
{
  s_str path;
  s_tag tag_module_name;
  s_tag tag_load_time;
  s_tag tag_time;
  assert(env);
  assert(module);
  assert(facts);
  if (! module_path(module, &env->module_path, &path)) {
    warnx("env_module_load: %s: module_path",
          module->str.ptr.ps8);
    return false;
  }
  if (facts_load_file(facts, &path) < 0) {
    warnx("env_module_load: %s: facts_load_file",
          path.ptr.ps8);
    str_clean(&path);
    return false;
  }
  str_clean(&path);
  tag_init_sym(&tag_module_name, module);
  tag_init_sym(&tag_load_time, sym_1("load_time"));
  tag_init_time(&tag_time);
  facts_replace_tags(facts, &tag_module_name, &tag_load_time, &tag_time);
  tag_clean(&tag_time);
  return true;
}

bool env_module_maybe_reload (const s_sym *module, s_env *env,
                              s_facts *facts)
{
  s_str path;
  bool r = true;
  s_tag tag_load_time;
  s_tag tag_mtime;
  if (! module_load_time(module, facts, &tag_load_time))
    return false;
  if (! module_path(module, &env->module_path, &path)) {
    return false;
  }
  if (! file_mtime(&path, &tag_mtime)) {
    str_clean(&path);
    return false;
  }
  if (compare_tag(&tag_load_time, &tag_mtime) < 0)
    r = module_load(module, facts);
  str_clean(&path);
  tag_clean(&tag_mtime);
  return r;
}

s8 env_operator_arity (s_env *env, const s_ident *op)
{
  s_facts_cursor cursor;
  s8 r = -1;
  s_tag tag_op;
  s_tag tag_arity;
  s_tag tag_var;
  assert(env);
  assert(op);
  tag_init_ident(&tag_op, op);
  tag_init_1(    &tag_arity, ":arity");
  tag_init_var(  &tag_var);
  facts_with_tags(&env->facts, &cursor, &tag_op, &tag_arity, &tag_var);
  if (facts_cursor_next(&cursor) &&
      tag_var.type == TAG_U8) {
    r = tag_var.data.u8;
  }
  else
    warnx("env_operator_arity: "
          "arity for operator %s not found in module %s",
          op->sym->str.ptr.ps8,
          op->module->str.ptr.ps8);
  facts_cursor_clean(&cursor);
  return r;
}

bool env_operator_find (s_env *env, const s_ident *op)
{
  s_tag tag_is_a;
  s_tag tag_op;
  s_tag tag_operator;
  assert(env);
  assert(op);
  tag_init_1(    &tag_is_a, ":is_a");
  tag_init_ident(&tag_op, op);
  tag_init_1(    &tag_operator, ":operator");
  return facts_find_fact_by_tags(&env->facts, &tag_op, &tag_is_a,
                                 &tag_operator) ? 1 : 0;
}

s_ident * env_operator_ident (s_env *env, const s_ident *op,
                              s_ident *dest)
{
  assert(env);
  assert(op);
  assert(dest);
  dest->module = op->module;
  dest->sym = env_operator_symbol(env, op);
  return dest;
}

bool env_operator_is_right_associative (s_env *env, const s_ident *op)
{
  s_tag tag_assoc;
  s_tag tag_op;
  s_tag tag_right;
  assert(env);
  assert(op);
  tag_init_1(    &tag_assoc, ":operator_associativity");
  tag_init_ident(&tag_op, op);
  tag_init_1(    &tag_right, ":right");
  return facts_find_fact_by_tags(&env->facts, &tag_op, &tag_assoc,
                                 &tag_right) ? 1 : 0;
}

s8 env_operator_precedence (s_env *env, const s_ident *op)
{
  s_facts_cursor cursor;
  s8 r = -1;
  s_tag tag_op;
  s_tag tag_precedence;
  s_tag tag_var;
  assert(env);
  assert(op);
  tag_init_ident(&tag_op, op);
  tag_init_1(    &tag_precedence, ":operator_precedence");
  tag_init_var(  &tag_var);
  facts_with_tags(&env->facts, &cursor, &tag_op, &tag_precedence,
                  &tag_var);
  if (facts_cursor_next(&cursor) &&
      tag_var.type == TAG_U8) {
    r = tag_var.data.u8;
  }
  else
    warnx("env_operator_precedence: "
          "precedence for operator %s not found in module %s",
          op->sym->str.ptr.ps8,
          op->module->str.ptr.ps8);
  facts_cursor_clean(&cursor);
  return r;
}

s_ident * env_operator_resolve (s_env *env, const s_ident *op,
                                u8 arity, s_ident *dest)
{
  s_facts_with_cursor cursor;
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
  tmp = *op;
  ident_resolve_module(&tmp, env);
  tag_init_1(  &tag_arity, ":arity");
  tag_init_u8( &tag_arity_u8, arity);
  tag_init_1(  &tag_is_a, ":is_a");
  tag_init_1(  &tag_module, ":module");
  tag_init_sym(&tag_module_name, tmp.module);
  tag_init_1(  &tag_operator, ":operator");
  tag_init_var(&tag_var);
  tag_init_sym(&tag_sym, tmp.sym);
  tag_init_1(  &tag_symbol, ":symbol");
  facts_with(&env->facts, &cursor, (t_facts_spec) {
      &tag_module_name, &tag_is_a, &tag_module,
      &tag_operator, &tag_var, NULL,   /* module exports operator */
      &tag_var, &tag_symbol, &tag_sym,
      &tag_arity, &tag_arity_u8,
      NULL, NULL });
  if (facts_with_cursor_next(&cursor)) {
    if (tag_var.type == TAG_IDENT) {
      *dest = tag_var.data.ident;
      facts_with_cursor_clean(&cursor);
      return dest;
    }
  }
  if (false)
    warnx("env_operator_resolve: operator %s/%d not found in module %s",
          tmp.sym->str.ptr.ps8,
          arity,
          tmp.module->str.ptr.ps8);
  facts_with_cursor_clean(&cursor);
  return NULL;
}

const s_sym * env_operator_symbol (s_env *env, const s_ident *op)
{
  s_facts_cursor cursor;
  const s_sym *r = NULL;
  s_tag tag_op;
  s_tag tag_symbol;
  s_tag tag_var;
  assert(env);
  assert(op);
  tag_init_ident(&tag_op, op);
  tag_init_1(    &tag_symbol, ":symbol");
  tag_init_var(  &tag_var);
  facts_with_tags(&env->facts, &cursor, &tag_op, &tag_symbol, &tag_var);
  if (facts_cursor_next(&cursor) &&
      tag_var.type == TAG_SYM) {
    r = tag_var.data.sym;
  }
  else
    warnx("env_operator_symbol: "
          "symbol for operator %s not found in module %s",
          op->sym->str.ptr.ps8,
          op->module->str.ptr.ps8);
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

bool env_tag_ident_is_bound (const s_env *env, const s_tag *tag,
                             s_facts *facts)
{
  s_tag tmp;
  assert(tag);
  assert(tag->type == TAG_IDENT);
  return tag->type == TAG_IDENT &&
    (frame_get(env->frame, tag->data.ident.sym) ||
     ident_get(&tag->data.ident, facts, &tmp));
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
