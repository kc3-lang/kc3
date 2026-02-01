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
#include "kqueue.h"

s32 kc3_kevent_add (s32 kqfd, s64 fd, s_tag *udata)
{
  s32 e;
  struct kevent event = {0};
  s32 r;
  event.ident = fd;
  event.filter = EVFILT_READ;
  event.flags = EV_ADD | EV_RECEIPT;
  event.data = SOMAXCONN;
  event.udata = (void *) udata;
  if ((r = kevent(kqfd, &event, 1, NULL, 0, NULL)) < 0) {
    e = errno;
    err_write_1("kc3_kqueue: kevent: ");
    err_puts(strerror(e));
    assert(! "kc3_kqueue: kevent");
  }
  return r;
}

s32 kc3_kqueue (void)
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
