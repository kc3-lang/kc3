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

s64 kc3_kqueue_add (s64 kqfd, s64 fd, s_tag *udata)
{
  s32 e;
  struct kevent event = {0};
  s32 r;
  event.ident = fd;
  event.filter = EVFILT_READ;
  event.flags = EV_ADD | EV_ONESHOT;
  event.data = SOMAXCONN;
  event.udata = tag_new_copy(udata);
  if ((r = kevent(kqfd, &event, 1, NULL, 0, NULL)) < 0) {
    e = errno;
    err_write_1("kc3_kqueue: kevent: ");
    err_puts(strerror(e));
    assert(! "kc3_kqueue: kevent");
  }
  return r;
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
    if (! tag_init_tuple(dest, 3) ||
        ! tag_init_s64(dest->data.tuple.tag, event.ident) ||
        ! tag_init_bool(dest->data.tuple.tag + 1, !! (event.flags &
                                                      EV_EOF)) ||
        ! tag_init_copy(dest->data.tuple.tag + 2, event.udata))
      return NULL;
    return dest;
  }
  return tag_init_void(dest);
}
