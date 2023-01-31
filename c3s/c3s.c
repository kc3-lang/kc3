/* c3
 * Copyright 2022,2023 kmx.io <contact@kmx.io>
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
#include "../libc3/c3.h"
#include "buf_readline.h"

#define BUFSZ 0x10000

int usage (char *argv0);

sw buf_ignore_character (s_buf *buf)
{
  u8 b;
  character c = 0;
  sw csize;
  sw r;
  if ((r = buf_peek_character_utf8(buf, &c)) > 0)
    csize = r;
  else if ((r = buf_peek_u8(buf, &b)) > 0)
    csize = 1;
  else
    return r;
  if ((r = buf_ignore(buf, csize)) < 0)
    return r;
  return csize;
}

sw buf_xfer_spaces (s_buf *out, s_buf *in)
{
  character c;
  sw csize;
  sw r;
  sw size = 0;
  assert(in);
  assert(out);
  while ((r = buf_peek_character_utf8(in, &c)) > 0 &&
         c >= 0 &&
         c < UCD_MAX &&
         g_ucd[c].flags & (UCD_OTHER_CONTROL | UCD_SEPARATOR_SPACE)) {
    csize = r;
    if ((r = buf_xfer(out, in, csize)) != csize)
      return -1;
    size += csize;
    if ((r = buf_flush(out)) < 0)
      return -1;
  }
  if (r < 0)
    return r;
  return size;
}

int main (int argc, char **argv)
{
  s8 i[BUF_SIZE];
  s_buf in;
  s_tag input;
  s8 o[BUF_SIZE];
  s_buf out;
  sw r;
  s_tag result;
  c3_init(NULL);
  if (argc < 1)
    return usage(argv[0]);
  buf_init(&in, false, sizeof(i), i);
  buf_file_open_r(&in, stdin);
  buf_init(&out, false, sizeof(o), o);
  buf_file_open_w(&out, stdout);
  while ((r = buf_xfer_spaces(&out, &in)) >= 0) {
    if ((r = buf_parse_tag(&in, &input)) > 0) {
      if (! eval_tag(&result, &input)) {
        tag_clean(&input);
        continue;
      }
      if (buf_inspect_tag(&out, &result) < 0) {
	tag_clean(&input);
	tag_clean(&result);
        break;
      }
      tag_clean(&input);
      tag_clean(&result);
      buf_write_u8(&out, '\n');
      if ((r = buf_flush(&out)) < 0)
        break;
    }
    if (r < 0 ||
        (r == 0 &&
         (r = buf_ignore_character(&in)) <= 0))
      break;
    if ((r = buf_refill_compact(&in)) < 0)
      break;
  }
  buf_readline_close(&in);
  buf_file_close(&out);
  c3_clean(NULL);
  return 0;
}

int usage (char *argv0)
{
  printf("Usage: %s\n", argv0);
  return 1;
}
