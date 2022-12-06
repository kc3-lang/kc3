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
/* Gen from skiplist_node.h.in NAME=fact TYPE=s_fact * */
/**
 * @file skiplist_node__fact.h
 * @brief Element of a skiplist of facts.
 *
 * Data structure for skiplist nodes containing s_fact *.
 */
#ifndef SKIPLIST_NODE__fact_H
#define SKIPLIST_NODE__fact_H

#include "types.h"

#define SKIPLIST_NODE_LINKS__fact(node) \
  ((s_skiplist_node__fact **) (((s_skiplist_node__fact *) node) + 1))

#define SKIPLIST_NODE_NEXT__fact(node, height) \
  (SKIPLIST_NODE_LINKS__fact(node)[height])

#define SKIPLIST_NODE_SIZE__fact(height) \
  (sizeof(s_skiplist_node__fact) + (height) * sizeof(s_fact *))

s_skiplist_node__fact *
skiplist_node_init (s_skiplist_node__fact *node, s_fact * value, unsigned long height);

s_skiplist_node__fact *
skiplist_node_new__fact (s_fact * value, unsigned long height);

void
skiplist_node_delete__fact (s_skiplist_node__fact *node);

void
skiplist_node_insert__fact (s_skiplist_node__fact *node, s_skiplist_node__fact *pred);

#endif /* SKIPLIST_NODE__fact_H */
