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
#include <dlfcn.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef WIN32
# include <sys/wait.h>
#endif

#include <unistd.h>
#include "alist.h"
#include "alloc.h"
#include "array.h"
#include "assert.h"
#include "bool.h"
#include "buf.h"
#include "buf_fd.h"
#include "buf_parse.h"
#include "call.h"
#include "env.h"
#include "fact.h"
#include "facts.h"
#include "facts_cursor.h"
#include "fd.h"
#include "kc3_main.h"
#include "list.h"
#include "map.h"
#include "str.h"
#include "struct.h"
#include "struct_type.h"
#include "sym.h"
#include "tag.h"
#include "u8.h"

const s_str g_kc3_base_binary = {{NULL}, 2, {"01"}};
const s_str g_kc3_base_octal = {{NULL}, 8, {"01234567"}};
const s_str g_kc3_base_decimal = {{NULL}, 10, {"0123456789"}};
const s_str g_kc3_base_hexadecimal = {{NULL}, 16, {"0123456789abcdef"}};
const s_str g_kc3_bases_hexadecimal[2] = {{{NULL}, 16,
                                           {"0123456789abcdef"}},
                                         {{NULL}, 16,
                                          {"0123456789ABCDEF"}}};
const s_str g_kc3_base64 = {{NULL}, 64, {"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                                         "abcdefghijklmnopqrstuvwxyz"
                                         "0123456789+/"}};
const s_str g_kc3_base64url = {{NULL}, 64, {"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                                            "abcdefghijklmnopqrstuvwxyz"
                                            "0123456789-_"}};
sw          g_kc3_exit_code = 1;

#ifndef WIN32
void kc3_system_pipe_exec (s32 pipe_fd, char **argv,
                           const s_list * const *list);
#endif

#ifdef WIN32
int fork (void)
{
  return -1;
}
#endif

s_tag * kc3_access (s_tag *tag, s_list **key,
                    s_tag *dest)
{
  s_struct s = {0};
  assert(tag);
  assert(key);
  assert(dest);
  switch (tag->type) {
  case TAG_ARRAY:
    return array_access(&tag->data.array, *key, dest);
  case TAG_LIST:
    if (list_is_alist(tag->data.list)) {
      if (! alist_access(tag->data.list, *key, dest))
        return list_access(tag->data.list, *key, dest);
      return dest;
    }
    else
      return list_access(tag->data.list, *key, dest);
  case TAG_MAP:
    return map_access(&tag->data.map, *key, dest);
  case TAG_STRUCT:
    return struct_access(&tag->data.struct_, *key, dest);
  case TAG_TIME:
    if (! struct_init_with_data(&s, &g_sym_Time,
                                (s_time *) &tag->data.time,
                                false))
      return NULL;
    return struct_access(&s, *key, dest);
  default:
    break;
  }
  err_write_1("kc3_access: cannot access tag type ");
  err_write_1(tag_type_to_string(tag->type));
  err_write_1(" for key ");
  err_inspect_list(*key);
  err_write_1("\n");
  return NULL;
}

bool * kc3_and (s_tag *a, s_tag *b, bool *dest)
{
  return env_and(&g_kc3_env, a, b, dest);
}

s_list ** kc3_args (s_list **dest)
{
  return env_args(&g_kc3_env, dest);
}

void kc3_break (void)
{
  err_puts("break");
  assert(! "break");
  abort();
}

s_tag * kc3_buf_parse_tag (s_buf *buf, s_tag *dest)
{
  sw r;
  r = buf_parse_tag(buf, dest);
  if (r <= 0)
    return NULL;
  return dest;
}

void kc3_clean (s_env *env)
{
  if (! env)
    env = &g_kc3_env;
  env_clean(env);
  sym_delete_all();
}

s_tag * kc3_def (const s_call *call, s_tag *dest)
{
  return env_kc3_def(&g_kc3_env, call, dest);
}

