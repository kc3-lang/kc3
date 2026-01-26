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
#include "../libkc3/kc3.h"
#include "entry.h"

/* Heap-allocation functions. */
GtkWidget ** kc3_gtk4_entry_new (GtkWidget **dest)
{
  GtkWidget *tmp;
  assert(dest);
  if (! (tmp = gtk_entry_new()))
    return NULL;
  *dest = tmp;
  return dest;
}

s_str * kc3_gtk4_entry_get_text (GtkEntry **entry, s_str *dest)
{
  GtkEntryBuffer *buffer;
  const char *s;
  if (! (buffer = gtk_entry_get_buffer(GTK_ENTRY(*entry)))) {
    err_puts("kc3_gtk4_entry_get_text: gtk_entry_get_buffer");
    assert(! "kc3_gtk4_entry_get_text: gtk_entry_get_buffer");
    return NULL;
  }
  if (! (s = gtk_entry_buffer_get_text(buffer))) {
    err_puts("kc3_gtk4_entry_get_text: gtk_entry_buffer_get_text");
    assert(! "kc3_gtk4_entry_get_text: gtk_entry_buffer_get_text");
    return NULL;
  }
  return str_init_1_alloc(dest, s);
}

void kc3_gtk4_entry_set_text (GtkEntry **entry, s_str *text)
{
  GtkEntryBuffer *buffer;
  if (! (buffer = gtk_entry_get_buffer(GTK_ENTRY(*entry)))) {
    err_puts("kc3_gtk4_entry_set_text: gtk_entry_get_buffer");
    assert(! "kc3_gtk4_entry_set_text: gtk_entry_get_buffer");
    return;
  }
  gtk_entry_buffer_set_text(buffer, text->ptr.pchar, text->size);
}
