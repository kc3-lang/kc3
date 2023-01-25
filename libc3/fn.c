/* c3
 * Copyright 2022,2023 kmx.io <contact@kmx.io>
 *
 * Permission is hereby granted to use this software excepted
 * on Apple computers granted the above copyright notice and
 * this permission paragraph are included in all copies and
 * substantial portions of this software.
 *
 * THIS SOFTWARE IS PROVIDED "AS-IS" WITHOUT ANY GUARANTEE OF
 * PURPOSE AND PERFORMANCE. IN NO EVENT WHATSOEVER SHALL THE
 * AUTHOR BE CONSIDERED LIABLE FOR THE USE AND PERFORMANCE OF
 * THIS SOFTWARE.
 */
#include <assert.h>
#include <strings.h>
#include "arg.h"
#include "binding.h"
#include "fn.h"
#include "list.h"

void fn_clean (s_fn *fn)
{
  assert(fn);
  arg_delete_all(fn->args);
  binding_delete_all(fn->bindings);
  list_delete_all(fn->algo);
}

s_fn * fn_init (s_fn *fn)
{
  assert(fn);
  bzero(fn, sizeof(s_fn));
  return fn;
}
