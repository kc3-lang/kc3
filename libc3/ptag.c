/* c3
 * Copyright 2022-2024 kmx.io <contact@kmx.io>
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
#include "tag_type.h"

p_tag * ptag_init_cast (p_tag *ptag, const s_tag *tag)
{
  switch (tag->type) {
  case TAG_PTAG:
    return ptag_init_copy(ptag, &tag->data.ptag);
  default:
    break;
  }
  err_write_1("ptag_init_cast: cannot cast ");
  err_write_1(tag_type_to_string(tag->type));
  err_puts(" to Ptag");
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
