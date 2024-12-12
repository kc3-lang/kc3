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
#include <libkc3/assert.h>
#include <libkc3/sym.h>
#include "paned.h"

GtkWidget ** kc3_gtk4_paned_new (GtkWidget **dest,
                                 const s_sym * const *orientation)
{
  GtkOrientation o;
  GtkWidget *tmp;
  if (*orientation == sym_1("horizontal"))
    o = GTK_ORIENTATION_HORIZONTAL;
  else if (*orientation == sym_1("vertical"))
    o = GTK_ORIENTATION_VERTICAL;
  else {
    err_puts("kc3_gtk4_paned_new: invalid orientation, choose between"
             " :horizontal or :vertical.");
    assert(!("kc3_gtk4_paned_new: invalid orientation, choose between"
             " :horizontal or :vertical."));
    return NULL;
  }
  if (! (tmp = gtk_paned_new(o)))
    return NULL;
  *dest = tmp;
  return dest;
}

void kc3_gtk4_paned_set_end_child (GtkPaned **paned,
                                   GtkWidget **child)
{
  gtk_paned_set_end_child(GTK_PANED(*paned), GTK_WIDGET(*child));
}

void kc3_gtk4_paned_set_resize_end_child (GtkPaned **paned,
                                            bool value)
{
  gtk_paned_set_resize_end_child(GTK_PANED(*paned), value);
}

void kc3_gtk4_paned_set_shrink_end_child (GtkPaned **paned,
                                            bool value)
{
  gtk_paned_set_shrink_end_child(GTK_PANED(*paned), value);
}

void kc3_gtk4_paned_set_resize_start_child (GtkPaned **paned,
                                            bool value)
{
  gtk_paned_set_resize_start_child(GTK_PANED(*paned), value);
}

void kc3_gtk4_paned_set_shrink_start_child (GtkPaned **paned,
                                            bool value)
{
  gtk_paned_set_shrink_start_child(GTK_PANED(*paned), value);
}

void kc3_gtk4_paned_set_start_child (GtkPaned **paned,
                                     GtkWidget **child)
{
  gtk_paned_set_end_child(GTK_PANED(*paned), GTK_WIDGET(*child));
}
