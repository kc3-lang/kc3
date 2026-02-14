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
#include "pdf_buf_parse.h"
#include "pdf_file.h"
#include "pdf_name.h"
#include <stdlib.h>

sw pdf_buf_ignore_until_token (s_buf *buf, const char *token)
{
  sw r;
  sw result = 0;
  assert(buf);
  assert(token);
  while (1) {
    if ((r = pdf_buf_parse_token(buf, token)) < 0)
      return r;
    if (r) {
      result += r;
      break;
    }
    if ((r = buf_ignore(buf, 1)) <= 0)
      return r;
    result += r;
  }
  return result;
}

sw pdf_buf_parse_object (s_buf *buf, s_pdf_file *pdf_file,
                         s_tag *dest)
{
  sw r;
  sw result = 0;
  s_tag tmp = {0};
  if ((r = buf_ignore_spaces(buf)) < 0)
    return r;
  result += r;
  if ((r = buf_parse_comments(buf)) < 0)
    goto ok;
  result += r;
  if ((r = pdf_buf_parse_indirect_object(buf, pdf_file,
                                         &tmp.data.ptuple)) > 0) {
    result += r;
    tmp.type = TAG_PTUPLE;
    goto ok;
  }
  if ((r = pdf_buf_parse_bool(buf, &tmp.data.bool_)) > 0) {
    result += r;
    tmp.type = TAG_BOOL;
    goto ok;
  }
  if ((r = pdf_buf_parse_null(buf, &tmp)) > 0) {
    result += r;
    goto ok;
  }
  if ((r = pdf_buf_parse_number(buf, &tmp)) > 0) {
    result += r;
    goto ok;
  }
  if ((r = pdf_buf_parse_dictionnary(buf, pdf_file,
                                     &tmp.data.map)) > 0) {
    result += r;
    tmp.type = TAG_MAP;
    goto ok;
  }
  if ((r = pdf_buf_parse_string(buf, &tmp)) > 0) {
    result += r;
    goto ok;
  }
  if ((r = pdf_buf_parse_name(buf, pdf_file, &tmp.data.psym)) > 0) {
    tmp.type = TAG_PSYM;
    result += r;
    goto ok;
  }
  if ((r = pdf_buf_parse_array(buf, pdf_file, &tmp.data.plist)) > 0) {
    tmp.type = TAG_PLIST;
    result += r;
    goto ok;
  }
 ok:
  if (result)
    *dest = tmp;
  return result;
}

sw pdf_buf_parse_array (s_buf *buf, s_pdf_file *pdf_file,
                        p_list *dest)
{
  bool end;
  sw r;
  sw result = 0;
  p_list *tail;
  p_list tmp = NULL;
  s_buf_save save = {0};
  assert(buf);
  assert(dest);
  buf_save_init(buf, &save);
  if ((r = buf_read_1(buf, "[")) <= 0)
    goto clean;
  result += r;
  tail = &tmp;
  while (1) {
    if ((r = buf_read_1(buf, "]")) < 0)
      goto restore;
    result += r;
    if (r) {
      result += r;
      if ((r = pdf_buf_parse_object_end(buf, &end)) > 0)
        result += r;
      if (! end)
        goto restore;
      break;
    }
    if (! (*tail = list_new(NULL))) {
      r = -2;
      goto restore;
    }
    if ((r = pdf_buf_parse_object(buf, pdf_file, &(*tail)->tag)) <= 0)
      goto restore;
    result += r;
    tail = &(*tail)->next.data.plist;
  }
  *dest = tmp;
  r = result;
  goto clean;
 restore:
  list_delete_all(tmp);
  buf_save_restore_rpos(buf, &save);
 clean:
  buf_save_clean(buf, &save);
  return r;
}

sw pdf_buf_parse_bool (s_buf *buf, bool *dest)
{
  bool end;
  sw r;
  sw result = 0;
  s_buf_save save = {0};
  bool tmp;
  assert(buf);
  assert(dest);
  buf_save_init(buf, &save);
  if ((r = buf_read_1(buf, "true")) > 0)
    tmp = true;
  else if ((r = buf_read_1(buf, "false")) > 0)
    tmp = false;
  else
    goto clean;
  result += r;
  if ((r = pdf_buf_parse_object_end(buf, &end)) > 0)
    result += r;
  if (! end) {
    r = 0;
    goto restore;
  }
  result += r;
  *dest = tmp;
  r = result;
  goto clean;
 restore:
  buf_save_restore_rpos(buf, &save);
 clean:
  buf_save_clean(buf, &save);
  return r;
}

sw pdf_buf_parse_comment (s_buf *buf)
{
  sw r;
  sw result = 0;
  s_str str = {0};
  assert(buf);
  if ((r = buf_read_1(buf, "%")) <= 0)
    return r;
  result += r;
  if ((r = buf_read_until_1_into_str(buf, "\n", &str)) < 0)
    return result;
  str_clean(&str);
  return result;
}

sw pdf_buf_parse_comments (s_buf *buf)
{
  sw r1 = 1;
  sw r2 = 1;
  sw result = 0;
  assert(buf);
  while (r1 | r2) {
    if ((r1 = pdf_buf_parse_comment(buf)) < 0)
      return r1;
    result += r1;
    if ((r2 = buf_ignore_spaces(buf)) < 0)
      return r2;
    result += r2;
  }
  return result;
}

