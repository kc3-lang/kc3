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
#include "button.h"

GtkWidget ** kc3_gtk4_button_new (GtkWidget ** dest )
{
  GtkWidget * tmp;
  if (! (tmp = gtk_button_new()))
    return NULL;
  *dest = tmp;
  return dest;
}
      
GtkWidget ** kc3_gtk4_button_new_with_label (GtkWidget **dest,
                                             s_str *label)
{
  GtkWidget * tmp;
  if (! (tmp = gtk_button_new_with_label(label->ptr.p_pchar)))
    return NULL;
  *dest = tmp;
  return dest;
}
