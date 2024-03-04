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
/* Gen from set.h.in NAME=tag TYPE=s_tag */
#ifndef LIBC3_SET__tag_H
#define LIBC3_SET__tag_H

#include "types.h"

s_set_item__tag *
set_add__tag (s_set__tag *set, const s_tag *data);

s_set_item__tag *
set_add_collision__tag (s_set__tag *set, const s_tag *data, uw hash, s_set_item__tag *item);

s_set_item__tag *
set_add_h__tag (s_set__tag *set, const s_tag *data, uw hash);

void
set_clean__tag (s_set__tag *set);

s_set_item__tag *
set_get__tag (const s_set__tag *set, const s_tag *data);

s_set_item__tag *
set_get_h__tag (const s_set__tag *set, const s_tag *data, uw hash);

s_set_item__tag *
set_get_hash__tag (const s_set__tag *set, uw hash);

s_set_item__tag *
set_get_hash_next__tag (const s_set_item__tag *item);

s_set__tag *
set_init__tag (s_set__tag *set, uw max);

bool
set_remove__tag (s_set__tag *set, const s_tag *data);

bool
set_remove_item__tag (s_set__tag *set, s_set_item__tag *item);

s_set__tag *
set_resize__tag (s_set__tag *set, uw max);

#endif /* LIBC3_SET__tag_H */
