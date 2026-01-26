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
/* Gen from set.h.in NAME=fact TYPE=s_fact * */
#ifndef LIBKC3_SET__fact_H
#define LIBKC3_SET__fact_H

#include "types.h"

/* Stack-allocation compatible functions, call set_clean__fact
   after use. */

void
set_clean__fact (s_set__fact *set);

s_set__fact *
set_init__fact (s_set__fact *set, uw max);

/* Observers. */

s_set_item__fact *
set_get__fact (const s_set__fact *set, const s_fact * data);

s_set_item__fact *
set_get_h__fact (const s_set__fact *set, const s_fact * data, uw hash);

s_set_item__fact *
set_get_hash__fact (const s_set__fact *set, uw hash);

s_set_item__fact *
set_get_hash_next__fact (const s_set_item__fact *item);

bool *
set_has__fact (const s_set__fact *set,
                 const s_fact * data,
                 bool *dest);

/* Operators. */

s_set_item__fact *
set_add__fact (s_set__fact *set, s_fact * data);

s_set_item__fact *
set_add_collision__fact (s_set__fact *set, s_fact * data, uw hash, s_set_item__fact *item);

s_set_item__fact *
set_add_h__fact (s_set__fact *set, s_fact * data, uw hash);

bool
set_remove__fact (s_set__fact *set, const s_fact * data);

void
set_remove_all__fact (s_set__fact *set);

bool
set_remove_item__fact (s_set__fact *set, s_set_item__fact *item);

s_set__fact *
set_resize__fact (s_set__fact *set, uw max);

#endif /* LIBKC3_SET__fact_H */
