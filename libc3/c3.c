/* c3
 * Copyright 2022-2024 kmx.io <contact@kmx.io>
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
#include "c3_main.h"
#include "env.h"
#include "str.h"
#include "sym.h"
#include "tag.h"

const s_str g_c3_base_binary = {{NULL}, 2, {"01"}};
const s_str g_c3_base_octal = {{NULL}, 8, {"01234567"}};
const s_str g_c3_base_decimal = {{NULL}, 10, {"0123456789"}};
const s_str g_c3_base_hexadecimal = {{NULL}, 16, {"0123456789abcdef"}};
const s_str g_c3_bases_hexadecimal[2] = {{{NULL}, 16, {"0123456789abcdef"}},
                                         {{NULL}, 16, {"0123456789ABCDEF"}}};
sw          g_c3_exit_code = 1;

void c3_break (void)
{
  err_puts("break");
  assert(! "break");
  abort();
}

void c3_clean (s_env *env)
{
  if (! env)
    env = &g_c3_env;
  env_clean(env);
  sym_delete_all();
}

s_tag * c3_def (const s_call *call, s_tag *dest)
{
  return env_def(&g_c3_env, call, dest);
}

s_tag * c3_defmodule (const s_sym *name, const s_block *block, s_tag *dest)
{
  return env_defmodule(&g_c3_env, name, block, dest);
}

void ** c3_dlopen (const s_str *path, void **dest)
{
  assert(path);
  assert(dest);
  printf("dlopen %p := %s\n", (void *) dest, path->ptr.pchar);
  *dest = dlopen(path->ptr.pchar, RTLD_GLOBAL);
  return dest;
}

void c3_exit (sw code)
{
  exit((int) code);
}

uw * c3_facts_next_id (uw *dest)
{
  assert(dest);
  *dest = g_c3_env.facts.next_id;
  return dest;
}

s_tag * c3_quote_cfn (const s_sym *sym, s_tag *dest)
{
  return tag_init_sym(dest, sym);
}

s_str * c3_getenv (const s_str *name, s_str *dest)
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
s_tag * c3_if_then_else (const s_tag *cond, const s_tag *then,
                         const s_tag *else_, s_tag *dest)
{
  bool b;
  s_tag tmp;
  if (! env_eval_tag(&g_c3_env, cond, &tmp))
    return NULL;
  if (! bool_init_cast(&b, &tmp)) {
    tag_clean(&tmp);
    return NULL;
  }
  tag_clean(&tmp);
  if (b) {
    if (! env_eval_tag(&g_c3_env, then, dest))
      return NULL;
    return dest;
  }
  if (! env_eval_tag(&g_c3_env, else_, dest))
    return NULL;
  return dest;
}

s_env * c3_init (s_env *env, int argc, char **argv)
{
  if (! env)
    env = &g_c3_env;
  return env_init(env, argc, argv);
}

void c3_license (void)
{
  buf_write_1(&g_c3_env.out, g_c3_license);
  buf_flush(&g_c3_env.out);
}

s_tag * c3_pin (const s_tag *a, s_tag *dest)
{
  if (! env_eval_tag(&g_c3_env, a, dest))
    return NULL;
  return dest;
}
