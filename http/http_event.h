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
#ifndef HTTP_EVENT_H
#define HTTP_EVENT_H

#include "types.h"

/* Heap-allocation functions, call *_delete after use. */
struct event_base * http_event_base_new (void);
void                http_event_delete (struct event *ev);
struct event *      http_event_new (struct event_base *event_base,
                                    s32 fd,
                                    const s_list * const *events,
                                    const s_fn *callback, s_tag *arg);

/* Operators. */
s32  http_event_add (struct event *ev, s_time *time);

#endif /* HTTP_H */
