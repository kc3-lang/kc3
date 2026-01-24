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
#include "buf_fd.h"
#include "buf_rw.h"

void buf_rw_clean (s_buf_rw *buf_rw)
{
  assert(buf_rw);
  if (buf_rw->w)
    buf_delete(buf_rw->w);
  if (buf_rw->r)
    buf_delete(buf_rw->r);
}

void buf_rw_fd_close (s_buf_rw *buf_rw)
{
  assert(buf_rw);
  buf_fd_close(buf_rw->r);
  buf_fd_close(buf_rw->w);
}

s_buf_rw * buf_rw_fd_open (s_buf_rw *buf_rw, s32 fd)
{
  s_buf_rw tmp = {0};
  assert(buf_rw);
  tmp = *buf_rw;
  if (! buf_fd_open_r(tmp.r, fd))
    return NULL;
  if (! buf_fd_open_w(tmp.w, fd)) {
    buf_fd_close(tmp.r);
    return NULL;
  }
  *buf_rw = tmp;
  return buf_rw;
}

s_buf_rw * buf_rw_init_alloc (s_buf_rw *buf_rw, uw size)
{
  s_buf_rw tmp = {0};
  if (! (tmp.r = buf_new_alloc(size)))
    return NULL;
  if (! (tmp.w = buf_new_alloc(size))) {
    buf_delete(tmp.r);
    return NULL;
  }
  *buf_rw = tmp;
  return buf_rw;
}
