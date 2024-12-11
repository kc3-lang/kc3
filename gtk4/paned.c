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

GtkWidget ** kc3_gtk4_paned_new (GtkWidget **ptr,
                                 const s_sym * const *orientation)
{
  GtkOrientation o;
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
  if (! (*ptr = gtk_paned_new(o)))
    return NULL;
  return ptr;
}
