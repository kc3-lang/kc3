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
#include <errno.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#if defined(__OpenBSD__)
# include <sys/sysctl.h>
#endif

#if defined(__NetBSD__) || defined(__APPLE__)
# include <sys/param.h>
# define HOST_NAME_MAX MAXHOSTNAMELEN
#endif
#if defined(WIN32) || defined(WIN64)
# define HOST_NAME_MAX 255
#endif

#ifndef WIN32
# include <sys/wait.h>
#endif

#include "alist.h"
#include "alloc.h"
#include "array.h"
#include "assert.h"
#include "block.h"
#include "bool.h"
#include "buf.h"
#include "buf_fd.h"
#include "buf_parse.h"
#include "call.h"
#include "counter.h"
#include "env.h"
#include "env_eval.h"
#include "env_eval_equal.h"
#include "env_fork.h"
#include "eval.h"
#include "fact.h"
#include "facts.h"
#include "facts_cursor.h"
#include "fd.h"
#include "file.h"
#include "kc3_main.h"
#include "list.h"
#include "map.h"
#include "marshall.h"
#include "marshall_read.h"
#include "mutex.h"
#include "pfacts.h"
#include "pointer.h"
#include "pstruct.h"
#include "pstruct_type.h"
#include "rwlock.h"
#include "s32.h"
#include "securelevel.h"
#include "str.h"
#include "struct.h"
#include "struct_type.h"
#include "sym.h"
#include "tag.h"
#include "time.h"
#include "timespec.h"
#include "tuple.h"
#include "u8.h"
#include "uw.h"

const s_str g_kc3_base_binary = STR("01");
const s_str g_kc3_base_octal = STR("01234567");
const s_str g_kc3_base_decimal = STR("0123456789");
const s_str g_kc3_base_hexadecimal = STR("0123456789abcdef");
const s_str g_kc3_bases_hexadecimal[2] = {STR("0123456789abcdef"),
                                          STR("0123456789ABCDEF")};
