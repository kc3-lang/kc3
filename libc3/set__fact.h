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
/* Gen from set.h.in NAME=fact TYPE=s_fact */
#ifndef SET__fact_H
#define SET__fact_H

#include "types.h"

s_set_item__fact *
set_add__fact (s_set__fact *set, const s_fact *data);

s_set_item__fact *
set_add_collision__fact (s_set__fact *set, const s_fact *data, uw hash, s_set_item__fact *item);

s_set_item__fact *
set_add_h__fact (s_set__fact *set, const s_fact *data, uw hash);

void
set_clean__fact (s_set__fact *set);

s_set_item__fact *
set_get__fact (const s_set__fact *set, const s_fact *data);

s_set_item__fact *
set_get_h__fact (const s_set__fact *set, const s_fact *data, uw hash);

s_set_item__fact *
set_get_hash__fact (const s_set__fact *set, uw hash);

s_set_item__fact *
set_get_hash_next__fact (const s_set_item__fact *item);

s_set__fact *
set_init__fact (s_set__fact *set, uw max);

e_bool
set_remove__fact (s_set__fact *set, const s_fact *data);

e_bool
set_remove_item__fact (s_set__fact *set, s_set_item__fact *item);

s_set__fact *
set_resize__fact (s_set__fact *set, uw max);

#endif /* SET__fact_H */
