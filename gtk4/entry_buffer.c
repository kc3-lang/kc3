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
#include "entry_buffer.h"

void kc3_gtk4_entry_buffer_delete (GtkEntryBuffer **eb)
{
  g_object_unref(*eb);
}

GtkEntryBuffer ** kc3_gtk4_entry_buffer_new (const s_str *value,
                                             GtkEntryBuffer **dest)
{
  GtkEntryBuffer *tmp;
  if (! (tmp = gtk_entry_buffer_new(value->ptr.p_pchar, -1)))
    return NULL;
  *dest = tmp;
  return dest;
}

void kc3_gtk4_entry_buffer_set_text (GtkEntryBuffer **eb,
                                     const s_str *text)
{
  gtk_entry_buffer_set_text(GTK_ENTRY_BUFFER(*eb), text->ptr.p_pchar, -1);
}
