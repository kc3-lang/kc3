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
#include "../libkc3/alloc.h"
#include "../libkc3/assert.h"
#include "frame.h"

void kc3_gtk4_frame_delete (GtkFrame **frame)
{
  g_object_unref(*frame);
}

GtkWidget ** kc3_gtk4_frame_new (GtkWidget **dest, const s_str *label)
{
  const char *l;
  GtkWidget *tmp;
  l = (label && label->size) ? label->ptr.pchar : NULL;
  if (! (tmp = gtk_frame_new(l))) {
    err_puts("kc3_gtk4_frame_new: error");
    assert(! "kc3_gtk4_frame_new: error");
    return NULL;
  }
  *dest = tmp;
  return dest;
}
