/* c3
 * Copyright 2022,2023 kmx.io <contact@kmx.io>
 *
 * Permission is hereby granted to use this software excepted
 * on Apple computers granted the above copyright notice and
 * this permission paragraph are included in all copies and
 * substantial portions of this software.
 *
 * THIS SOFTWARE IS PROVIDED "AS-IS" WITHOUT ANY GUARANTEE OF
 * PURPOSE AND PERFORMANCE. IN NO EVENT WHATSOEVER SHALL THE
 * AUTHOR BE CONSIDERED LIABLE FOR THE USE AND PERFORMANCE OF
 * THIS SOFTWARE.
 */
#include <assert.h>
#include <stdlib.h>
#include "quote.h"
#include "tag.h"

void quote_clean (p_quote quote)
{
  assert(quote);
  free(quote);
}

p_quote quote_copy (const p_quote src, p_quote *dest)
{
  return *dest = tag_new_copy(src);
}

p_quote quote_init (p_quote *quote, const s_tag *tag)
{
  return *quote = tag_new_copy(tag);
}
