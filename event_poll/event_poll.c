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
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include "../libkc3/alloc.h"
#include "../libkc3/assert.h"
#include "../libkc3/mutex.h"
#include "../libkc3/sym.h"
#include "../libkc3/tag.h"

#if defined(HAVE_KQUEUE)
# include <sys/types.h>
# include <sys/event.h>
# include <sys/time.h>
#elif defined(HAVE_EPOLL)
# include <sys/epoll.h>
# include <time.h>
#else
# error "No event polling backend available (neither kqueue nor epoll)"
#endif

#include "event_poll.h"

#if defined(HAVE_EPOLL) && ! defined(HAVE_KQUEUE)

static void timespec_now (struct timespec *ts)
{
  clock_gettime(CLOCK_MONOTONIC, ts);
}

static bool timespec_expired (const struct timespec *deadline)
{
  struct timespec now;
  timespec_now(&now);
  if (now.tv_sec > deadline->tv_sec)
    return true;
  if (now.tv_sec == deadline->tv_sec &&
      now.tv_nsec >= deadline->tv_nsec)
    return true;
  return false;
}

static bool timespec_lt (const struct timespec *a,
                         const struct timespec *b)
{
  if (a->tv_sec < b->tv_sec)
    return true;
  if (a->tv_sec == b->tv_sec && a->tv_nsec < b->tv_nsec)
    return true;
  return false;
}

static s_event_poll_entry * entry_new (s_event_poll *ep, s64 fd)
{
  s_event_poll_entry *entry;
  if (! (entry = alloc(sizeof(s_event_poll_entry))))
    return NULL;
  memset(entry, 0, sizeof(s_event_poll_entry));
  entry->fd = fd;
  entry->next = ep->entries;
  ep->entries = entry;
  return entry;
}

static s_event_poll_entry * entry_find (s_event_poll *ep, s64 fd)
{
  s_event_poll_entry *e;
  e = ep->entries;
  while (e) {
    if (e->fd == fd)
      return e;
    e = e->next;
  }
  return NULL;
}

static void timer_insert (s_event_poll *ep, s_event_poll_entry *entry)
{
  s_event_poll_entry **p;
  p = &ep->timers;
  while (*p && timespec_lt(&(*p)->deadline, &entry->deadline))
    p = &(*p)->timer_next;
  entry->timer_next = *p;
  *p = entry;
}

static void timer_remove (s_event_poll *ep, s_event_poll_entry *entry)
{
  s_event_poll_entry **p;
  p = &ep->timers;
  while (*p) {
    if (*p == entry) {
      *p = entry->timer_next;
      entry->timer_next = NULL;
      return;
    }
    p = &(*p)->timer_next;
  }
}

static void entry_free (s_event_poll *ep, s_event_poll_entry *entry)
{
  s_event_poll_entry **p;
  if (entry->has_timeout)
    timer_remove(ep, entry);
  p = &ep->entries;
  while (*p) {
    if (*p == entry) {
      *p = entry->next;
      break;
    }
    p = &(*p)->next;
  }
  alloc_free(entry);
}

#endif /* HAVE_EPOLL */

void * kc3_event_poll_new (void)
{
#if defined(HAVE_KQUEUE)
  s32 e;
  s64 kqfd;
  if ((kqfd = kqueue()) < 0) {
    e = errno;
    err_write_1("kc3_event_poll_new: kqueue: ");
    err_puts(strerror(e));
    assert(! "kc3_event_poll_new: kqueue");
    return NULL;
  }
  return (void *)(intptr_t) kqfd;
#elif defined(HAVE_EPOLL)
  s32 e;
  s_event_poll *ep;
  if (! (ep = alloc(sizeof(s_event_poll))))
    return NULL;
  ep->entries = NULL;
  ep->timers = NULL;
  if (! mutex_init(&ep->entries_mutex)) {
    alloc_free(ep);
    return NULL;
  }
  if ((ep->epfd = epoll_create1(EPOLL_CLOEXEC)) < 0) {
    e = errno;
    err_write_1("kc3_event_poll_new: epoll_create1: ");
    err_puts(strerror(e));
    assert(! "kc3_event_poll_new: epoll_create1");
    mutex_clean(&ep->entries_mutex);
    alloc_free(ep);
    return NULL;
  }
  return ep;
#endif
}