s_tag * kc3_defmodule (const s_sym **name, const s_block *block, s_tag *dest)
{
  return env_defmodule(&g_kc3_env, name, block, dest);
}

s_tag * kc3_defoperator (const s_sym **name, const s_sym **sym,
                         s_tag *symbol_value,
                         u8 operator_precedence,
                         const s_sym **operator_associativity,
                         s_tag *dest)
{
  return env_defoperator(&g_kc3_env, name, sym, symbol_value,
                         operator_precedence,
                         operator_associativity, dest);
}

s_tag * kc3_defstruct (s_list **spec, s_tag *dest)
{
  s_tag tmp = {0};
  assert(spec);
  if (! spec)
    return NULL;
  tmp.type = TAG_SYM;
  tmp.data.sym = env_defstruct(&g_kc3_env, *spec);
  *dest = tmp;
  return dest;
}

void ** kc3_dlopen (const s_str *path, void **dest)
{
  assert(path);
  assert(dest);
#if DEBUG_KC3_DLOPEN
  err_write_1("kc3_dlopen: ");
  err_inspect_str(path);
  err_write_1("\n");
#endif
  *dest = dlopen(path->ptr.pchar, RTLD_LAZY | RTLD_GLOBAL);
  if (! *dest) {
    err_write_1("kc3_dlopen: ");
    err_inspect_str(path);
    err_write_1(": dlopen: ");
    err_puts(dlerror());
    assert(! "kc3_dlopen: dlopen failed");
  }
  return dest;
}

s_facts ** kc3_env_db (s_facts **dest)
{
  *dest = &g_kc3_env.facts;
  return dest;
}

sw kc3_errno (void)
{
  return errno;
}

void kc3_exit (s_tag *code)
{
  u8 code_u8;
  const s_sym *type;
  type = &g_sym_U8;
  if (! u8_init_cast(&code_u8, &type, code)) {
    err_puts("kc3_exit: u8_init_cast");
    assert(! "kc3_exit: u8_init_cast");
    return;
  }
  exit((int) code_u8);
}

s_tag * kc3_fact_object (s_fact *fact, s_tag *dest)
{
  if (! fact->object)
    return tag_init_void(dest);
  return tag_init_copy(dest, fact->object);
}

s_tag * kc3_fact_predicate (s_fact *fact, s_tag *dest)
{
  if (! fact->predicate)
    return tag_init_void(dest);
  return tag_init_copy(dest, fact->predicate);
}

s_tag * kc3_fact_subject (s_fact *fact, s_tag *dest)
{
  if (! fact->subject)
    return tag_init_void(dest);
  return tag_init_copy(dest, fact->subject);
}

s_tag * kc3_fact_from_ptr (s_tag *tag, u_ptr_w *ptr)
{
  return tag_init_struct_with_data(tag, &g_sym_Fact, ptr->p, false);
}

bool * kc3_facts_add_tags (s_facts *facts, s_tag *subject,
                           s_tag *predicate,
                           s_tag *object,
                           bool *dest)
{
  const s_fact *fact;
  if (! (fact = facts_add_tags(facts, subject, predicate, object)))
    return NULL;
  *dest = true;
  return dest;
}

s_tag * kc3_facts_collect_with (s_facts *facts, s_list **spec,
                                p_callable *callback, s_tag *dest)
{
  return env_facts_collect_with(&g_kc3_env, facts, spec, *callback,
                                dest);
}

s_tag * kc3_facts_collect_with_tags (s_facts *facts,
                                     s_tag *subject,
                                     s_tag *predicate,
                                     s_tag *object,
                                     p_callable *callback,
                                     s_tag *dest)
{
  return env_facts_collect_with_tags(&g_kc3_env, facts, subject,
                                     predicate, object, *callback,
                                     dest);
}

s_tag * kc3_facts_first_with (s_facts *facts, s_list **spec,
                              p_callable *callback, s_tag *dest)
{
  return env_facts_first_with(&g_kc3_env, facts, spec, *callback, dest);
}

