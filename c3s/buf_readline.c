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
#include <unistd.h>
#include "../libc3/c3.h"

typedef struct buf_readline {
  s_buf       buf;
} s_buf_readline;

sw buf_readline_refill_fgets (s_buf *buf);

void buf_readline_close (s_buf *buf)
{
  assert(buf);
  buf->refill = NULL;
  free(buf->user_ptr);
  buf->user_ptr = NULL;
}

s_buf * buf_readline_open_r (s_buf *buf)
{
  s_buf_readline *buf_readline;
  assert(buf);
  buf_readline = malloc(sizeof(s_buf_readline));
  if (! buf_readline)
    errx(1, "buf_readline_open_r: out of memory");
  buf_init_1(&buf_readline->buf, "");
  buf->refill = buf_readline_refill_fgets;
  buf->user_ptr = buf_readline;
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
