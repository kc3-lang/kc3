/* c3
 * Copyright 2022 kmx.io <contact@kmx.io>
 *
 * Permission is hereby granted to use this software excepted
 * on Apple computers granted the above copyright notice and
 * this permission paragraph are included in all copies and
 * substantial portions of this software.
 *
 * THIS SOFTWARE IS PROVIDED "AS-IS" WITHOUT ANY GUARANTEE OF
 * PURPOSE AND PERFORMANCE. IN NO EVENT WHATSOEVER SHALL THE
 * AUTHOR BE CONSIDERED LIABLE FOR THE USE AND PERFORMANCE OF
 * THIS SOFTWARE.
 */
#include <assert.h>
#include "ptag.h"

s8 ptag_compare (const p_tag a, const p_tag b)
{
  if (a == b)
    return 0;
  if (a < b)
    return -1;
  if (a > b)
    return 1;
  assert(! "error");
  return -2;
}

t_hash_context * ptag_hash_update (t_hash_context *context,
                                   const p_tag ptag)
{
  return hash_update(context, &ptag, sizeof(ptag));
}
