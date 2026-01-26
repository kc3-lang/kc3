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
#include "unquote.h"
#include "tag.h"

void unquote_clean (s_unquote *unquote)
{
  assert(unquote);
  tag_delete(unquote->tag);
}

s_unquote * unquote_init (s_unquote *unquote, s_tag *tag)
{
  unquote->tag = tag_new_copy(tag);
  return unquote;
}

s_unquote * unquote_init_cast (s_unquote *unquote, s_tag *tag)
{
  switch (tag->type) {
  case TAG_UNQUOTE:
    return unquote_init_copy(unquote, &tag->data.unquote);
  default:
    break;
  }
  err_write_1("unquote_init_cast: cannot cast ");
  err_write_1(tag_type_to_string(tag->type));
  err_puts(" to Unquote");
  assert(! "unquote_init_cast: cannot cast to Unquote");
  return NULL;
}

s_unquote * unquote_init_copy (s_unquote *unquote, s_unquote *src)
{
  unquote->tag = tag_new_copy(src->tag);
  return unquote;
}
