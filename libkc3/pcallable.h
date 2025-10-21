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
#ifndef LIBKC3_PCALLABLE_H
#define LIBKC3_PCALLABLE_H

#include "types.h"

/* Heap pointer manipulation functions. Call pcallable_clean
   after use. */
void         pcallable_clean (p_callable *callable);
p_callable * pcallable_init (p_callable *callable);
p_callable * pcallable_init_cast (p_callable *callable,
                                  const s_sym * const *type,
                                  const s_tag *tag);
p_callable * pcallable_init_copy (p_callable *callable,
                                  p_callable *src);

#endif /* LIBKC3_PCALLABLE_H */
