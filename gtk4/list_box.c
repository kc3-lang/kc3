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
#include "list_box.h"

/* Heap-allocation functions. */
GtkWidget ** kc3_gtk4_list_box_new (GtkWidget **dest)
{
  GtkWidget *tmp;
  if (! (tmp = gtk_list_box_new()))
    return NULL;
  *dest = tmp;
  return dest;
}

/* Operators. */
void kc3_gtk4_list_box_append (GtkListBox **box, GtkWidget **child)
{
  gtk_list_box_append(*box, *child);
}