sw pdf_buf_parse_dictionnary (s_buf *buf, s_pdf_file *pdf_file,
                              s_map *dest)
{
  p_list  keys = NULL;
  p_list *keys_tail = &keys;
  const s_sym *name;
  sw r;
  sw result = 0;
  s_buf_save save;
  s_map tmp = {0};
  p_list  values = NULL;
  p_list *values_tail = &values;
  buf_save_init(buf, &save);
  if ((r = buf_read_1(buf, "<<")) <= 0)
    goto clean;
  result += r;
  if ((r = buf_ignore_spaces(buf)) < 0)
    goto clean;
  result += r;
  while (1) {
    if ((r = buf_read_1(buf, ">>")) < 0)
      goto clean;
    if (r) {
      result += r;
      if ((r = buf_ignore_spaces(buf)) > 0)
        result += r;
      goto ok;
    }
    if ((r = pdf_buf_parse_name(buf, pdf_file, &name)) <= 0)
      goto clean;
    result += r;
    if (! (*keys_tail = list_new_psym(name, NULL))) {
      r = -1;
      goto clean;
    }
    if (! (*values_tail = list_new(NULL))) {
      r = -1;
      goto clean;
    }
    if ((r = pdf_buf_parse_object(buf, pdf_file, &(*values_tail)->tag)) <= 0)
      goto clean;
    result += r;
    keys_tail = &(*keys_tail)->next.data.plist;
    values_tail = &(*values_tail)->next.data.plist;
  }
 ok:
  if (! map_init_from_lists(&tmp, keys, values)) {
    err_puts("pdf_buf_parse_dictionnary: map_init_from_lists");
    assert(! "pdf_buf_parse_dictionnary: map_init_from_lists");
    r = -1;
    goto restore;
  }
  *dest = tmp;
  r = result;
  goto clean;
 restore:
  buf_save_restore_rpos(buf, &save);
 clean:
  list_delete_all(values);
  list_delete_all(keys);
  buf_save_clean(buf, &save);
  return r;
}

sw pdf_buf_parse_file (s_buf *buf, s_pdf_file *dest)
{
  sw r;
  sw result = 0;
  s_pdf_file tmp = {0};
  if ((r = pdf_buf_parse_file_header(buf, &tmp.header)) <= 0) {
    err_puts("pdf_buf_parse_file: pdf_buf_parse_file_header");
    assert(! "pdf_buf_parse_file: pdf_buf_parse_file_header");
    return r;
  }
  result += r;
  if ((r = pdf_buf_parse_trailer(buf, NULL, &tmp.trailer)) <= 0) {
    err_puts("pdf_buf_parse_file: pdf_buf_parse_trailer");
    assert(! "pdf_buf_parse_file: pdf_buf_parse_trailer");
    return r;
  }
  result += r;
  if ((r = buf_seek(buf, tmp.trailer.startxref, SEEK_SET)) < 0 ||
      (u64) r != tmp.trailer.startxref) {
    err_puts("pdf_buf_parse_file: buf_seek");
    assert(! "pdf_buf_parse_file: buf_seek");
    return -1;
  }
  if ((r = pdf_buf_parse_xref(buf, &tmp.xref)) <= 0) {
    err_puts("pdf_buf_parse_file: pdf_buf_parse_xref f");
    assert(! "pdf_buf_parse_file: pdf_buf_parse_xref");
    pdf_file_clean(&tmp);
    return -1;
  }
  if ((r = pdf_buf_parse_file_body(buf, &tmp)) <= 0) {
    err_puts("pdf_buf_parse_file: pdf_buf_parse_file_body");
    assert(! "pdf_buf_parse_file: pdf_buf_parse_file_body");
    pdf_file_clean(&tmp);
    return -1;
  }
  result += r;
  *dest = tmp;
  return result;
}

sw pdf_buf_parse_file_body (s_buf *buf, s_pdf_file *pdf_file)
{
  uw i;
  s_map tmp = {0};
  p_tuple tmp_tuple = NULL;
  s64 offset;
  sw r;
  sw result = 0;
  p_tuple tuple = NULL;
  assert(buf);
  assert(pdf_file);
  if (! map_init(&tmp, pdf_file->xref.count)) {
    err_puts("pdf_buf_parse_file_body: map_init");
    assert(! "pdf_buf_parse_file_body: map_init");
    return -1;
  }
  i = 0;
  while (i < pdf_file->xref.count) {
    if (false) {
      err_write_1("pdf_buf_parse_file_body: i=");
      err_inspect_uw_decimal(i);
      err_write_1(" key=");
      err_inspect_tag(pdf_file->xref.key + i);
      err_write_1("\n");
    }
    if (! tag_init_copy(tmp.key + i, pdf_file->xref.key + i)) {
      err_puts("pdf_buf_parse_file_body: tag_init_copy key");
      assert(! "pdf_buf_parse_file_body: tag_init_copy key");
      goto clean;
    }
    if (pdf_file->xref.value[i].type == TAG_U64) {
      offset = pdf_file->xref.value[i].data.u64;
      if (false) {
        err_write_1("pdf_buf_parse_file_body: offset=");
        err_inspect_s64_decimal(offset);
        err_write_1("\n");
      }
      if (buf_seek(buf, offset, SEEK_SET) != offset) {
        err_puts("pdf_buf_parse_file_body: buf_seek");
        assert(! "pdf_buf_parse_file_body: buf_seek");
        goto clean;
      }
      if ((r = pdf_buf_parse_indirect_object(buf, pdf_file,
                                             &tmp_tuple)) <= 0) {
        err_write_1("pdf_buf_parse_file_body: failed to parse indirect"
                    " object ");
        err_inspect_u32_decimal(tmp.key[i].data.ptuple->tag[0].data.u32);
        err_write_1(" ");
        err_inspect_u16_decimal(tmp.key[i].data.ptuple->tag[1].data.u16);
        err_write_1(" at offset ");
        err_inspect_s64_decimal(offset);
        err_write_1(" , r = ");
        err_inspect_sw_decimal(r);
        err_write_1("\n");
        err_inspect_buf(buf);
        if (! tag_init_ptuple(tmp.value + i, 2) ||
            ! (tuple = tmp.value[i].data.ptuple) ||
            ! tag_init_psym(tuple->tag, &g_sym_error) ||
            ! tag_init_str_inspect_buf(tuple->tag + 1, buf))
          goto clean;
      }
      else {
        result += r;
        if (tmp_tuple->count != 4) {
          err_puts("pdf_buf_parse_file_body:"
                   " pdf_buf_parse_indirect_object count != 4)");
          assert(!("pdf_buf_parse_file_body:"
                   " pdf_buf_parse_indirect_object count != 4)"));
          tuple_delete(tmp_tuple);
          goto clean;
        }
        if (! tag_init_copy(tmp.value + i, tmp_tuple->tag + 3)) {
          err_puts("pdf_buf_parse_file_body: tag_init_copy value");
          assert(! "pdf_buf_parse_file_body: tag_init_copy value");
          tuple_delete(tmp_tuple);
          goto clean;
        }
        tuple_delete(tmp_tuple);
      }
    }
    i++;
  }
  pdf_file->body = tmp;
  return result;
 clean:
  map_clean(&tmp);
  return -1;
}

