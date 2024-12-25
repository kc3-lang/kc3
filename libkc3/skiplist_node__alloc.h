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
/* Gen from skiplist_node.h.in NAME=alloc TYPE=s_alloc * */
/**
 * @file skiplist_node__alloc.h
 * @brief Element of a skiplist of allocs.
 *
 * Data structure for skiplist nodes containing s_alloc *.
 */
#ifndef LIBKC3_SKIPLIST_NODE__alloc_H
#define LIBKC3_SKIPLIST_NODE__alloc_H

#include "types.h"

#define SKIPLIST_NODE_LINKS__alloc(node) \
  ((s_skiplist_node__alloc **) (((s_skiplist_node__alloc *) node) + 1))

#define SKIPLIST_NODE_NEXT__alloc(node, height) \
  (SKIPLIST_NODE_LINKS__alloc(node)[height])

#define SKIPLIST_NODE_SIZE__alloc(height) \
  (sizeof(s_skiplist_node__alloc) + (height) * sizeof(s_alloc *))

s_skiplist_node__alloc *
skiplist_node_init (s_skiplist_node__alloc *node, s_alloc * value, u8 height);

s_skiplist_node__alloc *
skiplist_node_new__alloc (s_alloc * value, u8 height);

void
skiplist_node_delete__alloc (s_skiplist_node__alloc *node);

void
skiplist_node_insert__alloc (s_skiplist_node__alloc *node, s_skiplist_node__alloc *pred);

#endif /* LIBKC3_SKIPLIST_NODE__alloc_H */
