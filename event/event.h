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
#ifndef KC3_EVENT_H
#define KC3_EVENT_H

#include "../libkc3/types.h"

/* Library initialization. */
void libkc3_event_init (void);

/* Heap-allocation functions, call *_delete after use. */
struct event_base * kc3_event_base_new (void);
void                kc3_event_delete (struct event *ev);
struct event *      kc3_event_new (struct event_base **event_base,
                                   s64 fd,
                                   const s_list * const *events,
                                   p_callable *callback,
                                   s_tag *arg);

/* Operators. */
s32 kc3_event_add (struct event **ev, s_time *time);
s32 kc3_event_base_loop (struct event_base **eb);
s32 kc3_event_base_loopbreak (struct event_base **eb);
s32 kc3_event_base_loopexit (struct event_base **eb);

#endif /* KC3_EVENT_H */
