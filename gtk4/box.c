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
#include "../libkc3/assert.h"
#include "../libkc3/s32.h"
#include "../libkc3/sym.h"
#include "box.h"

void kc3_gtk4_box_append (GtkBox **box, GtkWidget **child)
{
  gtk_box_append(*box, *child);
}

GtkWidget ** kc3_gtk4_box_new (GtkWidget **dest,
                               const s_sym * const *orientation,
                               s_tag *spacing)
{
  GtkOrientation o;
  s32 s;
  GtkWidget *tmp;
  const s_sym *sym_S32 = &g_sym_S32;
  if (*orientation == sym_1("horizontal"))
    o = GTK_ORIENTATION_HORIZONTAL;
  else if (*orientation == sym_1("vertical"))
    o = GTK_ORIENTATION_VERTICAL;
  else {
    err_puts("kc3_gtk4_box_new: invalid orientation, choose between"
             " :horizontal or :vertical.");
    assert(!("kc3_gtk4_box_new: invalid orientation, choose between"
             " :horizontal or :vertical."));
    return NULL;
  }
  if (! s32_init_cast(&s, &sym_S32, spacing)){
    err_puts("kc3_gtk4_box_set_pixel: invalid spacing");
    assert(! "kc3_gtk4_box_set_pixel: invalid spacing");
    return NULL;
  }
  if (! (tmp = gtk_box_new(o, s)))
    return NULL;
  *dest = tmp;
  return dest;
}
