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
#ifndef LIBC3_QUOTE_H
#define LIBC3_QUOTE_H

#include "types.h"

/* Stack-allocation compatible functions, call quote_clean after use. */
void      quote_clean (s_quote *quote);
s_quote * quote_init (s_quote *quote, const s_tag *tag);
s_quote * quote_init_1 (s_quote *quote, const s8 *p);
s_quote * quote_init_cast (s_quote *quote, const s_tag *src);
s_quote * quote_init_copy (s_quote *quote, const s_quote *src);

#endif /* LIBC3_QUOTE_H */