sw pdf_buf_parse_file_header (s_buf *buf, s_str *dest)
{
  u16 i;
  sw r;
  sw result = 0;
  s_buf_save save;
  s_str tmp = {0};
  buf_save_init(buf, &save);
  if ((r = buf_read_1(buf, "%PDF-")) <= 0) 
    goto clean;
  result += r;
  if ((r = buf_parse_u16_decimal(buf, &i)) <= 0 || i < 1) {
    err_puts("pdf_buf_parse_file_header: buf_parse_u16_decimal 1");
    assert(! "pdf_buf_parse_file_header: buf_parse_u16_decimal 1");
    goto restore;
  }
  result += r;
  if ((r = buf_read_1(buf, ".")) <= 0) {
    err_puts("pdf_buf_parse_file_header: buf_read_1 2");
    assert(! "pdf_buf_parse_file_header: buf_read_1 2");
    goto restore;
  }
  result += r;
  if ((r = buf_parse_u16_decimal(buf, &i)) <= 0) {
    err_puts("pdf_buf_parse_file_header: buf_parse_u16_decimal");
    assert(! "pdf_buf_parse_file_header: buf_parse_u16_decimal");
    goto restore;
  }
  result += r;
  if (! str_init_alloc_copy(&tmp, result, buf->ptr.pchar + save.rpos)) {
    r = -1;
    goto clean;
  }
  *dest = tmp;
  r = result;
  goto clean;
 restore:
  buf_save_restore_rpos(buf, &save);
 clean:
  buf_save_clean(buf, &save);
  return r;
}

sw pdf_buf_parse_float (s_buf *buf, f64 *dest)
{
  u8 digit;
  bool end;
  f64 i;
  bool negative = false;
  sw r;
  sw result = 0;
  s_buf_save save;
  f64 tmp;
  assert(buf);
  assert(dest);
  buf_save_init(buf, &save);
  if ((r = buf_read_1(buf, "+")) < 0)
    goto clean;
  if (r)
    result += r;
  else {
    if ((r = buf_read_1(buf, "-")) < 0)
      goto clean;
    if (r) {
      negative = true;
      result += r;
    }
  }
  if ((r = buf_parse_digit_dec(buf, &digit)) <= 0)
    goto restore;
  result += r;
  tmp = digit;
  while ((r = buf_parse_digit_dec(buf, &digit)) > 0) {
    result += r;
    tmp = tmp * 10 + digit;
  }
  if (r < 0 ||
      (r = buf_read_1(buf, ".")) <= 0)
    goto ok;
  result += r;
  i = 10;
  while ((r = buf_parse_digit_dec(buf, &digit)) > 0) {
    result += r;
    tmp += (f32) digit / i;
    i *= 10;
  }
  if ((r = pdf_buf_parse_object_end(buf, &end)) > 0)
    result += r;
  if (! end) {
    r = -1;
    goto restore;
  }
  result += r;
 ok:
  if (negative)
    tmp = -tmp;
  *dest = tmp;
  r = result;
  goto clean;
 restore:
  buf_save_restore_rpos(buf, &save);
 clean:
  buf_save_clean(buf, &save);
  return r;
}

