/* c3
 * Copyright 2022,2023 kmx.io <contact@kmx.io>
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
#include <assert.h>
#include <err.h>
#include <stdlib.h>
#include "buf.h"
#include "buf_save.h"

s_buf * buf_save_clean (s_buf *buf, s_buf_save *save)
{
  assert(buf);
  assert(save);
  assert(buf->save == save);
  buf->save = save->next;
  return buf;
}

s_buf * buf_save_init (s_buf *buf, s_buf_save *save)
{
  assert(buf);
  assert(save);
  save->rpos = buf->rpos;
  save->wpos = buf->wpos;
  save->next = buf->save;
  buf->save = save;
  return buf;
}

uw buf_save_min_rpos (const s_buf *buf)
{
  uw rpos;
  s_buf_save *save;
  rpos = buf->rpos;
  save = buf->save;
  while (save) {
    if (save->rpos < rpos)
      rpos = save->rpos;
    save = save->next;
  }
  return rpos;
}

uw buf_save_min_wpos (const s_buf *buf)
{
  uw wpos;
  s_buf_save *save;
  wpos = buf->wpos;
  save = buf->save;
  while (save) {
    if (save->wpos < wpos)
      wpos = save->wpos;
    save = save->next;
  }
  return wpos;
}

s_buf * buf_save_restore_rpos (s_buf *buf, const s_buf_save *save)
{
  assert(buf);
  assert(save);
  assert(buf->save == save);
  buf->rpos = save->rpos;
  return buf;
}

s_buf * buf_save_restore_wpos (s_buf *buf, const s_buf_save *save)
{
  assert(buf);
  assert(save);
  assert(buf->save == save);
  buf->wpos = save->wpos;
  return buf;
}
