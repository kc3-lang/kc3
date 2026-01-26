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
#include "assert.h"
#include "facts.h"
#include "pfacts.h"

void pfacts_clean (p_facts *pfacts)
{
  assert(pfacts);
  if (*pfacts) {
    facts_delete(*pfacts);
    *pfacts = NULL;
  }
}

p_facts * pfacts_init (p_facts *pfacts)
{
  p_facts tmp;
  assert(pfacts);
  if (! (tmp = facts_new()))
    return NULL;
  assert(tmp->ref_count == 1);
  *pfacts = tmp;
  return pfacts;
}

p_facts * pfacts_init_cast (p_facts *pfacts, const p_sym *type,
                            s_tag *src)
{
  assert(pfacts);
  assert(src);
  switch (src->type) {
  case TAG_PFACTS:
    return pfacts_init_copy(pfacts, &src->data.pfacts);
  default:
    err_write_1("pfacts_init_cast: cannot cast to ");
    err_inspect_sym(*type);
    err_write_1("\n");
    assert(! "pfacts_init_cast: cannot cast");
    return NULL;
  }
  err_puts("pfacts_init_cast: unknown tag type");
  assert(! "pfacts_init_cast: unknown tag type");
  return NULL;
}

p_facts * pfacts_init_copy (p_facts *pfacts, p_facts *src)
{
  p_facts tmp;
  assert(pfacts);
  assert(src);
  assert(*src);
  if (! (tmp = facts_new_ref(*src)))
    return NULL;
  *pfacts = tmp;
  return pfacts;
}
