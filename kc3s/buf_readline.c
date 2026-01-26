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
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "../libkc3/kc3.h"

sw buf_readline_refill_fgets (s_buf *buf);

void buf_readline_close (s_buf *buf)
{
  assert(buf);
  buf->refill = NULL;
}

s_buf * buf_readline_open_r (s_buf *buf)
{
  assert(buf);
  buf->refill = buf_readline_refill_fgets;
  return buf;
}

sw buf_readline_refill_fgets (s_buf *buf)
{
  sw c = 0;
  uw result = 0;
  assert(buf);
  assert(buf->user_ptr);
  if (buf->rpos > buf->wpos ||
      buf->wpos > buf->size)
    return -1;
  if (feof(stdin))
    return -1;
  while (buf->wpos < buf->size &&
         c != '\n' &&
         (c = getchar()) != EOF &&
         c <= 255) {
    buf_write_u8(buf, c);
    result++;
  }
  return result;
}
