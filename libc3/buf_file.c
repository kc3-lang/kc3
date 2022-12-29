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
#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include "buf.h"
#include "buf_file.h"
#include "buf_save.h"

typedef struct buf_file {
  FILE *fp;
} s_buf_file;

sw buf_file_open_r_refill (s_buf *buf);
sw buf_file_open_w_flush (s_buf *buf);
sw buf_file_open_w_seek (s_buf *buf, sw offset, u8 whence);

void buf_file_close (s_buf *buf)
{
  assert(buf);
  buf_flush(buf);
  buf->flush = NULL;
  buf->refill = NULL;
  free(buf->user_ptr);
  buf->user_ptr = NULL;
}

e_bool buf_file_is_open (s_buf *buf)
{
  s_buf_file *buf_file;
  assert(buf);
  if (buf->user_ptr) {
    buf_file = (s_buf_file *) buf->user_ptr;
    if (! ferror(buf_file->fp) && ! feof(buf_file->fp))
      return true;
  }
  return false;
}

s_buf * buf_file_open_r (s_buf *buf, FILE *fp)
{
  s_buf_file *buf_file;
  assert(buf);
  assert(fp);
  buf_file = malloc(sizeof(s_buf_file));
  if (! buf_file)
    errx(1, "buf_file_open_r: out of memory");
  buf_file->fp = fp;
  buf->refill = buf_file_open_r_refill;
  buf->user_ptr = buf_file;
  return buf;
}

sw buf_file_open_r_refill (s_buf *buf)
{
  uw r;
  uw size;
  assert(buf);
  assert(buf->user_ptr);
  if (buf->rpos > buf->wpos ||
      buf->wpos > buf->size)
    return -1;
  size = buf->size - buf->wpos;
  r = fread(buf->ptr.ps8 + buf->wpos, 1, size,
            ((s_buf_file *) (buf->user_ptr))->fp);
  if (buf->wpos + r > buf->size) {
    assert(! "buffer overflow");
    return -1;
  }
  buf->wpos += r;
  return r;
}

s_buf * buf_file_open_w (s_buf *buf, FILE *fp)
{
  s_buf_file *buf_file;
  assert(buf);
  assert(fp);
  buf_file = malloc(sizeof(s_buf_file));
  if (! buf_file)
    errx(1, "buf_file_open_w: out of memory");
  buf_file->fp = fp;
  buf->flush = buf_file_open_w_flush;
  buf->seek = buf_file_open_w_seek;
  buf->user_ptr = buf_file;
  return buf;
}

sw buf_file_open_w_flush (s_buf *buf)
{
  s_buf_file *buf_file;
  uw min_wpos;
  s_buf_save *save;
  sw size;
  assert(buf);
  assert(buf->user_ptr);
  if (buf->rpos)
    return -1;
  if (buf->rpos > buf->wpos)
    return -1;
  if (buf->wpos > buf->size)
    return -1;
  min_wpos = buf_save_min_wpos(buf);
  size = min_wpos;
  if (size == 0)
    return buf->size - buf->wpos;
  buf_file = buf->user_ptr;
  if (fwrite(buf->ptr.p, size, 1, buf_file->fp) != 1) {
    warn("buf_file_open_w_flush: fwrite");
    return -1;
  }
  fflush(buf_file->fp);
  buf->wpos -= size;
  save = buf->save;
  while (save) {
    save->wpos -= size;
    save = save->next;
  }
  return size;
}

sw buf_file_open_w_seek (s_buf *buf, sw offset, u8 whence)
{
  s_buf_file *buf_file;
  sw r;
  assert(buf);
  assert(! buf->save);
  assert(buf->user_ptr);
  buf_file = buf->user_ptr;
  if ((r = buf_flush(buf)) < 0)
    return r;
  if (fseek(buf_file->fp, offset, whence)) {
    warn("buf_file_open_w_seek: fseek");
    return -1;
  }
  return 0;
}
