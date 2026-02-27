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
#include <string.h>
#include <unistd.h>
#include <tls.h>
#include "alloc.h"
#include "buf.h"
#include "buf_save.h"
#include "io.h"
#include "assert.h"
#include "tls_buf.h"

sw tls_buf_open_r_refill (s_buf *buf);
sw tls_buf_open_w_flush (s_buf *buf);

void tls_buf_close (s_buf *buf)
{
  assert(buf);
  assert(buf->user_ptr);
  buf_flush(buf);
  buf->flush = NULL;
  buf->refill = NULL;
  alloc_free(buf->user_ptr);
  buf->user_ptr = NULL;
}

s_buf * tls_buf_open_r (s_buf *buf, p_tls ctx)
{
  s_tls_buf * tls_buf = NULL;
  assert(buf);
  assert(ctx);
  if (buf == NULL || ctx == NULL) {
    err_puts("buf_tls_open_r: invalid input");
    assert(! "buf_tls_open_r: invalid input");
    return NULL;
  }
  if (! (tls_buf = alloc(sizeof(s_tls_buf))))
    return NULL;
  tls_buf->ctx = ctx;
  buf->refill = tls_buf_open_r_refill;
  buf->user_ptr = tls_buf;
  return buf;
}

sw tls_buf_open_r_refill (s_buf *buf)
{
  sw r;
  uw size;
  s_tls_buf *tls_buf;
  assert(buf);
  assert(buf->user_ptr);
  if (buf->rpos > buf->wpos) {
    err_puts("tls_buf_open_r_refill: buf->rpos > buf->wpos");
    assert(! "tls_buf_open_r_refill: buf->rpos > buf->wpos");
    return -1;
  }
  if (buf->wpos > buf->size) {
    err_puts("tls_buf_open_r_refill: buf->wpos > buf->size");
    assert(! "tls_buf_open_r_refill: buf->wpos > buf->size");
    return -1;
  }
  size = buf->size - buf->wpos;
  tls_buf = buf->user_ptr;
  while (1) {
    r = tls_read(tls_buf->ctx, buf->ptr.p_pchar + buf->wpos, size);
    if (r == TLS_WANT_POLLIN || r == TLS_WANT_POLLOUT)
      continue;
    if (r < 0)
      return -1;
    break;
  }
  if (buf->wpos + r > buf->size) {
    err_puts("tls_buf_open_r_refill: buffer overflow");
    assert(! "tls_buf_open_r_refill: buffer overflow");
    return -1;
  }
  buf->wpos += r;
  return r;
}

s_buf * tls_buf_open_w (s_buf *buf, p_tls ctx)
{
  s_tls_buf * tls_buf = NULL;
  assert(buf);
  assert(ctx);
  if (buf == NULL || ctx == NULL) {
    err_puts("buf_tls_open_w: invalid input");
    assert(! "buf_tls_open_w: invalid input");
    return NULL;
  }
  if (! (tls_buf = alloc(sizeof(s_tls_buf))))
    return NULL;
  tls_buf->ctx = ctx;
  buf->flush = tls_buf_open_w_flush;
  buf->user_ptr = tls_buf;
  return buf;
}

sw tls_buf_open_w_flush (s_buf *buf)
{
  sw bytes;
  uw min_wpos;
  s_buf_save *save;
  sw size;
  s_tls_buf *tls_buf;
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
  tls_buf = buf->user_ptr;
  bytes = 0;
  while (bytes < size) {
    w = tls_write(tls_buf->ctx, buf->ptr.p_pchar + bytes,
                  size - bytes);
    if (w == TLS_WANT_POLLIN || w == TLS_WANT_POLLOUT)
      continue;
    if (w < 0) {
      err_write_1("tls_buf_open_w_flush: tls_write: ");
      err_puts(tls_error(tls_buf->ctx));
      return -1;
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
