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
#ifndef KC3_GTK4_WIDGET_H
#define KC3_GTK4_WIDGET_H

#include "types.h"

/* Operators. */
GtkWidget ** kc3_gtk4_widget_set_halign (GtkWidget **widget,
                                         const s_sym * const *align);
GtkWidget ** kc3_gtk4_widget_set_size_request (GtkWidget **widget,
                                               const s_tag *width,
                                               const s_tag *height);

#endif /* KC3_GTK4_WIDGET_H */