s_tag * kc3_facts_first_with_tags (s_facts *facts, s_tag *subject,
                                   s_tag *predicate, s_tag *object,
                                   p_callable *callback, s_tag *dest)
{
  return env_facts_first_with_tags(&g_kc3_env, facts, subject,
                                   predicate, object, *callback, dest);
}

uw * kc3_facts_next_id (uw *dest)
{
  assert(dest);
  *dest = g_kc3_env.facts.next_id;
  return dest;
}

bool * kc3_facts_remove_tags (s_facts *facts, s_tag *subject,
                              s_tag *predicate,
                              s_tag *object,
                              bool *dest)
{
  bool b;
  if (! facts_remove_fact_tags(facts, subject, predicate, object, &b))
    return NULL;
  *dest = true;
  return dest;
}

bool * kc3_facts_replace_tags (s_facts *facts,
                               s_tag *subject,
                               s_tag *predicate,
                               s_tag *object,
                               bool *dest)
{
  const s_fact *fact;
  if (! (fact = facts_replace_tags(facts, subject, predicate, object)))
    return NULL;
  *dest = true;
  return dest;
}

s_tag * kc3_facts_with (s_facts *facts, s_list **spec,
                        p_callable *callback, s_tag *dest)
{
  return env_facts_with(&g_kc3_env, facts, spec, *callback, dest);
}

s_tag * kc3_facts_with_macro (s_tag *facts_tag, s_tag *spec_tag,
                              s_tag *block_tag, s_tag *dest)
{
  return env_facts_with_macro(&g_kc3_env, facts_tag, spec_tag, block_tag, dest);
}

s_tag * kc3_facts_with_tags (s_facts *facts, s_tag *subject,
                             s_tag *predicate, s_tag *object,
                             p_callable *callback, s_tag *dest)
{
  return env_facts_with_tags(&g_kc3_env, facts, subject, predicate,
                             object, *callback, dest);
}

s_tag * kc3_facts_with_tuple (s_facts *facts, s_tuple *tuple,
                              p_callable *callback, s_tag *dest)
{
  assert(facts);
  assert(tuple);
  assert(callback);
  assert(dest);
  if (tuple->count < 3) {
    err_puts("kc3_facts_with_tuple: tuple count < 3");
    assert(! "kc3_facts_with_tuple: tuple count < 3");
    return NULL;
  }
  return env_facts_with_tags(&g_kc3_env, facts, tuple->tag,
                             tuple->tag + 1, tuple->tag + 2,
                             *callback, dest);
}

s_tag * kc3_quote_cfn (const s_sym **sym, s_tag *dest)
{
  assert(sym);
  return tag_init_sym(dest, *sym);
}

s_str * kc3_getenv (const s_str *name, s_str *dest)
{
  char *p;
  assert(name);
  assert(dest);
  p = getenv(name->ptr.pchar);
  if (! p)
    return NULL;
  return str_init_1(dest, NULL, p);
}

sw kc3_getpid (void)
{
  return getpid();
}

sw kc3_getppid (void)
{
  return getppid();
}

s_tag * kc3_identity (s_tag *tag, s_tag *dest)
{
  return tag_init_copy(dest, tag);
}

/* Special operator. */
s_tag * kc3_if_then_else (s_tag *cond, s_tag *then,
                          s_tag *else_, s_tag *dest)
{
  bool  cond_bool = false;
  s_tag cond_eval = {0};
  const s_sym *type;
  if (env_eval_tag(&g_kc3_env, cond, &cond_eval)) {
    if (cond_eval.type == TAG_BOOL)
      cond_bool = cond_eval.data.bool;
    else {
      type = &g_sym_Bool;
      if (! bool_init_cast(&cond_bool, &type, &cond_eval)) {
        tag_clean(&cond_eval);
        return NULL;
      }
    }
    tag_clean(&cond_eval);
  }
  if (cond_bool) {
    if (! env_eval_tag(&g_kc3_env, then, dest))
      return NULL;
    return dest;
  }
  if (! env_eval_tag(&g_kc3_env, else_, dest))
    return NULL;
  return dest;
}

