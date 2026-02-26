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
#include <stdlib.h>
#include "io.h"
#include "list.h"
#include "memleak.h"

s_memleak *g_memleak = NULL;
bool       g_memleak_enabled = false;

void memleak_add (void *ptr, uw size, s_list *stacktrace)
{
  s_memleak *m;
  m = calloc(1, sizeof(s_memleak));
  if (! m)
    abort();
  m->ptr = ptr;
  m->size = size;
  m->stacktrace = stacktrace;
  m->next = g_memleak;
  g_memleak = m;
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
      list_delete_all(tmp->stacktrace);
      free(tmp);
      return;
    }
    m = &(*m)->next;
  }
}

void memleak_report (void)
{
  s_memleak *m;
  uw count = 0;
  uw total = 0;
  m = g_memleak;
  err_puts("Leak report:");
  while (m) {
    err_write_1("\n0x");
    err_inspect_uw_hexadecimal((uw) m->ptr);
    err_write_1(" ");
    err_inspect_uw_decimal(m->size);
    err_write_1(" bytes ----------------\n");
    err_inspect_stacktrace(m->stacktrace);
    err_write_1("\n");
    count++;
    total += m->size;
    m = m->next;
  }
  err_write_1("Total: ");
  err_inspect_uw_decimal(count);
  err_write_1(" leaks, ");
  err_inspect_uw_decimal(total);
  err_puts(" bytes");
}
