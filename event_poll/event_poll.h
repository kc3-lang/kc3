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
#ifndef EVENT_POLL_H
#define EVENT_POLL_H

#include "../libkc3/types.h"

#if defined(HAVE_EPOLL) && ! defined(HAVE_KQUEUE)

typedef struct event_poll_entry {
  s64                        fd;
  void                      *udata;
  struct timespec            deadline;
  bool                       has_timeout;
  struct event_poll_entry   *next;
  struct event_poll_entry   *timer_next;
} s_event_poll_entry;

typedef struct event_poll {
  s64                   epfd;
  s_event_poll_entry   *entries;
  s_event_poll_entry   *timers;
  s_mutex               entries_mutex;
} s_event_poll;

#endif

void   *kc3_event_poll_new    (void);
s64     kc3_event_poll_add    (void **handle, s64 fd, s_tag *timeout,
                               s_tag **udata);
s64     kc3_event_poll_delete (void **handle, s64 fd, s_tag *filter);
s_tag * kc3_event_poll_poll   (void **handle, s_tag *timeout, s_tag *dest);

#endif /* EVENT_POLL_H */
