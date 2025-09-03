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

#include "../libkc3/alloc.h"
#include "../libkc3/io.h"
#include "assert.h"
#include "types.h"
#include <string.h>
#include "tls_buf.h"

static sw tls_buf_open_w_flush(s_buf *buf)
{
    assert(buf);
    return buf && (sw)(! memset(buf->user_ptr, 0, buf->size));
}

static sw tls_buf_open_r_refill(s_buf *buf)
{
  assert(buf);
  if (buf == NULL)
    return -1;
  return 0;
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

s_buf * tls_buf_open_w (s_buf *buf, struct tls *ctx)
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
