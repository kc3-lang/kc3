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
#include "ptag.h"
#include "sym.h"
#include "tag.h"
#include "tag_type.h"

void ptag_clean (p_tag *ptag)
{
  tag_delete(*ptag);
}

p_tag * ptag_init_cast (p_tag *ptag, const s_sym * const *type,
                        const s_tag *tag)
{
  assert(ptag);
  assert(type);
  assert(tag);
  switch (tag->type) {
  case TAG_PTAG:
    return ptag_init_copy(ptag, &tag->data.ptag);
  default:
    break;
  }
  err_write_1("ptag_init_cast: cannot cast ");
  err_write_1(tag_type_to_string(tag->type));
  if (*type == &g_sym_Ptag)
    err_puts(" to Ptag");
  else {
    err_write_1(" to ");
    err_inspect_psym(type);
    err_puts(" aka Ptag");
  }
  assert(! "ptag_init_cast: cannot cast to Ptag");
  return NULL;
}

p_tag * ptag_init_copy (p_tag *dest, const p_tag *src)
{
  assert(src);
  assert(dest);
  *dest = *src;
  return dest;
}
