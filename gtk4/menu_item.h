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
#ifndef KC3_GTK4_MENU_ITEM_H
#define KC3_GTK4_MENU_ITEM_H

#include "types.h"

/* Heap-allocation functions, call kc3_gtk4_application_delete
   after use. */
GMenuItem ** kc3_gtk4_menu_item_new (GMenuItem **dest,
				    const s_str *label,
				    const s_str *detailed_action);

#endif /*  KC3_GTK4_MENU_ITEM_H */
