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
#include "mime_type.h"

const s_sym ** http_mime_type (const s_str *ext, const s_sym **dest)
{
  s_facts_cursor cursor;
  const s_sym *default_mime_type;
  s_fact *fact = NULL;
  s_tag tag_ext = {0};
  s_tag tag_mime_type_sym;
  s_tag tag_mime_type_value;
  if ((tag_ext.data.td_psym = sym_find(ext)))
    tag_ext.type = TAG_PSYM;
  tag_init_psym(&tag_mime_type_sym, sym_1("mime_type"));
  tag_init_pvar(&tag_mime_type_value, &g_sym_Sym);
  default_mime_type = sym_1("application/octet-stream");
  if (! facts_with_tags(env_global()->facts, &cursor, &tag_ext,
                        &tag_mime_type_sym, &tag_mime_type_value))
    goto default_mime_type;
  if (! facts_cursor_next(&cursor, &fact) ||
      ! fact ||
      ! tag_mime_type_value.data.td_pvar->tag.data.td_psym)
    goto default_mime_type;
  *dest = tag_mime_type_value.data.td_pvar->tag.data.td_psym;
  return dest;
 default_mime_type:
  *dest = default_mime_type;
  return dest;
}

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
  bool b;
  character c;
  s_tag ext;
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
  str_clean(&str);
  while (1) {
    if ((r = buf_ignore_spaces(buf)) <= 0)
      return false;
    buf_save_init(buf, &save);
    while (1) {
      if ((r = buf_peek_character_utf8(buf, &c)) < 0)
        goto restore;
      if (r && (character_is_space(c) || c == ';')) {
        buf_init(&tmp, false, buf->size, buf->ptr.p_pchar);
        tmp.rpos = save.rpos;
        tmp.wpos = buf->rpos;
        if (buf_read_to_str(&tmp, &str) <= 0) {
	  buf_clean(&tmp);
          r = -1;
          goto restore;
        }
	buf_clean(&tmp);
        r = result;
        break;
      }
      if ((r = buf_read_character_utf8(buf, &c)) <= 0)
        goto restore;
      result += r;
    }
    buf_save_clean(buf, &save);
    if (r > 0) {
      tag_init_psym(&ext, str_to_sym(&str));
      str_clean(&str);
      if (! http_mime_type_def(&ext, &type, &b) || ! b)
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

bool * http_mime_type_def (s_tag *ext, const s_sym * const *mime_type,
                           bool *dest)
{
  s_tag tag_mime_type_sym;
  s_tag tag_mime_type_value;
  assert(ext);
  assert(mime_type);
  tag_init_psym(&tag_mime_type_sym, sym_1("mime_type"));
  tag_init_psym(&tag_mime_type_value, *mime_type);
  if (! facts_replace_tags(env_global()->facts, ext,
                           &tag_mime_type_sym,
                           &tag_mime_type_value)) {
    *dest = false;
    return dest;
  }
  *dest = true;
  return dest;
}

bool * http_mime_type_load (s_str *path, bool *dest)
{
  s_buf buf;
  FILE *fp;
  fp = file_open(path, "rb");
  if (! fp)
    goto ko;
  if (! buf_init_alloc(&buf, BUF_SIZE)) {
    fclose(fp);
    goto ko;
  }
  if (! buf_file_open_r(&buf, fp)) {
    buf_clean(&buf);
    fclose(fp);
    goto ko;
  }
  if (! http_mime_type_buf_parse(&buf)) {
    err_puts("http_mime_type_load: http_mime_type_buf_parse");
    err_inspect_buf(&buf);
    buf_file_close(&buf);
    buf_clean(&buf);
    fclose(fp);
    goto ko;
  }
  buf_file_close(&buf);
  buf_clean(&buf);
  fclose(fp);
  *dest = true;
  return dest;
 ko:
  *dest = false;
  return dest;
}
