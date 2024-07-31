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
#include <libkc3/kc3.h>
#include "config.h"

int main (int argc, char **argv)
{
  s_call call = {0};
  const s_sym *module = NULL;
  int r = 1;
  s_tag tmp = {0};
  kc3_init(NULL, &argc, &argv);
  io_puts("KC3 HTTPd loading, please wait...");
  module = sym_1("HTTPd");
  if (! module_load(module)) {
    kc3_clean(NULL);
    return 1;
  }
  call_init(&call);
  call.ident.module = module;
  call.ident.sym = sym_1("main");
  if (argc >= 2)
    call.arguments = list_new_str_1
      (NULL, argv[0], list_new_str_1
       (NULL, argv[1], NULL));
  else if (argc == 1)
    call.arguments = list_new_str_1(NULL, argv[0], NULL);
  else
    call.arguments = NULL;
  if (! eval_call(&call, &tmp))
    goto clean;
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
