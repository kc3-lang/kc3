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
#include "assert.h"
#include <err.h>
#include <libgen.h>
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
#include "data.h"
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
#include "map.h"
#include "module.h"
#include "str.h"
#include "struct.h"
#include "struct_type.h"
#include "tag.h"
#include "tuple.h"

s_env g_c3_env;

static s_env * env_init_args (s_env *env, int argc, char **argv);

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
  str_clean(&env->argv0_dir);
  str_clean(&env->module_path);
  list_delete_all(env->path);
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
      tmp.data = tmp.free_data = calloc(tmp.dimensions[0].count,
                                        tmp.dimensions[0].item_size);
      if (! tmp.data) {
        warn("env_eval_array: failed to allocate memory");
        assert(! "env_eval_array: failed to allocate memory");
        return false;
      }
      data = tmp.data;
      tag = tmp.tags;
      i = 0;
      while (i < tmp.count) {
        if (! env_eval_tag(env, tag, &tag_eval))
          goto ko;
        if (! data_init_cast(tmp.element_type, data, &tag_eval)) {
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

bool env_eval_call (s_env *env, const s_call *call, s_tag *dest)
{
  s_call c;
  bool result = false;
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
    warnx("env_eval_call: could not resolve call %s.%s.",
          c.ident.module->str.ptr.pchar,
          c.ident.sym->str.ptr.pchar);
    result = false;
  }
  call_clean(&c);
  return result;
}

bool env_eval_call_arguments (s_env *env, const s_list *args,
                              s_list **dest)
{
  s_list **tail;
  s_list *tmp;
  tail = &tmp;
  while (args) {
    *tail = list_new(NULL);
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
  assert(env);
  assert(call);
  assert(dest);
  return env_eval_fn_call(env, call->fn, call->arguments, dest);
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
    if (! env_eval_equal_tag(env, &a->tag, &b->tag,
                             &(*t)->tag))
      goto ko;
    a_next = list_next(a);
    b_next = list_next(b);
    if (! a_next || ! b_next) {
      if (! env_eval_equal_tag(env, &a->next, &b->next,
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

bool env_eval_equal_map (s_env *env, const s_map *a,
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
        if (! env_eval_equal_tag(env, a->value + i, b->value + j,
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
          a->data.ident.sym->str.ptr.pchar,
          b->data.ident.sym->str.ptr.pchar);
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
  case TAG_F128:
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
    case TAG_F128:
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
    return env_eval_equal_list(env, a->data.list, b->data.list,
                               &dest->data.list);
  case TAG_MAP:
    dest->type = TAG_MAP;
    return env_eval_equal_map(env, &a->data.map, &b->data.map,
                              &dest->data.map);
  /*
  case TAG_STRUCT:
    dest->type = TAG_STRUCT;
    return env_eval_equal_struct(env, &a->data.struct_,
                                 &b->data.struct_, &dest->data.struct_);
  */
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
  case TAG_PTR:
  case TAG_PTR_FREE:
  case TAG_STR:
  case TAG_STRUCT:
  case TAG_STRUCT_TYPE:
  case TAG_SYM:
  case TAG_VAR:
    if (compare_tag(a, b)) {
      warnx("env_eval_compare_tag: value mismatch");
      return false;
    }
    tag_init_copy(dest, a);
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

bool env_eval_fn_call (s_env *env, const s_fn *fn,
                       const s_list *arguments, s_tag *dest)
{
  s_list *args = NULL;
  const s_list *args_final = NULL;
  s_fn_clause *clause;
  s_frame frame;
  s_tag tag;
  s_list *tmp = NULL;
  assert(env);
  assert(fn);
  assert(dest);
  frame_init(&frame, env->frame);
  env->frame = &frame;
  clause = fn->clauses;
  if (arguments) {
    if (fn->macro || fn->special_operator)
      args_final = arguments;
    else {
      if (! env_eval_call_arguments(env, arguments, &args)) {
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

bool env_eval_ident (s_env *env, const s_ident *ident, s_tag *dest)
{
  const s_tag *tag;
  s_tag tmp;
  s_ident tmp_ident;
  assert(env);
  assert(ident);
  ident_init_copy(&tmp_ident, ident);
  ident_resolve_module(&tmp_ident, env);
  if (! ((tag = frame_get(env->frame, tmp_ident.sym)) ||
         (tag = ident_get(&tmp_ident, &env->facts, &tmp)))) {
    warnx("unbound ident: %s.%s",
          tmp_ident.module->str.ptr.pchar,
          tmp_ident.sym->str.ptr.pchar);
    return false;
  }
  tag_init_copy(dest, tag);
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

bool env_eval_quote (s_env *env, const s_quote *quote, s_tag *dest)
{
  assert(env);
  assert(quote);
  assert(dest);
  (void) env;
  if (! tag_init_copy(dest, quote->tag))
    return false;
  return true;
}

bool env_eval_struct (s_env *env, const s_struct *s, s_tag *dest)
{
  uw i;
  s_struct *t;
  s_tag tag = {0};
  s_tag tmp = {0};
  const s_sym *type;
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
    if (! tag_type(t->type->map.value + i, &type))
      goto ko;
    if (s->tag) {
      if (! env_eval_tag(env, s->tag + i, &tag))
        goto ko;
      if (! data_init_cast(type, (s8 *) t->data + t->type->offset[i],
                           &tag)) {
        warnx("env_eval_struct:"
              " invalid type %s for key %s, expected %s.",
              tag_type_to_string(tag.type),
              t->type->map.key[i].data.sym->str.ptr.pchar,
              tag_type_to_string(t->type->map.value[i].type));
        goto ko_tag;
      }
      tag_clean(&tag);
    }
    else {
      const void *value;
      if (! tag_to_const_pointer(t->type->map.value + i, type, &value))
        goto ko;
      data_init_copy(type, (s8 *) t->data + t->type->offset[i], value);
    }
    i++;
  }
  *dest = tmp;
  return true;
 ko_tag:
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
  case TAG_CALL:
    return env_eval_call(env, &tag->data.call, dest);
  case TAG_IDENT:
    return env_eval_ident(env, &tag->data.ident, dest);
  case TAG_LIST:
    return env_eval_list(env, tag->data.list, dest);
  case TAG_MAP:
    return env_eval_map(env, &tag->data.map, dest);
  case TAG_QUOTE:
    return env_eval_quote(env, &tag->data.quote, dest);
  case TAG_STRUCT:
    return env_eval_struct(env, &tag->data.struct_, dest);
  case TAG_TUPLE:
    return env_eval_tuple(env, &tag->data.tuple, dest);
  case TAG_BOOL:
  case TAG_CFN:
  case TAG_CHARACTER:
  case TAG_F32:
  case TAG_F64:
  case TAG_F128:
  case TAG_FACT:
  case TAG_FN:
  case TAG_INTEGER:
  case TAG_PTAG:
  case TAG_PTR:
  case TAG_PTR_FREE:
  case TAG_S8:
  case TAG_S16:
  case TAG_S32:
  case TAG_S64:
  case TAG_SW:
  case TAG_STR:
  case TAG_STRUCT_TYPE:
  case TAG_SYM:
  case TAG_U8:
  case TAG_U16:
  case TAG_U32:
  case TAG_U64:
  case TAG_UW:
  case TAG_VAR:
    tag_init_copy(dest, tag);
    return true;
  }
  warnx("env_eval_tag: unknown tag type: %d", tag->type);
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
    if (! env_eval_tag(env, &tuple->tag[i], &tmp.tag[i]))
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

s_env * env_init (s_env *env, int argc, char **argv)
{
  s_str path;
  assert(env);
  if (! env_init_args(env, argc, argv))
    return NULL;
  sym_init_g_sym();
  env->error_handler = NULL;
  env->frame = frame_new(NULL);
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
  str_init_1(&path, NULL, "lib/c3/0.1/");
  if (! file_search(&path, &g_sym_x, &env->module_path)) {
    assert(! "env_init: module path not found");
    warn("env_init: module_path not found");
    return NULL;
  }
  env->current_module = &g_sym_C3;
  if (! module_load(&g_sym_C3, &env->facts)) {
    env_clean(env);
    return NULL;
  }
  return env;
}

s_env * env_init_args (s_env *env, int argc, char **argv)
{
  char a[PATH_MAX];
  s_str argv0;
  s_buf buf;
  s_str dir;
  sw r;
  assert(env);
  if (argc && argv) {
    env->argc = argc;
    env->argv = argv;
    str_init_1(&argv0, NULL, argv[0]);
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

bool env_module_is_loading (s_env *env, const s_sym *module)
{
  s_facts_cursor cursor;
  s_tag tag_module;
  s_tag tag_is_loading;
  s_tag tag_true;
  assert(env);
  assert(module);
  tag_init_sym(&tag_module, module);
  tag_init_sym(&tag_is_loading, sym_1("is_loading"));
  tag_init_bool(&tag_true, true);
  facts_with_tags(&env->facts, &cursor, &tag_module, &tag_is_loading,
                  &tag_true);
  if (facts_cursor_next(&cursor)) {
    facts_cursor_clean(&cursor);
    return true;
  }
  facts_cursor_clean(&cursor);
  return false;  
}

void env_module_is_loading_set (s_env *env, const s_sym *module,
                                bool is_loading)
{
  s_tag tag_module;
  s_tag tag_is_loading;
  s_tag tag_true;
  assert(env);
  assert(module);
  tag_init_sym(&tag_module, module);
  tag_init_sym(&tag_is_loading, sym_1("is_loading"));
  tag_init_bool(&tag_true, true);
  if (is_loading)
    facts_replace_tags(&env->facts, &tag_module, &tag_is_loading,
                       &tag_true);
  else
    facts_remove_fact_tags(&env->facts, &tag_module, &tag_is_loading,
                           &tag_true);
}

bool env_module_load (s_env *env, const s_sym *module, s_facts *facts)
{
  s_str path;
  s_struct_type *st;
  s_list        *st_spec;
  s_tag tag_module_name;
  s_tag tag_load_time;
  s_tag tag_st = {0};
  s_tag tag_struct_type;
  s_tag tag_time;
  assert(env);
  assert(module);
  assert(facts);
  if (env_module_is_loading(env, module)) {
    return true;
  }
  env_module_is_loading_set(env, module, true);
  if (! module_path(module, &env->module_path, &path)) {
    warnx("env_module_load: %s: module_path",
          module->str.ptr.pchar);
    goto ko;
  }
  tag_init_time(&tag_time);
  if (facts_load_file(facts, &path) < 0) {
    warnx("env_module_load: %s: facts_load_file",
          path.ptr.pchar);
    str_clean(&path);
    goto ko;
  }
  str_clean(&path);
  tag_init_sym(&tag_module_name, module);
  tag_init_sym(&tag_load_time, &g_sym_load_time);
  facts_replace_tags(facts, &tag_module_name, &tag_load_time,
                     &tag_time);
  tag_clean(&tag_time);
  if (env_struct_type_get_spec(env, module, &st_spec)) {
    tag_init_sym(&tag_struct_type, &g_sym_struct_type);
    tag_st.type = TAG_STRUCT_TYPE;
    st = &tag_st.data.struct_type;
    struct_type_init(st, module, st_spec);
    st->clean = env_struct_type_get_clean(env, module);
    facts_replace_tags(facts, &tag_module_name, &tag_struct_type,
                       &tag_st);
    list_delete_all(st_spec);
  }
  env_module_is_loading_set(env, module, false);
  return true;
 ko:
  env_module_is_loading_set(env, module, false);
  return false;
}

bool env_module_maybe_reload (s_env *env, const s_sym *module,
                              s_facts *facts)
{
  s_str path;
  bool r = true;
  s_tag tag_load_time = {0};
  s_tag tag_mtime;
  module_load_time(module, facts, &tag_load_time);
  if (! module_path(module, &env->module_path, &path)) {
    return false;
  }
  //io_inspect_str(&path);
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
          op->sym->str.ptr.pchar,
          op->module->str.ptr.pchar);
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
  if (env->current_module == op->module)
    dest->module = NULL;
  else
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
          op->sym->str.ptr.pchar,
          op->module->str.ptr.pchar);
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
          tmp.sym->str.ptr.pchar,
          arity,
          tmp.module->str.ptr.pchar);
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
          op->sym->str.ptr.pchar,
          op->module->str.ptr.pchar);
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

bool env_struct_type_exists (s_env *env, const s_sym *module)
{
  s_facts_cursor cursor;
  bool result;
  s_tag tag_defstruct;
  s_tag tag_module;
  s_tag tag_var;
  assert(env);
  assert(module);
  tag_init_sym(&tag_module, module);
  tag_init_sym(&tag_defstruct, &g_sym_defstruct);
  tag_init_var(&tag_var);
  env_module_maybe_reload(env, module, &env->facts);
  facts_with_tags(&env->facts, &cursor, &tag_module,
                  &tag_defstruct, &tag_var);
  result = facts_cursor_next(&cursor) ? true : false;
  facts_cursor_clean(&cursor);
  return result;
}

const s_struct_type * env_struct_type_find (s_env *env,
                                            const s_sym *module)
{
  s_facts_with_cursor cursor;
  s_fact *found;
  const s_struct_type *result;
  s_tag tag_struct_type;
  s_tag tag_module;
  s_tag tag_var;
  const s_sym *type;
  assert(env);
  assert(module);
  tag_init_sym(&tag_module, module);
  tag_init_sym(&tag_struct_type, &g_sym_struct_type);
  tag_init_var(&tag_var);
  env_module_maybe_reload(env, module, &env->facts);
  facts_with(&env->facts, &cursor, (t_facts_spec) {
      &tag_module, &tag_struct_type, &tag_var, NULL, NULL });
  found = facts_with_cursor_next(&cursor);
  if (! found) {
    facts_with_cursor_clean(&cursor);
    return NULL;
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
  result = &found->object->data.struct_type;
  facts_with_cursor_clean(&cursor);
  return result;
}

f_clean env_struct_type_get_clean (s_env *env, const s_sym *module)
{
  s_facts_with_cursor cursor;
  s_fact *found;
  s_tag tag_clean;
  s_tag tag_module;
  s_tag tag_var;
  f_clean tmp;
  const s_sym *type;
  tag_init_sym(&tag_module, module);
  tag_init_sym(&tag_clean, sym_1("clean"));
  tag_init_var(&tag_var);
  facts_with(&env->facts, &cursor, (t_facts_spec) {
      &tag_module, &tag_clean, &tag_var, NULL, NULL });
  found = facts_with_cursor_next(&cursor);
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
  s_facts_with_cursor cursor;
  s_fact *found;
  s_tag tag_defstruct;
  s_tag tag_module;
  s_tag tag_var;
  s_tag tmp;
  assert(env);
  assert(module);
  assert(dest);
  tag_init_sym(&tag_defstruct, &g_sym_defstruct);
  tag_init_sym(&tag_module, module);
  tag_init_var(&tag_var);
  if (! env_module_maybe_reload(env, module, &env->facts))
    return NULL;
  facts_with(&env->facts, &cursor, (t_facts_spec) {
      &tag_module, &tag_defstruct, &tag_var, NULL, NULL });
  found = facts_with_cursor_next(&cursor);
  if (! found) {
    facts_with_cursor_clean(&cursor);
    return NULL;
  }
  if (! env_eval_tag(env, found->object, &tmp)) {
    facts_with_cursor_clean(&cursor);
    return NULL;
  }
  facts_with_cursor_clean(&cursor);
  if (tmp.type != TAG_LIST ||
      ! list_is_plist(tmp.data.list)) {
    warnx("env_get_struct_type_spec: module %s"
          " has a defstruct that is not a property list",
          module->str.ptr.pchar);
    tag_clean(&tmp);
    return NULL;
  }
  *dest = tmp.data.list;
  return dest;
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
