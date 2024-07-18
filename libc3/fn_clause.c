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
#include "assert.h"
#include <stdlib.h>
#include <strings.h>
#include "arg.h"
#include "binding.h"
#include "block.h"
#include "fn_clause.h"
#include "list.h"

void fn_clause_clean (s_fn_clause *fn_clause)
{
  assert(fn_clause);
  list_delete_all(fn_clause->pattern);
  block_clean(&fn_clause->algo);
}

s_fn_clause * fn_clause_copy (const s_fn_clause *src, s_fn_clause **dest)
{
  s_fn_clause *tmp = NULL;
  s_fn_clause **tail = &tmp;
  while (src) {
    *tail = fn_clause_new(NULL);
    (*tail)->arity = src->arity;
    list_init_copy(&(*tail)->pattern, (const s_list **) &src->pattern);
    block_init_copy(&(*tail)->algo, &src->algo);
    src = src->next_clause;
    tail = &(*tail)->next_clause;
  }
  *dest = tmp;
  return tmp;
}

s_fn_clause * fn_clause_delete (s_fn_clause *fn_clause)
{
  s_fn_clause *next_clause;
  assert(fn_clause);
  next_clause = fn_clause->next_clause;
  fn_clause_clean(fn_clause);
  free(fn_clause);
  return next_clause;
}

void fn_clause_delete_all (s_fn_clause *fn_clause)
{
  while (fn_clause)
    fn_clause = fn_clause_delete(fn_clause);
}

s_fn_clause * fn_clause_init (s_fn_clause *fn_clause, s_fn_clause *next_clause)
{
  s_fn_clause tmp = {0};
  assert(fn_clause);
  tmp.next_clause = next_clause;
  *fn_clause = tmp;
  return fn_clause;
}

s_fn_clause * fn_clause_new (s_fn_clause *next_clause)
{
  s_fn_clause *fn_clause;
  fn_clause = calloc(1, sizeof(s_fn_clause));
  if (! fn_clause) {
    err_puts("fn_clause_new: failed to allocate memory");
    assert(! "fn_clause_new: failed to allocate memory");
    return NULL;
  }
  return fn_clause_init(fn_clause, next_clause);
}
