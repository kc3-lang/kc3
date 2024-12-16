/* kc3
 * Copyright 2022,2023,2024 kmx.io <contact@kmx.io>
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
#include "label.h"

GtkWidget ** kc3_gtk4_label_new (GtkWidget **dest, s_str *str)
{
  GtkWidget *tmp;
  if (! (tmp = gtk_label_new(str->ptr.pchar)))
    return NULL;
  *dest = tmp;
  return dest;
}


