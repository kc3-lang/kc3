/* kc3
 * Copyright from 2022 to 2026 kmx.io <contact@kmx.io>
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
#include "alloc.h"
#include "assert.h"
#include "arg.h"
#include "binding.h"
#include "do_block.h"
#include "fn_clause.h"
#include "list.h"

void fn_clause_clean (s_fn_clause *fn_clause)
{
  assert(fn_clause);
  list_delete_all(fn_clause->pattern);
  do_block_clean(&fn_clause->algo);
}

s_fn_clause * fn_clause_delete (s_fn_clause *fn_clause)
{
  s_fn_clause *next;
  assert(fn_clause);
  next = fn_clause->next;
  fn_clause_clean(fn_clause);
  free(fn_clause);
  return next;
}

void fn_clause_delete_all (s_fn_clause *fn_clause)
{
  while (fn_clause)
    fn_clause = fn_clause_delete(fn_clause);
}

s_fn_clause * fn_clause_init (s_fn_clause *fn_clause,
                              s_fn_clause *next)
{
  s_fn_clause tmp = {0};
  assert(fn_clause);
  tmp.next = next;
  *fn_clause = tmp;
  return fn_clause;
}

s_fn_clause * fn_clause_new (s_fn_clause *next)
{
  s_fn_clause *fn_clause;
  fn_clause = alloc(sizeof(s_fn_clause));
  if (! fn_clause)
    return NULL;
  return fn_clause_init(fn_clause, next);
}

s_fn_clause * fn_clause_new_copy (s_fn_clause *src)
{
  s_fn_clause *tmp = NULL;
  s_fn_clause **tail = NULL;
  tail = &tmp;
  while (src) {
    *tail = fn_clause_new(NULL);
    (*tail)->arity = src->arity;
    (*tail)->pattern = list_new_copy_all(src->pattern);
    do_block_init_copy(&(*tail)->algo, &src->algo);
    tail = &(*tail)->next;
    src = src->next;
  }
  return tmp;
}