sw pdf_buf_parse_indirect_object (s_buf *buf,
                                  s_pdf_file *pdf_file,
                                  p_tuple *dest)
{
  s_tag generation_number = {0};
  s_tag object_number = {0};
  sw r;
  sw result = 0;
  s_buf_save save = {0};
  p_sym sym_indirect_object = sym_1("indirect_object");
  p_sym sym_U16 = &g_sym_U16;
  p_sym sym_U32 = &g_sym_U32;
  p_tuple tmp = NULL;
  buf_save_init(buf, &save);
  if ((r = pdf_buf_parse_integer(buf, &object_number)) <= 0)
    goto restore;
  result += r;
  if ((r = pdf_buf_parse_integer(buf, &generation_number)) <= 0)
    goto restore;
  result += r;
  if ((r = pdf_buf_parse_token(buf, "obj")) < 0)
    goto restore;
  if (r) {
    result += r;
    buf_save_clean(buf, &save);
    if (! (tmp = tuple_new(4))) {
      r = -1;
      goto ko;
    }
    if ((r = pdf_buf_parse_stream(buf, pdf_file, tmp->tag + 3)) < 0)
      goto ko;
    if (! r &&
        (r = pdf_buf_parse_object(buf, pdf_file, tmp->tag + 3)) <= 0)
      goto ko;
    result += r;
    goto ok;
  }
  if ((r = pdf_buf_parse_token(buf, "R")) < 0)
    goto restore;
  if (r) {
    result += r;
    buf_save_clean(buf, &save);
    if (! (tmp = tuple_new(3))) {
      r = -1;
      goto ko;
    }
    goto ok;
  }
  goto restore;
 ok:
  tag_init_psym(tmp->tag, sym_indirect_object);
  tmp->tag[1].type = TAG_U32;
  if (! u32_init_cast(&tmp->tag[1].data.u32, &sym_U32, &object_number)) {
    err_puts("pdf_buf_parse_indirect_object: invalid object number");
    assert(! "pdf_buf_parse_indirect_object: invalid object number");
    r = -1;
    goto ko;
  }
  tag_void(&object_number);
  tmp->tag[2].type = TAG_U16;
  if (! u16_init_cast(&tmp->tag[2].data.u16, &sym_U16,
                      &generation_number)) {
    err_puts("pdf_buf_parse_indirect_object: invalid generation"
             " number");
    assert(!("pdf_buf_parse_indirect_object: invalid generation"
             " number"));
    r = -1;
    goto ko;
  }
  tag_void(&generation_number);
  *dest = tmp;
  return result;
 restore:
  tuple_delete(tmp);
  tag_clean(&object_number);
  tag_clean(&generation_number);
  buf_save_restore_rpos(buf, &save);
  buf_save_clean(buf, &save);
  return r;
 ko:
  tuple_delete(tmp);
  tag_clean(&object_number);
  tag_clean(&generation_number);
  return r;
}

sw pdf_buf_parse_integer (s_buf *buf, s_tag *dest)
{
  bool end;
  bool negative = false;
  sw r;
  sw result = 0;
  s_buf_save save = {0};
  s_tag tmp = {0};
  assert(buf);
  assert(dest);
  buf_save_init(buf, &save);
  if ((r = buf_read_1(buf, "+")) < 0)
    goto clean;
  if (! r) {
    if ((r = buf_read_1(buf, "-")) < 0)
      goto clean;
    if (r)
      negative = true;
  }
  result += r;
  if ((r = buf_parse_integer_decimal(buf, negative,
                                     &tmp.data.integer)) <= 0)
    goto restore;
  result += r;
  if ((r = pdf_buf_parse_object_end(buf, &end)) > 0)
    result += r;
  if (! end) {
    integer_clean(&tmp.data.integer);
    r = -1;
    goto restore;
  }
  tmp.type = TAG_INTEGER;
  if (! tag_integer_reduce(&tmp, dest)) {
    integer_clean(&tmp.data.integer);
    r = -1;
    goto clean;
  }
  integer_clean(&tmp.data.integer);
  r = result;
  goto clean;
 restore:
  buf_save_restore_rpos(buf, &save);
 clean:
  buf_save_clean(buf, &save);
  return r;
}

sw pdf_buf_parse_name (s_buf *buf, s_pdf_file *pdf_file,
                       p_pdf_name *dest)
{
  char a[BUF_SIZE];
  character c;
  u8 d;
  u8 d1;
  bool end;
  p_pdf_name_list *name_list = NULL;
  sw r;
  sw result = 0;
  s_buf_save save = {0};
  s_str str = {0};
  p_pdf_name tmp = NULL;
  s_buf      tmp_buf = {0};
  assert(buf);
  assert(dest);
  buf_save_init(buf, &save);
  if ((r = buf_read_1(buf, "/")) <= 0)
    goto clean;
  result += r;
  buf_init(&tmp_buf, false, sizeof(a) - 1, a);
  while (1) {
    if ((r = pdf_buf_parse_object_end(buf, &end)) > 0)
      result += r;
    if (end)
      goto ok;
    if ((r = buf_read_character_utf8(buf, &c)) <= 0)
      break;
    result += r;
    if (c == '#') {
      if ((r = buf_read_character_utf8(buf, &c)) <= 0)
        break;
      result += r;
      if (c >= '0' && c <= '9')
        d = c - '0';
      else if (c >= 'A' && c <= 'F')
        d = c - 'A';
      else if (c >= 'a' && c <= 'f')
        d = c - 'a';
      else {
        r = 0;
        goto restore;
      }
      if ((r = buf_read_character_utf8(buf, &c)) <= 0)
        break;
      result += r;
      if (c >= '0' && c <= '9')
        d1 = c - '0';
      else if (c >= 'A' && c <= 'F')
        d1 = c - 'A';
      else if (c >= 'a' && c <= 'f')
        d1 = c - 'a';
      else {
        r = 0;
        goto restore;
      }
      c = d * 0x10 + d1;
    }
    if ((r = buf_write_character_utf8(&tmp_buf, c)) <= 0)
      goto restore;
  }
 ok:
  if (! buf_read_to_str(&tmp_buf, &str)) {
    r = -2;
    goto clean;
  }
  name_list = pdf_file ? &pdf_file->name_list : &g_pdf_name_list;
  if (! (tmp = pdf_name_from_str(name_list, &str))) {
    r = -2;
    str_clean(&str);
    goto clean;
  }
  str_clean(&str);
  *dest = tmp;
  r = result;
  goto clean;
 restore:
  buf_save_restore_rpos(buf, &save);
 clean:
  buf_clean(&tmp_buf);
  buf_save_clean(buf, &save);
  return r;
}

