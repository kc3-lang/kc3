/* kc3
 * Copyright from 2022 to 2025 kmx.io <contact@kmx.io>
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
#include "../libkc3/kc3.h"

const char *g_env_argv0_default = PROG;
const char *g_env_argv0_dir_default = PREFIX;

int main (int argc, char **argv)
{
  bool b;
  s_call call = {0};
  s_env *env;
  const s_sym *module = NULL;
  sw r = 1;
  s_tag tmp = {0};
  kc3_init(NULL, &argc, &argv);
  env = env_global();
  assert(env);
  module = sym_1("Kpkg");
  call_init(&call);
  call.ident.module = module;
  call.ident.sym = sym_1("main");
  if (! env_module_has_ident(env, module, &call.ident, &b)) {
    err_puts("kpkg: env_module_has_ident");
    goto clean;
  }
  if (! b &&
      ! env_module_load(env, module)) {
    err_puts("kpkg: env_module_load");
    goto clean;
  }
  if (! env_eval_call(env, &call, &tmp)) {
    err_puts("kpkg: eval_call");
    goto clean;
  }
  switch (tmp.type) {
  case TAG_U8:
    r = tmp.data.u8;
    break;
  default:
    err_write_1("invalid return type from main: ");
    err_inspect_tag(&tmp);
    err_write_1("\n");
  }
 clean:
  tag_clean(&tmp);
  call_clean(&call);
  kc3_clean(NULL);
  return r;
}
