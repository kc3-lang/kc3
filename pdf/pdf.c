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
#include "pdf.h"
#include "pdf_buf_parse.h"

s_tag * pdf_parse_from_file (s_str *path, s_tag *dest)
{
  s_buf buf = {0};
  FILE *fp = NULL;
  s_tag tmp = {0};
  if (! (fp = file_open(path, "rb")))
    return NULL;
  if (! buf_file_open_r(&buf, fp)) {
    fclose(fp);
    return NULL;
  }
  if (pdf_buf_parse(&buf, &tmp) < 0) { 
    buf_file_close(&buf);
    fclose(fp);
    return NULL;
  }
  buf_file_close(&buf);
  fclose(fp);
  *dest = tmp;
  return dest;
}

s_tag * pdf_parse_from_str (s_str *str, s_tag *dest)
{
  s_buf buf = {0};
  s_tag tmp = {0};
  buf_init_str_const(&buf, str);
  if (pdf_buf_parse(&buf, &tmp) < 0)
    return NULL;
  *dest = tmp;
  return dest;
}
