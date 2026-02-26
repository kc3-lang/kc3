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
/* Gen from skiplist_node.c.in NAME=fact TYPE=s_fact * */
#include <string.h>
#include "alloc.h"
#include "skiplist_node__fact.h"

s_skiplist_node__fact *
skiplist_node_init
(s_skiplist_node__fact *node,
 s_fact * fact,
 u8 height)
{
  node->fact = fact;
  node->height = height;
  memset(SKIPLIST_NODE_LINKS__fact(node), 0,
         height * sizeof(s_skiplist_node__fact *));
  return node;
}

s_skiplist_node__fact *
skiplist_node_new__fact
(s_fact * fact,
 u8 height)
{
  s_skiplist_node__fact *node;
  node = alloc(SKIPLIST_NODE_SIZE__fact(height));
  if (! node)
    return NULL;
  skiplist_node_init(node, fact, height);
  return node;
}

void
skiplist_node_delete__fact
(s_skiplist_node__fact *node)
{
  alloc_free(node);
}

void
skiplist_node_insert__fact
(s_skiplist_node__fact *node,
 s_skiplist_node__fact *pred)
{
  uw level;
  for (level = 0; level < node->height; level++) {
    s_skiplist_node__fact *p = SKIPLIST_NODE_NEXT__fact(pred, level);
    SKIPLIST_NODE_NEXT__fact(node, level) =
      SKIPLIST_NODE_NEXT__fact(p, level);
    SKIPLIST_NODE_NEXT__fact(p, level) = node;
  }
}
