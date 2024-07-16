/* kc3
 * Copyright 2022,2023,2024 kmx.io <contact@kmx.io>
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
#include "../libkc3/kc3.h"
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
  char i[BUF_SIZE];
  s_buf in;
  s_tag input;
  char o[BUF_SIZE];
  s_buf out;
  s_str path;
  sw r;
  s_tag result;
  if (argc < 1)
    return usage(argv[0]);
  if (! kc3_init(NULL, &argc, &argv))
    return 1;
  buf_init(&in, false, sizeof(i), i);
  buf_file_open_r(&in, stdin);
  buf_init(&out, false, sizeof(o), o);
  buf_file_open_w(&out, stdout);
  while (argc) {
    if (! strncmp("--load", *argv, 6) ||
        ! strncmp("-l", *argv, 2)) {
      if (argc < 2) {
        err_write_1("ikc3: ");
        err_write_1(*argv);
        err_write_1(" without an argument\n");
        assert(! "env_init: -l or --load without an argument");
        r = 1;
        goto clean;
      }
      str_init_1(&path, NULL, argv[1]);
      if (! env_load(&g_kc3_env, &path)) {
        r = 1;
        goto clean;
      }
      argc -= 2;
      argv += 2;
    }
    else if (argc == 1 && ! strcmp("--quit", *argv)) {
      r = 0;
      goto clean;
    }
    else
      break;
  }
  while ((r = buf_ignore_spaces(&in)) >= 0) {
    if ((r = buf_parse_tag(&in, &input)) > 0) {
      if (! eval_tag(&input, &result)) {
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
  }
  r = 0;
 clean:
  buf_readline_close(&in);
  buf_file_close(&out);
  kc3_clean(NULL);
  return r;
}

int usage (char *argv0)
{
  printf("Usage: %s\n", argv0);
  return 1;
}