sw pdf_buf_parse_null (s_buf *buf, s_tag *dest)
{
  sw r;
  if ((r = pdf_buf_parse_token(buf, "null")) > 0)
    tag_init_void(dest);
  return r;
}

sw pdf_buf_parse_number (s_buf *buf, s_tag *dest)
{
  sw r;
  s_tag tmp = {0};
  assert(buf);
  assert(dest);
  if ((r = pdf_buf_parse_integer(buf, &tmp)) > 0)
    goto ok;
  if ((r = pdf_buf_parse_float(buf, &tmp.data.f64)) > 0) {
    tmp.type = TAG_F64;
    goto ok;
  }
  return 0;
 ok:
  *dest = tmp;
  return r;
}

sw pdf_buf_parse_object_end (s_buf *buf, bool *end)
{
  character c;
  sw r;
  sw result = 0;
  if ((r = buf_ignore_spaces(buf)) < 0)
    goto end;
  result += r;
  if ((r = pdf_buf_parse_comments(buf)) < 0)
    goto end;
  result += r;
  if ((r = buf_peek_character_utf8(buf, &c)) < 0)
    goto end;
  if (r && pdf_character_is_delimiter(c))
    goto end;
  *end = result > 0 ? true : false;
  if (r < 0)
    return r;
  return result;
 end:
  *end = true;
  if (r < 0)
    return r;
  return result;
}

sw pdf_buf_parse_rewind_to_trailer (s_buf *buf)
{
  bool end;
  sw pos;
  sw r;
  sw result = 0;
  assert(buf);
  if (false) {
    err_write_1("pdf_buf_parse_rewind_to_trailer: rpos = ");
    err_inspect_sw_decimal(buf->rpos);
    err_write_1("\n");
  }
  if (buf->rpos < 9)
    return -1;
  buf->rpos -= 9;
  while (1) {
    if (! buf->rpos)
      return -1;
    buf->rpos--;
    pos = buf->rpos;
    if ((r = pdf_buf_parse_object_end(buf, &end)) > 0 &&
        (result += r) &&
        end &&
        (r = pdf_buf_parse_token(buf, "trailer")) > 0) {
      result += r;
      return result;
    }
    buf->rpos = pos;
  }
  return -1;
}
    
sw pdf_buf_parse_stream (s_buf *buf, s_pdf_file *pdf_file, s_tag *dest)
{
  s_map map = {0};
  p_pdf_name_list *name_list = NULL;
  s_tag name_tag = {0};
  s64 offset;
  s_pdf_stream *pdf_stream;
  u64           pdf_stream_offset;
  sw r;
  sw result = 0;
  s_buf_save save;
  s64   size = 0;
  s_tag size_tag = {0};
  s_tag size_tmp = {0};
  const p_sym sym_PDF_Stream = sym_1("PDF.Stream");
  const p_sym sym_S64 = &g_sym_S64;
  s_tag tmp = {0};
  assert(buf);
  assert(dest);
  buf_save_init(buf, &save);
  if ((r = pdf_buf_parse_dictionnary(buf, pdf_file, &map)) <= 0)
    goto clean;
  result += r;
  if ((r = buf_read_1(buf, "stream")) <= 0) {
    map_clean(&map);
    goto restore;
  }
  result += r;
  if ((r = buf_read_1(buf, "\r")) < 0) {
    map_clean(&map);
    goto restore;
  }
  result += r;
  if ((r = buf_read_1(buf, "\n")) <= 0) {
    map_clean(&map);
    goto restore;
  }
  result += r;
  buf_save_clean(buf, &save);
  if (! tag_init_pstruct(&tmp, sym_PDF_Stream) ||
      ! struct_allocate(tmp.data.pstruct) ||
      ! (pdf_stream = tmp.data.pstruct->data)) {
    err_puts("pdf_buf_parse_stream: failed to init pstruct");
    assert(! "pdf_buf_parse_stream: failed to init pstruct");
    map_clean(&map);
    goto ko;
  }
  pdf_stream->dictionnary = map;
  if (! buf_tell_r(buf, &pdf_stream_offset)) {
    err_puts("pdf_buf_parse_stream: buf_tell_r");
    assert(! "pdf_buf_parse_stream: buf_tell_r");
    goto ko;
  }
  pdf_stream->offset = pdf_stream_offset;
  name_list = pdf_file ? &pdf_file->name_list : &g_pdf_name_list;
  name_tag.type = TAG_PSYM;
  name_tag.data.psym = pdf_name_1(name_list, "Length");
  if (map_get(&pdf_stream->dictionnary, &name_tag, &size_tag)) {
    if (size_tag.type == TAG_PTUPLE &&
        size_tag.data.ptuple->count == 3 &&
        size_tag.data.ptuple->tag[0].type == TAG_PSYM &&
        size_tag.data.ptuple->tag[0].data.psym == sym_1("indirect_object")) {
      if (! pdf_file_get_indirect_object(pdf_file, &size_tag, &size_tmp)) {
        err_puts("pdf_buf_parse_stream: pdf_file_get_indirect_object");
        assert(! "pdf_buf_parse_stream: pdf_file_get_indirect_object");
        tag_clean(&size_tag);
        goto ko;
      }
      tag_clean(&size_tag);
      size_tag = size_tmp;
    }
    s64_init_cast(&size, &sym_S64, &size_tag); // XXX soft fail
    tag_clean(&size_tag);
  }
  pdf_stream->length = size;
  if (! size) {
    if (false) {
      err_write_1("pdf_buf_parse_stream: zero Length: ");
      err_inspect_map(&pdf_stream->dictionnary);
      err_write_1("\n");
    }
    if ((r = pdf_buf_ignore_until_token(buf, "endstream")) <= 0) {
      err_puts("pdf_buf_parse_stream: endstream:"
               " pdf_buf_ignore_until_token");
      assert(!("pdf_buf_parse_stream: endstream:"
               " pdf_buf_ignore_until_token"));
      goto ko;
    }
  }
  else {
    if (false) {
      err_write_1("pdf_buf_parse_stream: Length = ");
      err_inspect_s64_decimal(size);
      err_write_1(", offset = ");
      err_inspect_s64_decimal(pdf_stream->offset);
      err_write_1("\n");
    }
    offset = pdf_stream->offset + size;
    if (buf_seek(buf, offset, SEEK_SET) != offset) {
      err_puts("pdf_buf_parse_stream: buf_seek");
      assert(! "pdf_buf_parse_stream: buf_seek");
      goto ko;
    }
    if (false) {
      err_write_1("pdf_buf_parse_stream: after buf_seek:");
      err_inspect_buf(buf);
    }
    if ((r = buf_ignore_spaces(buf)) < 0) {
      if (true) {
        err_puts("pdf_buf_parse_stream: endstream: buf_ignore_spaces");
      }
      goto ko;
    }
    result += r;
    if ((r = pdf_buf_parse_token(buf, "endstream")) < 0) {
      if (true) {
        err_puts("pdf_buf_parse_stream: endstream: pdf_buf_parse_token");
        err_inspect_buf(buf);
      }
      goto ko;
    }
    if (r) {
      result += r;
      goto ok;
    }
    if ((r = pdf_buf_ignore_until_token(buf, "endstream")) <= 0) {
      err_puts("pdf_buf_parse_stream: endstream:"
               " pdf_buf_ignore_until_token");
      assert(!("pdf_buf_parse_stream: endstream:"
               " pdf_buf_ignore_until_token"));
      goto ko;
    }
  }
 ok:
  *dest = tmp;
  return result;
 restore:
  buf_save_restore_rpos(buf, &save);
 clean:
  buf_save_clean(buf, &save);
  return r;
 ko:
  tag_clean(&tmp);
  return -1;
}