s64 kc3_event_poll_add (void **handle, s64 fd, s_tag *timeout,
                        s_tag **udata)
{
  if (! handle || ! *handle)
    return -1;
#if defined(HAVE_KQUEUE)
  {
    s64 kqfd = (s64)(intptr_t)(*handle);
    s32 e;
    struct kevent events[2];
    s32 nevents = 1;
    s32 r;
    void *stored_udata;
    memset(events, 0, sizeof(events));
    stored_udata = udata ? *udata : NULL;
    events[0].ident = fd;
    events[0].filter = EVFILT_READ;
    events[0].flags = EV_ADD | EV_ONESHOT;
    events[0].data = SOMAXCONN;
    events[0].udata = stored_udata;
    if (timeout && timeout->type == TAG_TIME) {
      s_time *t = &timeout->data.td_time;
      s64 timeout_ms = t->tv_sec * 1000 + t->tv_nsec / 1000000;
      events[1].ident = fd;
      events[1].filter = EVFILT_TIMER;
      events[1].flags = EV_ADD | EV_ONESHOT;
      events[1].data = timeout_ms;
      events[1].udata = stored_udata;
      nevents = 2;
    }
    if ((r = kevent(kqfd, events, nevents, NULL, 0, NULL)) < 0) {
      e = errno;
      err_write_1("kc3_event_poll_add: kevent: ");
      err_puts(strerror(e));
      assert(! "kc3_event_poll_add: kevent");
    }
    return r;
  }
#elif defined(HAVE_EPOLL)
  {
    s_event_poll *ep = *(s_event_poll **) handle;
    s32 e;
    s_event_poll_entry *entry;
    struct epoll_event ev = {0};
    void *stored_udata;
    stored_udata = udata ? *udata : NULL;
    mutex_lock(&ep->entries_mutex);
    entry = entry_find(ep, fd);
    if (! entry) {
      entry = entry_new(ep, fd);
      if (! entry) {
        mutex_unlock(&ep->entries_mutex);
        err_puts("kc3_event_poll_add: entry_new");
        assert(! "kc3_event_poll_add: entry_new");
        return -1;
      }
    }
    else if (entry->has_timeout) {
      timer_remove(ep, entry);
      entry->has_timeout = false;
    }
    entry->udata = stored_udata;
    if (timeout && timeout->type == TAG_TIME) {
      s_time *t = &timeout->data.td_time;
      struct timespec now;
      timespec_now(&now);
      entry->deadline.tv_sec = now.tv_sec + t->tv_sec;
      entry->deadline.tv_nsec = now.tv_nsec + t->tv_nsec;
      if (entry->deadline.tv_nsec >= 1000000000L) {
        entry->deadline.tv_sec++;
        entry->deadline.tv_nsec -= 1000000000L;
      }
      entry->has_timeout = true;
      timer_insert(ep, entry);
    }
    else {
      entry->has_timeout = false;
    }
    ev.events = EPOLLIN | EPOLLONESHOT | EPOLLRDHUP;
    ev.data.ptr = entry;
    if (epoll_ctl(ep->epfd, EPOLL_CTL_MOD, fd, &ev) < 0) {
      if (errno == ENOENT) {
        if (epoll_ctl(ep->epfd, EPOLL_CTL_ADD, fd, &ev) < 0) {
          e = errno;
          entry_free(ep, entry);
          mutex_unlock(&ep->entries_mutex);
          if (e != EBADF) {
            err_write_1("kc3_event_poll_add: epoll_ctl ADD: ");
            err_puts(strerror(e));
            assert(! "kc3_event_poll_add: epoll_ctl ADD");
          }
          return -1;
        }
      }
      else if (errno == EBADF) {
        entry_free(ep, entry);
        mutex_unlock(&ep->entries_mutex);
        return -1;
      }
      else {
        e = errno;
        entry_free(ep, entry);
        mutex_unlock(&ep->entries_mutex);
        err_write_1("kc3_event_poll_add: epoll_ctl MOD: ");
        err_puts(strerror(e));
        assert(! "kc3_event_poll_add: epoll_ctl MOD");
        return -1;
      }
    }
    mutex_unlock(&ep->entries_mutex);
    return 0;
  }
#endif
}