s_env * kc3_init (s_env *env, int *argc, char ***argv)
{
  if (! env)
    env = &g_kc3_env;
  return env_init(env, argc, argv);
}

s_tag * kc3_integer_reduce (s_tag *tag, s_tag *dest)
{
  s_tag tmp;
  if (! tag_init_copy(&tmp, tag))
    return NULL;
  tag_integer_reduce(&tmp);
  *dest = tmp;
  return dest;
}

s_tag * kc3_let (s_tag *vars, s_tag *tag, s_tag *dest)
{
  return env_let(&g_kc3_env, vars, tag, dest);
}

void kc3_license (void)
{
  buf_write_1(&g_kc3_env.out, g_kc3_license);
  buf_flush(&g_kc3_env.out);
}

s_array * kc3_list_to_array (s_list **list,
                             const s_sym * const *array_type,
                             s_array *dest)
{
  return list_to_array(*list, *array_type, dest);
}

bool kc3_load (const s_str *path)
{
  return env_load(&g_kc3_env, path);
}

bool kc3_maybe_reload (const s_str *path)
{
  return env_maybe_reload(&g_kc3_env, path);
}

const s_sym ** kc3_module (const s_sym **dest)
{
  return env_module(&g_kc3_env, dest);
}

bool * kc3_must_clean (const s_sym * const *sym, bool *dest)
{
  assert(sym);
  return sym_must_clean(*sym, dest);
}

uw * kc3_offsetof (const s_sym * const *module,
                   const s_sym * const *key, uw *dest)
{
  uw i = 0;
  s_struct_type *st;
  if (! struct_type_find(*module, &st) ||
      ! st ||
      ! struct_type_find_key_index(st, *key, &i))
    return NULL;
  *dest = st->offset[i];
  return dest;
}

s_tag * kc3_operator_find_by_sym (const s_sym * const *sym, s_tag *dest)
{
  return env_operator_find_by_sym(&g_kc3_env, *sym, dest);
}

bool * kc3_or (s_tag *a, s_tag *b, bool *dest)
{
  return env_or(&g_kc3_env, a, b, dest);
}

sw kc3_puts (const s_tag *tag)
{
  sw r;
  sw result = 0;
  if (tag->type == TAG_STR) {
    if ((r = io_write_str(&tag->data.str)) < 0)
      return r;
  }
  else {
    if ((r = io_inspect_tag(tag)) < 0)
      return r;
  }
  result += r;
  if ((r = io_write_1("\n")) < 0)
    return r;
  result += r;
  if ((r = io_flush()) < 0)
    return r;
  return result;
}

bool kc3_require (const s_sym * const *module)
{
  return env_module_ensure_loaded(&g_kc3_env, *module);
}

s_list ** kc3_search_modules (s_list **dest)
{
  return env_search_modules(&g_kc3_env, dest);
}

s_list ** kc3_stacktrace (s_list **dest)
{
  return env_stacktrace(&g_kc3_env, dest);
}

s_str * kc3_str (const s_tag *tag, s_str *dest)
{
  const s_sym *sym;
  switch (tag->type) {
  case TAG_LIST:
    return str_init_concatenate_list(dest,
                                     (const s_list **)
                                     &tag->data.list);
  case TAG_STR:
    return str_init_copy(dest, &tag->data.str);
  default:
    sym = &g_sym_Str;
    return str_init_cast(dest, &sym, tag);
  }
}

s_str * kc3_strerror (sw err_no, s_str *dest)
{
  const char *s;
  s = strerror(err_no);
  return str_init_1_alloc(dest, s);
}

