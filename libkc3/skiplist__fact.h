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
/* Gen from skiplist.h.in NAME=fact TYPE=s_fact * */
/**
 * @file skiplist__fact.h
 * @brief Skiplist of facts.
 *
 * Skiplist containing s_fact *.
 */
#ifndef LIBKC3_SKIPLIST__fact_H
#define LIBKC3_SKIPLIST__fact_H

#include "types.h"

#define SKIPLIST_HEIGHT_TABLE__fact(skiplist) \
  ((t_skiplist_height *) (((s_skiplist__fact *) skiplist) + 1))

#define SKIPLIST_SIZE__fact(max_height) \
  (sizeof(s_skiplist__fact) + (max_height) * sizeof(t_skiplist_height))

void
skiplist_clean__fact
(s_skiplist__fact *skiplist);

void
skiplist_delete__fact
(s_skiplist__fact *skiplist);

s_skiplist_node__fact *
skiplist_find__fact
(s_skiplist__fact *skiplist,
 const s_fact * value);

/* do not call directly */
s_skiplist__fact *
skiplist_init__fact
(s_skiplist__fact *skiplist,
 u8 max_height,
 f64 spacing);

s_skiplist_node__fact *
skiplist_insert__fact
(s_skiplist__fact *skiplist,
 s_fact * value);

s_skiplist__fact *
skiplist_new__fact
(u8 max_height,
 f64 spacing);

s_skiplist_node__fact *
skiplist_pred__fact
(s_skiplist__fact *skiplist,
 const s_fact * value);

u8
skiplist_random_height__fact (s_skiplist__fact *skiplist);

bool
skiplist_remove__fact
(s_skiplist__fact *skiplist,
 const s_fact * value);

void
skiplist_remove_all__fact
(s_skiplist__fact *skiplist);

#endif /* LIBKC3_SKIPLIST__fact_H */
