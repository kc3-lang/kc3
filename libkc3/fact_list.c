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
#include "fact_list.h"

s_fact_list * fact_list_delete (s_fact_list *fl)
{
  s_fact_list *next;
  assert(fl);
  next = fl->next;
  alloc_free(fl);
  return next;
}

void fact_list_delete_all (s_fact_list *fl)
{
  s_fact_list *list;
  assert(fl);
  list = fl;
  while (list)
    list = fact_list_delete(list);
}

s_fact_list * fact_list_init (s_fact_list *fl, s_fact *fact,
                              s_fact_list *next)
{
  assert(fl);
  fl->fact = fact;
  fl->next = next;
  return fl;
}

s_fact_list * fact_list_new (s_fact *fact, s_fact_list *next)
{
  s_fact_list *fl;
  fl = alloc(sizeof(s_fact_list));
  if (! fl)
    return NULL;
  if (! fact_list_init(fl, fact, next)) {
    alloc_free(fl);
    return NULL;
  }
  return fl;
}
