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
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/event.h>
#include <sys/time.h>
#include "../libkc3/assert.h"
#include "../libkc3/sym.h"
#include "../libkc3/tag.h"
#include "kqueue.h"

s64 kc3_kqueue (void)
{
  s32 e;
  s32 r;
  if ((r = kqueue()) < 0) {
    e = errno;
    err_write_1("kc3_kqueue: kqueue: ");
    err_puts(strerror(e));
    assert(! "kc3_kqueue: kqueue");
  }
  return r;
}

s64 kc3_kqueue_add (s64 kqfd, s64 fd, s_tag *timeout, s_tag *udata)
{
  s32 e;
  struct kevent events[2];
  s32 nevents = 1;
  s32 r;
  memset(events, 0, sizeof(events));
  events[0].ident = fd;
  events[0].filter = EVFILT_READ;
  events[0].flags = EV_ADD | EV_ONESHOT;
  events[0].data = SOMAXCONN;
  events[0].udata = udata;
  if (timeout && timeout->type == TAG_TIME) {
    s_time *t = &timeout->data.time;
    s64 timeout_ms = t->tv_sec * 1000 + t->tv_nsec / 1000000;
    events[1].ident = fd;
    events[1].filter = EVFILT_TIMER;
    events[1].flags = EV_ADD | EV_ONESHOT;
    events[1].data = timeout_ms;
    events[1].udata = udata;
    nevents = 2;
  }
  if ((r = kevent(kqfd, events, nevents, NULL, 0, NULL)) < 0) {
    e = errno;
    err_write_1("kc3_kqueue_add: kevent: ");
    err_puts(strerror(e));
    assert(! "kc3_kqueue_add: kevent");
  }
  return r;
}

s64 kc3_kqueue_delete (s64 kqfd, s64 fd, s_tag *filter)
{
  struct kevent event = {0};
  event.ident = fd;
  event.flags = EV_DELETE;
  if (filter && filter->type == TAG_PSYM) {
    if (filter->data.psym == &g_sym_read)
      event.filter = EVFILT_READ;
    else if (filter->data.psym == &g_sym_timer)
      event.filter = EVFILT_TIMER;
    else {
      err_write_1("kc3_kqueue_delete: invalid filter: ");
      err_inspect_sym(filter->data.psym);
      err_write_1("\n");
      return -1;
    }
  }
  else {
    err_puts("kc3_kqueue_delete: filter must be a symbol");
    return -1;
  }
  return kevent(kqfd, &event, 1, NULL, 0, NULL);
}

s_tag * kc3_kqueue_poll (s64 kqfd, s_tag *timeout, s_tag *dest)
{
  s32 e;
  struct kevent event = {0};
  s_timespec *p = NULL;
  s32 r;
  s_timespec timespec = {0};
  if (timeout && timeout->type != TAG_VOID) {
    switch (timeout->type) {
    case TAG_TIME:
      timespec.tv_sec = timeout->data.time.tv_sec;
      timespec.tv_nsec = timeout->data.time.tv_nsec;
      p = &timespec;
      break;
    default:
      err_puts("kc3_kqueue_poll: invalid timeout");
      assert(! "kc3_kqueue_poll: invalid timeout");
      return NULL;
    }
  }
  if ((r = kevent(kqfd, NULL, 0, &event, 1, p)) < 0) {
    e = errno;
    err_write_1("kc3_kqueue: kevent: ");
    err_puts(strerror(e));
    assert(! "kc3_kqueue_poll: kevent");
    return tag_init_void(dest);
  }
  if (r > 0) {
    s_tag *udata = event.udata;
    const s_sym *event_type;
    if (event.filter == EVFILT_TIMER)
      event_type = &g_sym_timer;
    else if (event.flags & EV_EOF)
      event_type = &g_sym_eof;
    else
      event_type = &g_sym_read;
    if (! tag_init_tuple(dest, 3) ||
        ! tag_init_s64(dest->data.tuple.tag, event.ident) ||
        ! tag_init_psym(dest->data.tuple.tag + 1, event_type))
      return NULL;
    if (! tag_init_copy(dest->data.tuple.tag + 2, udata))
      return NULL;
    return dest;
  }
  return tag_init_void(dest);
}
