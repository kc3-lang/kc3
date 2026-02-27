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
#include "menu_item.h"

GMenuItem ** kc3_gtk4_menu_item_new (GMenuItem **dest,
				     const s_str *label,
				     const s_str *detailed_action)
{
  GMenuItem * tmp;
  if (! (tmp = g_menu_item_new(label->ptr.p_pchar,
			       detailed_action->ptr.p_pchar)))
    return NULL;
  *dest = tmp;
  return dest;
}

