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
#include "assert.h"
#include "queue.h"
#include "list.h"

void queue_clean (s_queue *queue)
{
  assert(queue);
  list_delete_all(queue->head);
}

void queue_dequeue (s_queue *queue)
{
  assert(queue);
  if (! queue->count)
    return;
  queue->head = list_delete(queue->head);
  queue->count--;
  if (! queue->count)
    queue->tail = NULL;
}

s_tag * queue_enqueue (s_queue *queue)
{
  s_list *list;
  assert(queue);
  if (! (list = list_new(NULL)))
    return NULL;
  if (! queue->count) {
    queue->head = list;
    queue->tail = list;
  }
  else {
    queue->tail->next.data.td_plist = list;
    queue->tail = queue->tail->next.data.td_plist;
  }
  queue->count++;
  return &queue->tail->tag;
}

s_tag * queue_first (s_queue *queue)
{
  assert(queue);
  if (! queue->count)
    return NULL;
  return &queue->head->tag;
}

s_queue * queue_init (s_queue *queue)
{
  s_queue tmp = {0};
  *queue = tmp;
  return queue;
}

s_tag * queue_last (s_queue *queue)
{
  assert(queue);
  if (! queue->count)
    return NULL;
  return &queue->tail->tag;
}
