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
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef __OpenBSD__
# include <sys/sysctl.h>
#endif

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
#include "deserialize.h"
#include "env.h"
#include "env_fork.h"
#include "eval.h"
#include "fact.h"
#include "facts.h"
#include "facts_cursor.h"
#include "fd.h"
#include "kc3_main.h"
#include "list.h"
#include "map.h"
#include "s32.h"
#include "serialize.h"
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

s_tag * kc3_and (s_tag *a, s_tag *b, s_tag *dest)
{
  return env_and(env_global(), a, b, dest);
}

s_list ** kc3_args (s_list **dest)
{
  return env_args(env_global(), dest);
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
    env = env_global();
  env_clean(env);
  sym_delete_all();
}

s_tag * kc3_def (const s_call *call, s_tag *dest)
{
  return env_kc3_def(env_global(), call, dest);
}

s_tag * kc3_defmodule (const s_sym **name, const s_block *block, s_tag *dest)
{
  return env_defmodule(env_global(), name, block, dest);
}

s_tag * kc3_defoperator (s_tag *op_tag, s_tag *dest)
{
  s_env *env;
  s_tag tag = {0};
  if (! op_tag || op_tag->type != TAG_STRUCT ||
      op_tag->data.struct_.type->module != &g_sym_KC3_Op) {
    err_puts("kc3_defoperator: not a KC3.Op struct");
    assert(! "kc3_defoperator: not a KC3.Op struct");
    return NULL;
  }
  env = env_global();
  if (! op_tag->data.struct_.data) {
    tag.type = TAG_STRUCT;
    if (! env_eval_struct(env, &op_tag->data.struct_,
                          &tag.data.struct_)) {
      err_puts("kc3_defoperator: env_eval_struct");
      assert(! "kc3_defoperator: env_eval_struct");
      return NULL;
    }
    if (! env_defoperator(env, tag.data.struct_.data, dest)) {
      err_puts("kc3_defoperator: env_defoperator 1");
      assert(! "kc3_defoperator: env_defoperator 1");
      tag_clean(&tag);
      return NULL;
    }
    return dest;
  }
  if (! env_defoperator(env, op_tag->data.struct_.data, dest)) {
    err_puts("kc3_defoperator: env_defoperator 2");
    assert(! "kc3_defoperator: env_defoperator 2");
    tag_clean(&tag);
    return NULL;
  }
  return dest;
}

s_tag * kc3_defstruct (s_list **spec, s_tag *dest)
{
  s_tag tmp = {0};
  assert(spec);
  if (! spec)
    return NULL;
  tmp.type = TAG_SYM;
  tmp.data.sym = env_defstruct(env_global(), *spec);
  *dest = tmp;
  return dest;
}

void kc3_deserialize_delete (s_deserialize **ds)
{
  deserialize_delete(*ds);
}

bool kc3_deserialize_from_str (s_deserialize **ds, s_str *str)
{
  return deserialize_from_str(*ds, str);
}

s_deserialize ** kc3_deserialize_new (s_deserialize **ds)
{
  s_deserialize *tmp;
  tmp = deserialize_new();
  if (! tmp)
    return NULL;
  *ds = tmp;
  return ds;
}

s_tag * kc3_deserialize_tag (s_deserialize **ds, s_tag *dest)
{
  return deserialize_tag(*ds, dest);
}

