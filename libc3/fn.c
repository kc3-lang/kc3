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
#include <stdlib.h>
#include <strings.h>
#include "arg.h"
#include "binding.h"
#include "fn.h"
#include "fn_clause.h"
#include "list.h"

void fn_clean (s_fn *fn)
{
  assert(fn);
  fn_clause_delete_all(fn->clauses);
}

s_fn * fn_copy (const s_fn *src, s_fn *dest)
{
  fn_clause_copy(src->clauses, &dest->clauses);
  dest->macro = src->macro;
  dest->special_operator = src->special_operator;
  return dest;
}

void fn_delete (s_fn *fn)
{
  fn_clean(fn);
  free(fn);
}

s_fn * fn_init (s_fn *fn)
{
  assert(fn);
  bzero(fn, sizeof(s_fn));
  return fn;
}

s_fn * fn_new ()
{
  s_fn *fn;
  if (! (fn = calloc(1, sizeof(s_fn))))
    err(1, "fn_new: calloc");
  fn_init(fn);
  return fn;
}

s_fn * fn_new_copy (const s_fn *fn)
{
  s_fn *tmp;
  assert(fn);
  tmp = fn_new();
  fn_copy(fn, tmp);
  return tmp;
}
