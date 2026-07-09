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
#ifndef KC3_GTK4_TEXT_H
#define KC3_GTK4_TEXT_H

#include "types.h"

/* Heap-allocation functions, call kc3_gtk4_text_delete
   after use. */
void         kc3_gtk4_text_delete (GtkText **app);
GtkWidget ** kc3_gtk4_text_new (GtkWidget **dest);
GtkWidget ** kc3_gtk4_text_new_with_buffer (GtkEntryBuffer **eb,
                                            GtkWidget **dest);

#endif /* KC3_GTK4_TEXT_H */
