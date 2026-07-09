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
#include "application_window.h"

void kc3_gtk4_application_window_delete (GtkWindow **window)
{
  g_object_unref(G_OBJECT(*window));
}

GtkWidget ** kc3_gtk4_application_window_new (GtkApplication **app,
                                              GtkWidget **window)
{
  GtkWidget *tmp;
  if (! (tmp = gtk_application_window_new(*app)))
    return NULL;
  *window = tmp;
  return window;
}
