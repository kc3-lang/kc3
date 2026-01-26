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
/* Gen from set_item.h.in NAME=fact TYPE=s_fact * */
/**
 * @file set_item__fact.h
 * @brief Element of a set of facts.
 *
 * Data structure for set items containing s_fact *.
 */
#ifndef LIBKC3_SET_ITEM__fact_H
#define LIBKC3_SET_ITEM__fact_H

#include "types.h"

s_set_item__fact *
set_item_new__fact (s_fact * data, uw hash, s_set_item__fact *next);

void
set_item_delete__fact (s_set_item__fact *x);

void
set_item_delete_all__fact (s_set_item__fact *x);

#endif /* LIBKC3_SET_ITEM__fact_H */
