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
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "../libkc3/kc3.h"

static sw buf_getc_refill (s_buf *buf);

void buf_getc_close (s_buf *buf)
{
  FILE *fp;
  assert(buf);
  assert(buf->refill == buf_getc_refill);
  assert(buf->user_ptr);
  buf->refill = NULL;
  fp = buf->user_ptr;
  fclose(fp);
}

s_buf * buf_getc_open_r (s_buf *buf, const s_str *path)
{
  FILE *fp;
  assert(buf);
  assert(! buf->refill);
  assert(! buf->user_ptr);
  fp = file_open(path, "rb");
  if (! fp)
    return NULL;
  buf->refill = buf_getc_refill;
  buf->user_ptr = fp;
  return buf;
}

sw buf_getc_refill (s_buf *buf)
{
  sw c = 0;
  FILE *fp;
  uw result = 0;
  assert(buf);
  assert(buf->user_ptr);
  if (buf->rpos > buf->wpos ||
      buf->wpos > buf->size)
    return -1;
  fp = buf->user_ptr;
  if (feof(fp))
    return 0;
  while (buf->wpos < buf->size &&
         c != '\n' &&
         (c = getc(fp)) != EOF &&
         c <= 255) {
    if (buf_write_u8(buf, c) < 0)
      return -1;
    result++;
  }
  return result;
}
