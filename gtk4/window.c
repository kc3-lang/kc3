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

void kc3_gtk4_window_destroy (GtkWindow **ptr)
{
  gtk_window_destroy(*ptr);
}

GtkWidget ** kc3_gtk4_window_new (GtkWidget **ptr)
{
  if (! (*ptr = gtk_window_new()))
    return NULL;
  return ptr;
}

void kc3_gtk4_window_present (GtkWindow **ptr)
{
  gtk_window_present(*ptr);
}
