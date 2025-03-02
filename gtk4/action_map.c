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
#include <libkc3/assert.h>
#include "action_map.h"

void kc3_gtk4_action_map_add_action (GActionMap **map,
                                     GAction **action)
{
  assert(map);
  assert(*map);
  assert(action);
  assert(*action);
  g_action_map_add_action(G_ACTION_MAP(*map), G_ACTION(*action));
}
