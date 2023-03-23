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
/* Gen from set_item.h.in NAME=tag TYPE=s_tag */
/**
 * @file set_item__tag.h
 * @brief Element of a set of tags.
 *
 * Data structure for set items containing s_tag.
 */
#ifndef SET_ITEM__tag_H
#define SET_ITEM__tag_H

#include "types.h"

s_set_item__tag *
set_item_new__tag (const s_tag *data, uw hash, s_set_item__tag *next);

void
set_item_delete__tag (s_set_item__tag *x);

void
set_item_delete_all__tag (s_set_item__tag *x);

#endif /* SET_ITEM__tag_H */
