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
/* Gen from set_item.h.in NAME=uw TYPE=uw */
/**
 * @file set_item__uw.h
 * @brief Element of a set of uws.
 *
 * Data structure for set items containing uw.
 */
#ifndef LIBKC3_SET_ITEM__uw_H
#define LIBKC3_SET_ITEM__uw_H

#include "types.h"

s_set_item__uw *
set_item_new__uw (uw data, uw hash, s_set_item__uw *next);

void
set_item_delete__uw (s_set_item__uw *x);

void
set_item_delete_all__uw (s_set_item__uw *x);

#endif /* LIBKC3_SET_ITEM__uw_H */