s64 kc3_event_poll_delete (void **handle, s64 fd, s_tag *filter)
{
  if (! handle || ! *handle)
    return -1;
#if defined(HAVE_KQUEUE)
  {
    s64 kqfd = (s64)(intptr_t)(*handle);
    if (! filter || filter->type == TAG_VOID) {
      close(kqfd);
      *handle = NULL;
      return 0;
    }
    if (filter->type == TAG_PSYM) {
      struct kevent event = {0};
      event.ident = fd;
      event.flags = EV_DELETE;
      if (filter->data.td_psym == &g_sym_read)
        event.filter = EVFILT_READ;
      else if (filter->data.td_psym == &g_sym_timer)
        event.filter = EVFILT_TIMER;
      else {
        err_write_1("kc3_event_poll_delete: invalid filter: ");
        err_inspect_sym(filter->data.td_psym);
        err_write_1("\n");
        return -1;
      }
      return kevent(kqfd, &event, 1, NULL, 0, NULL);
    }
    err_puts("kc3_event_poll_delete: filter must be a symbol");
    return -1;
  }
#elif defined(HAVE_EPOLL)
  {
    s_event_poll *ep = *(s_event_poll **) handle;
    s_event_poll_entry *entry;
    s_event_poll_entry *next;
    if (! filter || filter->type == TAG_VOID) {
      entry = ep->entries;
      while (entry) {
        next = entry->next;
        alloc_free(entry);
        entry = next;
      }
      close(ep->epfd);
      mutex_clean(&ep->entries_mutex);
      alloc_free(ep);
      *handle = NULL;
      return 0;
    }
    if (filter->type == TAG_PSYM) {
      if (filter->data.td_psym == &g_sym_timer) {
        mutex_lock(&ep->entries_mutex);
        entry = entry_find(ep, fd);
        if (entry && entry->has_timeout) {
          timer_remove(ep, entry);
          entry->has_timeout = false;
        }
        mutex_unlock(&ep->entries_mutex);
        return 0;
      }
      else if (filter->data.td_psym == &g_sym_read) {
        epoll_ctl(ep->epfd, EPOLL_CTL_DEL, fd, NULL);
        mutex_lock(&ep->entries_mutex);
        entry = entry_find(ep, fd);
        if (entry)
          entry_free(ep, entry);
        mutex_unlock(&ep->entries_mutex);
        return 0;
      }
      err_write_1("kc3_event_poll_delete: invalid filter: ");
      err_inspect_sym(filter->data.td_psym);
      err_write_1("\n");
      return -1;
    }
    err_puts("kc3_event_poll_delete: filter must be a symbol");
    return -1;
  }
#endif
}

