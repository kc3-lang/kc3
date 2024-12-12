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
#include <libkc3/alloc.h>
#include <libkc3/assert.h>
#include "frame.h"

void kc3_gtk4_frame_delete (GtkFrame **frame)
{
  g_object_unref(*frame);
}

GtkWidget ** kc3_gtk4_frame_new (GtkWidget **dest)
{
  GtkWidget *tmp;
  if (! (tmp = gtk_frame_new(NULL)))
    return NULL;
  *dest = tmp;
  return dest;
}
