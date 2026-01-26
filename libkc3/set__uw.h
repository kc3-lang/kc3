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
/* Gen from set.h.in NAME=uw TYPE=uw */
#ifndef LIBKC3_SET__uw_H
#define LIBKC3_SET__uw_H

#include "types.h"

/* Stack-allocation compatible functions, call set_clean__uw
   after use. */

void
set_clean__uw (s_set__uw *set);

s_set__uw *
set_init__uw (s_set__uw *set, uw max);

/* Observers. */

s_set_item__uw *
set_get__uw (const s_set__uw *set, const uw data);

s_set_item__uw *
set_get_h__uw (const s_set__uw *set, const uw data, uw hash);

s_set_item__uw *
set_get_hash__uw (const s_set__uw *set, uw hash);

s_set_item__uw *
set_get_hash_next__uw (const s_set_item__uw *item);

bool *
set_has__uw (const s_set__uw *set,
                 const uw data,
                 bool *dest);

/* Operators. */

s_set_item__uw *
set_add__uw (s_set__uw *set, uw data);

s_set_item__uw *
set_add_collision__uw (s_set__uw *set, uw data, uw hash, s_set_item__uw *item);

s_set_item__uw *
set_add_h__uw (s_set__uw *set, uw data, uw hash);

bool
set_remove__uw (s_set__uw *set, const uw data);

void
set_remove_all__uw (s_set__uw *set);

bool
set_remove_item__uw (s_set__uw *set, s_set_item__uw *item);

s_set__uw *
set_resize__uw (s_set__uw *set, uw max);

#endif /* LIBKC3_SET__uw_H */
