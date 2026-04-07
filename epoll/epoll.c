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
#include <sys/epoll.h>
#include <time.h>
#include <unistd.h>
#include "../libkc3/alloc.h"
#include "../libkc3/assert.h"
#include "../libkc3/mutex.h"
#include "../libkc3/sym.h"
#include "../libkc3/tag.h"
#include "epoll.h"

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

static s_epoll_entry * entry_new (s_epoll *epoll, s64 fd)
{
  s_epoll_entry *entry;
  if (! (entry = alloc(sizeof(s_epoll_entry))))
    return NULL;
  memset(entry, 0, sizeof(s_epoll_entry));
  entry->fd = fd;
  entry->next = epoll->entries;
  epoll->entries = entry;
  return entry;
}

static s_epoll_entry * entry_find (s_epoll *epoll, s64 fd)
{
  s_epoll_entry *e;
  e = epoll->entries;
  while (e) {
    if (e->fd == fd)
      return e;
    e = e->next;
  }
  return NULL;
}

static void timer_insert (s_epoll *epoll, s_epoll_entry *entry)
{
  s_epoll_entry **p;
  p = &epoll->timers;
  while (*p && timespec_lt(&(*p)->deadline, &entry->deadline))
    p = &(*p)->timer_next;
  entry->timer_next = *p;
  *p = entry;
}

static void timer_remove (s_epoll *epoll, s_epoll_entry *entry)
{
  s_epoll_entry **p;
  p = &epoll->timers;
  while (*p) {
    if (*p == entry) {
      *p = entry->timer_next;
      entry->timer_next = NULL;
      return;
    }
    p = &(*p)->timer_next;
  }
}

static void entry_free (s_epoll *epoll, s_epoll_entry *entry)
{
  s_epoll_entry **p;
  if (entry->has_timeout)
    timer_remove(epoll, entry);
  p = &epoll->entries;
  while (*p) {
    if (*p == entry) {
      *p = entry->next;
      break;
    }
    p = &(*p)->next;
  }
  alloc_free(entry);
}

s_epoll * kc3_epoll (void)
{
  s32 e;
  s_epoll *epoll;
  if (! (epoll = alloc(sizeof(s_epoll))))
    return NULL;
  epoll->entries = NULL;
  epoll->timers = NULL;
  if (! mutex_init(&epoll->entries_mutex)) {
    alloc_free(epoll);
    return NULL;
  }
  if ((epoll->epfd = epoll_create1(EPOLL_CLOEXEC)) < 0) {
    e = errno;
    err_write_1("kc3_epoll: epoll_create1: ");
    err_puts(strerror(e));
    assert(! "kc3_epoll: epoll_create1");
    mutex_clean(&epoll->entries_mutex);
    alloc_free(epoll);
    return NULL;
  }
  return epoll;
}

s64 kc3_epoll_add (s_epoll **epoll_ptr, s64 fd, s_tag *timeout,
                   s_tag **udata)
{
  s32 e;
  s_epoll *epoll;
  s_epoll_entry *entry;
  struct epoll_event ev = {0};
  void *stored_udata;
  if (! epoll_ptr || ! *epoll_ptr)
    return -1;
  epoll = *epoll_ptr;
  stored_udata = udata ? *udata : NULL;
  mutex_lock(&epoll->entries_mutex);
  entry = entry_find(epoll, fd);
  if (! entry) {
    entry = entry_new(epoll, fd);
    if (! entry) {
      mutex_unlock(&epoll->entries_mutex);
      err_puts("kc3_epoll_add: entry_new");
      assert(! "kc3_epoll_add: entry_new");
      return -1;
    }
  }
  else if (entry->has_timeout) {
    timer_remove(epoll, entry);
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
    timer_insert(epoll, entry);
  }
  else {
    entry->has_timeout = false;
  }
  ev.events = EPOLLIN | EPOLLONESHOT | EPOLLRDHUP;
  ev.data.ptr = entry;
  if (epoll_ctl(epoll->epfd, EPOLL_CTL_MOD, fd, &ev) < 0) {
    if (errno == ENOENT) {
      if (epoll_ctl(epoll->epfd, EPOLL_CTL_ADD, fd, &ev) < 0) {
        e = errno;
        entry_free(epoll, entry);
        mutex_unlock(&epoll->entries_mutex);
        if (e != EBADF) {
          err_write_1("kc3_epoll_add: epoll_ctl ADD: ");
          err_puts(strerror(e));
          assert(! "kc3_epoll_add: epoll_ctl ADD");
        }
        return -1;
      }
    }
    else if (errno == EBADF) {
      entry_free(epoll, entry);
      mutex_unlock(&epoll->entries_mutex);
      return -1;
    }
    else {
      e = errno;
      entry_free(epoll, entry);
      mutex_unlock(&epoll->entries_mutex);
      err_write_1("kc3_epoll_add: epoll_ctl MOD: ");
      err_puts(strerror(e));
      assert(! "kc3_epoll_add: epoll_ctl MOD");
      return -1;
    }
  }
  mutex_unlock(&epoll->entries_mutex);
  return 0;
}