s_tag * kc3_event_poll_poll (void **handle, s_tag *timeout, s_tag *dest)
{
  if (! handle || ! *handle)
    return NULL;
#if defined(HAVE_KQUEUE)
  {
    s64 kqfd = (s64)(intptr_t)(*handle);
    s32 e;
    struct kevent event = {0};
    const s_sym *event_type;
    s_timespec *p = NULL;
    s32 r;
    s_timespec timespec = {0};
    s_tag *udata;
    if (timeout && timeout->type != TAG_VOID) {
      switch (timeout->type) {
      case TAG_TIME:
        timespec.tv_sec = timeout->data.td_time.tv_sec;
        timespec.tv_nsec = timeout->data.td_time.tv_nsec;
        p = &timespec;
        break;
      default:
        err_puts("kc3_event_poll_poll: invalid timeout");
        assert(! "kc3_event_poll_poll: invalid timeout");
        return NULL;
      }
    }
    if ((r = kevent(kqfd, NULL, 0, &event, 1, p)) < 0) {
      e = errno;
      if (e == EINTR)
        return tag_init_void(dest);
      err_write_1("kc3_event_poll_poll: kevent: ");
      err_puts(strerror(e));
      assert(! "kc3_event_poll_poll: kevent");
      return tag_init_void(dest);
    }
    if (r > 0) {
      udata = event.udata;
      if (event.filter == EVFILT_TIMER)
        event_type = &g_sym_timer;
      else if (event.flags & EV_EOF)
        event_type = &g_sym_eof;
      else
        event_type = &g_sym_read;
      if (! tag_init_ptuple(dest, 3))
        return NULL;
      if (! tag_init_s64(dest->data.td_ptuple->tag, event.ident) ||
          ! tag_init_psym(dest->data.td_ptuple->tag + 1, event_type)) {
        tag_clean(dest);
        return NULL;
      }
      if (udata) {
        if (! tag_init_copy(dest->data.td_ptuple->tag + 2, udata)) {
          tag_clean(dest);
          return NULL;
        }
      }
      else
        tag_init_void(dest->data.td_ptuple->tag + 2);
      return dest;
    }
    return tag_init_void(dest);
  }
#elif defined(HAVE_EPOLL)
  {
    s_event_poll *ep = *(s_event_poll **) handle;
    s32 e;
    struct epoll_event event = {0};
    const s_sym *event_type;
    s_event_poll_entry *entry;
    s32 r;
    s32 timeout_ms = -1;
    s_tag *udata;
    if (timeout && timeout->type == TAG_TIME) {
      s_time *t = &timeout->data.td_time;
      timeout_ms = t->tv_sec * 1000 + t->tv_nsec / 1000000;
    }
    mutex_lock(&ep->entries_mutex);
    if (ep->timers && timespec_expired(&ep->timers->deadline)) {
      entry = ep->timers;
      ep->timers = entry->timer_next;
      entry->timer_next = NULL;
      entry->has_timeout = false;
      udata = entry->udata;
      if (! tag_init_ptuple(dest, 3)) {
        mutex_unlock(&ep->entries_mutex);
        return NULL;
      }
      if (! tag_init_s64(dest->data.td_ptuple->tag, entry->fd) ||
          ! tag_init_psym(dest->data.td_ptuple->tag + 1,
                          &g_sym_timer)) {
        mutex_unlock(&ep->entries_mutex);
        tag_clean(dest);
        return NULL;
      }
      if (udata) {
        if (! tag_init_copy(dest->data.td_ptuple->tag + 2, udata)) {
          mutex_unlock(&ep->entries_mutex);
          tag_clean(dest);
          return NULL;
        }
      }
      else
        tag_init_void(dest->data.td_ptuple->tag + 2);
      mutex_unlock(&ep->entries_mutex);
      return dest;
    }
    if (ep->timers) {
      struct timespec now;
      s32 timer_ms;
      timespec_now(&now);
      timer_ms = (ep->timers->deadline.tv_sec - now.tv_sec) * 1000 +
                 (ep->timers->deadline.tv_nsec - now.tv_nsec) / 1000000;
      if (timer_ms < 0)
        timer_ms = 0;
      if (timeout_ms < 0 || timer_ms < timeout_ms)
        timeout_ms = timer_ms;
    }
    mutex_unlock(&ep->entries_mutex);
    r = epoll_wait(ep->epfd, &event, 1, timeout_ms);
    if (r < 0) {
      e = errno;
      if (e == EINTR)
        return tag_init_void(dest);
      err_write_1("kc3_event_poll_poll: epoll_wait: ");
      err_puts(strerror(e));
      assert(! "kc3_event_poll_poll: epoll_wait");
      return tag_init_void(dest);
    }
    if (r > 0) {
      entry = event.data.ptr;
      if (! entry)
        return tag_init_void(dest);
      mutex_lock(&ep->entries_mutex);
      udata = entry ? entry->udata : NULL;
      if (entry->has_timeout) {
        timer_remove(ep, entry);
        entry->has_timeout = false;
      }
      if (event.events & (EPOLLRDHUP | EPOLLHUP | EPOLLERR))
        event_type = &g_sym_eof;
      else
        event_type = &g_sym_read;
      if (! tag_init_ptuple(dest, 3)) {
        mutex_unlock(&ep->entries_mutex);
        return NULL;
      }
      if (! tag_init_s64(dest->data.td_ptuple->tag, entry->fd) ||
          ! tag_init_psym(dest->data.td_ptuple->tag + 1, event_type)) {
        mutex_unlock(&ep->entries_mutex);
        tag_clean(dest);
        return NULL;
      }
      if (udata) {
        if (! tag_init_copy(dest->data.td_ptuple->tag + 2, udata)) {
          mutex_unlock(&ep->entries_mutex);
          tag_clean(dest);
          return NULL;
        }
      }
      else
        tag_init_void(dest->data.td_ptuple->tag + 2);
      mutex_unlock(&ep->entries_mutex);
      return dest;
    }
    return tag_init_void(dest);
  }
#endif
}
