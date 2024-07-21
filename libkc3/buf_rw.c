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
#include "assert.h"
#include "buf.h"
#include "buf_rw.h"

void buf_rw_clean (s_buf_rw *buf_rw)
{
  assert(buf_rw);
  buf_clean(&buf_rw->w);
  buf_clean(&buf_rw->r);
}

s_buf_rw * buf_rw_init_alloc (s_buf_rw *buf_rw, uw size)
{
  s_buf_rw tmp = {0};
  if (! buf_init_alloc(&tmp.r, size))
    return NULL;
  if (! buf_init_alloc(&tmp.w, size)) {
    buf_clean(&tmp.r);
    return NULL;
  }
  *buf_rw = tmp;
  return buf_rw;
}
