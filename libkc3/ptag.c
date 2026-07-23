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
#include "tag.h"
#include "ptag.h"

void ptag_clean (p_tag *ptag)
{
  assert(ptag);
  if (*ptag) {
    tag_delete(*ptag);
    *ptag = NULL;
  }
}

p_tag * ptag_init (p_tag *ptag)
{
  p_tag tmp;
  assert(ptag);
  if (! (tmp = tag_new()))
    return NULL;
  assert(tmp->ref_count == 1);
  *ptag = tmp;
  return ptag;
}

p_tag * ptag_init_cast (p_tag *ptag, const p_sym *type,
                        s_tag *src)
{
  assert(ptag);
  assert(src);
  (void) type;
  return ptag_init_copy(ptag, &src);
}

p_tag * ptag_init_copy (p_tag *ptag, p_tag *src)
{
  p_tag tmp;
  assert(ptag);
  assert(src);
  if (*src) {
    if (! (tmp = tag_new_ref(*src)))
      return NULL;
    *ptag = tmp;
  }
  else
    *ptag = NULL;
  return ptag;
}
