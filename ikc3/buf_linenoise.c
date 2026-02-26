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
#include <string.h>
#include <unistd.h>
#include "../libkc3/kc3.h"
#include "../linenoise/linenoise.h"

typedef struct buf_linenoise {
  s_buf buf;
  bool eof;
  const char *prompt;
} s_buf_linenoise;

sw buf_linenoise_refill_fgets (s_buf *buf);
sw buf_linenoise_refill_linenoise (s_buf *buf);

void buf_linenoise_close (s_buf *buf, const char *history_path)
{
  s_buf_linenoise *buf_linenoise;
  assert(buf);
  buf_linenoise = buf->user_ptr;
  if (history_path)
    linenoiseHistorySave(history_path);
  buf->refill = NULL;
  buf_clean(&buf_linenoise->buf);
  alloc_free(buf->user_ptr);
  buf->user_ptr = NULL;
  if (isatty(STDIN_FILENO))
    puts("");
}

s_buf * buf_linenoise_open_r (s_buf *buf, const char *prompt,
                              const char *history_path)
{
  s_buf_linenoise *buf_linenoise;
  assert(buf);
  buf->line = 0;
  buf_linenoise = alloc(sizeof(s_buf_linenoise));
  if (! buf_linenoise) {
    err_puts("buf_linenoise_open_r: failed to allocate memory");
    return NULL;
  }
  buf_init(&buf_linenoise->buf, false, 1, "");
  buf_linenoise->eof = false;
  if (isatty(STDIN_FILENO))
    buf->refill = buf_linenoise_refill_linenoise;
  else
    buf->refill = buf_linenoise_refill_fgets;
  buf_linenoise->prompt = prompt;
  if (history_path)
    linenoiseHistoryLoad(history_path);
  buf->user_ptr = buf_linenoise;
  return buf;
}

sw buf_linenoise_refill_fgets (s_buf *buf)
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

sw buf_linenoise_refill_linenoise (s_buf *buf)
{
  s_buf_linenoise *buf_linenoise;
  uw buf_linenoise_len;
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
  buf_linenoise = buf->user_ptr;
  if (buf_linenoise->eof)
    return -1;
  buf_linenoise_len = buf_linenoise->buf.wpos - buf_linenoise->buf.rpos;
  if (buf_linenoise_len == 0) {
    if (buf_linenoise->buf.free)
      alloc_free(buf_linenoise->buf.ptr.p);
    if (! (buf_linenoise->buf.ptr.p = linenoise(buf_linenoise->prompt))) {
      buf_linenoise->eof = true;
      buf_linenoise->buf.rpos =
        buf_linenoise->buf.wpos =
        buf_linenoise->buf.size = 0;
      return -1;
    }
    linenoiseHistoryAdd(buf_linenoise->buf.ptr.p);
    buf_linenoise_len = strlen(buf_linenoise->buf.ptr.p);
    buf_linenoise->buf.ptr.pchar[buf_linenoise_len++] = '\n';
    buf_linenoise->buf.size = buf_linenoise->buf.wpos = buf_linenoise_len;
    buf_linenoise->buf.rpos = 0;
  }
  if (buf_linenoise_len < size)
    size = buf_linenoise_len;
  if ((r = buf_xfer(buf, &buf_linenoise->buf, size)) != size)
    return -1;
  return size;
}
