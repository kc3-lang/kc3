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
#include <math.h>
#include "../libkc3/kc3.h"
#include "pdf_buf_write.h"

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
    if ((r = pdf_buf_write_tag(buf, &n->tag)) < 0) {
      err_write_1("pdf_buf_write_array: pdf_buf_write_tag: ");
      err_inspect_tag(&n->tag);
      err_write_1("\n");
      return r;
    }
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
    if ((r = pdf_buf_write_tag(buf, &src->value[i])) < 0) {
      err_write_1("pdf_buf_write_dictionnary: pdf_buf_write_tag: key ");
      err_inspect_sym(src->key[i].data.psym);
      err_write_1(", value ");
      err_inspect_tag(&src->value[i]);
      err_write_1("\n");
      return r;
    }
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
  uw i;
  sw r;
  sw result = 0;
  uw count = 0;
  f32 *value;
  assert(buf);
  assert(src);
  if (src->dimension_count != 1) {
    err_puts("pdf_buf_write_flat_array: array dimension count != 1");
    assert(! "pdf_buf_write_flat_array: array dimension count != 1");
    return -1;
  }
  count = src->dimensions[0].count;
  for (i = 0; i < count; i++) {
    if ((value = array_data(src, &i)) == NULL)
      return -1;
    if ((r = pdf_buf_write_float(buf, *value)) < 0)
      return r;
    result += r;
    if (i < count - 1 &&
        (r = pdf_buf_write_separator(buf, false)) < 0)
      return r;
    result += r;
  }
  return result;
}

sw pdf_buf_write_float (s_buf *buf, f32 src)
{
  u8 d[64] = {0};
  u8 i;
  u8 n;
  sw r;
  sw result = 0;
  f32 x;
  f32 frac;
  assert(buf);
  x = src;
  if (x == 0.0f)
    return buf_write_1(buf, "0");
  if (x < 0.0f) {
    if ((r = buf_write_1(buf, "-")) < 0)
      return r;
    result += r;
    x = -x;
  }
  /* Extract integer part. */
  n = 0;
  frac = x - floorf(x);
  x = floorf(x);
  if (x >= 1.0f) {
    while (n < sizeof(d) && x >= 1.0f) {
      d[n] = (u8) fmodf(x, 10.0f);
      x = floorf(x / 10.0f);
      n++;
    }
    /* Write digits in reverse order. */
    i = n;
    while (i > 0) {
      i--;
      if ((r = buf_write_u8(buf, d[i] + '0')) < 0)
        return r;
      result += r;
    }
  }
  else {
    if ((r = buf_write_u8(buf, '0')) < 0)
      return r;
    result += r;
  }
  /* Write fractional part. */
  if (frac > 0.0f) {
    if ((r = buf_write_1(buf, ".")) < 0)
      return r;
    result += r;
    i = 0;
    while (frac > 0.0f && i < 6) {
      frac = frac * 10.0f;
      d[0] = (u8) floorf(frac);
      if ((r = buf_write_u8(buf, d[0] + '0')) < 0)
        return r;
      result += r;
      frac = frac - (f32) d[0];
      i++;
    }
  }
  return result;
}

sw pdf_buf_write_header (s_buf *buf)
{
  sw r;
  sw result = 0;
  s_str str = STR("%PDF-1.4\r%\xe2\xe3\xcf\xd3\n");
  if ((r = buf_write_str(buf, &str)) < 0)
    return r;
  result += r;
  return result;
}

sw pdf_buf_write_indirect_ref (s_buf *buf, const s_tuple *tuple)
{
  sw r = 0;
  sw result = 0;
  assert(buf);
  assert(tuple);
  if (tuple->count != 3 ||
      tuple->tag[0].type != TAG_PSYM ||
      tuple->tag[0].data.psym != sym_1("indirect_object")) {
    err_puts("pdf_buf_write_indirect_object: invalid Tuple");
    assert(! "pdf_buf_write_indirect_object: invalid Tuple");
    return -1;
  }
  if ((r = pdf_buf_write_tag(buf, tuple->tag + 1)) < 0) {
    return r;
  }
  result += r;
  if ((r = pdf_buf_write_separator(buf, false)) < 0) {
    return r;
  }
  result += r;
  if ((r = pdf_buf_write_tag(buf, tuple->tag + 2)) < 0) {
    return r;
  }
  result += r;
  if ((r = pdf_buf_write_token(buf, " R")) < 0)
    return r;
  result += r;
  return result;
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
  return result + pdf_buf_write_token_with_separator(buf, " obj", true);
}

