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
#if defined(WIN32) || defined(WIN64)
# include <winsock2.h>
# define MSG_NOSIGNAL 0
#else
# include <sys/types.h>
# include <sys/ioctl.h>
# include <sys/socket.h>
#endif

#include <errno.h>
#include <string.h>
#include <unistd.h>
#include "alloc.h"
#include "assert.h"
#include "buf.h"
#include "buf_fd.h"
#include "buf_save.h"

sw    buf_fd_open_r_refill (s_buf *buf);
u64 * buf_fd_open_r_tell (s_buf *buf, u64 *dest);
sw    buf_fd_open_w_flush (s_buf *buf);
s64   buf_fd_open_w_seek (s_buf *buf, s64 offset, s8 from);
u64 * buf_fd_open_w_tell (s_buf *buf, u64 *dest);

void buf_fd_close (s_buf *buf)
{
  assert(buf);
  buf_flush(buf);
  buf->flush = NULL;
  buf->refill = NULL;
  buf->tell = NULL;
  alloc_free(buf->user_ptr);
  buf->user_ptr = NULL;
}

s_buf * buf_fd_open_r (s_buf *buf, t_fd fd)
{
  s_buf_fd *buf_fd;
  assert(buf);
  assert(fd);
  buf_fd = alloc(sizeof(s_buf_fd));
  if (! buf_fd)
    return NULL;
  buf_fd->fd = fd;
  buf->line = 0;
  buf->refill = buf_fd_open_r_refill;
  buf->tell = buf_fd_open_r_tell;
  buf->user_ptr = buf_fd;
  return buf;
}

sw buf_fd_open_r_refill (s_buf *buf)
{
  s32 avail;
  t_fd fd;
  sw r;
  uw size;
  assert(buf);
  assert(buf->user_ptr);
  if (buf->rpos > buf->wpos) {
    err_puts("buf_fd_open_r_refill: buf->rpos > buf->wpos");
    assert(! "buf_fd_open_r_refill: buf->rpos > buf->wpos");
    return -1;
  }
  if (buf->wpos > buf->size) {
    err_puts("buf_fd_open_r_refill: buf->wpos > buf->size");
    assert(! "buf_fd_open_r_refill: buf->wpos > buf->size");
    return -1;
  }
  size = buf->size - buf->wpos;
  fd = ((s_buf_fd *) (buf->user_ptr))->fd;
  //r = read(fd, buf->ptr.pchar + buf->wpos, size);
#if defined(WIN32) || defined(WIN64)
  WSAIoctl(fd, FIONREAD, NULL, 0, &avail, sizeof(avail), NULL, NULL,
           NULL);
#else
  if (ioctl(fd, FIONREAD, &avail) == -1) {
    err_puts("buf_fd_open_r_refill: ioctl FIONREAD: -1");
    return -1;
  }
#endif
  if (avail < 0) {
    err_write_1("buf_fd_open_r_refill: avail: ");
    err_inspect_s32_decimal(avail);
    err_write_1("\n");
    return -1;
  }
  if (! avail)
    avail = 1;
  if ((uw) avail > size)
    avail = size;
  r = read(fd, buf->ptr.pchar + buf->wpos, avail);
  if (r < 0)
    return r;
  if (buf->wpos + r > buf->size) {
    err_puts("buf_fd_open_r_refill: buffer overflow");
    assert(! "buf_fd_open_r_refill: buffer overflow");
    return -1;
  }
  buf->wpos += r;
  return r;
}

sw buf_fd_open_r_seek (s_buf *buf, sw offset, u8 from)
{
  s_buf_fd *buf_fd;
  sw r;
  s64 result;
  assert(buf);
  assert(! buf->save);
  assert(buf->user_ptr);
  buf_fd = buf->user_ptr;
  if ((result = lseek(buf_fd->fd, offset, from)) < 0) {
    err_puts("buf_fd_open_r_seek: lseek");
    return -1;
  }
  buf->rpos = 0;
  buf->wpos = 0;
  if ((r = buf_refill(buf, buf->size)) < 0)
    return r;
  return result;
}

u64 * buf_fd_open_r_tell (s_buf *buf, u64 *dest)
{
  s_buf_fd *buf_fd;
  s32 e;
  s64 tmp;
  assert(buf);
  assert(buf->user_ptr);
  assert(dest);
  buf_fd = buf->user_ptr;
  if ((tmp = lseek(buf_fd->fd, 0, SEEK_CUR)) < 0) {
    e = errno;
    err_write_1("buf_fd_tell: lseek: ");
    err_puts(strerror(e));
    assert(! "buf_fd_tell: lseek");
    return NULL;
  }
  *dest = tmp + buf->rpos;
  return dest;
}

s_buf * buf_fd_open_w (s_buf *buf, t_fd fd)
{
  s_buf_fd *buf_fd;
  assert(buf);
  assert(fd);
  buf_fd = alloc(sizeof(s_buf_fd));
  if (! buf_fd)
    return NULL;
  buf_fd->fd = fd;
  buf->flush = buf_fd_open_w_flush;
  buf->seek = buf_fd_open_w_seek;
  buf->tell = buf_fd_open_w_tell;
  buf->user_ptr = buf_fd;
  return buf;
}

sw buf_fd_open_w_flush (s_buf *buf)
{
  sw bytes;
  s_buf_fd *buf_fd;
  sw e;
  uw min_wpos;
  s_buf_save *save;
  sw size;
  sw w;
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
  buf_fd = buf->user_ptr;
  bytes = 0;
  while (bytes < size) {
    // XXX TODO: #ifdef WIN32 || WIN64
    if ((w = send(buf_fd->fd, buf->ptr.pchar + bytes,
                  size - bytes, MSG_NOSIGNAL)) < 0) {
      if ((w = write(buf_fd->fd, buf->ptr.pchar + bytes,
                     size - bytes)) < 0) {
        e = errno;
        err_write_1("buf_fd_open_w_flush: write: ");
        err_puts(strerror(e));
        return -1;
      }
    }
    bytes += w;
  }
  buf->wpos -= size;
  save = buf->save;
  while (save) {
    save->wpos -= size;
    save = save->next;
  }
  return size;
}

s64 buf_fd_open_w_seek (s_buf *buf, s64 offset, s8 from)
{
  s_buf_fd *buf_fd;
  s64 r;
  assert(buf);
  assert(! buf->save);
  assert(buf->user_ptr);
  buf_fd = buf->user_ptr;
  if ((r = buf_flush(buf)) < 0)
    return r;
  if ((r = lseek(buf_fd->fd, offset, from)) < 0) {
    err_puts("buf_fd_open_w_seek: lseek");
    return -1;
  }
  return r;
}

u64 * buf_fd_open_w_tell (s_buf *buf, u64 *dest)
{
  s_buf_fd *buf_fd;
  s32 e;
  s64 tmp;
  assert(buf);
  assert(buf->user_ptr);
  assert(dest);
  buf_fd = buf->user_ptr;
  if ((tmp = lseek(buf_fd->fd, 0, SEEK_CUR)) < 0) {
    e = errno;
    err_write_1("buf_fd_tell: lseek: ");
    err_puts(strerror(e));
    assert(! "buf_fd_tell: lseek");
    return NULL;
  }
  *dest = tmp + buf->wpos;
  return dest;
}
