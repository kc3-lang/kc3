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
#ifndef LIBC3_FACT_H
#define LIBC3_FACT_H

#include "types.h"

/* Stack-allocation compatible functions */
#define  fact_clean(fact) do {} while(0)
s_fact * fact_init (s_fact *fact, const s_tag *subject,
                    const s_tag *predicate, const s_tag *object);
s_fact * fact_init_cast (s_fact *fact, const s_sym *type,
                         const s_tag *tag);
s_fact * fact_init_copy (s_fact *fact, const s_fact *src);

/* Observers */
uw       fact_hash_uw (const s_fact *x);
s_str *  fact_inspect (const s_fact *fact, s_str *dest);
s_fact * fact_r (const s_fact_w *fact);

/* Modifiers */
void fact_w_clean (s_fact_w *fact);

#endif /* LIBC3_FACT_H */
