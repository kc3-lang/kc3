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
#include "../libkc3/kc3.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <readline.h>

typedef struct buf_wineditline {
  s_buf       buf;
  bool        eof;
  const char *prompt;
} s_buf_wineditline;

sw buf_wineditline_refill_fgets (s_buf *buf);
sw buf_wineditline_refill_wineditline (s_buf *buf);

void buf_wineditline_close (s_buf *buf, const char *history_path)
{
  assert(buf);
  if (history_path)
    write_history(history_path);
  buf->refill = NULL;
  alloc_free(buf->user_ptr);
  buf->user_ptr = NULL;
  if (isatty(STDIN_FILENO))
    puts("");
}

s_buf * buf_wineditline_open_r (s_buf *buf, const char *prompt,
                                const char *history_path)
{
  s_buf_wineditline *buf_wineditline;
  assert(buf);
  buf->line = 0;
  buf_wineditline = alloc(sizeof(s_buf_wineditline));
  if (! buf_wineditline)
    return NULL;
  buf_init(&buf_wineditline->buf, false, 1, "");
  buf_wineditline->eof = false;
  if (isatty(STDIN_FILENO))
    buf->refill = buf_wineditline_refill_wineditline;
  else
    buf->refill = buf_wineditline_refill_fgets;
  buf_wineditline->prompt = prompt;
  if (history_path) {
    using_history();
    read_history(history_path);
  }
  buf->user_ptr = buf_wineditline;
  return buf;
}

sw buf_wineditline_refill_fgets (s_buf *buf)
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

sw buf_wineditline_refill_wineditline (s_buf *buf)
{
  s_buf_wineditline *buf_wineditline;
  uw buf_wineditline_len;
  uw r;
  uw size;
  assert(buf);
  assert(buf->user_ptr);
  if (! buf->user_ptr) {
    err_puts("buf_wineditline_refill_wineditline: buf is not"
             " initialized");
    assert(!("buf_wineditline_refill_wineditline: buf is not"
             " initialized"));
    abort();
  }
  if (buf->rpos > buf->wpos ||
      buf->wpos > buf->size)
    return -1;
  size = buf->size - buf->wpos;
  if (size == 0)
    return 0;
  buf_wineditline = buf->user_ptr;
  if (buf_wineditline->eof)
    return -1;
  buf_wineditline_len =
    buf_wineditline->buf.wpos - buf_wineditline->buf.rpos;
  if (buf_wineditline_len == 0) {
    if (buf_wineditline->buf.free)
      alloc_free(buf_wineditline->buf.ptr.p);
    if (! (buf_wineditline->buf.ptr.p =
           readline(buf_wineditline->prompt))) {
      buf_wineditline->eof = true;
      buf_wineditline->buf.rpos =
        buf_wineditline->buf.wpos =
        buf_wineditline->buf.size = 0;
      return -1;
    }
    add_history(buf_wineditline->buf.ptr.p);
    buf_wineditline_len = strlen(buf_wineditline->buf.ptr.p);
    buf_wineditline->buf.ptr.ps8[buf_wineditline_len++] = '\n';
    buf_wineditline->buf.size = buf_wineditline->buf.wpos = buf_wineditline_len;
    buf_wineditline->buf.rpos = 0;
  }
  if (buf_wineditline_len < size)
    size = buf_wineditline_len;
  if ((r = buf_xfer(buf, &buf_wineditline->buf, size)) != size)
    return -1;
  return size;
}
