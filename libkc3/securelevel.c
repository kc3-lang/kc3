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
#include "assert.h"
#include "securelevel.h"

u8 securelevel (u8 level)
{
  static u8 securelevel = 0;
  const u8 securelevel_max = 3;
  assert(securelevel < securelevel_max);
  if (level > securelevel_max)
    level = securelevel_max;
  if (level > securelevel)
    securelevel = level;
  return securelevel;
}
