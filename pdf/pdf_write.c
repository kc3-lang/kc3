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

sw pdf_buf_write_array (s_buf *buf, const p_list src)
{
  p_list n = src;
  sw r;
  sw result = 0;
  assert(src);
  if ((r = buf_write_1(buf, "[")) < 0) {
    return r;
  }
  result += r;
  while (n) {
    if ((r = pdf_buf_write_tag(buf, &n->tag)) < 0)
      return r;
    result += r;
    n = list_next(n);
    if (n && (r = buf_write_1(buf, " ")) < 0)
      return r;
    result += r;
  }
  if ((r = buf_write_1(buf, "]")) < 0) {
    return r;
  }
  result += r;
  return result;
}

sw pdf_buf_write_bool (s_buf *buf, bool src)
{
  if (src) {
    return buf_write_1(buf, "true");
  }
  return buf_write_1(buf, "false");
}

sw pdf_buf_write_dictionnary (s_buf *buf, const s_map *src)
{
  uw i;
  sw r;
  sw result = 0;
  if ((r = buf_write_1(buf, "<<")) < 0) {
    return r;
  }
  result += r;
  for (i = 0; i < src->count; i++) {
    if (src->key[i].type != TAG_PSYM) {
      err_puts("pdf_buf_write_dictionary: key is not a Sym");
      assert(! "pdf_buf_write_dictionary: key is not a Sym");
      return -1;
    }
    if ((r = pdf_buf_write_name(buf, src->key[i].data.psym)) < 0)
      return r;
    result += r;
    if ((r = pdf_buf_write_separator(buf, false)) < 0)
      return r;
    result += r;
    if ((r = pdf_buf_write_tag(buf, &src->value[i])) < 0)
      return r;
    result += r;
    if (i < src->count - 1 &&
        (r = pdf_buf_write_separator(buf, true)) < 0)
      return r;
    result += r;
  }
  if ((r = pdf_buf_write_token(buf, ">>")) < 0) {
    return r;
  }
  result += r;
  return result;
}

sw pdf_buf_write_flat_array (s_buf *buf, const s_array *src)
{
  assert(buf);
}

sw pdf_buf_write_float (s_buf *buf, f32 src)
{
  assert(buf);
  return buf_inspect_f32(buf, src);
}

sw pdf_buf_write_indirect_ref (s_buf *buf, const s_tuple *src)
{
  sw r = 0;
  sw result = 0;
  assert(buf);
  assert(src);
  if (src->count != 2) {
    err_puts("pdf_buf_write_indirect_object: invalid Tuple");
    assert(! "pdf_buf_write_indirect_object: invalid Tuple");
    return -1;
  }
  if ((r = pdf_buf_write_tag(buf, &src->tag[0])) < 0) {
    return r;
  }
  result += r;
  if ((r = pdf_buf_write_separator(buf, false)) < 0) {
    return r;
  }
  result += r;
  if ((r = pdf_buf_write_tag(buf, &src->tag[1])) < 0) {
    return r;
  }
  result += r;
  return result + pdf_buf_write_token(buf, " R");
}

sw pdf_buf_write_indirect_start (s_buf *buf, const s_tuple *src)
{
  sw r = 0;
  sw result = 0;
  assert(buf);
  assert(src);
  if (src->count != 2) {
    err_puts("pdf_buf_write_indirect_object: invalid tuple size");
    assert(! "pdf_buf_write_indirect_object: invalid tuple size");
    return -1;
  }
  if ((r = pdf_buf_write_tag(buf, &src->tag[0])) < 0) {
    return r;
  }
  result += r;
  if ((r = pdf_buf_write_separator(buf, false)) < 0) {
    return r;
  }
  result += r;
  if ((r = pdf_buf_write_tag(buf, &src->tag[1])) < 0) {
    return r;
  }
  result += r;
  return result + pdf_buf_write_token_clean(buf, " obj", true);
}

sw pdf_buf_write_indirect_end (s_buf *buf)
{
  sw r = 0;
  assert(buf);
  if ((r = pdf_buf_write_separator(buf, true)) < 0) {
    return r;
  }
  return r + pdf_buf_write_token_clean(buf, "endobj", true);
}