const s_str g_kc3_base32 = STR("ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                               "234567");
const s_str g_kc3_base64 = STR("ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                               "abcdefghijklmnopqrstuvwxyz"
                               "0123456789+/");
const s_str g_kc3_base64url = STR("ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                                  "abcdefghijklmnopqrstuvwxyz"
                                  "0123456789-_");
sw          g_kc3_exit_code = 1;

#ifndef WIN32
static void kc3_system_pipe_exec (s32 pipe_w, char **argv,
                                  p_list *list);
#endif

#if defined(WIN32) || defined(WIN64)

static void kc3_wsa_cleanup(void){ WSACleanup(); }

static void kc3_wsa_init_once(void){
  static int done = 0;
  if (! done) {
    WSADATA w;
    done = 1;
    if (WSAStartup(MAKEWORD(2,2), &w) != 0) {
      err_puts("kc3_wsa_init_once: WSAStartup");
      assert(! "kc3_wsa_init_once: WSAStartup");
    } else {
      atexit(kc3_wsa_cleanup);
    }
  }
}

#endif

#if defined(WIN32)

int fork (void)
{
  return -1;
}

#endif

s_pointer * kc3_address_of (s_tag *tag, s_pointer *dest)
{
  s_env *env;
  env = env_global();
  assert(tag);
  assert(dest);
  assert(env);
  return env_address_of(env, tag, dest);
}

s_tag * kc3_access (s_tag *tag, s_list **key,
                    s_tag *dest)
{
  assert(tag);
  assert(key);
  assert(dest);
  switch (tag->type) {
  case TAG_ARRAY:
    return array_access(&tag->data.array, *key, dest);
  case TAG_PLIST:
    if (list_is_alist(tag->data.plist)) {
      if (! alist_access(tag->data.plist, *key, dest))
        return list_access(tag->data.plist, *key, dest);
      return dest;
    }
    else
      return list_access(tag->data.plist, *key, dest);
  case TAG_MAP:
    return map_access(&tag->data.map, *key, dest);
  case TAG_POINTER:
    return pointer_access(&tag->data.pointer, *key, dest);
  case TAG_PSTRUCT:
    return struct_access(tag->data.pstruct, *key, dest);
  case TAG_STR:
    return str_access(&tag->data.str, *key, dest);
  case TAG_TIME:
    return time_access(&tag->data.time, *key, dest);
  case TAG_VOID:
    return tag_init_void(dest);
  default:
    break;
  }
  if (false) {
    err_write_1("kc3_access: cannot access tag type ");
    err_write_1(tag_type_to_string(tag->type));
    err_write_1(" for key ");
    err_inspect_list(*key);
    err_write_1("\n");
    return NULL;
  }
  return tag_init_void(dest);
}

s_tag * kc3_and (s_tag *a, s_tag *b, s_tag *dest)
{
  return env_and(env_global(), a, b, dest);
}

s_list ** kc3_args (s_list **dest)
{
  return env_args(env_global(), dest);
}

s_tag * kc3_array_dimension (s_array *a, s_tag *index, s_tag *dest)
{
  uw index_uw;
  const s_sym *sym_Uw = &g_sym_Uw;
  s_tag tmp = {0};
  assert(a);
  assert(index);
  assert(dest);
  if (! uw_init_cast(&index_uw, &sym_Uw, index))
    return NULL;
  if (index_uw > a->dimension_count) {
    err_puts("kc3_array_dimension: index out of bounds");
    assert(! "kc3_array_dimension: index out of bounds");
    return NULL;
  }
  tag_init_uw(&tmp, a->dimensions[index_uw].count);
  *dest = tmp;
  return dest;
}

s_tag * kc3_block (s_tag *name, s_tag *do_block, s_tag *dest)
{
  s_tag * volatile dest_v = dest;
  s_env * volatile env;
  const s_sym *name_sym = NULL;
  s_tag tmp = {0};
  struct {
    s_block block;
    s_unwind_protect up;
  } jump;
  env = env_global();
  switch (name->type) {
  case TAG_PSYM:
    name_sym = name->data.psym;
  case TAG_VOID:
    break;
  default:
    err_puts("kc3_block: first argument must be a Sym or void");
    err_stacktrace();
    return NULL;
  }
  if (! block_init(&jump.block, name_sym))
    return NULL;
  env_unwind_protect_push(env, &jump.up);
  if (setjmp(jump.up.buf)) {
    env_unwind_protect_pop(env, &jump.up);
    tag_clean(&tmp);
    block_clean(&jump.block);
    longjmp(*jump.up.jmp, 1);
  }
  if (setjmp(jump.block.buf)) {
    *dest_v = jump.block.tag;
    env_unwind_protect_pop(env, &jump.up);
    block_clean(&jump.block);
    return dest_v;
  }
  if (! env_eval_tag(env, do_block, &tmp)) {
    env_unwind_protect_pop(env, &jump.up);
    block_clean(&jump.block);
    return NULL;
  }
  env_unwind_protect_pop(env, &jump.up);
  block_clean(&jump.block);
  *dest_v = tmp;
  return dest_v;
}

void kc3_break (void)
{
  s_loop_context *lc;
  s_env *env;
  env = env_global();
  assert(env);
  if (! (lc = env->loop_context)) {
    err_puts("kc3_break: no loop context");
    assert(! "kc3_break: no loop context");
    return;
  }
  env_longjmp(env, &lc->break_buf);
  abort();
}

s_tag * kc3_buf_parse_tag (s_buf *buf, s_tag *dest)
{
  if (buf_parse_tag(buf, dest) <= 0)
    return NULL;
  return dest;
}

s_str * kc3_buf_read_line (s_buf *buf, s_str *dest)
{
  if (buf_read_until_1_into_str(buf, "\n", dest) < 0)
    return NULL;
  return dest;
}

s_str * kc3_buf_read_to_str (s_buf *buf, s_str *dest)
{
  if (buf_read_to_str(buf, dest) < 0)
    return NULL;
  return dest;
}

void kc3_clean (s_env *env)
{
  env_clean(env);
}

void kc3_continue (void)
{
  s_loop_context *lc;
  s_env *env;
  env = env_global();
  assert(env);
  if (! (lc = env->loop_context)) {
    err_puts("kc3_continue: no loop context");
    assert(! "kc3_continue: no loop context");
    return;
  }
  env_longjmp(env, &lc->continue_buf);
  abort();
}

s_tag * kc3_counter_decrease (s_counter **counter, s_tag *positive,
                              s_tag *dest)
{
  assert(counter);
  assert(dest);
  return counter_decrease(*counter, positive, dest);
}

void kc3_counter_delete (s_counter **counter)
{
  counter_delete(*counter);
}

s_tag * kc3_counter_get (s_counter **counter, s_tag *dest)
{
  assert(counter);
  assert(dest);
  return counter_get(*counter, dest);
}

s_tag * kc3_counter_increase (s_counter **counter, s_tag *positive,
                              s_tag *dest)
{
  assert(counter);
  assert(dest);
  return counter_increase(*counter, positive, dest);
}

s_counter ** kc3_counter_new (s_counter **dest, s_tag *value)
{
  s_counter *counter;
  assert(dest);
  assert(value);
  counter = counter_new(value);
  if (! counter)
    return NULL;
  *dest = counter;
  return dest;
}

s_tag * kc3_def (p_call *pcall, s_tag *dest)
{
  if (securelevel(0) > 1) {
    err_stacktrace();
    err_puts("kc3_def: cannot use def with securelevel > 1");
    abort();
  }
  return env_kc3_def(env_global(), *pcall, dest);
}

s_tag * kc3_defmodule (p_sym const *name, const s_do_block *do_block,
                       s_tag *dest)
{
  if (securelevel(0) > 1) {
    err_stacktrace();
    err_puts("kc3_defmodule: cannot use def with securelevel > 1");
    abort();
  }
  return env_defmodule(env_global(), name, do_block, dest);
}

/* FIXME: multiple env and env->ops. See env_defoperator. */
s_tag * kc3_defoperator (s_tag *tag_op, s_tag *dest)
{
  s_env *env;
  s_tag tmp = {0};
  if (securelevel(0) > 1) {
    err_stacktrace();
    err_puts("kc3_defoperator: cannot use defoperator with"
             " securelevel > 1");
    abort();
  }
  if (! tag_op || tag_op->type != TAG_PSTRUCT ||
      tag_op->data.pstruct->pstruct_type->module != &g_sym_KC3_Op) {
    err_puts("kc3_defoperator: not a %KC3.Op{}");
    assert(! "kc3_defoperator: not a %KC3.Op{}");
    return NULL;
  }
  env = env_global();
  tmp.type = TAG_PSTRUCT;
  if (! env_eval_struct(env, tag_op->data.pstruct,
                        &tmp.data.pstruct)) {
    err_puts("kc3_defoperator: env_eval_struct");
    assert(! "kc3_defoperator: env_eval_struct");
    return NULL;
  }
  if (! env_defoperator(env, &tmp)) {
    err_puts("kc3_defoperator: env_defoperator 1");
    assert(! "kc3_defoperator: env_defoperator 1");
    return NULL;
  }
  *dest = tmp;
  return dest;
}

s_tag * kc3_defspecial_operator (s_tag *tag, s_tag *dest)
{
  s_env *env;
  assert(tag);
  assert(dest);
  if (securelevel(0) > 1) {
    err_stacktrace();
    err_puts("kc3_defspecial_operator: cannot use defspecial_operator"
             " with securelevel > 1");
    abort();
  }
  env = env_global();
  return env_defspecial_operator(env, tag, dest);
}

s_tag * kc3_defstruct (s_list **spec, s_tag *dest)
{
  s_tag tag;
  s_tag tmp = {0};
  assert(spec);
  if (securelevel(0) > 1) {
    err_stacktrace();
    err_puts("kc3_defstruct: cannot use defstruct"
             " with securelevel > 1");
    abort();
  }
  if (! spec)
    return NULL;
  if (! env_eval_list(env_global(), *spec, &tag))
    return NULL;
  if (tag.type != TAG_PLIST)
    return NULL;
  tmp.type = TAG_PSYM;
  tmp.data.psym = env_defstruct(env_global(), tag.data.plist);
  *dest = tmp;
  tag_clean(&tag);
  return dest;
}

void ** kc3_dlopen (const s_str *path, void **dest)
{
  assert(path);
  assert(dest);
  if (securelevel(0) > 0) {
    err_stacktrace();
    err_puts("kc3_dlopen: cannot dlopen with securelevel > 0");
    abort();
  }
  return env_dlopen(env_global(), path, dest);
}

sw kc3_dump (const s_str *path)
{
  s_env *env;
  assert(path);
  env = env_global();
  return env_dump(env, path);
}

p_facts * kc3_env_db (p_facts *dest)
{
  return pfacts_init_copy(dest, &env_global()->facts);
}

s_buf ** kc3_err_buf (s_buf **dest)
{
  s_env *env;
  assert(dest);
  env = env_global();
  assert(env);
  *dest = env->err;
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

void kc3_exit_subprocess (s_tag *code)
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

s64 * kc3_fork (s64 *dest)
{
  s32 e;
  pid_t result;
  result = fork();
  if (result < 0) {
    e = errno;
    err_write_1("kc3_fork: fork: ");
    err_puts(strerror(e));
    return NULL;
  }
  *dest = result;
  return dest;
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
  return tag_init_pstruct_with_data(tag, &g_sym_Fact, ptr->p, false);
}

s_facts_connection * kc3_facts_accept (s_facts *facts,
                                       p_socket server_fd,
                                       p_tls *tls)
{
  assert(facts);
  assert(server_fd);
  assert(tls);
  return facts_accept(facts, *server_fd, *tls);
}

s_facts_acceptor ** kc3_facts_acceptor_loop (s_facts *facts,
                                             p_socket server,
                                             p_tls *tls,
                                             s_facts_acceptor **dest)
{
  s_facts_acceptor *tmp;
  assert(facts);
  assert(server);
  assert(tls);
  assert(dest);
  tmp = facts_acceptor_loop(facts, *server, *tls);
  if (! tmp)
    return NULL;
  *dest = tmp;
  return dest;
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

s_facts * kc3_facts_connect (s_facts *facts, const s_str *host,
                              const s_str *service, p_tls_config *config)
{
  assert(facts);
  assert(host);
  assert(service);
  assert(config);
  return facts_connect(facts, host, service, *config) ? facts : NULL;
}

s_tag * kc3_facts_first_with (s_facts *facts, s_list **spec,
                              p_callable *callback, s_tag *dest)
{
  return env_facts_first_with(env_global(), facts, spec, *callback,
                              dest);
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

s_facts * kc3_facts_set_priority (s_facts *facts, u8 priority)
{
  return facts_set_priority(facts, priority);
}

s_facts * kc3_facts_set_secret (s_facts *facts, const s_str *secret)
{
  return facts_set_secret(facts, secret);
}

s_facts * kc3_facts_set_server_count (s_facts *facts, uw server_count)
{
  return facts_set_server_count(facts, server_count);
}

s_tag * kc3_facts_with (s_facts *facts, s_list **spec,
                        p_callable *callback, s_tag *dest)
{
  return env_facts_with(env_global(), facts, spec, *callback, dest);
}

s_tag * kc3_facts_with_macro (s_tag *facts_tag, s_tag *spec_tag,
                              s_tag *do_block_tag, s_tag *dest)
{
  return env_facts_with_macro(env_global(), facts_tag, spec_tag,
                              do_block_tag, dest);
}

s_tag * kc3_facts_with_tags (s_facts *facts, s_tag *subject,
                             s_tag *predicate, s_tag *object,
                             p_callable *callback, s_tag *dest)
{
  return env_facts_with_tags(env_global(), facts, subject, predicate,
                             object, *callback, dest);
}

s_tag * kc3_facts_with_transaction (s_tag *facts_arg, s_tag *tag_arg,
                                    s_tag *dest)
{
  return env_facts_with_transaction(env_global(), facts_arg, tag_arg,
                                    dest);
}

s_tag * kc3_facts_with_tuple (s_facts *facts, p_tuple *tuple,
                              p_callable *callback, s_tag *dest)
{
  assert(facts);
  assert(tuple);
  assert(callback);
  assert(dest);
  if ((*tuple)->count < 3) {
    err_puts("kc3_facts_with_tuple: tuple count < 3");
    assert(! "kc3_facts_with_tuple: tuple count < 3");
    return NULL;
  }
  return env_facts_with_tags(env_global(), facts, (*tuple)->tag,
                             (*tuple)->tag + 1, (*tuple)->tag + 2,
                             *callback, dest);
}

bool * kc3_file_access (s_str *path, p_sym *mode, bool *dest)
{
  *dest = file_access(path, *mode);
  return dest;
}

s_tag * kc3_quote_cfn (const s_sym **sym, s_tag *dest)
{
  assert(sym);
  return tag_init_psym(dest, *sym);
}

s_tag * kc3_getenv (const s_str *name, s_tag *dest)
{
  char *p;
  assert(name);
  assert(dest);
  p = getenv(name->ptr.pchar);
  if (! p)
    return tag_init_void(dest);
  return tag_init_str_1(dest, NULL, p);
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

s_str * kc3_hostname (s_str *dest)
{
  sw e;
#if defined(HOST_NAME_MAX)
  char name[HOST_NAME_MAX + 1] = {0};
  size_t size = HOST_NAME_MAX;
#else
# if defined(__FreeBSD__)
  char *name;
  s_str *result;
  size_t size;
  size = sysconf(_SC_HOST_NAME_MAX);
  name = calloc(1, size + 1);
# endif
#endif
#if defined(WIN32) || defined(WIN64)
  kc3_wsa_init_once();
#endif
  if (gethostname(name, size)) {
    e = errno;
    err_write_1("kc3_hostname: gethostname: ");
    err_puts(strerror(e));
    assert(! "kc3_hostname: gethostname");
    return NULL;
  }
#if defined(HOST_NAME_MAX)
  return str_init_1_alloc(dest, name);
#else
# if defined(__FreeBSD__)
  result = str_init_1_alloc(dest, name);
  free(name);
  return result;
# endif
#endif
}

s_ident * kc3_ident_init (s_ident *ident, p_sym *module, p_sym *sym)
{
  s_ident tmp = {0};
  assert(ident);
  assert(module);
  assert(sym);
  tmp.module = *module;
  tmp.sym = *sym;
  *ident = tmp;
  return ident;
}

s_tag * kc3_identity (s_tag *tag, s_tag *dest)
{
  return tag_init_copy(dest, tag);
}

/* Special operator. */
s_tag * kc3_if_then_else (s_tag *cond, s_tag *then,
                          s_tag *else_, s_tag *dest)
{
  return env_if_then_else(env_global(), cond, then, else_, dest);
}

s_tag * kc3_ignore_errors (s_tag *src, s_tag *dest)
{
  return env_ignore_errors(env_global(), src, dest);
}

s_buf ** kc3_in_buf (s_buf **dest)
{
  s_env *env;
  assert(dest);
  env = env_global();
  assert(env);
  *dest = env->in;
  return dest;
}

s_env * kc3_init (s_env *env, int *argc, char ***argv)
{
  return env_init(env, argc, argv);
}

s_tag * kc3_integer_reduce (s_tag *tag, s_tag *dest)
{
  return tag_integer_reduce(tag, dest);
}

bool kc3_killpg (sw process_group, p_sym *signal)
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
    err_inspect_psym(signal);
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

s_buf ** kc3_out_buf (s_buf **dest)
{
  s_env *env;
  assert(dest);
  env = env_global();
  assert(env);
  *dest = env->out;
  return dest;
}

#ifdef __OpenBSD__

void kc3_pledge (const s_str *promises, const s_str *execpromises)
{
  s32 e;
  assert(promises);
  assert(execpromises);
  if (pledge(promises->ptr.pchar, execpromises->ptr.pchar)) {
    e = errno;
    err_write_1("kc3_pledge: ");
    err_inspect_str(promises);
    err_write_1(": ");
    err_write_1(strerror(e));
    err_write_1("\n");
    assert(! "kc3_pledge");
    abort();
  }
}

#else

void kc3_pledge (const s_str *promises, const s_str *execpromises)
{
  assert(promises);
  assert(execpromises);
  (void) promises;
  (void) execpromises;
  err_puts("kc3_pledge: not implemented");
}

#endif

s_tag * kc3_plist_length (const s_list **plist, s_tag *dest)
{
  s_tag s = {0};
  tag_init_sw(&s, list_length(*plist));
  return tag_integer_reduce(&s, dest);
}

s_array * kc3_plist_to_array (p_list *plist,
                              p_sym *array_type,
                              s_array *dest)
{
  return list_to_array(*plist, *array_type, dest);
}

bool kc3_load (const s_str *path)
{
  return env_load(env_global(), path);
}

p_list * kc3_map_map_to_list (s_map *map, p_callable *callable,
                              p_list *dest)
{
  assert(map);
  assert(callable);
  assert(*callable);
  assert(dest);
  if (! map_map_to_list(map, *callable, dest))
    return NULL;
  return dest;
}

void kc3_marshall_delete (p_marshall *m)
{
  marshall_delete(*m);
}

p_marshall * kc3_marshall_new (p_marshall *m, uw buf_size)
{
  assert(m);
  *m = marshall_new(buf_size);
  return m;
}

void kc3_marshall_read_delete (p_marshall_read *mr)
{
  marshall_read_delete(*mr);
}

p_marshall_read * kc3_marshall_read_new (p_marshall_read *mr)
{
  assert(mr);
  *mr = marshall_read_new();
  return mr;
}

p_marshall_read * kc3_marshall_read_new_str (p_marshall_read *mr,
                                             const s_str *input)
{
  assert(mr);
  if (! (*mr = marshall_read_new_str(input))) {
    err_puts("kc3_marshall_read_new_str: marshall_read_new_str");
    assert(! "kc3_marshall_read_new_str: marshall_read_new_str");
    return NULL;
  }
  return mr;
}

bool kc3_marshall_read_tag (p_marshall_read *mr, s_tag *tag)
{
  if (! marshall_read_tag(*mr, false, tag))
    return false;
  return true;
}

s_tag * kc3_marshall_read_tag_from_str (const s_str *input,
                                        s_tag *dest)
{
  s_marshall_read mr = {0};
  s_tag tmp = {0};
  assert(input);
  assert(dest);
  if (! marshall_read_init_str(&mr, input))
    return NULL;
  if (! marshall_read_tag(&mr, false, &tmp)) {
    marshall_read_clean(&mr);
    return NULL;
  }
  marshall_read_clean(&mr);
  *dest = tmp;
  return dest;
}

bool kc3_marshall_tag (p_marshall *m, const s_tag *tag)
{
  if (! marshall_tag(*m, false, tag))
    return false;
  return true;
}

s_str * kc3_marshall_tag_to_str (const s_tag *tag, s_str *dest)
{
  s_marshall m = {0};
  s_str tmp = {0};
  assert(tag);
  assert(dest);
  if (! marshall_init(&m, 128 * 1024 * 1024)) {
    err_puts("kc3_marshall_tag_to_str: marshall_init");
    assert(! "kc3_marshall_tag_to_str: marshall_init");
    return NULL;
  }
  if (! marshall_tag(&m, false, tag)) {
    err_puts("kc3_marshall_tag_to_str: marshall_tag");
    assert(! "kc3_marshall_tag_to_str: marshall_tag");
    marshall_clean(&m);
    return NULL;
  }
  if (! marshall_to_str(&m, &tmp)) {
    err_puts("kc3_marshall_tag_to_str: marshall_to_str");
    assert(! "kc3_marshall_tag_to_str: marshall_to_str");
    marshall_clean(&m);
    return NULL;
  }
  marshall_clean(&m);
  *dest = tmp;
  return dest;
}

s_str * kc3_marshall_to_str (p_marshall *m, s_str *dest)
{
  return marshall_to_str(*m, dest);
}

s_tag * kc3_match (s_tag *tag, s_map *map, s_tag *dest)
{
  s_env *env;
  uw i;
  bool silence_errors;
  s_tag tag_eval;
  s_tag tag_tmp = {0};
  s_unwind_protect unwind_protect;
  assert(tag);
  assert(map);
  assert(dest);
  env = env_global();
  assert(env);
  if (! env_eval_tag(env, tag, &tag_eval))
    return NULL;
  env_unwind_protect_push(env, &unwind_protect);
  if (setjmp(unwind_protect.buf)) {
    env_unwind_protect_pop(env, &unwind_protect);
    tag_clean(&tag_tmp);
    tag_clean(&tag_eval);
    longjmp(*unwind_protect.jmp, 1);
  }
  i = 0;
  while (i < map->count) {
    silence_errors = env->silence_errors;
    env->silence_errors = true;
    if (env_eval_equal_tag(env, false, &tag_eval, map->key + i,
                           &tag_tmp)) {
      env->silence_errors = silence_errors;
      env_unwind_protect_pop(env, &unwind_protect);
      tag_clean(&tag_eval);
      tag_clean(&tag_tmp);
      if (map->value[i].type != TAG_DO_BLOCK) {
        err_puts("kc3_match: invalid match: not a Block");
        assert(! "kc3_match: invalid match: not a Block");
        return NULL;
      }
      if (! env_eval_do_block(env, &map->value[i].data.do_block, dest))
        return NULL;
      return dest;
    }
    tag_clean(&tag_tmp);
    env->silence_errors = silence_errors;
    i++;
  }
  env_unwind_protect_pop(env, &unwind_protect);
  tag_clean(&tag_eval);
  return tag_init_void(dest);
}

bool kc3_maybe_reload (const s_str *path)
{
  return env_maybe_reload(env_global(), path);
}

const s_sym ** kc3_module (const s_sym **dest)
{
  return env_module(env_global(), dest);
}

bool * kc3_must_clean (p_sym *sym, bool *dest)
{
  assert(sym);
  return sym_must_clean(*sym, dest);
}

void kc3_mutex_delete (s_mutex **mutex)
{
  mutex_delete(*mutex);
}

void kc3_mutex_lock (s_mutex **mutex)
{
  mutex_lock(*mutex);
}

s_mutex ** kc3_mutex_new (s_mutex **dest)
{
  s_mutex *tmp;
  if (! (tmp = mutex_new()))
    return NULL;
  *dest = tmp;
  return dest;
}

void kc3_mutex_unlock (s_mutex **mutex)
{
  mutex_unlock(*mutex);
}

uw * kc3_offsetof (p_sym *module,
                   p_sym *key, uw *dest)
{
  uw i = 0;
  s_struct_type *st;
  if (! pstruct_type_find(*module, &st) ||
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
    err_inspect_sw_decimal(r);
    err_write_1(" != ");
    err_inspect_u32_decimal(src->size);
    err_write_1("\n");
    assert(! "kc3_parse_tag: invalid tag");
    buf_clean(&buf);
    return NULL;
  }
  buf_clean(&buf);
  return tag;
}

sw kc3_puts (const s_tag *tag)
{
  sw r;
  sw result = 0;
#if HAVE_PTHREAD
  s_env *env;
  env = env_global();
  rwlock_w(env->out->rwlock);
#endif
  if (tag->type == TAG_STR) {
    if ((r = io_write_str(&tag->data.str)) < 0)
      goto clean;
  }
  else {
    if ((r = io_inspect_tag(tag)) < 0)
      goto clean;
  }
  result += r;
  if ((r = io_write_1("\n")) < 0)
    goto clean;
  result += r;
  if ((r = io_flush()) < 0)
    goto clean;
  r = result;
 clean:
#if HAVE_PTHREAD
  rwlock_unlock_w(env->out->rwlock);
#endif
  return r;
}

s_tag * kc3_require (p_sym *module, s_tag *dest)
{
  assert(module);
  assert(*module);
  assert(sym_is_module(*module));
  assert(dest);
  if (! env_module_ensure_loaded(env_global(), *module)) {
    err_write_1("kc3_require: ");
    err_inspect_sym(*module);
    err_puts(": env_module_ensure_loaded");
    assert(! "kc3_require: env_module_ensure_loaded");
    return NULL;
  }
  return tag_init_psym(dest, *module);
}

void kc3_return (s_tag *value)
{
  s_block *block;
  s_env *env;
  s32 exit_code = -1;
  const s_sym *sym_S32 = &g_sym_S32;
  s_tag tmp = {0};
  assert(value);
  env = env_global();
  assert(env);
  block = env_global()->block;
  if (! env_eval_tag(env, value, &tmp)) {
    err_write_1("kc3_return: env_eval_tag(");
    err_inspect_tag(value);
    err_write_1(")\n");
  }
  if (! block) {
    s32_init_cast(&exit_code, &sym_S32, &tmp);
    tag_clean(&tmp);
    exit(exit_code);
  }
  block_return(block, &tmp);
}

void kc3_return_from (const s_sym **name, s_tag *value)
{
  s_env *env;
  s_tag tmp = {0};
  assert(name);
  assert(value);
  env = env_global();
  assert(env);
  if (! env_eval_tag(env, value, &tmp)) {
    err_write_1("kc3_return: env_eval_tag(");
    err_inspect_tag(value);
    err_write_1(")\n");
  }
  block_return_from(*name, &tmp);
}

s_list ** kc3_search_modules (s_list **dest)
{
  return env_search_modules(env_global(), dest);
}

void kc3_sleep (s_tag *timeout)
{
  s_timespec time = {0};
  const s_sym *sym_Timespec;
  sym_Timespec = sym_1("Timespec");
  if (timespec_init_cast(&time, &sym_Timespec, timeout))
    nanosleep(&time, NULL);
}

s_list ** kc3_stacktrace (s_list **dest)
{
  return env_stacktrace(env_global(), dest);
}

s_str * kc3_str (const s_tag *tag, s_str *dest)
{
  const s_sym *sym = &g_sym_Str;
  switch (tag->type) {
  case TAG_PLIST:
    return str_init_concatenate_list(dest, tag->data.plist);
  case TAG_STR:
    return str_init_copy(dest, &tag->data.str);
  default:
    return str_init_cast(dest, &sym, tag);
  }
}

void kc3_str_zero (s_str **str)
{
  str_zero(*str);
}

s_str * kc3_strerror (sw err_no, s_str *dest)
{
  const char *s;
  s = strerror(err_no);
  return str_init_1_alloc(dest, s);
}

s_tag * kc3_struct_put (s_tag *s, p_sym *key,
                        s_tag *value, s_tag *dest)
{
  p_struct tmp;
  assert(s);
  assert(key);
  assert(value);
  assert(dest);
  if (s->type != TAG_PSTRUCT) {
    err_puts("kc3_struct_put: not a struct");
    assert(! "kc3_struct_put: not a struct");
    return NULL;
  }
  if (! pstruct_init_put(&tmp, s->data.pstruct, *key, value))
    return NULL;
  dest->type = TAG_PSTRUCT;
  dest->data.pstruct = tmp;
  return dest;
}

s_tag * kc3_sysctl (s_tag *dest, const s_list * const *list)
{
#if (defined(__OpenBSD__))
  s32 e;
  const s_list *l;
  s32 mib[64];
  s32 mib_len = 0;
  s_tag  tmp = {0};
  size_t tmp_data_size;
  l = *list;
  if (! l ||
      l->tag.type != TAG_PSYM) {
    err_puts("kc3_sysctl: argument is not a Sym list");
    assert(! "kc3_sysctl: argument is not a Sym list");
  }
  if (l->tag.data.psym == sym_1("ddb")) {
    mib[mib_len] = CTL_DDB;
    mib_len++;
    l = list_next(l);
  }
  else if (l->tag.data.psym == sym_1("debug")) {
    mib[mib_len] = CTL_DEBUG;
    mib_len++;
    l = list_next(l);
  }
#ifdef CTL_FS
  else if (l->tag.data.psym == sym_1("fs")) {
    mib[mib_len] = CTL_FS;
    mib_len++;
    if ((l = list_next(l))) {
      if (l->tag.data.psym == sym_1("posix")) {
        mib[mib_len] = FS_POSIX;
        mib_len++;
        if ((l = list_next(l))) {
          if (l->tag.data.psym == sym_1("setuid")) {
            mib[mib_len] = FS_POSIX;
            mib_len++;
            if (! (l = list_next(l)))
              tmp.type = TAG_S32;
          }
        }
      }
    }
  }
#endif
  else if (l->tag.data.psym == sym_1("hw")) {
    mib[mib_len] = CTL_HW;
    mib_len++;
    if ((l = list_next(l))) {
      if (l->tag.data.psym == sym_1("ncpu")) {
        mib[mib_len] = HW_NCPU;
        mib_len++;
        if (! (l = list_next(l)))
          tmp.type = TAG_S32;
      }
      else if (l->tag.data.psym == sym_1("ncpufound")) {
        mib[mib_len] = HW_NCPUFOUND;
        mib_len++;
        l = list_next(l);
        if (! l)
          tmp.type = TAG_S32;
      }
      else if (l->tag.data.psym == sym_1("ncpuonline")) {
        mib[mib_len] = HW_NCPUONLINE;
        mib_len++;
        l = list_next(l);
        if (! l)
          tmp.type = TAG_S32;
      }
      else if (l->tag.data.psym == sym_1("pagesize")) {
        mib[mib_len] = HW_PAGESIZE;
        mib_len++;
        l = list_next(l);
        if (! l)
          tmp.type = TAG_S32;
      }
      else if (l->tag.data.psym == sym_1("physmem64")) {
        mib[mib_len] = HW_PHYSMEM64;
        mib_len++;
        l = list_next(l);
        if (! l)
          tmp.type = TAG_S64;
      }
    }
  }
  else if (l->tag.data.psym == sym_1("kern")) {
    mib[mib_len] = CTL_KERN;
    mib_len++;
    l = list_next(l);
  }
  else if (l->tag.data.psym == sym_1("machdep")) {
    mib[mib_len] = CTL_MACHDEP;
    mib_len++;
    l = list_next(l);
  }
  else if (l->tag.data.psym == sym_1("net")) {
    mib[mib_len] = CTL_NET;
    mib_len++;
    l = list_next(l);
  }
  else if (l->tag.data.psym == sym_1("vfs")) {
    mib[mib_len] = CTL_VFS;
    mib_len++;
    l = list_next(l);
  }
  else if (l->tag.data.psym == sym_1("vm")) {
    mib[mib_len] = CTL_VM;
    mib_len++;
    l = list_next(l);
  }
  if (! tmp.type) {
    err_write_1("kc3_sysctl: invalid argument: ");
    if (l)
      err_inspect_sym(l->tag.data.psym);
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

p_tuple * kc3_system (p_list *list, p_tuple *dest)
{
#if defined(WIN32) || defined(WIN64)
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
  p_tuple *r = NULL;
  s32 status;
  const s_str *str;
  p_tuple tuple = NULL;
  s_str tmp_str = {0};
  assert(list);
  assert(dest);
  if (securelevel(0) > 0) {
    err_puts("kc3_system: cannot call system when securelevel > 0");
    abort();
  }
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
  a--;
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
  if (! pid) {
    kc3_system_pipe_exec(pipe_fd[1], argv, list);
    _exit(1);
  }
  close(pipe_fd[1]);
  if (! fd_read_until_eof(pipe_fd[0], &tmp_str)) {
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
    str_clean(&tmp_str);
    goto clean;
  }
  if (! (tuple = tuple_new(2)))
    goto clean;
  tuple->tag[0].type = TAG_S32;
  tuple->tag[0].data.s32 = status;
  tag_integer_reduce(tuple->tag, tuple->tag);
  tuple->tag[1].type = TAG_STR;
  tuple->tag[1].data.str = tmp_str;
  *dest = tuple;
  r = dest;
 clean:
  while (a >= argv) {
    free(*a);
    a--;
  }
  free(argv);
  return r;
#endif
}

#if ! (defined(WIN32) || defined(WIN64))

static void kc3_system_pipe_exec (s32 pipe_w, char **argv,
                                  p_list *list)
{
  sw e;
  if (securelevel(0) > 0) {
    err_puts("kc3_system_pipe_exec: cannot call system when"
             " securelevel > 0");
    abort();
  }
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

#endif

void kc3_tag_delete (s_tag **tag)
{
  tag_delete(*tag);
}

s_tag ** kc3_tag_new_copy (s_tag **tag, s_tag *src)
{
  s_tag *tmp;
  if (false) {
    err_write_1("kc3_tag_new_copy: tag=");
    err_inspect_uw_hexadecimal((uw) tag);
    err_write_1(" src=");
    err_inspect_uw_hexadecimal((uw) src);
    err_write_1("\n");
    if (src) {
      err_write_1("  src->type=");
      err_inspect_u8((u8) src->type);
      err_write_1("\n");
    }
  }
  if (! (tmp = tag_new_copy(src)))
    return NULL;
  *tag = tmp;
  return tag;
}

s_pointer * kc3_tag_to_pointer (s_tag *tag, s_pointer *dest)
{
  s_env *env;
  s_tag *resolved = NULL;
  env = env_global();
  assert(tag);
  assert(dest);
  assert(env);
  if (tag->type != TAG_IDENT) {
    err_puts("kc3_tag_to_pointer: expected ident");
    assert(! "kc3_tag_to_pointer: expected ident");
    return NULL;
  }
  if (! tag->data.ident.module) {
    resolved = env_frames_get(env, tag->data.ident.sym);
    if (resolved)
      return pointer_init_tag(dest, resolved);
  }
  if (! (resolved = env_ident_get_address(env, &tag->data.ident))) {
    err_write_1("kc3_tag_to_pointer: undeclared ident ");
    err_inspect_ident(&tag->data.ident);
    err_write_1("\n");
    assert(! "kc3_tag_to_pointer: undeclared ident");
    return NULL;
  }
  return pointer_init_tag(dest, resolved);
}

s_marshall ** kc3_tag_marshall (s_marshall **m, bool heap, s_tag **tag)
{
  assert(m);
  assert(*m);
  assert(tag);
  assert(*tag);
  if (! marshall_tag(*m, heap, *tag))
    return NULL;
  return m;
}

s_tag ** kc3_tag_marshall_read (s_marshall_read **mr, bool heap,
                                s_tag **dest)
{
  s_tag *tmp = NULL;
  assert(mr);
  assert(*mr);
  assert(dest);
  if (! (tmp = tag_new()))
    return NULL;
  if (! marshall_read_tag(*mr, heap, tmp)) {
    tag_delete(tmp);
    return NULL;
  }
  *dest = tmp;
  return dest;
}

s32 kc3_tag_type (const s_tag *tag)
{
  return tag->type;
}

s_time * kc3_uptime (s_time *dest)
{
  s_env *env;
  assert(dest);
  env = env_global();
  assert(env);
  return env_uptime(env, dest);
}

p_tuple * kc3_wait (p_tuple *dest)
{
#if defined(WIN32) || defined(WIN64)
  (void) dest;
  return NULL;
#else
  pid_t pid;
  s32 status;
  p_tuple tuple;
  assert(dest);
  pid = wait(&status);
  if (pid < 0)
    return NULL;
  if (! (tuple = tuple_new(2)))
    return NULL;
  tuple->tag[0].type = TAG_S32;
  tuple->tag[0].data.s32 = pid;
  tag_integer_reduce(tuple->tag, tuple->tag);
  tuple->tag[1].type = TAG_S32;
  tuple->tag[1].data.s32 = status;
  tag_integer_reduce(tuple->tag + 1, tuple->tag + 1);
  *dest = tuple;
  return dest;
#endif
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
  if (tag->type != TAG_PTUPLE ||
      tag->data.ptuple->count != 3) {
    err_puts("kc3_thread_delete: invalid value");
    assert(! "kc3_thread_delete: invalid value");
    tag_delete(tag);
    return NULL;
  }
  if (! tag_init_copy(dest, tag->data.ptuple->tag)) {
    tag_delete(tag);
    return NULL;
  }
  tag_delete(tag);
  return dest;
}

u_ptr_w * kc3_thread_new (u_ptr_w *dest, p_callable *start)
{
  s_tag *tag;
  if (! (tag = tag_new_ptuple(3)))
    return NULL;
  if (! tag_init_pcallable_copy(tag->data.ptuple->tag + 1, start)) {
    tag_delete(tag);
    return NULL;
  }
  tag->data.ptuple->tag[2].type = TAG_PTR;
  if (! (tag->data.ptuple->tag[2].data.ptr.p =
         env_fork_new(env_global()))) {
    tag_delete(tag);
    return NULL;
  }
  if (pthread_create((pthread_t *) &dest->p, NULL, kc3_thread_start,
                     tag)) {
    err_puts("kc3_thread_new: pthread_create");
    assert(! "kc3_thread_new: pthread_create");
    env_fork_delete(tag->data.ptuple->tag[2].data.ptr.p);
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
  if (tag->type != TAG_PTUPLE) {
    fprintf(stderr,
            "kc3_thread_start: invalid argument: not a tuple\n");
    assert(! "kc3_thread_start: invalid argument: not a tuple");
    return NULL;
  }
  if (tag->data.ptuple->count != 3) {
    fprintf(stderr, "kc3_thread_start: invalid argument:"
	    " tuple arity mismatch\n");
    assert(!("kc3_thread_start: invalid argument:"
             " tuple arity mismatch"));
    return NULL;
  }
  if (tag->data.ptuple->tag[2].type != TAG_PTR) {
    fprintf(stderr, "kc3_thread_start: invalid argument: not a Ptr\n");
    assert(! "kc3_thread_start: invalid argument: not a Ptr");
    return NULL;
  }
  env = tag->data.ptuple->tag[2].data.ptr.p;
  if (tag->data.ptuple->tag[1].type != TAG_PCALLABLE) {
    fprintf(stderr, "kc3_thread_start: invalid argument:"
	    " not a Callable (Fn or Cfn)\n");
    assert(!("kc3_thread_start: invalid argument:"
             " not a Callable (Fn or Cfn)"));
    env_fork_delete(env);
    return NULL;
  }
  start = tag->data.ptuple->tag[1].data.pcallable;
  env_global_set(env);
  if (! eval_callable_call(start, NULL, tag->data.ptuple->tag)) {
    env_fork_delete(env);
    return NULL;
  }
  env_fork_delete(env);
  return tag;
}

#ifdef __OpenBSD__

void kc3_unveil (const s_str *path, const s_str *permissions)
{
  s32 e;
  const char *unveil_path = NULL;
  const char *unveil_permissions = NULL;
  assert(path);
  assert(permissions);
  if (path && path->size)
    unveil_path = path->ptr.pchar;
  if (permissions && permissions->size)
    unveil_permissions = permissions->ptr.pchar;
  if (unveil(unveil_path, unveil_permissions)) {
    e = errno;
    err_write_1("kc3_unveil: ");
    err_inspect_str(path);
    err_write_1(": ");
    err_write_1(strerror(e));
    err_write_1("\n");
    assert(! "kc3_unveil");
    abort();
  }
}

#else

void kc3_unveil (const s_str *path, const s_str *permissions)
{
  assert(path);
  assert(permissions);
  (void) path;
  (void) permissions;
  err_puts("kc3_unveil: not implemented");
}

#endif

s_tag * kc3_unwind_protect (s_tag *protected_, s_tag *cleanup,
                            s_tag *dest)
{
  return env_unwind_protect(env_global(), protected_, cleanup, dest);
}

s_tag * kc3_while (s_tag *cond, s_tag *body, s_tag *dest)
{
  return env_while(env_global(), cond, body, dest);
}