s_tag * kc3_struct_put (s_tag *s, const s_sym * const *key,
                        s_tag *value, s_tag *dest)
{
  s_struct tmp;
  assert(s);
  assert(key);
  assert(value);
  assert(dest);
  if (s->type != TAG_STRUCT) {
    err_puts("kc3_struct_put: not a struct");
    assert(! "kc3_struct_put: not a struct");
    return NULL;
  }
  if (! struct_put(&s->data.struct_, *key, value, &tmp))
    return NULL;
  dest->type = TAG_STRUCT;
  dest->data.struct_ = tmp;
  return dest;
}

s_str * kc3_system (const s_list * const *list, s_str *dest)
{
#ifdef WIN32
  (void) list;
  (void) dest;
  return NULL;
#else
  char **a = NULL;
  char **argv = NULL;
  sw e;
  const s_list *l;
  sw len;
  pid_t pid;
  s32 pipe_fd[2];
  s_str *r = NULL;
  s32 status;
  const s_str *str;
  s_str tmp;
  assert(list);
  assert(dest);
  if ((len = list_length(*list)) < 1) {
    err_puts("kc3_system: empty argument list");
    assert(! "kc3_system: empty argument list");
    return NULL;
  }
  argv = alloc((len + 1) * sizeof(char *));
  a = argv;
  l = *list;
  while (l) {
    if (l->tag.type != TAG_STR) {
      err_write_1("kc3_system: argument that is not a Str: ");
      err_inspect_tag(&l->tag);
      err_write_1("\n");
      assert(! "kc3_system: argument that is not a Str");
      goto clean;
    }
    str = &l->tag.data.str;
    if (! (*a = alloc(str->size + 1)))
      goto clean;
    memcpy(*a, str->ptr.pchar, str->size);
    a++;
    l = list_next(l);
  }
  if (pipe(pipe_fd)) {
    e = errno;
    err_write_1("kc3_system: pipe: ");
    err_puts(strerror(e));
    assert(! "kc3_system: pipe");
    goto clean;
  }
  if ((pid = fork()) < 0) {
    e = errno;
    err_write_1("kc3_system: fork: ");
    err_puts(strerror(e));
    assert(! "kc3_system: fork");
    goto clean;
  }
  if (! pid)
    kc3_system_pipe_exec(pipe_fd[1], argv, list);
  close(pipe_fd[1]);
  if (! fd_read_until_eof(pipe_fd[0], &tmp)) {
    err_puts("kc3_system: fd_read_until_eof");
    assert(! "kc3_system: fd_read_until_eof");
    goto clean;
  }
  close(pipe_fd[0]);
  if (waitpid(pid, &status, 0) < 0) {
    e = errno;
    err_write_1("kc3_system: waitpid: ");
    err_puts(strerror(e));
    assert(! "kc3_system: waitpid");
    goto clean;
  }
  *dest = tmp;
  r = dest;
 clean:
  while (a > argv) {
    a--;
    free(*a);
  }
  return r;
#endif
}

void kc3_system_pipe_exec (s32 pipe_w, char **argv,
                           const s_list * const *list)
{
  sw e;
  if (dup2(pipe_w, 1) < 0) {
    e = errno;
    err_write_1("kc3_system: dup2(pipe_w, 1): ");
    err_puts(strerror(e));
    assert(! "kc3_system: dup2(pipe_w, 1)");
    _exit(1);
  }
  close(pipe_w);
  if (dup2(1, 2) < 0) {
    e = errno;
    err_write_1("kc3_system: dup2(1, 2): ");
    err_puts(strerror(e));
    assert(! "kc3_system: dup2(1, 2)");
    _exit(1);
  }
  execvp(argv[0], argv);
  e = errno;
  err_write_1("kc3_system: execvp ");
  err_inspect_list(*list);
  err_write_1(": ");
  err_puts(strerror(e));
  assert(! "kc3_system: execvp");
  _exit(1);
}

s_tag * kc3_while (s_tag *cond, s_tag *body, s_tag *dest)
{
  return env_while(&g_kc3_env, cond, body, dest);
}
