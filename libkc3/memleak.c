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
#include <execinfo.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "buf.h"
#include "buf_inspect.h"
#include "hash.h"
#include "list.h"
#include "memleak.h"
#include "mutex.h"
#include "rwlock.h"

#define MEMLEAK_BACKTRACE_LEN 1024
#define MEMLEAK_HASH_SIZE 65536

s_memleak      *g_memleak = NULL;
static s_memleak *g_memleak_last = NULL;
static s_memleak *g_memleak_hash[MEMLEAK_HASH_SIZE] = {0};
bool            g_memleak_enabled = false;
static s_mutex  g_memleak_mutex = {0};
static bool     g_memleak_mutex_init = false;

static uw memleak_hash_ptr (void *ptr)
{
  t_hash h;
  hash_init(&h);
  hash_update_uw(&h, (uw) ptr);
  return hash_to_uw(&h) & (MEMLEAK_HASH_SIZE - 1);
}

void memleak_add (void *ptr, uw size, s_list *stacktrace)
{
  char a[BUF_SIZE];
  s_buf buf = {0};
  uw hash;
  s_memleak *m;
  s_memleak *check;
  sw r;
  s_rwlock rwlock = {0};
  hash = memleak_hash_ptr(ptr);
  mutex_lock(&g_memleak_mutex);
  check = g_memleak_hash[hash];
  while (check) {
    if (check->ptr == ptr) {
      fprintf(stderr, "memleak_add: duplicate pointer: %p (size %lu, prev size %lu)\n",
              ptr, (unsigned long) size, (unsigned long) check->size);
      mutex_unlock(&g_memleak_mutex);
      return;
    }
    check = check->hash_next;
  }
  if (! (m = calloc(1, sizeof(s_memleak))))
    abort();
  m->ptr = ptr;
  m->size = size;
  rwlock_init(&rwlock);
  buf.line = 1;
  buf.ptr.p_pchar = a;
  buf.size = BUF_SIZE;
  buf.rwlock = &rwlock;
  if ((r = buf_inspect_stacktrace(&buf, stacktrace)) < 0)
    abort();
  rwlock_clean(&rwlock);
  if (! (m->env_stacktrace = calloc(1, buf.wpos + 1)))
    abort();
  memcpy(m->env_stacktrace, buf.ptr.p_pchar, buf.wpos);
  m->hash_next = g_memleak_hash[hash];
  g_memleak_hash[hash] = m;
  m->next = NULL;
  m->prev = g_memleak_last;
  if (g_memleak_last)
    g_memleak_last->next = m;
  else
    g_memleak = m;
  g_memleak_last = m;
  mutex_unlock(&g_memleak_mutex);
}

void memleak_remove (void *ptr)
{
  uw hash;
  s_memleak **h;
  s_memleak *m;
  hash = memleak_hash_ptr(ptr);
  mutex_lock(&g_memleak_mutex);
  h = &g_memleak_hash[hash];
  while (*h) {
    if ((*h)->ptr == ptr) {
      m = *h;
      *h = m->hash_next;
      if (m->prev)
        m->prev->next = m->next;
      else
        g_memleak = m->next;
      if (m->next)
        m->next->prev = m->prev;
      else
        g_memleak_last = m->prev;
      free(m->backtrace);
      free(m->env_stacktrace);
      free(m);
      mutex_unlock(&g_memleak_mutex);
      return;
    }
    h = &(*h)->hash_next;
  }
  if (false)
    fprintf(stderr, "memleak_remove: pointer not found: %p\n", ptr);
  mutex_unlock(&g_memleak_mutex);
}

void memleak_remove_all (void)
{
  s_memleak *m;
  s_memleak *next;
  uw i;
  mutex_lock(&g_memleak_mutex);
  m = g_memleak;
  while (m) {
    next = m->next;
    free(m->backtrace);
    free(m->env_stacktrace);
    free(m);
    m = next;
  }
  g_memleak = NULL;
  g_memleak_last = NULL;
  i = 0;
  while (i < MEMLEAK_HASH_SIZE) {
    g_memleak_hash[i] = NULL;
    i++;
  }
  mutex_unlock(&g_memleak_mutex);
}

void memleak_init (void)
{
  if (g_memleak_mutex_init)
    return;
  mutex_init(&g_memleak_mutex);
  g_memleak_mutex_init = true;
  g_memleak_enabled = true;
}

void memleak_report (void)
{
  uw i;
  s_memleak *m;
  uw count = 0;
  uw total = 0;
  m = g_memleak;
  if (m) {
    fprintf(stderr, "Leak report:\n");
    while (m) {
      fprintf(stderr, "\n0x%lx %lu bytes ----------------\n",
              (unsigned long) m->ptr, (unsigned long) m->size);
      if (m->env_stacktrace && m->env_stacktrace[0])
        fprintf(stderr, "%s\n", m->env_stacktrace);
      fprintf(stderr, "C backtrace (%lu frames):\n",
              (unsigned long) m->backtrace_len);
      if (m->backtrace) {
        i = 0;
        while (i < m->backtrace_len) {
          fprintf(stderr, "  %s\n", m->backtrace[i]);
          i++;
        }
      }
      else
        fprintf(stderr, "  (null)\n");
      count++;
      total += m->size;
      m = m->next;
    }
    fprintf(stderr, "Total: %lu leaks, %lu bytes\n",
            (unsigned long) count, (unsigned long) total);
    exit(1);
  }
}
