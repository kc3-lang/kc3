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
#include "pdf.h"
#include "pdf_buf_parse.h"
#include "pdf_name.h"

bool pdf_character_is_delimiter (character c)
{
  return (c == '%' ||
          c == '/' ||
          c == '(' ||
          c == ')' ||
          c == '<' ||
          c == '>' ||
          c == '[' ||
          c == ']' ||
          character_is_space(c));
}

s_pdf_file * pdf_parse_from_file (s_str *path, s_pdf_file *dest)
{
  s_buf buf = {0};
  FILE *fp = NULL;
  s_pdf_file tmp = {0};
  if (! (fp = file_open(path, "rb")))
    return NULL;
  if (! buf_init_alloc(&buf, BUF_SIZE)) {
    err_puts("pdf_parse_from_file: buf_init_alloc");
    assert(! "pdf_parse_from_file: buf_init_alloc");
    fclose(fp);
    return NULL;
  }
  if (! buf_file_open_r(&buf, fp)) {
    err_puts("pdf_parse_from_file: buf_file_open_r");
    assert(! "pdf_parse_from_file: buf_file_open_r");
    buf_clean(&buf);
    fclose(fp);
    return NULL;
  }
  if (pdf_buf_parse_file(&buf, &tmp) <= 0) {
    err_puts("pdf_parse_from_file: pdf_buf_parse_file");
    assert(! "pdf_parse_from_file: pdf_buf_parse_file");
    buf_file_close(&buf);
    buf_clean(&buf);
    fclose(fp);
    return NULL;
  }
  buf_file_close(&buf);
  buf_clean(&buf);
  fclose(fp);
  *dest = tmp;
  return dest;
}

s_tag * pdf_parse_object_from_str (s_str *str,
                                   s_pdf_file *pdf_file,
                                   s_tag *dest)
{
  s_buf buf = {0};
  s_tag tmp = {0};
  buf_init_str_const(&buf, str);
  if (pdf_buf_parse_object(&buf, pdf_file, &tmp) < 0)
    return NULL;
  buf_clean(&buf);
  *dest = tmp;
  return dest;
}

s_tag * pdf_parse_from_str (s_str *str,
                            s_tag *dest)
{
  return pdf_parse_object_from_str(str, NULL, dest);
}

s_tag * pdf_tag_init_xref (s_tag *tag, u32 object_number,
                           u16 generation)
{
  s_tag tmp;
  assert(tag);
  if (! tag_init_tuple(&tmp, 3))
    return NULL;
  tag_init_psym(tmp.data.tuple.tag, sym_1("indirect_object"));
  tag_init_u32(tmp.data.tuple.tag + 1, object_number);
  tag_init_u16(tmp.data.tuple.tag + 2, generation);
  *tag = tmp;
  return tag;
}
