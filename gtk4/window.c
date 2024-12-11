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
#include "window.h"

void kc3_gtk4_window_set_default_size (GtkWindow **window, uw width,
				       uw height)
{
  gtk_window_set_default_size(*window, width, height);
}

GtkWindow ** kc3_gtk4_window (GObject **ptr, GtkWindow **dest)
{
  if (! (*dest = GTK_WINDOW(*ptr)))
    return NULL;
  return dest;
}

void kc3_gtk4_window_destroy (GtkWindow **window)
{
  gtk_window_destroy(*window);
}

GtkWidget ** kc3_gtk4_window_new (GtkWidget **window)
{
  if (! (*window = gtk_window_new()))
    return NULL;
  return window;
}

void kc3_gtk4_window_present (GtkWindow **window)
{
  gtk_window_present(*window);
}

void kc3_gtk4_window_set_child (GtkWindow **window,
                                GtkWidget **child)
{
  gtk_window_set_child(*window, *child);
}

void kc3_gtk4_window_set_title (GtkWindow **window, const s_str *title)
{
  gtk_window_set_title(*window, title->ptr.pchar);
}
