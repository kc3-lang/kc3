/* c3
 * Copyright 2022 kmx.io <contact@kmx.io>
 *
 * Permission is hereby granted to use this software granted
 * the above copyright notice and this permission paragraph
 * are included in all copies and substantial portions of this
 * software.
 *
 * THIS SOFTWARE IS PROVIDED "AS-IS" WITHOUT ANY GUARANTEE OF
 * PURPOSE AND PERFORMANCE. IN NO EVENT WHATSOEVER SHALL THE
 * AUTHOR BE CONSIDERED LIABLE FOR THE USE AND PERFORMANCE OF
 * THIS SOFTWARE.
 */
#ifndef QUOTE_H
#define QUOTE_H

#include "hash.h"
#include "types.h"

void             quote_clean (p_quote quote);
s8               quote_compare (const p_quote a, const p_quote b);
p_quote          quote_copy (const p_quote src, p_quote *dest);
t_hash_context * quote_hash_update (t_hash_context *context,
                                    const p_quote x);

#endif /* QUOTE_H */
