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
#include "simple_action.h"

GSimpleAction ** kc3_gtk4_simple_action_new (GSimpleAction **dest,
                                             const s_str *name)
{
  GSimpleAction *tmp;
  if (! (tmp = g_simple_action_new(name->ptr.pchar, NULL)))
    return NULL;
  *dest = tmp;
  return dest;
}