sw pdf_buf_write_integer (s_buf *buf, s32 src)
{
  assert(buf);
  return buf_inspect_s32(buf, src);
}

sw pdf_buf_write_name (s_buf *buf, p_pdf_name src)
{
  sw r = 0;
  if ((r = pdf_buf_write_token(buf, "/")) < 0) {
    return r;
  }
  return r += buf_write_str(buf, &src->str);
}

sw pdf_buf_write_null (s_buf *buf)
{
    return pdf_buf_write_token(buf, "null");
}

sw pdf_buf_write_separator(s_buf *buf, bool newline)
{
  return buf_write_1(buf, newline ? "\n" : " ");
}

sw pdf_buf_write_string_hex (s_buf *buf, const s_str *str)
{
  u8 c;
  u32 i;
  sw r = 0;
  sw result = 0;
  s_buf read_buf = {0};
  const char *base = g_kc3_base_hexadecimal.ptr.pchar;
  buf_init_str_const(&read_buf, str);
  if ((r = pdf_buf_write_token(buf, "<")) < 0)
    goto cleanup;
  result += r;
  for (i = 0; i < str->size; i++) {
    if ((r = buf_read_u8(&read_buf, &c)) < 0
      || (r = buf_write_u8(buf, base[c & 0xf])) < 0
      || (r = buf_write_u8(buf, base[(c >> 4) & 0xf])) < 0)
      goto cleanup;
    result += 2;
  }
  if ((r = pdf_buf_write_token(buf, ">")) < 0)
    goto cleanup;
  result += r;
 cleanup:
  buf_clean(&read_buf);
  return result;
}

sw pdf_buf_write_tag (s_buf *buf, const s_tag *src)
{
  assert(buf);
  assert(src);
  switch (src->type) {
    case TAG_F32:  return pdf_buf_write_float(buf, src->data.f32);
    case TAG_F64:  return pdf_buf_write_float(buf, src->data.f64);
    case TAG_SW:   return pdf_buf_write_integer(buf, src->data.sw);
    case TAG_S64:  return pdf_buf_write_integer(buf, src->data.s64);
    case TAG_S32:  return pdf_buf_write_integer(buf, src->data.s32);
    case TAG_S16:  return pdf_buf_write_integer(buf, src->data.s16);
    case TAG_S8:   return pdf_buf_write_integer(buf, src->data.s8);
    case TAG_U8:   return pdf_buf_write_integer(buf, src->data.u8);
    case TAG_U16:  return pdf_buf_write_integer(buf, src->data.u16);
    case TAG_U32:  return pdf_buf_write_integer(buf, src->data.u32);
    case TAG_U64:  return pdf_buf_write_integer(buf, src->data.u64);
    case TAG_UW:   return pdf_buf_write_integer(buf, src->data.uw);
    case TAG_INTEGER:
      return buf_inspect_integer(buf, &src->data.integer);
    case TAG_MAP:
      return pdf_buf_write_dictionnary(buf, &src->data.map);
    case TAG_STR:  return pdf_buf_write_string_hex(buf, &src->data.str);
    case TAG_PLIST:
      return pdf_buf_write_array(buf, src->data.plist);
    case TAG_PSYM: return pdf_buf_write_name(buf, src->data.psym);
    default:
      err_write_1("pdf_buf_write_tag: invalid PDF tag type: ");
      err_puts(tag_type_to_string(src->type));
      assert(! "pdf_buf_write_tag: invalid PDF tag type");
      return -1;
  }
  err_puts("pdf_buf_write_tag: unknown tag type");
  assert(! "pdf_buf_write_tag: unknown tag type");
  return -1;
}

// writes token without separators
sw pdf_buf_write_token(s_buf *buf, const char *pchar)
{
  assert(buf);
  return buf_write_1(buf, pchar);
}

// writes token with separator
sw pdf_buf_write_token_clean(s_buf *buf, const char *pchar,
                             bool newline)
{
  sw r = 0;
  assert(buf);
  if ((r = pdf_buf_write_token(buf, pchar)) < 0) {
    return r;
  }
  return r + pdf_buf_write_separator(buf, newline);
}
