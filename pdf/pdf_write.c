/* kc3
 * Copyright from 2022 to 2025 kmx.io <contact@kmx.io>
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
#include "../libkc3/kc3_main.h"
#include "pdf_write.h"

sw pdf_buf_write_bool (s_buf *buf, bool src)
{
  if (src) {
    return pdf_buf_write_token(buf, "true");
  }
  return pdf_buf_write_token(buf, "false");
}

/*sw pdf_buf_write_dictionnary (s_buf *buf, s_map *src)
{
  uw i;
  s_tag *tag;
  for (i = 0; i < src->count; i++) {
    pdf_buf_write_name(s_buf *buf, p_sym src)
  }
}*/

sw pdf_buf_write_float (s_buf *buf, f64 src)
{
  return buf_inspect_f64(buf, src);
}

sw pdf_buf_write_integer (s_buf *buf, const s_integer *x)
{
  return buf_inspect_integer(buf, x);
}

sw pdf_buf_write_name(s_buf *buf, p_sym src)
{
  sw r = 0;
  if ((r = buf_write_u8(buf, '/')) < 0
    || (r = buf_write_str(buf, &src->str)) < 0) {
    return r;
  }
  return r;
}

sw pdf_buf_write_null (s_buf *buf)
{
    return pdf_buf_write_token(buf, "null");
}

sw pdf_buf_write_string_hex (s_buf *buf, s_str *dest)
{
  u8 c;
  sw i;
  sw r = 0;
  s_buf read_buf = {0};
  const char *base = g_kc3_base_hexadecimal.ptr.pchar;
  buf_init_str_const(&read_buf, dest);

  if ((r = buf_write_1(buf, "<")) < 0) {
    goto cleanup;
  }
  for (i = 0; i < dest->size; i++) {
    if ((r = buf_read_u8(&read_buf, &c)) < 0
      || (r = buf_write_u8(buf, base[c & 0xf])) < 0
      || (r = buf_write_u8(buf, base[(c >> 4) & 0xf])) < 0) {
      goto cleanup;
    }
  }
  if ((r = buf_write_1(buf, ">")) < 0) {
    goto cleanup;
  }
 cleanup:
  buf_clean(&read_buf);
  return r;
}

sw pdf_buf_write_token(s_buf *buf, const char *pchar)
{
  return buf_write_1(buf, pchar);
}
