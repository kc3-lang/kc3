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
#include "text.h"

void kc3_gtk4_text_delete (GtkText **text)
{
  g_object_unref(G_OBJECT(*text));
}

GtkWidget ** kc3_gtk4_text_new (GtkWidget **dest)
{
  GtkWidget *tmp;
  if (! (tmp = gtk_text_new()))
    return NULL;
  *dest = tmp;
  return dest;
}

GtkWidget ** kc3_gtk4_text_new_with_buffer (GtkEntryBuffer **eb,
                                            GtkWidget **dest)
{
  GtkWidget *tmp;
  if (! (tmp = gtk_text_new_with_buffer(GTK_ENTRY_BUFFER(*eb))))
    return NULL;
  *dest = tmp;
  return dest;
}
