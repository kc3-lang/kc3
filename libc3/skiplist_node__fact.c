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
/* Gen from skiplist_node.c.in NAME=fact TYPE=s_fact * */
#include <stdlib.h>
#include <strings.h>
#include "skiplist_node__fact.h"

s_skiplist_node__fact *
skiplist_node_init (s_skiplist_node__fact *node, s_fact * fact, uw height)
{
  node->fact = fact;
  node->height = height;
  bzero(SKIPLIST_NODE_LINKS__fact(node),
        height * sizeof(s_skiplist_node__fact *));
  return node;
}

s_skiplist_node__fact *
skiplist_node_new__fact (s_fact * fact, uw height)
{
  s_skiplist_node__fact *node = malloc(SKIPLIST_NODE_SIZE__fact(height));
  if (node)
    skiplist_node_init(node, fact, height);
  return node;
}

void
skiplist_node_delete__fact (s_skiplist_node__fact *node)
{
  free(node);
}

void
skiplist_node_insert__fact (s_skiplist_node__fact *node, s_skiplist_node__fact *pred)
{
  uw level;
  for (level = 0; level < node->height; level++) {
    s_skiplist_node__fact *p = SKIPLIST_NODE_NEXT__fact(pred, level);
    SKIPLIST_NODE_NEXT__fact(node, level) =
      SKIPLIST_NODE_NEXT__fact(p, level);
    SKIPLIST_NODE_NEXT__fact(p, level) = node;
  }
}
