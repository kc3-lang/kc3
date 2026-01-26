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
#include "frame.h"
#include "pframe.h"

p_frame * pframe_init_copy (p_frame *pframe, p_frame *src)
{
  p_frame tmp;
  assert(pframe);
  assert(src);
  if (env_global()->pass_by_copy)
    tmp = frame_new_copy(*src);
  else
    tmp = frame_new_ref(*src);
  if (! tmp)
    return NULL;
  *pframe = tmp;
  return pframe;
}
