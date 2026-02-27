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
#include "list.h"
#include "memleak.h"

#define MEMLEAK_BACKTRACE_LEN 1024

s_memleak *g_memleak = NULL;
bool       g_memleak_enabled = false;

void memleak_add (void *ptr, uw size, s_list *stacktrace)
{
  char a[BUF_SIZE];
  void *addrlist[MEMLEAK_BACKTRACE_LEN];
  s_buf buf = {0};
  uw len;
  s_memleak *m;
  sw r;
  if (! (m = calloc(1, sizeof(s_memleak))))
    abort();
  m->ptr = ptr;
  m->size = size;
  len = backtrace(addrlist, MEMLEAK_BACKTRACE_LEN);
  fprintf(stderr, "DEBUG memleak_add: backtrace returned %lu frames\n",
          (unsigned long) len);
  m->backtrace = backtrace_symbols(addrlist, len);
  m->backtrace_len = len;
  buf_init(&buf, false, BUF_SIZE, a);
  if ((r = buf_inspect_stacktrace(&buf, stacktrace)) < 0)
    abort();
  if (! (m->env_stacktrace = calloc(1, buf.wpos + 1)))
    abort();
  memcpy(m->env_stacktrace, buf.ptr.p_pchar, buf.wpos);
  m->next = g_memleak;
  g_memleak = m;
  buf_clean(&buf);
}

void memleak_remove (void *ptr)
{
  s_memleak **m;
  s_memleak *tmp;
  m = &g_memleak;
  while (*m) {
    if ((*m)->ptr == ptr) {
      tmp = *m;
      *m = (*m)->next;
      free(tmp->backtrace);
      free(tmp->env_stacktrace);
      free(tmp);
      return;
    }
    m = &(*m)->next;
  }
}

void memleak_remove_all (void)
{
  s_memleak *m;
  s_memleak *next;
  m = g_memleak;
  while (m) {
    next = m->next;
    free(m->backtrace);
    free(m->env_stacktrace);
    free(m);
    m = next;
  }
  g_memleak = NULL;
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