void ** kc3_dlopen (const s_str *path, void **dest)
{
  assert(path);
  assert(dest);
  if (env_global()->trace) {
    err_write_1("kc3_dlopen: ");
    err_inspect_str(path);
    err_write_1("\n");
  }
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
  *dest = env_global()->facts;
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

void kc3__exit (s_tag *code)
{
  u8 code_u8;
  const s_sym *type;
  type = &g_sym_U8;
  if (! u8_init_cast(&code_u8, &type, code)) {
    err_puts("kc3__exit: u8_init_cast");
    assert(! "kc3__exit: u8_init_cast");
    return;
  }
  _exit((int) code_u8);
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
  return env_facts_collect_with(env_global(), facts, spec, *callback,
                                dest);
}

s_tag * kc3_facts_collect_with_tags (s_facts *facts,
                                     s_tag *subject,
                                     s_tag *predicate,
                                     s_tag *object,
                                     p_callable *callback,
                                     s_tag *dest)
{
  return env_facts_collect_with_tags(env_global(), facts, subject,
                                     predicate, object, *callback,
                                     dest);
}

s_tag * kc3_facts_first_with (s_facts *facts, s_list **spec,
                              p_callable *callback, s_tag *dest)
{
  return env_facts_first_with(env_global(), facts, spec, *callback, dest);
}

s_tag * kc3_facts_first_with_tags (s_facts *facts, s_tag *subject,
                                   s_tag *predicate, s_tag *object,
                                   p_callable *callback, s_tag *dest)
{
  return env_facts_first_with_tags(env_global(), facts, subject,
                                   predicate, object, *callback, dest);
}

uw * kc3_facts_next_id (uw *dest)
{
  assert(dest);
  *dest = env_global()->facts->next_id;
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
  return env_facts_with(env_global(), facts, spec, *callback, dest);
}

s_tag * kc3_facts_with_macro (s_tag *facts_tag, s_tag *spec_tag,
                              s_tag *block_tag, s_tag *dest)
{
  return env_facts_with_macro(env_global(), facts_tag, spec_tag, block_tag, dest);
}

s_tag * kc3_facts_with_tags (s_facts *facts, s_tag *subject,
                             s_tag *predicate, s_tag *object,
                             p_callable *callback, s_tag *dest)
{
  return env_facts_with_tags(env_global(), facts, subject, predicate,
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
  return env_facts_with_tags(env_global(), facts, tuple->tag,
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
#if defined(WIN32) || defined(WIN64)
  err_puts("kc3_getppid: not implemented");
  assert(! "kc3_getppid: not implemented");
  abort();
#else
  return getppid();
#endif
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
  if (env_eval_tag(env_global(), cond, &cond_eval)) {
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
      if (! env_eval_tag(env_global(), then, dest))
        return NULL;
      return dest;
    }
  }
  if (! env_eval_tag(env_global(), else_, dest))
    return NULL;
  return dest;
}

s_env * kc3_init (s_env *env, int *argc, char ***argv)
{
  if (! env)
    env = env_global();
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

bool kc3_killpg (sw process_group, const s_sym * const *signal)
{
#if (defined(WIN32) || defined(WIN64))
  (void) process_group;
  (void) signal;
  err_puts("kc3_killpg: not implemented");
  assert(! "kc3_killpg: not implemented");
  abort();
#else
  sw e;
  s32 sig = -1;
  if      (*signal == sym_1("SIGHUP"))
    sig = SIGHUP;
  else if (*signal == sym_1("SIGINT"))
    sig = SIGINT;
  else if (*signal == sym_1("SIGQUIT"))
    sig = SIGQUIT;
  else if (*signal == sym_1("SIGILL"))
    sig = SIGILL;
  else if (*signal == sym_1("SIGTRAP"))
    sig = SIGTRAP;
  else if (*signal == sym_1("SIGABRT"))
    sig = SIGABRT;
#ifdef __OpenBSD__
  else if (*signal == sym_1("SIGEMT"))
    sig = SIGEMT;
#endif
  else if (*signal == sym_1("SIGFPE"))
    sig = SIGFPE;
  else if (*signal == sym_1("SIGKILL"))
    sig = SIGKILL;
  else if (*signal == sym_1("SIGBUS"))
    sig = SIGBUS;
  else if (*signal == sym_1("SIGSEGV"))
    sig = SIGSEGV;
  else if (*signal == sym_1("SIGSYS"))
    sig = SIGSYS;
  else if (*signal == sym_1("SIGPIPE"))
    sig = SIGPIPE;
  else if (*signal == sym_1("SIGALRM"))
    sig = SIGALRM;
  else if (*signal == sym_1("SIGTERM"))
    sig = SIGTERM;
  else if (*signal == sym_1("SIGURG"))
    sig = SIGURG;
  else if (*signal == sym_1("SIGSTOP"))
    sig = SIGSTOP;
  else if (*signal == sym_1("SIGTSTP"))
    sig = SIGTSTP;
  else if (*signal == sym_1("SIGCONT"))
    sig = SIGCONT;
  else if (*signal == sym_1("SIGCHLD"))
    sig = SIGCHLD;
  else if (*signal == sym_1("SIGTTIN"))
    sig = SIGTTIN;
  else if (*signal == sym_1("SIGTTOU"))
    sig = SIGTTOU;
  else if (*signal == sym_1("SIGIO"))
    sig = SIGIO;
  else if (*signal == sym_1("SIGXCPU"))
    sig = SIGXCPU;
  else if (*signal == sym_1("SIGXFSZ"))
    sig = SIGXFSZ;
  else if (*signal == sym_1("SIGVTALRM"))
    sig = SIGVTALRM;
  else if (*signal == sym_1("SIGPROF"))
    sig = SIGPROF;
  else if (*signal == sym_1("SIGWINCH"))
    sig = SIGWINCH;
#ifdef __OpenBSD__
  else if (*signal == sym_1("SIGINFO"))
    sig = SIGINFO;
#endif
  else if (*signal == sym_1("SIGUSR1"))
    sig = SIGUSR1;
  else if (*signal == sym_1("SIGUSR2"))
    sig = SIGUSR2;
#ifdef __OpenBSD__
  else if (*signal == sym_1("SIGTHR"))
    sig = SIGTHR;
#endif
  else {
    err_write_1("kc3_killpg: unknown signal: ");
    err_inspect_sym(signal);
    err_write_1("\n");
    assert(! "kc3_killpg: unknown signal");
    return false;
  }
  if (killpg(process_group, sig)) {
    e = errno;
    err_write_1("kc3_killpg: killpg: ");
    err_write_1(strerror(e));
    err_write_1("\n");
    assert(! "kc3_killpg: killpg");
    return false;
  }
  return true;
#endif
}

s_tag * kc3_let (s_tag *vars, s_tag *tag, s_tag *dest)
{
  return env_let(env_global(), vars, tag, dest);
}

void kc3_license (void)
{
  io_write_1(g_kc3_license);
}

s_tag * kc3_list_length (const s_list **list, s_tag *dest)
{
  s_tag tmp = {0};
  tag_init_sw(&tmp, list_length(*list));
  tag_integer_reduce(&tmp);
  *dest = tmp;
  return dest;
}

s_array * kc3_list_to_array (s_list **list,
                             const s_sym * const *array_type,
                             s_array *dest)
{
  return list_to_array(*list, *array_type, dest);
}

bool kc3_load (const s_str *path)
{
  return env_load(env_global(), path);
}

bool kc3_maybe_reload (const s_str *path)
{
  return env_maybe_reload(env_global(), path);
}

const s_sym ** kc3_module (const s_sym **dest)
{
  return env_module(env_global(), dest);
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

s_tag * kc3_or (s_tag *a, s_tag *b, s_tag *dest)
{
  return env_or(env_global(), a, b, dest);
}

s_tag * kc3_parse_tag (s_tag *tag, const s_str *src)
{
  s_buf buf;
  sw r;
  buf_init_str_const(&buf, src);
  r = buf_parse_tag(&buf, tag);
  if (r < 0 || (uw) r != src->size) {
    err_write_1("kc3_parse_tag: invalid tag: ");
    err_inspect_sw_decimal(&r);
    err_write_1(" != ");
    err_inspect_u32_decimal(&src->size);
    err_write_1("\n");
    assert(! "kc3_parse_tag: invalid tag");
    return NULL;
  }
  return tag;
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
  return env_module_ensure_loaded(env_global(), *module);
}

s_list ** kc3_search_modules (s_list **dest)
{
  return env_search_modules(env_global(), dest);
}

void kc3_serialize_delete (s_serialize **serialize)
{
  serialize_delete(*serialize);
}

s_serialize ** kc3_serialize_new (s_serialize **serialize)
{
  assert(serialize);
  *serialize = serialize_new();
  return serialize;
}

bool kc3_serialize_tag (s_serialize **serialize, const s_tag *tag)
{
  if (! serialize_tag(*serialize, tag))
    return false;
  return true;
}

s_str * kc3_serialize_to_str (s_serialize **serialize, s_str *dest)
{
  return serialize_to_str(*serialize, dest);
}

s_list ** kc3_stacktrace (s_list **dest)
{
  return env_stacktrace(env_global(), dest);
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


s_tag * kc3_sysctl (s_tag *dest, const s_list * const *list)
{
#ifdef __OpenBSD__
  s32 e;
  const s_list *l;
  s32 mib[64];
  s32 mib_len = 0;
  s_tag  tmp = {0};
  size_t tmp_data_size;
  l = *list;
  if (! l ||
      l->tag.type != TAG_SYM) {
    err_puts("kc3_sysctl: argument is not a Sym list");
    assert(! "kc3_sysctl: argument is not a Sym list");
  }
  if (l->tag.data.sym == sym_1("ddb")) {
    mib[mib_len] = CTL_DDB;
    mib_len++;
    l = list_next(l);
    
  }
  else if (l->tag.data.sym == sym_1("debug")) {
    mib[mib_len] = CTL_DEBUG;
    mib_len++;
    l = list_next(l);
    
  }
  else if (l->tag.data.sym == sym_1("fs")) {
    mib[mib_len] = CTL_FS;
    mib_len++;
    l = list_next(l);
    
  }
  else if (l->tag.data.sym == sym_1("hw")) {
    mib[mib_len] = CTL_HW;
    mib_len++;
    if ((l = list_next(l))) {
      if (l->tag.data.sym == sym_1("ncpu")) {
        mib[mib_len] = HW_NCPU;
        mib_len++;
        l = list_next(l);
        if (! l)
          tmp.type = TAG_S32;
      }
      else if (l->tag.data.sym == sym_1("ncpufound")) {
        mib[mib_len] = HW_NCPUFOUND;
        mib_len++;
        l = list_next(l);
        if (! l)
          tmp.type = TAG_S32;
      }
      else if (l->tag.data.sym == sym_1("ncpuonline")) {
        mib[mib_len] = HW_NCPUONLINE;
        mib_len++;
        l = list_next(l);
        if (! l)
          tmp.type = TAG_S32;
      }
      else if (l->tag.data.sym == sym_1("pagesize")) {
        mib[mib_len] = HW_PAGESIZE;
        mib_len++;
        l = list_next(l);
        if (! l)
          tmp.type = TAG_S32;
      }
      else if (l->tag.data.sym == sym_1("physmem64")) {
        mib[mib_len] = HW_PHYSMEM64;
        mib_len++;
        l = list_next(l);
        if (! l)
          tmp.type = TAG_S64;
      }
    }
  }
  else if (l->tag.data.sym == sym_1("kern")) {
    mib[mib_len] = CTL_KERN;
    mib_len++;
    l = list_next(l);
  }
  else if (l->tag.data.sym == sym_1("machdep")) {
    mib[mib_len] = CTL_MACHDEP;
    mib_len++;
    l = list_next(l);
  }
  else if (l->tag.data.sym == sym_1("net")) {
    mib[mib_len] = CTL_NET;
    mib_len++;
    l = list_next(l);
  }
  else if (l->tag.data.sym == sym_1("vfs")) {
    mib[mib_len] = CTL_VFS;
    mib_len++;
    l = list_next(l);
  }
  else if (l->tag.data.sym == sym_1("vm")) {
    mib[mib_len] = CTL_VM;
    mib_len++;
    l = list_next(l);
  }
  if (! tmp.type) {
    err_write_1("kc3_sysctl: invalid argument: ");
    if (l)
      err_inspect_sym(&l->tag.data.sym);
    else
      err_inspect_list(*list);
    err_write_1("\n");
    assert("kc3_sysctl: invalid argument");
    return NULL;
  }
  tmp_data_size = sizeof(tmp.data);
  if (sysctl(mib, mib_len, &tmp.data, &tmp_data_size, NULL, 0) < 0) {
    e = errno;
    err_write_1("kc3_sysctl: sysctl: ");
    err_write_1(strerror(e));
    err_write_1("\n");
    assert("kc3_sysctl: sysctl");
    return NULL;
  }
  assert(tmp_data_size <= sizeof(tmp.data));
  *dest = tmp;
  return dest;
#else
  (void) dest;
  (void) list;
  return NULL;
#endif
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

s_tag * kc3_thread_delete (u_ptr_w *thread, s_tag *dest)
{
  pthread_t t;
  s_tag *tag;
  t = (pthread_t) thread->p;
  if (pthread_join(t, (void **) &tag)) {
    err_puts("kc3_thread_delete: pthread_join");
    assert(! "kc3_thread_delete: pthread_join");
    return NULL;
  }
  if (tag->type != TAG_TUPLE ||
      tag->data.tuple.count != 3) {
    err_puts("kc3_thread_delete: invalid value");
    assert(! "kc3_thread_delete: invalid value");
    tag_delete(tag);
    return NULL;
  }
  if (! tag_init_copy(dest, tag->data.tuple.tag)) {
    tag_delete(tag);
    return NULL;
  }
  tag_delete(tag);
  return dest;
}

u_ptr_w * kc3_thread_new (u_ptr_w *dest, p_callable *start)
{
  s_tag *tag;
  if (! (tag = tag_new_tuple(3)))
    return NULL;
  if (! tag_init_callable_copy(tag->data.tuple.tag + 1, start)) {
    tag_delete(tag);
    return NULL;
  }
  tag->data.tuple.tag[2].type = TAG_PTR;
  if (! (tag->data.tuple.tag[2].data.ptr.p = env_fork_new(env_global()))) {
    tag_delete(tag);
    return NULL;
  }
  if (pthread_create((pthread_t *) &dest->p, NULL, kc3_thread_start,
                     tag)) {
    err_puts("kc3_thread_new: pthread_create");
    assert(! "kc3_thread_new: pthread_create");
    tag_delete(tag);
    return NULL;
  }
  return dest;
}

void * kc3_thread_start (void *arg)
{
  s_env *env;
  s_tag *tag;
  s_callable *start;
  tag = arg;
  if (tag->type != TAG_TUPLE) {
    fprintf(stderr, "kc3_thread_start: invalid argument: not a tuple\n");
    assert(! "kc3_thread_start: invalid argument: not a tuple");
    return NULL;
  }
  if (tag->data.tuple.count != 3) {
    fprintf(stderr, "kc3_thread_start: invalid argument:"
	    " tuple arity mismatch\n");
    assert(!("kc3_thread_start: invalid argument:"
             " tuple arity mismatch"));
    return NULL;
  }
  if (tag->data.tuple.tag[1].type != TAG_CALLABLE) {
    fprintf(stderr, "kc3_thread_start: invalid argument:"
	    " not a Callable (Fn or Cfn)\n");
    assert(!("kc3_thread_start: invalid argument:"
             " not a Callable (Fn or Cfn)"));
    return NULL;
  }
  if (tag->data.tuple.tag[2].type != TAG_PTR) {
    fprintf(stderr, "kc3_thread_start: invalid argument: not a Ptr\n");
    assert(! "kc3_thread_start: invalid argument: not a Ptr");
    return NULL;
  }
  start = tag->data.tuple.tag[1].data.callable;
  env = tag->data.tuple.tag[2].data.ptr.p;
  env_global_set(env);
  if (! eval_callable_call(start, NULL, tag->data.tuple.tag))
    return NULL;
  env_fork_delete(env);
  return tag;
}

s_tag * kc3_while (s_tag *cond, s_tag *body, s_tag *dest)
{
  return env_while(env_global(), cond, body, dest);
}
