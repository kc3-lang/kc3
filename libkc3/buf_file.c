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
#include "alloc.h"
#include "assert.h"
#include <stdio.h>
#include "buf.h"
#include "buf_file.h"
#include "buf_save.h"

typedef struct buf_file {
  FILE *fp;
} s_buf_file;

sw  buf_file_open_r_refill (s_buf *buf);
s64 buf_file_open_r_seek (s_buf *buf, s64 offset, s8 from);
sw  buf_file_open_w_flush (s_buf *buf);
s64 buf_file_open_w_seek (s_buf *buf, s64 offset, s8 from);

void buf_file_close (s_buf *buf)
{
  assert(buf);
  buf_flush(buf);
  buf->flush = NULL;
  buf->refill = NULL;
  free(buf->user_ptr);
  buf->user_ptr = NULL;
}

bool buf_file_is_open (const s_buf *buf)
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

FILE * buf_file_fp (const s_buf *buf)
{
  s_buf_file *buf_file;
  assert(buf);
  if (! buf->user_ptr)
    return NULL;
  buf_file = (s_buf_file *) buf->user_ptr;
  return buf_file->fp;
}

s_buf * buf_file_open_r (s_buf *buf, FILE *fp)
{
  s_buf_file *buf_file;
  assert(buf);
  assert(fp);
  buf_file = alloc(sizeof(s_buf_file));
  if (! buf_file)
    return NULL;
  buf_file->fp = fp;
  buf->line = 0;
  buf->refill = buf_file_open_r_refill;
  buf->seek = buf_file_open_r_seek;
  buf->user_ptr = buf_file;
  return buf;
}

sw buf_file_open_r_refill (s_buf *buf)
{
  uw r;
  uw size;
  assert(buf);
  assert(buf->user_ptr);
  if (buf->rpos > buf->wpos) {
    err_puts("buf_file_open_r_refill: buf->rpos > buf->wpos");
    assert(! "buf_file_open_r_refill: buf->rpos > buf->wpos");
    return -1;
  }
  if (buf->wpos >= buf->size) {
    err_puts("buf_file_open_r_refill: buf->wpos >= buf->size");
    assert(! "buf_file_open_r_refill: buf->wpos >= buf->size");
    return -1;
  }
  size = buf->size - buf->wpos;
  r = fread(buf->ptr.pchar + buf->wpos, 1, size,
            ((s_buf_file *) (buf->user_ptr))->fp);
  if (buf->wpos + r > buf->size) {
    err_puts("buf_file_open_r_refill: buffer overflow");
    assert(! "buf_file_open_r_refill: buffer overflow");
    return -1;
  }
  buf->wpos += r;
  return r;
}

s64 buf_file_open_r_seek (s_buf *buf, s64 offset, s8 from)
{
  s_buf_file *buf_file;
  off_t result;
  assert(buf);
  assert(! buf->save);
  assert(buf->user_ptr);
  buf_file = buf->user_ptr;
  if (fseeko(buf_file->fp, (off_t) offset, from) < 0) {
    err_puts("buf_file_open_r_seek: fseeko");
    return -1;
  }
  if ((result = ftello(buf_file->fp)) < 0) {
    err_puts("buf_file_open_r_seek: ftello");
    return -1;
  }
  buf->rpos = 0;
  buf->wpos = 0;
  buf_refill(buf, buf->size); // soft fail
  return (s64) result;
}

s_buf * buf_file_open_w (s_buf *buf, FILE *fp)
{
  s_buf_file *buf_file;
  assert(buf);
  assert(fp);
  buf_file = alloc(sizeof(s_buf_file));
  if (! buf_file)
    return NULL;
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
    err_puts("buf_file_open_w_flush: fwrite");
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

s64 buf_file_open_w_seek (s_buf *buf, s64 offset, s8 from)
{
  s_buf_file *buf_file;
  sw r;
  off_t result;
  assert(buf);
  assert(! buf->save);
  assert(buf->user_ptr);
  buf_file = buf->user_ptr;
  if ((r = buf_flush(buf)) < 0)
    return r;
  if (fseeko(buf_file->fp, (off_t) offset, from) < 0) {
    err_puts("buf_file_open_w_seek: fseeko");
    return -1;
  }
  if ((result = ftello(buf_file->fp)) < 0) {
    err_puts("buf_file_open_w_seek: ftello");
    return -1;
  }
  return (s64) result;
}
