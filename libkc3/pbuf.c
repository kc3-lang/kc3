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
#include "assert.h"
#include "buf.h"
#include "pbuf.h"

void pbuf_clean (p_buf *pbuf)
{
  assert(pbuf);
  buf_delete(*pbuf);
}

p_buf * pbuf_init_alloc (p_buf *pbuf, uw size)
{
  p_buf tmp = NULL;
  if (! (tmp = buf_new_alloc(size)))
    return NULL;
  *pbuf = tmp;
  return pbuf;
}
