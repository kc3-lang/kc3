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
#include <pthread.h>
#include <string.h>
#include <sys/epoll.h>
#include <time.h>
#include <unistd.h>
#include "../libkc3/assert.h"
#include "../libkc3/sym.h"
#include "../libkc3/tag.h"
#include "epoll.h"

#define EPOLL_MAX_ENTRIES 4096

typedef struct epoll_entry {
  s64      fd;
  void    *udata;
  struct timespec deadline;
  bool     active;
  bool     has_timeout;
} s_epoll_entry;

static s_epoll_entry g_entries[EPOLL_MAX_ENTRIES];
static pthread_mutex_t g_entries_mutex = PTHREAD_MUTEX_INITIALIZER;

static s_epoll_entry * entry_find (s64 fd)
{
  uw i = 0;
  while (i < EPOLL_MAX_ENTRIES) {
    if (g_entries[i].active && g_entries[i].fd == fd)
      return &g_entries[i];
    i++;
  }
  return NULL;
}

static s_epoll_entry * entry_alloc (s64 fd)
{
  uw i = 0;
  s_epoll_entry *existing = entry_find(fd);
  if (existing)
    return existing;
  while (i < EPOLL_MAX_ENTRIES) {
    if (! g_entries[i].active) {
      memset(&g_entries[i], 0, sizeof(s_epoll_entry));
      g_entries[i].fd = fd;
      g_entries[i].active = true;
      return &g_entries[i];
    }
    i++;
  }
  return NULL;
}

static void entry_free (s64 fd)
{
  s_epoll_entry *e = entry_find(fd);
  if (e) {
    e->active = false;
    e->has_timeout = false;
    e->udata = NULL;
    e->fd = -1;
  }
}

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

s64 kc3_epoll (void)
{
  s32 e;
  s32 r;
  pthread_mutex_lock(&g_entries_mutex);
  memset(g_entries, 0, sizeof(g_entries));
  pthread_mutex_unlock(&g_entries_mutex);
  if ((r = epoll_create1(EPOLL_CLOEXEC)) < 0) {
    e = errno;
    err_write_1("kc3_epoll: epoll_create1: ");
    err_puts(strerror(e));
    assert(! "kc3_epoll: epoll_create1");
  }
  return r;
}

s64 kc3_epoll_add (s64 epfd, s64 fd, s_tag *timeout, s_tag **udata)
{
  s32 e;
  struct epoll_event ev = {0};
  s_epoll_entry *entry;
  void *stored_udata;
  stored_udata = udata ? *udata : NULL;
  ev.events = EPOLLIN | EPOLLONESHOT | EPOLLRDHUP;
  ev.data.fd = fd;
  pthread_mutex_lock(&g_entries_mutex);
  entry = entry_alloc(fd);
  if (! entry) {
    pthread_mutex_unlock(&g_entries_mutex);
    err_puts("kc3_epoll_add: max entries reached");
    assert(! "kc3_epoll_add: max entries reached");
    return -1;
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
  }
  else {
    entry->has_timeout = false;
  }
  pthread_mutex_unlock(&g_entries_mutex);
  if (epoll_ctl(epfd, EPOLL_CTL_MOD, fd, &ev) < 0) {
    if (errno == ENOENT) {
      if (epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &ev) < 0) {
        e = errno;
        err_write_1("kc3_epoll_add: epoll_ctl ADD: ");
        err_puts(strerror(e));
        assert(! "kc3_epoll_add: epoll_ctl ADD");
        return -1;
      }
    }
    else {
      e = errno;
      err_write_1("kc3_epoll_add: epoll_ctl MOD: ");
      err_puts(strerror(e));
      assert(! "kc3_epoll_add: epoll_ctl MOD");
      return -1;
    }
  }
  return 0;
}

s64 kc3_epoll_delete (s64 epfd, s64 fd, s_tag *filter)
{
  s_epoll_entry *entry;
  if (filter && filter->type == TAG_PSYM) {
    if (filter->data.td_psym == &g_sym_timer) {
      pthread_mutex_lock(&g_entries_mutex);
      entry = entry_find(fd);
      if (entry)
        entry->has_timeout = false;
      pthread_mutex_unlock(&g_entries_mutex);
      return 0;
    }
    else if (filter->data.td_psym == &g_sym_read) {
      epoll_ctl(epfd, EPOLL_CTL_DEL, fd, NULL);
      pthread_mutex_lock(&g_entries_mutex);
      entry_free(fd);
      pthread_mutex_unlock(&g_entries_mutex);
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

s_tag * kc3_epoll_poll (s64 epfd, s_tag *timeout, s_tag *dest)
{
  s32 e;
  struct epoll_event event = {0};
  const s_sym *event_type;
  s_epoll_entry *entry;
  uw i;
  s32 r;
  s32 timeout_ms = 1000;
  s_tag *udata;
  if (timeout && timeout->type == TAG_TIME) {
    s_time *t = &timeout->data.td_time;
    timeout_ms = t->tv_sec * 1000 + t->tv_nsec / 1000000;
  }
  r = epoll_wait(epfd, &event, 1, timeout_ms);
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
    pthread_mutex_lock(&g_entries_mutex);
    entry = entry_find(event.data.fd);
    udata = entry ? entry->udata : NULL;
    pthread_mutex_unlock(&g_entries_mutex);
    if (event.events & (EPOLLRDHUP | EPOLLHUP | EPOLLERR))
      event_type = &g_sym_eof;
    else
      event_type = &g_sym_read;
    if (! tag_init_ptuple(dest, 3))
      return NULL;
    if (! tag_init_s64(dest->data.td_ptuple->tag, event.data.fd) ||
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
  pthread_mutex_lock(&g_entries_mutex);
  i = 0;
  while (i < EPOLL_MAX_ENTRIES) {
    if (g_entries[i].active && g_entries[i].has_timeout &&
        timespec_expired(&g_entries[i].deadline)) {
      s64 fd = g_entries[i].fd;
      udata = g_entries[i].udata;
      g_entries[i].has_timeout = false;
      pthread_mutex_unlock(&g_entries_mutex);
      if (! tag_init_ptuple(dest, 3))
        return NULL;
      if (! tag_init_s64(dest->data.td_ptuple->tag, fd) ||
          ! tag_init_psym(dest->data.td_ptuple->tag + 1,
                          &g_sym_timer)) {
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
    i++;
  }
  pthread_mutex_unlock(&g_entries_mutex);
  return tag_init_void(dest);
}
