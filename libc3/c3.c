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
#include "buf.h"
#include "c3_main.h"
#include "env.h"
#include "sym.h"

const s_str g_c3_base_binary = {{NULL}, 2, {"01"}};
const s_str g_c3_base_octal = {{NULL}, 8, {"01234567"}};
const s_str g_c3_base_decimal = {{NULL}, 10, {"0123456789"}};
const s_str g_c3_base_hexadecimal = {{NULL}, 16, {"0123456789abcdef"}};
const s_str g_c3_bases_hexadecimal[2] = {{{NULL}, 16, {"0123456789abcdef"}},
                                         {{NULL}, 16, {"0123456789ABCDEF"}}};
sw          g_c3_exit_code = 1;

void c3_break (void)
{
  assert(! "break");
  errx(1, "break");
  exit(1);
}

void c3_clean (s_env *env)
{
  if (! env)
    env = &g_c3_env;
  env_clean(env);
  sym_delete_all();
}

void c3_exit (sw code)
{
  exit((int) code);
}

uw * c3_facts_next_id (uw *dest)
{
  *dest = g_c3_env.facts.next_id;
  return dest;
}

s_env * c3_init (s_env *env, int argc, s8 **argv)
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