sw pdf_buf_parse_string (s_buf *buf, s_tag *dest)
{
  sw r;
  sw result = 0;
  s_str str = {0};
  s_tag tmp = {0};
  assert(buf);
  assert(dest);
  if ((r = pdf_buf_parse_string_paren(buf, &str)) < 0)
    return r;
  if (r) {
    result += r;
    goto ok;
  }
  if ((r = pdf_buf_parse_string_hex(buf, &str)) < 0)
    return r;
  if (r) {
    result += r;
    goto ok;
  }
  return 0;
 ok:
  if (true) {
    err_write_1("pdf_buf_parse_string: str = ");
    err_inspect_str(&str);
    err_write_1("\n");
  }
  if (str_parse_eval(&str, &tmp)) {
    str_clean(&str);
    *dest = tmp;
  } else {
    tmp.type = TAG_STR;
    tmp.data.str = str;
    *dest = tmp;
  }
  return result;
}

sw pdf_buf_parse_string_hex (s_buf *buf, s_str *dest)
{
  char a[BUF_SIZE] = {0};
  character c;
  u8 d;
  u8 d1;
  bool end;
  sw r;
  sw result = 0;
  s_buf_save save = {0};
  s_str tmp = {0};
  s_buf tmp_buf = {0};
  assert(buf);
  assert(dest);
  buf_save_init(buf, &save);
  if ((r = buf_read_1(buf, "<")) <= 0)
    goto clean;
  result += r;
  buf_init(&tmp_buf, false, sizeof(a) - 1, a);
  while (1) {
    if ((r = buf_read_character_utf8(buf, &c)) <= 0)
      goto restore;
    result += r;
    if (character_is_space(c))
      continue;
    if (c == '>')
      break;
    if (c >= '0' && c <= '9')
      d = c - '0';
    else if (c >= 'a' && c <= 'f')
      d = c - 'a' + 10;
    else if (c >= 'A' && c <= 'F')
      d = c - 'A' + 10;
    else {
      r = -1;
      goto restore;
    }
    if ((r = buf_ignore_spaces(buf)) < 0)
      goto restore;
    result += r;
    if ((r = buf_read_character_utf8(buf, &c)) <= 0)
      goto restore;
    result += r;
    if (c >= '0' && c <= '9')
      d1 = c - '0';
    else if (c >= 'a' && c <= 'f')
      d1 = c - 'a' + 10;
    else if (c >= 'A' && c <= 'F')
      d1 = c - 'A' + 10;
    else {
      r = -1;
      goto restore;
    }
    if ((r = buf_write_u8(&tmp_buf, (d << 4) | d1)) <= 0)
      goto restore;
  }
  if ((r = pdf_buf_parse_object_end(buf, &end)) > 0)
    result += r;
  if (! end) {
    r = -1;
    goto restore;
  }
  if (! buf_read_to_str(&tmp_buf, &tmp)) {
    r = -1;
    goto restore;
  }
  *dest = tmp;
  r = result;
  goto clean;
 restore:
  buf_save_restore_rpos(buf, &save);
 clean:
  buf_clean(&tmp_buf);
  buf_save_clean(buf, &save);
  return r;
}

