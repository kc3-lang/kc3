/* kc3
 * Copyright from 2022 to 2025 kmx.io <contact@kmx.io>
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
#ifndef KC3_GTK4_ENTRY_BUFFER_H
#define KC3_GTK4_ENTRY_BUFFER_H

#include "types.h"

/* Heap-allocation functions, call kc3_gtk4_entry_buffer_delete
   after use. */
void              kc3_gtk4_entry_buffer_delete (GtkEntryBuffer **eb);
GtkEntryBuffer ** kc3_gtk4_entry_buffer_new (const s_str *value,
                                             GtkEntryBuffer **dest);

/* Operators. */
void kc3_gtk4_entry_buffer_set_text (GtkEntryBuffer **eb,
                                     const s_str *text);
  
#endif /* KC3_GTK4_ENTRY_BUFFER_H */
