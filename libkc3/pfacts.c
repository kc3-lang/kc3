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
#include "assert.h"
#include "facts.h"
#include "pfacts.h"

void pfacts_clean (p_facts *pfacts)
{
  facts_delete(*pfacts);
}

p_facts * pfacts_init (p_facts *pfacts)
{
  p_facts tmp;
  if (! (tmp = facts_new()))
    return NULL;
  *pfacts = tmp;
  return pfacts;
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
