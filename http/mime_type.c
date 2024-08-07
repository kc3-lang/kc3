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
#include <libkc3/kc3.h>
#include "mime_type.h"

bool http_mime_type_buf_parse (s_buf *buf)
{
  sw r;
  if ((r = buf_ignore_spaces(buf)) < 0)
    return false;
  if ((r = buf_read_1(buf, "types")) <= 0)
    return false;
  if ((r = buf_ignore_spaces(buf)) < 0)
    return false;
  if ((r = buf_read_1(buf, "{")) <= 0)
    return false;
  while (1) {
    if (! http_mime_type_buf_parse_type(buf))
      return false;
    if ((r = buf_ignore_spaces(buf)) < 0)
      return false;
    if ((r = buf_read_1(buf, "}")) < 0)
      return false;
    if (r > 0)
      break;
  }
  return true;
}

bool http_mime_type_buf_parse_type (s_buf *buf)
{
  character c;
  const s_sym *ext;
  sw r;
  sw result = 0;
  s_buf_save save;
  s_str str;
  s_buf tmp;
  const s_sym *type;
  if ((r = buf_ignore_spaces(buf)) < 0)
    return false;
  if ((r = buf_read_until_space_into_str(buf, &str)) <= 0)
    return false;
  type = str_to_sym(&str);
  while (1) {
    if ((r = buf_ignore_spaces(buf)) <= 0)
      return false;
    buf_save_init(buf, &save);
    while (1) {
      if ((r = buf_peek_character_utf8(buf, &c)) < 0)
        goto restore;
      if (r && (character_is_space(c) || c == ';')) {
        buf_init(&tmp, false, buf->size, buf->ptr.pchar);
        tmp.rpos = save.rpos;
        tmp.wpos = buf->rpos;
        if (! buf_read_to_str(&tmp, &str)) {
          r = -1;
          goto restore;
        }
        r = result;
        break;
      }
      if ((r = buf_read_character_utf8(buf, &c)) <= 0)
        goto restore;
      result += r;
    }
    buf_save_clean(buf, &save);
    if (r > 0) {
      ext = str_to_sym(&str);
      if (! http_mime_type_def(type, ext))
        return false;
    }
    if ((r = buf_read_1(buf, ";")) < 0)
      return false;
    if (r > 0)
      break;
  }
  return true;
 restore:
  buf_save_restore_rpos(buf, &save);
  buf_save_clean(buf, &save);
  return false;
}

bool http_mime_type_def (const s_sym *mime_type, const s_sym *ext)
{
  err_write_1("http_mime_type_def: ");
  err_inspect_sym(&mime_type);
  err_write_1(" => ");
  err_inspect_sym(&ext);
  err_write_1("\n");
  return true;
}

bool http_mime_type_load (s_str *path)
{
  s_buf buf;
  FILE *fp;
  fp = file_open(path->ptr.pchar, "rb");
  if (! fp)
    return false;
  if (! buf_init_alloc(&buf, BUF_SIZE)) {
    fclose(fp);
    return false;
  }
  if (! buf_file_open_r(&buf, fp)) {
    buf_clean(&buf);
    fclose(fp);
    return false;
  }
  if (! http_mime_type_buf_parse(&buf)) {
    err_puts("http_mime_type_load: http_mime_type_buf_parse");
    err_inspect_buf(&buf);
    buf_file_close(&buf);
    buf_clean(&buf);
    fclose(fp);
    return false;
  }
  buf_file_close(&buf);
  buf_clean(&buf);
  fclose(fp);
  return true;
}
