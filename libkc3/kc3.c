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
#include "alist.h"
#include "array.h"
#include "assert.h"
#include "bool.h"
#include "buf.h"
#include "buf_parse.h"
#include "call.h"
#include "env.h"
#include "facts_cursor.h"
#include "kc3_main.h"
#include "list.h"
#include "map.h"
#include "str.h"
#include "struct.h"
#include "struct_type.h"
#include "sym.h"
#include "tag.h"

const s_str g_kc3_base_binary = {{NULL}, 2, {"01"}};
const s_str g_kc3_base_octal = {{NULL}, 8, {"01234567"}};
const s_str g_kc3_base_decimal = {{NULL}, 10, {"0123456789"}};
const s_str g_kc3_base_hexadecimal = {{NULL}, 16, {"0123456789abcdef"}};
const s_str g_kc3_bases_hexadecimal[2] = {{{NULL}, 16,
                                           {"0123456789abcdef"}},
                                         {{NULL}, 16,
                                          {"0123456789ABCDEF"}}};
sw          g_kc3_exit_code = 1;

s_tag * kc3_access (const s_tag *tag, const s_list * const *key,
                    s_tag *dest)
{
  assert(tag);
  assert(key);
  assert(dest);
  switch (tag->type) {
  case TAG_ARRAY:
    return array_access(&tag->data.array, key, dest);
  case TAG_LIST:
    if (list_is_alist((const s_list * const *) &tag->data.list)) {
      if (! alist_access((const s_list * const *) &tag->data.list,
                         key, dest))
        return tag_init_void(dest);
      return dest;
    }
    break;
  case TAG_MAP:
    return map_access(&tag->data.map, key, dest);
  case TAG_STRUCT:
    return struct_access(&tag->data.struct_, key, dest);
  default:
    break;
  }
  err_write_1("kc3_access: cannot access tag type ");
  err_write_1(tag_type_to_string(tag->type));
  err_write_1(" for key ");
  err_inspect_list(key);
  err_write_1("\n");
  return NULL;
}

bool * kc3_and (const s_tag *a, const s_tag *b, bool *dest)
{
  return env_and(&g_kc3_env, a, b, dest);
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
                        const s_tag *symbol_value,
                        u8 operator_precedence,
                        const s_sym **operator_associativity,
                        s_tag *dest)
{
  return env_defoperator(&g_kc3_env, name, sym, symbol_value,
                         operator_precedence,
                         operator_associativity, dest);
}

s_tag * kc3_defstruct (const s_list * const *spec, s_tag *dest)
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

s_facts * kc3_env_facts (void)
{
  return &g_kc3_env.facts;
}

sw kc3_errno (void)
{
  return errno;
}

void kc3_exit (sw code)
{
  exit((int) code);
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

s_tag * kc3_facts_first_with_tags (s_facts *facts, s_tag *subject,
                                   s_tag *predicate, s_tag *object,
                                   s_fn *callback, s_tag *dest)
{
  return env_facts_with_tags(&g_kc3_env, facts, subject, predicate,
                             object, callback, dest);
}

uw * kc3_facts_next_id (uw *dest)
{
  assert(dest);
  *dest = g_kc3_env.facts.next_id;
  return dest;
}

s_tag * kc3_facts_with (s_facts *facts, s_list **spec,
                        s_fn *callback, s_tag *dest)
{
  return env_facts_with(&g_kc3_env, facts, spec, callback, dest);
}

s_tag * kc3_facts_with_tags (s_facts *facts, s_tag *subject,
                             s_tag *predicate, s_tag *object,
                             s_fn *callback, s_tag *dest)
{
  return env_facts_with_tags(&g_kc3_env, facts, subject, predicate,
                             object, callback, dest);
}

s_tag * kc3_facts_with_tuple (s_facts *facts, s_tuple *tuple,
                              s_fn *callback, s_tag *dest)
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
                             tuple->tag + 1, tuple->tag + 2, callback,
                             dest);
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

/* Special operator. */
s_tag * kc3_if_then_else (const s_tag *cond, const s_tag *then,
                          const s_tag *else_, s_tag *dest)
{
  bool  cond_bool;
  s_tag cond_eval = {0};
  const s_sym *type;
  if (! env_eval_tag(&g_kc3_env, cond, &cond_eval))
    return NULL;
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

s_tag * kc3_let (const s_tag *tag, const s_block *block, s_tag *dest)
{
  return env_let(&g_kc3_env, tag, block, dest);
}

void kc3_license (void)
{
  buf_write_1(&g_kc3_env.out, g_kc3_license);
  buf_flush(&g_kc3_env.out);
}

bool kc3_load (const s_str *path)
{
  return env_load(&g_kc3_env, path);
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
  const s_struct_type *st;
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

bool * kc3_or (const s_tag *a, const s_tag *b, bool *dest)
{
  return env_or(&g_kc3_env, a, b, dest);
}

s_tag * kc3_identity (const s_tag *tag, s_tag *dest)
{
  return tag_init_copy(dest, tag);
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

s_tag * kc3_while (const s_tag *cond, const s_tag *body, s_tag *dest)
{
  return env_while(&g_kc3_env, cond, body, dest);
}
