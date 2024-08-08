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
#include <sys/time.h>
#include <event.h>
#include "http_event.h"

void http_event_set (struct event *ev, s32 fd, s16 event,
                     const s_cfn *cfn, void *arg)
{
  event_set(ev, fd, event, (void (*) (int, short, void *)) cfn->ptr.f,
            arg);
}
