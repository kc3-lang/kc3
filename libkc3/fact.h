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
#ifndef LIBKC3_FACT_H
#define LIBKC3_FACT_H

#include "types.h"

/* Stack-allocation compatible functions */
#define    fact_clean(fact) do {} while(0)
void       fact_clean_all (s_fact *fact);
s_fact *   fact_init (s_fact *fact, s_tag *subject,
                      s_tag *predicate, s_tag *object);
s_fact *   fact_init_cast (s_fact *fact, const s_sym * const *type,
                           s_tag *tag);
s_fact *   fact_init_copy (s_fact *fact, const s_fact *src);
s_fact_w * fact_w_init (s_fact_w *fact);
s_fact_w * fact_w_init_fact (s_fact_w *fact, s_fact *src);
void       fact_w_clean (s_fact_w *fact);

/* Observers */
uw *     fact_hash_uw (const s_fact *fact, uw *dest);

/* Operators */
void       fact_r (s_fact_w *fact, s_fact *dest);
s_fact_w * fact_w_eval (s_fact_w *fact, s_fact_w *dest);

#endif /* LIBKC3_FACT_H */
