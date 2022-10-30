/* c3
 * Copyright 2022 kmx.io <contact@kmx.io>
 *
 * Permission is hereby granted to use this software granted
 * the above copyright notice and this permission paragraph
 * are included in all copies and substantial portions of this
 * software.
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
#include <readline/readline.h>
#include <readline/history.h>
#include <unistd.h>
#include "../libc3/c3.h"

typedef struct buf_readline {
  s_buf       buf;
  bool        eof;
  const char *prompt;
  bool        readline;
} s_buf_readline;

sw buf_readline_refill_fgets (s_buf *buf);
sw buf_readline_refill_readline (s_buf *buf);

void buf_readline_close (s_buf *buf)
{
  assert(buf);
  buf->refill = NULL;
  free(buf->user_ptr);
  buf->user_ptr = NULL;
  if (isatty(STDIN_FILENO))
    puts("");
}

s_buf * buf_readline_open_r (s_buf *buf, const char *prompt)
{
  s_buf_readline *buf_readline;
  assert(buf);
  buf_readline = malloc(sizeof(s_buf_readline));
  if (! buf_readline)
    errx(1, "buf_readline_open_r: out of memory");
  buf_init_1(&buf_readline->buf, "");
  buf_readline->eof = false;
  if (isatty(STDIN_FILENO))
    buf->refill = buf_readline_refill_readline;
  else
    buf->refill = buf_readline_refill_fgets;
  buf_readline->prompt = prompt;
  buf->user_ptr = buf_readline;
  using_history();
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

sw buf_readline_refill_readline (s_buf *buf)
{
  s_buf_readline *buf_readline;
  uw buf_readline_len;
  uw r;
  uw size;
  assert(buf);
  assert(buf->user_ptr);
  if (buf->rpos > buf->wpos ||
      buf->wpos > buf->size)
    return -1;
  size = buf->size - buf->wpos;
  if (size == 0)
    return 0;
  buf_readline = buf->user_ptr;
  if (buf_readline->eof)
    return -1;
  buf_readline_len = buf_readline->buf.wpos - buf_readline->buf.rpos;
  if (buf_readline_len == 0) {
    free(buf_readline->buf.ptr.p);
    if (! (buf_readline->buf.ptr.p = readline(buf_readline->prompt))) {
      buf_readline->eof = true;
      buf_readline->buf.rpos =
        buf_readline->buf.wpos =
        buf_readline->buf.size = 0;
      return -1;
    }
    add_history(buf_readline->buf.ptr.p);
    buf_readline_len = strlen(buf_readline->buf.ptr.p);
    buf_readline->buf.ptr.ps8[buf_readline_len++] = '\n';
    buf_readline->buf.size = buf_readline->buf.wpos = buf_readline_len;
    buf_readline->buf.rpos = 0;
  }
  if (buf_readline_len < size)
    size = buf_readline_len;
  if ((r = buf_xfer(buf, &buf_readline->buf, size)) != size)
    return -1;
  return size;
}
