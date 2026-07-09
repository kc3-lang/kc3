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
#ifndef KC3_GTK4_PANED_H
#define KC3_GTK4_PANED_H

#include "types.h"

/* Heap-allocation functions. */
GtkWidget ** kc3_gtk4_paned_new (GtkWidget **widget,
                                 const s_sym * const *orientation);

/* Operators. */
GtkPaned ** kc3_gtk4_paned_set_end_child (GtkPaned **paned,
                                          GtkWidget **child);
GtkPaned ** kc3_gtk4_paned_set_position (GtkPaned **paned,
                                         s_tag *position);
GtkPaned ** kc3_gtk4_paned_set_resize_start_child (GtkPaned **paned,
                                                   bool value);
GtkPaned ** kc3_gtk4_paned_set_shrink_start_child (GtkPaned **paned,
                                                   bool value);
GtkPaned ** kc3_gtk4_paned_set_start_child (GtkPaned **paned,
                                            GtkWidget **child);

#endif /* KC3_GTK4_PANED_H */
