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
#ifndef KC3_GTK4_TEXT_VIEW_H
#define KC3_GTK4_TEXT_VIEW_H

#include "types.h"

/* Heap-allocation functions. */
GtkWidget ** kc3_gtk4_text_view_new (GtkWidget **dest);
GtkWidget ** kc3_gtk4_text_view_new_with_buffer (GtkWidget **dest,
                                                 GtkTextBuffer **tb);

#endif /* KC3_GTK4_TEXT_VIEW_H */
