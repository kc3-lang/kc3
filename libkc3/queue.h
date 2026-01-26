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
#ifndef LIBKC3_QUEUE_H
#define LIBKC3_QUEUE_H

#include "types.h"

/* Stack-allocation compatible functions, call queue_clean after use. */
void      queue_clean (s_queue *queue);
s_queue * queue_init (s_queue *queue);

/* Observers. */
s_tag * queue_first (s_queue *queue);
s_tag * queue_last (s_queue *queue);

/* Operators. */
s_tag * queue_enqueue (s_queue *queue);
void    queue_dequeue (s_queue *queue);

#endif /* LIBKC3_QUEUE_H */
