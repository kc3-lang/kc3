/* c3
 * Copyright 2022 kmx.io <contact@kmx.io>
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
#ifndef FACT_H
#define FACT_H

#include "hash.h"
#include "types.h"

/* Stack-allocation compatible functions */
#define  fact_clean(fact) do {} while(0)
s_fact * fact_init (s_fact *fact, const s_tag *subject,
                    const s_tag *predicate, const s_tag *object);

/* Observers */
s8               fact_compare (const s_fact *a, const s_fact *b);
s8               fact_compare_pos (const s_fact *a, const s_fact *b);
s8               fact_compare_osp (const s_fact *a, const s_fact *b);
s8               fact_compare_unbound_var_count (const s_fact *a,
                                                 const s_fact *b);
s_fact *         fact_copy (const s_fact *src, s_fact *dest);
t_hash_context * fact_hash_update (t_hash_context *context,
                                   const s_fact *fact);
uw               fact_hash (const s_fact *x);
s_str *          fact_inspect (const s_fact *fact, s_str *dest);

#endif /* FACT_H */
