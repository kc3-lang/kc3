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
#ifndef KC3_GTK4_WINDOW_H
#define KC3_GTK4_WINDOW_H

#include "types.h"

/* Heap-allocation functions, call kc3_gtk4_window_destroy
   after use. */
void         kc3_gtk4_window_destroy (GtkWindow **window);
GtkWidget ** kc3_gtk4_window_new (GtkWidget **window);

/* Operators. */
void kc3_gtk4_window_present (GtkWindow **window);
void kc3_gtk4_window_set_child (GtkWindow **window,
                                GtkWidget **child);
void kc3_gtk4_window_set_title (GtkWindow **window, const s_str *title);
void kc3_gtk4_set_default_size(GtkWindow **window, uw width, uw height);

#endif /* KC3_GTK4_WINDOW_H */
