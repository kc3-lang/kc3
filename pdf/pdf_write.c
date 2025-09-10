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

sw pdf_buf_write_null (s_buf *buf)
{
    return pdf_buf_write_token(buf, "null");
}

sw pdf_buf_write_string_hex (s_buf *buf, s_str *dest)
{
  sw r = 0;
  u8 c;
  s_buf read_buf = {0};
  const char *base = g_kc3_base_hexadecimal.ptr.pchar;
  buf_init_str_const(&read_buf, dest);
  for (sw i = 0; i < dest->size; i++) {
    if ((r = buf_read_u8(&read_buf, &c)) < 0
      || (r = buf_write_u8(buf, base[c & 0xf])) < 0
      || (r = buf_write_u8(buf, base[(c >> 4) & 0xf])) < 0) {
      break;
    }
  }
  buf_clean(&read_buf);
  return r;
}

sw pdf_buf_write_token(s_buf *buf, const char *pchar)
{
  return buf_write_1(buf, pchar);
}
