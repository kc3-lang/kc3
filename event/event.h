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
#ifndef KC3_EVENT_H
#define KC3_EVENT_H

#include <libkc3/types.h>

/* Heap-allocation functions, call *_delete after use. */
struct event_base * kc3_event_base_new (void);
void                kc3_event_delete (struct event *ev);
struct event *      kc3_event_new (struct event_base **event_base,
                                   s32 fd,
                                   const s_list * const *events,
                                   p_callable *callback,
                                   s_tag *arg);

/* Operators. */
s32  kc3_event_add (struct event **ev, s_time *time);

#endif /* KC3_EVENT_H */
