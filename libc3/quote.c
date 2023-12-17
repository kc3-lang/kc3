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
#include "quote.h"
#include "tag.h"

void quote_clean (s_quote *quote)
{
  assert(quote);
  tag_delete(quote->tag);
}

s_quote * quote_init (s_quote *quote, const s_tag *tag)
{
  quote->tag = tag_new_copy(tag);
  return quote;
}

s_quote * quote_init_cast (s_quote *quote, const s_tag *tag)
{
  switch (tag->type) {
  case TAG_QUOTE:
    return quote_init_copy(quote, &tag->data.quote);
  default:
    break;
  }
  err_write_1("quote_init_cast: cannot cast ");
  err_write_1(tag_type_to_string(tag->type));
  err_puts(" to Quote");
  assert(! "quote_init_cast: cannot cast to Quote");
  return NULL;
}

s_quote * quote_init_copy (s_quote *quote, const s_quote *src)
{
  quote->tag = tag_new_copy(src->tag);
  return quote;
}
