/* kc3
 * Copyright 2022,2023,2024 kmx.io <contact@kmx.io>
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
/* Gen from skiplist.h.in NAME=alloc TYPE=s_alloc * */
/**
 * @file skiplist__alloc.h
 * @brief Skiplist of allocs.
 *
 * Skiplist containing s_alloc *.
 */
#ifndef LIBKC3_SKIPLIST__alloc_H
#define LIBKC3_SKIPLIST__alloc_H

#include "types.h"

#define SKIPLIST_HEIGHT_TABLE__alloc(skiplist) \
  ((t_skiplist_height *) (((s_skiplist__alloc *) skiplist) + 1))

#define SKIPLIST_SIZE__alloc(max_height) \
  (sizeof(s_skiplist__alloc) + (max_height) * sizeof(t_skiplist_height))

void
skiplist_clean__alloc (s_skiplist__alloc *skiplist);

void
skiplist_delete__alloc (s_skiplist__alloc *skiplist);

s_skiplist_node__alloc *
skiplist_find__alloc (s_skiplist__alloc *skiplist, const s_alloc * value);

/* do not call directly */
s_skiplist__alloc *
skiplist_init__alloc (s_skiplist__alloc *skiplist, u8 max_height, f64 spacing);

s_skiplist_node__alloc *
skiplist_insert__alloc (s_skiplist__alloc *skiplist, s_alloc * value);

s_skiplist__alloc *
skiplist_new__alloc (u8 max_height, f64 spacing);

s_skiplist_node__alloc *
skiplist_pred__alloc (s_skiplist__alloc *skiplist, const s_alloc * value);

u8
skiplist_random_height__alloc (s_skiplist__alloc *skiplist);

bool
skiplist_remove__alloc (s_skiplist__alloc *skiplist, const s_alloc * value);

#endif /* LIBKC3_SKIPLIST__alloc_H */
