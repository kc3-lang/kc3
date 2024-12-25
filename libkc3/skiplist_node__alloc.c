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
/* Gen from skiplist_node.c.in NAME=alloc TYPE=s_alloc * */
#include <string.h>
#include "alloc.h"
#include "skiplist_node__alloc.h"

s_skiplist_node__alloc *
skiplist_node_init (s_skiplist_node__alloc *node, s_alloc * alloc, u8 height)
{
  node->alloc = alloc;
  node->height = height;
  memset(SKIPLIST_NODE_LINKS__alloc(node), 0,
         height * sizeof(s_skiplist_node__alloc *));
  return node;
}

s_skiplist_node__alloc *
skiplist_node_new__alloc (s_alloc * alloc, u8 height)
{
  s_skiplist_node__alloc *node;
  node = alloc(SKIPLIST_NODE_SIZE__alloc(height));
  if (! node)
    return NULL;
  skiplist_node_init(node, alloc, height);
  return node;
}

void
skiplist_node_delete__alloc (s_skiplist_node__alloc *node)
{
  free(node);
}

void
skiplist_node_insert__alloc (s_skiplist_node__alloc *node, s_skiplist_node__alloc *pred)
{
  uw level;
  for (level = 0; level < node->height; level++) {
    s_skiplist_node__alloc *p = SKIPLIST_NODE_NEXT__alloc(pred, level);
    SKIPLIST_NODE_NEXT__alloc(node, level) =
      SKIPLIST_NODE_NEXT__alloc(p, level);
    SKIPLIST_NODE_NEXT__alloc(p, level) = node;
  }
}
