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
#include "quote.h"
#include "sym.h"
#include "tag.h"

void quote_clean (s_quote *quote)
{
  assert(quote);
  tag_delete(quote->tag);
}

s_quote * quote_init (s_quote *quote, s_tag *tag)
{
  quote->tag = tag_new_copy(tag);
  return quote;
}

s_quote * quote_init_cast (s_quote *quote, const s_sym * const *type,
                           s_tag *tag)
{
  assert(quote);
  assert(type);
  assert(tag);
  switch (tag->type) {
  case TAG_QUOTE:
    return quote_init_copy(quote, &tag->data.td_quote);
  default:
    break;
  }
  err_write_1("quote_init_cast: cannot cast ");
  err_write_1(tag_type_to_string(tag->type));
  if (*type == &g_sym_Quote)
    err_puts(" to Quote");
  else {
    err_write_1(" to ");
    err_inspect_psym(type);
    err_puts(" aka Quote");
  }
  assert(! "quote_init_cast: cannot cast to Quote");
  return NULL;
}

s_quote * quote_init_copy (s_quote *quote, s_quote *src)
{
  quote->tag = tag_new_copy(src->tag);
  return quote;
}