sw pdf_buf_write_indirect_end (s_buf *buf)
{
  sw r = 0;
  assert(buf);
  if ((r = pdf_buf_write_separator(buf, true)) < 0) {
    return r;
  }
  return r + pdf_buf_write_token_with_separator(buf, "endobj", true);
}

sw pdf_buf_write_integer (s_buf *buf, s32 src)
{
  assert(buf);
  return buf_inspect_s32_decimal(buf, src);
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

sw pdf_buf_write_rect (s_buf *buf, s_pdf_rect *rect)
{
  sw r;
  sw result = 0;
  assert(rect);
  if ((r = buf_write_1(buf, "[")) < 0)
    return r;
  result += r;
  if ((r = pdf_buf_write_float(buf, rect->x)) < 0)
    return r;
  result += r;
  if ((r = buf_write_1(buf, " ")) < 0)
    return r;
  result += r;
  if ((r = pdf_buf_write_float(buf, rect->y)) < 0)
    return r;
  result += r;
  if ((r = buf_write_1(buf, " ")) < 0)
    return r;
  result += r;
  if ((r = pdf_buf_write_float(buf, rect->w)) < 0)
    return r;
  result += r;
  if ((r = buf_write_1(buf, " ")) < 0)
    return r;
  result += r;
  if ((r = pdf_buf_write_float(buf, rect->h)) < 0)
    return r;
  result += r;
  if ((r = buf_write_1(buf, "]")) < 0)
    return r;
  result += r;
  return result;
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
    case TAG_PSTRUCT:
      if (src->data.pstruct->pstruct_type->module == sym_1("PDF.Rect"))
        return pdf_buf_write_rect(buf,
          (s_pdf_rect *) src->data.pstruct->data);
      err_write_1("pdf_buf_write_tag: unsupported struct type: ");
      err_inspect_sym(src->data.pstruct->pstruct_type->module);
      err_write_1("\n");
      assert(! "pdf_buf_write_tag: unsupported struct type");
      return -1;
    case TAG_TUPLE:
      if (src->data.tuple.count == 3 &&
          src->data.tuple.tag[0].type == TAG_PSYM &&
          src->data.tuple.tag[0].data.psym == sym_1("indirect_object"))
        return pdf_buf_write_indirect_ref(buf, &src->data.tuple);
      goto invalid_type;
    default:
      goto invalid_type;
  }
  err_puts("pdf_buf_write_tag: unknown tag type");
  assert(! "pdf_buf_write_tag: unknown tag type");
  return -1;
 invalid_type:
  err_write_1("pdf_buf_write_tag: invalid PDF tag type: ");
  err_puts(tag_type_to_string(src->type));
  assert(! "pdf_buf_write_tag: invalid PDF tag type");
  return -1;
}

sw pdf_buf_write_token (s_buf *buf, const char *pchar)
{
  assert(buf);
  return buf_write_1(buf, pchar);
}

sw pdf_buf_write_token_with_separator (s_buf *buf, const char *pchar,
                                       bool newline)
{
  sw r;
  sw result = 0;
  assert(buf);
  if ((r = pdf_buf_write_token(buf, pchar)) < 0)
    return r;
  result += r;
  if ((r = pdf_buf_write_separator(buf, newline)) < 0)
    return r;
  result += r;
  return result;
}

sw pdf_buf_write_vec2 (s_buf *buf, const s_vec2 *src)
{
  assert(buf);
  assert(src);
  sw r;
  sw result = 0;
  if ((r = pdf_buf_write_float(buf, src->x)) < 0)
    return r;
  result += r;
  if ((r = pdf_buf_write_separator(buf, false)) < 0)
    return r;
  result += r;
  if ((r = pdf_buf_write_float(buf, src->y)) < 0)
    return r;
  result += r;
  return result;
}
