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
#ifndef KC3_GTK4_APPLICATION_H
#define KC3_GTK4_APPLICATION_H

#include "types.h"

/* Heap-allocation functions, call kc3_gtk4_application_delete
   after use. */
void              kc3_gtk4_application_delete (GtkApplication **app);
GtkApplication ** kc3_gtk4_application_new (GtkApplication **dest,
                                            const s_str *name,
                                            const s_str *id);

/* Operators. */
s32  kc3_gtk4_application_run (GtkApplication **app);
void kc3_gtk4_application_set_accel_for_action (GtkApplication **app,
                                                const s_str *action,
                                                const s_str *accel);
void kc3_gtk4_application_set_menubar (GtkApplication **app,
                                       GMenuModel **menu);

#endif /* KC3_GTK4_APPLICATION_H */
