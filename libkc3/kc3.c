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
#include "assert.h"
#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include "bool.h"
#include "buf.h"
#include "buf_parse.h"
#include "kc3_main.h"
#include "env.h"
#include "map.h"
#include "str.h"
#include "struct.h"
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

s_tag * kc3_access (const s_tag *tag, const s_sym * const *sym,
                   s_tag *dest)
{
  assert(tag);
  assert(sym);
  assert(dest);
  switch (tag->type) {
  case TAG_MAP:
    return map_access(&tag->data.map, *sym, dest);
  case TAG_STRUCT:
    return struct_access(&tag->data.struct_, *sym, dest);
  default:
    break;
  }
  err_write_1("kc3_access: cannot access tag type ");
  err_write_1(tag_type_to_string(tag->type));
  err_write_1(" for key ");
  err_inspect_sym(sym);
  err_write_1("\n");
  return NULL;
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
  return env_def(&g_kc3_env, call, dest);
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

void kc3_exit (sw code)
{
  exit((int) code);
}

bool * kc3_must_clean(const s_sym * const *sym, bool *dest)
{
    assert(sym);
    return sym_must_clean(*sym, dest);
}

uw * kc3_facts_next_id (uw *dest)
{
  assert(dest);
  *dest = g_kc3_env.facts.next_id;
  return dest;
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
  bool b;
  s_tag tmp = {0};
  const s_sym *type;
  if (! env_eval_tag(&g_kc3_env, cond, &tmp))
    return NULL;
  type = &g_sym_Bool;
  if (! bool_init_cast(&b, &type, &tmp)) {
    tag_clean(&tmp);
    return NULL;
  }
  tag_clean(&tmp);
  if (b) {
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

s_tag * kc3_pin (const s_tag *a, s_tag *dest)
{
  if (! env_eval_tag(&g_kc3_env, a, dest))
    return NULL;
  return dest;
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
