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
#include "../libkc3/io.h"
#include "tls_buf.h"

int buf_tls_open_r(s_tls_buf *buf, p_tls ctx)
{
  assert(buf);
  assert(ctx);
  if (buf == NULL || ctx == NULL) {
    err_puts(" buf_tls_open_r: invalid input");
    assert(! " buf_tls_open_r: invalid input");
    return -1;
  }
  return 0;
}

int buf_tls_open_w(s_tls_buf *buf, struct tls *ctx)
{
  assert(buf);
  assert(ctx);
  if (buf == NULL || ctx == NULL) {
    err_puts(" buf_tls_open_w: invalid input");  
    assert(! " buf_tls_open_w: invalid input");

    return -1;
  }
  return 0;
}