s64 kc3_epoll_delete (s_epoll **epoll_ptr, s64 fd, s_tag *filter)
{
  s_epoll *epoll;
  s_epoll_entry *entry;
  s_epoll_entry *next;
  if (! epoll_ptr || ! *epoll_ptr)
    return -1;
  epoll = *epoll_ptr;
  if (! filter || filter->type == TAG_VOID) {
    entry = epoll->entries;
    while (entry) {
      next = entry->next;
      alloc_free(entry);
      entry = next;
    }
    close(epoll->epfd);
    mutex_clean(&epoll->entries_mutex);
    alloc_free(epoll);
    *epoll_ptr = NULL;
    return 0;
  }
  if (filter->type == TAG_PSYM) {
    if (filter->data.td_psym == &g_sym_timer) {
      mutex_lock(&epoll->entries_mutex);
      entry = entry_find(epoll, fd);
      if (entry && entry->has_timeout) {
        timer_remove(epoll, entry);
        entry->has_timeout = false;
      }
      mutex_unlock(&epoll->entries_mutex);
      return 0;
    }
    else if (filter->data.td_psym == &g_sym_read) {
      epoll_ctl(epoll->epfd, EPOLL_CTL_DEL, fd, NULL);
      mutex_lock(&epoll->entries_mutex);
      entry = entry_find(epoll, fd);
      if (entry)
        entry_free(epoll, entry);
      mutex_unlock(&epoll->entries_mutex);
      return 0;
    }
    else {
      err_write_1("kc3_epoll_delete: invalid filter: ");
      err_inspect_sym(filter->data.td_psym);
      err_write_1("\n");
      return -1;
    }
  }
  else {
    err_puts("kc3_epoll_delete: filter must be a symbol");
    return -1;
  }
}

s_tag * kc3_epoll_poll (s_epoll **epoll_ptr, s_tag *timeout, s_tag *dest)
{
  s32 e;
  s_epoll *epoll;
  struct epoll_event event = {0};
  const s_sym *event_type;
  s_epoll_entry *entry;
  s32 r;
  s32 timeout_ms = -1;
  s_tag *udata;
  if (! epoll_ptr || ! *epoll_ptr)
    return NULL;
  epoll = *epoll_ptr;
  if (timeout && timeout->type == TAG_TIME) {
    s_time *t = &timeout->data.td_time;
    timeout_ms = t->tv_sec * 1000 + t->tv_nsec / 1000000;
  }
  mutex_lock(&epoll->entries_mutex);
  if (epoll->timers && timespec_expired(&epoll->timers->deadline)) {
    entry = epoll->timers;
    epoll->timers = entry->timer_next;
    entry->timer_next = NULL;
    entry->has_timeout = false;
    udata = entry->udata;
    if (! tag_init_ptuple(dest, 3)) {
      mutex_unlock(&epoll->entries_mutex);
      return NULL;
    }
    if (! tag_init_s64(dest->data.td_ptuple->tag, entry->fd) ||
        ! tag_init_psym(dest->data.td_ptuple->tag + 1,
                        &g_sym_timer)) {
      mutex_unlock(&epoll->entries_mutex);
      tag_clean(dest);
      return NULL;
    }
    if (udata) {
      if (! tag_init_copy(dest->data.td_ptuple->tag + 2, udata)) {
        mutex_unlock(&epoll->entries_mutex);
        tag_clean(dest);
        return NULL;
      }
    }
    else
      tag_init_void(dest->data.td_ptuple->tag + 2);
    mutex_unlock(&epoll->entries_mutex);
    return dest;
  }
  if (epoll->timers) {
    struct timespec now;
    s32 timer_ms;
    timespec_now(&now);
    timer_ms = (epoll->timers->deadline.tv_sec - now.tv_sec) * 1000 +
               (epoll->timers->deadline.tv_nsec - now.tv_nsec) / 1000000;
    if (timer_ms < 0)
      timer_ms = 0;
    if (timeout_ms < 0 || timer_ms < timeout_ms)
      timeout_ms = timer_ms;
  }
  mutex_unlock(&epoll->entries_mutex);
  r = epoll_wait(epoll->epfd, &event, 1, timeout_ms);
  if (r < 0) {
    e = errno;
    if (e == EINTR)
      return tag_init_void(dest);
    err_write_1("kc3_epoll_poll: epoll_wait: ");
    err_puts(strerror(e));
    assert(! "kc3_epoll_poll: epoll_wait");
    return tag_init_void(dest);
  }
  if (r > 0) {
    entry = event.data.ptr;
    if (! entry)
      return tag_init_void(dest);
    mutex_lock(&epoll->entries_mutex);
    udata = entry ? entry->udata : NULL;
    if (entry->has_timeout) {
      timer_remove(epoll, entry);
      entry->has_timeout = false;
    }
    if (event.events & (EPOLLRDHUP | EPOLLHUP | EPOLLERR))
      event_type = &g_sym_eof;
    else
      event_type = &g_sym_read;
    if (! tag_init_ptuple(dest, 3)) {
      mutex_unlock(&epoll->entries_mutex);
      return NULL;
    }
    if (! tag_init_s64(dest->data.td_ptuple->tag, entry->fd) ||
        ! tag_init_psym(dest->data.td_ptuple->tag + 1, event_type)) {
      mutex_unlock(&epoll->entries_mutex);
      tag_clean(dest);
      return NULL;
    }
    if (udata) {
      if (! tag_init_copy(dest->data.td_ptuple->tag + 2, udata)) {
        mutex_unlock(&epoll->entries_mutex);
        tag_clean(dest);
        return NULL;
      }
    }
    else
      tag_init_void(dest->data.td_ptuple->tag + 2);
    mutex_unlock(&epoll->entries_mutex);
    return dest;
  }
  return tag_init_void(dest);
}
