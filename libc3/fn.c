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
#include <err.h>
#include <stdlib.h>
#include <strings.h>
#include "arg.h"
#include "binding.h"
#include "fn.h"
#include "list.h"

void fn_clean (s_fn *fn)
{
  assert(fn);
  list_delete_all(fn->pattern);
  list_delete_all(fn->algo);
}

s_fn * fn_copy (const s_fn *src, s_fn **dest)
{
  s_fn *tmp = NULL;
  s_fn **tail = &tmp;
  while (src) {
    *tail = fn_new(NULL);
    (*tail)->arity = src->arity;
    list_copy(src->pattern, &(*tail)->pattern);
    list_copy(src->algo, &(*tail)->algo);
    src = src->next_clause;
    tail = &(*tail)->next_clause;
  }
  *dest = tmp;
  return tmp;
}

s_fn * fn_delete (s_fn *fn)
{
  s_fn *next_clause;
  assert(fn);
  next_clause = fn->next_clause;
  fn_clean(fn);
  free(fn);
  return next_clause;
}

void fn_delete_all (s_fn *fn)
{
  while (fn)
    fn = fn_delete(fn);
}

s_fn * fn_init (s_fn *fn, s_fn *next_clause)
{
  assert(fn);
  bzero(fn, sizeof(s_fn));
  fn->next_clause = next_clause;
  return fn;
}

s_fn * fn_new (s_fn *next_clause)
{
  s_fn *fn;
  if (! (fn = malloc(sizeof(s_fn))))
    err(1, "out of memory");
  fn_init(fn, next_clause);
  return fn;
}