// parentheses must be balanced or backslashed
sw pdf_buf_parse_string_paren (s_buf *buf, s_str *dest)
{
  char a[BUF_SIZE] = {0};
  character c;
  character c1;
  character c2;
  s32 paren = 0;
  sw r;
  sw result = 0;
  s_buf_save save = {0};
  s_str tmp = {0};
  s_buf tmp_buf = {0};
  u8 u;
  assert(buf);
  assert(dest);
  buf_save_init(buf, &save);
  if ((r = buf_read_1(buf, "(")) <= 0)
    goto clean;
  result += r;
  paren++;
  buf_init(&tmp_buf, false, sizeof(a) - 1, a);
  while (1) {
    if ((r = buf_read_character_utf8(buf, &c)) < 0)
      goto restore;
    if (! r) {
      if ((r = buf_read_u8(buf, &u)) <= 0 ||
          (r = buf_write_u8(&tmp_buf, u)) <= 0)
        goto restore;
      continue;
    }
    result += r;
    if (c == '(')
      paren++;
    else if (c == ')') {
      paren--;
      if (! paren)
        goto ok;
    }
    else if (c == '\\') {
      if ((r = buf_read_character_utf8(buf, &c)) <= 0)
        goto restore;
      result += r;
      switch (c) {
      case 'n':  c = '\n'; break;
      case 'r':  c = '\r'; break;
      case 't':  c = '\t'; break;
      case 'b':  c = '\b'; break;
      case 'f':  c = '\f'; break;
      case '(':  c = '(';  break;
      case ')':  c = ')';  break;
      case '\\': c = '\\'; break;
      case '0': case '1': case '2': case '3':
        if ((r = buf_read_character_utf8(buf, &c1)) <= 0)
          break;
        if (c1 < '0' || c1 > '7') {
          r = -1;
          break;
        }
        result += r;
        if ((r = buf_read_character_utf8(buf, &c2)) <= 0)
          break;
        if (c2 < '0' || c2 > '7') {
          r = -1;
          break;
        }
        result += r;
        c = (c - '0') * 0100 + (c1 - '0') * 010 + (c2 - '0');
        break;
      case '\n':
        goto next;
      }
    }
    if ((r = buf_write_character_utf8(&tmp_buf, c)) <= 0)
      goto restore;
  next:
    continue;
  }
 ok:
  if ((r = buf_read_to_str(&tmp_buf, &tmp)) < 0)
    goto restore;
  *dest = tmp;
  r = result;
  goto clean;
 restore:
  buf_save_restore_rpos(buf, &save);
 clean:
  buf_clean(&tmp_buf);
  buf_save_clean(buf, &save);
  return r;
}

sw pdf_buf_parse_token (s_buf *buf, const char *pchar)
{
  bool end;
  sw r;
  sw result = 0;
  s_buf_save save;
  buf_save_init(buf, &save);
  if ((r = buf_read_1(buf, pchar)) <= 0) {
    goto clean;
  }
  result += r;
  if ((r = pdf_buf_parse_object_end(buf, &end)) > 0)
    result += r;
  if (! end) {
    r = 0;
    goto restore;
  }
  r = result;
  goto clean;
 restore:
  buf_save_restore_rpos(buf, &save);
 clean:
  buf_save_clean(buf, &save);
  return r;
}

sw pdf_buf_parse_trailer (s_buf *buf, s_pdf_file *pdf_file,
                          s_pdf_trailer *dest)
{
  u64 pos;
  sw r;
  sw result = 0;
  sw size;
  const s_sym *sym_U64 = &g_sym_U64;
  s_tag tag = {0};
  s_pdf_trailer tmp = {0};
  if (! buf_total_size(buf, &pos) || ! pos) {
    err_puts("pdf_buf_parse_trailer: invalid buf_total_size");
    assert(! "pdf_buf_parse_trailer: invalid buf_total_size");
    return -1;
  }
  if (pos < buf->size) {
    size = pos;
    pos = 0;
  }
  else {
    size = buf->size;
    pos -= size;
  }
  if ((r = buf_seek(buf, pos, SEEK_SET)) < 0 ||
      (uw) r != pos) {
    err_puts("pdf_buf_parse_trailer: buf_seek");
    assert(! "pdf_buf_parse_trailer: buf_seek");
    return r;
  }
  if (buf_refill(buf, size) < size) {
    err_puts("pdf_buf_parse_trailer: buf_refill");
    assert(! "pdf_buf_parse_trailer: buf_refill");
    return -1;
  }
  buf->rpos = buf->wpos;
  if ((r = pdf_buf_parse_rewind_to_trailer(buf)) <= 0) {
    err_puts("pdf_buf_parse_trailer: pdf_buf_parse_rewind_to_trailer");
    assert(! "pdf_buf_parse_trailer: pdf_buf_parse_rewind_to_trailer");
    return r;
  }
  result += r;
  if ((r = pdf_buf_parse_dictionnary(buf, pdf_file,
                                     &tmp.dictionnary)) <= 0) {
    err_puts("pdf_buf_parse_trailer: pdf_buf_parse_dictionnary");
    err_inspect_buf(buf);
    assert(! "pdf_buf_parse_trailer: pdf_buf_parse_dictionnary");
    return r;
  }
  result += r;
  if ((r = pdf_buf_parse_token(buf, "startxref")) <= 0) {
    err_puts("pdf_buf_parse_trailer: pdf_buf_parse_token startxref");
    assert(! "pdf_buf_parse_trailer: pdf_buf_parse_token startxref");
    return r;
  }
  result += r;
  if ((r = pdf_buf_parse_integer(buf, &tag)) <= 0) {
    err_puts("pdf_buf_parse_trailer: pdf_buf_parse_integer");
    assert(! "pdf_buf_parse_trailer: pdf_buf_parse_integer");
    return r;
  }
  result += r;
  if (! u64_init_cast(&tmp.startxref, &sym_U64, &tag)) {
    err_puts("pdf_buf_parse_trailer: u64_init_cast");
    assert(! "pdf_buf_parse_trailer: u64_init_cast");
    tag_clean(&tag);
    return -1;
  }
  if (false) {
    err_write_1("pdf_buf_parse_trailer: startxref = ");
    err_inspect_u64_decimal(tmp.startxref);
    err_write_1("\n");
  }
  tag_clean(&tag);
  if (buf->rpos < buf->wpos) {
    err_puts("pdf_buf_parse_trailer: data after %%EOF");
    assert(! "pdf_buf_parse_trailer: data after %%EOF");
    return -1;
  }
  result += r;
  *dest = tmp;
  return result;
}

sw pdf_buf_parse_xref (s_buf *buf, s_map *dest)
{
  char c;
  u32 count;
  u16 generation_number;
  u32 i;
  p_list  keys;
  p_list *keys_tail;
  u32 object_number;
  u32 offset;
  sw r;
  sw r1;
  sw r2;
  sw r3;
  sw r4;
  sw r5;
  sw result = 0;
  sw result_inner;
  s_buf_save save;
  s_map tmp = {0};
  p_list  values;
  p_list *values_tail;
  assert(buf);
  assert(dest);
  if ((r = pdf_buf_parse_token(buf, "xref")) <= 0) {
    err_puts(" pdf_buf_parse_xref: pdf_buf_parse_token");
    assert(! " pdf_buf_parse_xref: pdf_buf_parse_token");
    return r;
  }
  result += r;
  buf_save_init(buf, &save);
  result_inner = result;
  keys = NULL;
  keys_tail = &keys;
  values = NULL;
  values_tail = &values;
  while (1) {
    buf_save_update(buf, &save);
    result = result_inner;
    if ((r = buf_parse_u32_decimal(buf, &object_number)) <= 0 ||
        (r1 = buf_read_1(buf, " ")) <= 0 ||
        (r2 = buf_parse_u32_decimal(buf, &count)) <= 0 ||
        ((r3 = buf_read_1(buf, "\n")) <= 0 &&
         (r3 = buf_read_1(buf, "\r\n")) <= 0)) {
      break;
    }
    result_inner += r + r1 + r2 + r3;
    i = 0;
    while (i < count) {
      if ((r = buf_parse_u32_decimal(buf, &offset)) != 10 ||
          (r1 = buf_read_1(buf, " ")) != 1 ||
          (r2 = buf_parse_u16_decimal(buf, &generation_number)) != 5 ||
          (r3 = buf_read_1(buf, " ")) != 1 ||
          (r4 = buf_read_u8(buf, (u8 *) &c)) != 1 ||
          ((r5 = buf_read_1(buf, " \n")) <= 0 &&
           (r5 = buf_read_1(buf, "\r\n")) != 2)) {
        err_puts("pdf_buf_parse_xref: invalid xref");
        err_inspect_buf(buf);
        assert(! "pdf_buf_parse_xref: invalid xref");
        goto error;
      }
      result_inner += r + r1 + r2 + r3 + r4 + r5;
      if (! (*keys_tail = list_new_ptuple(2, NULL)))
        goto error;
      if (c == 'f')
        *values_tail = list_new(NULL);
      else if (c == 'n')
        *values_tail = list_new_u64(offset, NULL);
      else {
        err_puts("pdf_buf_parse_xref: expected 'n' or 'f'");
        assert(! "pdf_buf_parse_xref: expected 'n' or 'f'");
        goto error;
      }
      tag_init_u32((*keys_tail)->tag.data.ptuple->tag, object_number);
      tag_init_u16((*keys_tail)->tag.data.ptuple->tag + 1, generation_number);
      keys_tail = &(*keys_tail)->next.data.plist;
      values_tail = &(*values_tail)->next.data.plist;
      i++;
      object_number++;
    }
  }
  //restore_inner:
  buf_save_restore_rpos(buf, &save);
  buf_save_clean(buf, &save);
  if (! keys || ! values) {
    err_puts("pdf_buf_parse_xref: empty xref");
    err_inspect_buf(buf);
    assert(! "pdf_buf_parse_xref: empty xref");
    return -1;
  }
  if (! map_init_from_lists(&tmp, keys, values)) {
    err_puts("pdf_buf_parse_xref: map_init_from_lists");
    assert(! "pdf_buf_parse_xref: map_init_from_lists");
    list_delete_all(keys);
    list_delete_all(values);
    return -1;
  }
  list_delete_all(keys);
  list_delete_all(values);
  *dest = tmp;
  return result;
 error:
  list_delete_all(keys);
  list_delete_all(values);
  buf_save_restore_rpos(buf, &save);
  buf_save_clean(buf, &save);
  return -1;
}
