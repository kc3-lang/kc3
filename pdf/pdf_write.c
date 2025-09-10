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

sw pdf_buf_write(s_buf *buf, s_tag *src)
{
  if (! src) {
      err_puts("pdf_buf_write: invalid tag");
      assert(! "pdf_buf_write: invalid tag");
      return -1;
  }
  switch (src->type) {
    case TAG_F32:
    case TAG_F64:
    case TAG_F128:
      return pdf_buf_write_float(buf, src);
    case TAG_INTEGER:
    case TAG_SW:
    case TAG_S64:
    case TAG_S32:
    case TAG_S16:
    case TAG_S8:
    case TAG_U8:
    case TAG_U16:
    case TAG_U32:
    case TAG_U64:
    case TAG_UW:
      return pdf_buf_write_integer(buf, src);
    case TAG_MAP:
      return pdf_buf_write_dictionnary(buf, &src->data.map);
    case TAG_STR:
      return pdf_buf_write_string_hex(buf, &src->data.str);
    case TAG_PSYM: return pdf_buf_write_name(buf, src->data.psym);
    default:
      err_puts("pdf_buf_write: unsupported tag");
      assert(! "pdf_buf_write: unsupported tag");
      return -1;
  }
}

sw pdf_buf_write_bool (s_buf *buf, bool src)
{
  if (src) {
    return pdf_buf_write_token(buf, "true");
  }
  return pdf_buf_write_token(buf, "false");
}

sw pdf_buf_write_dictionnary (s_buf *buf, s_map *src)
{
  uw i;
  sw r;
  if ((r = pdf_buf_write_token(buf, "<<")) < 0) {
    return r;
  }
  for (i = 0; i < src->count; i++) {
    if (src->key[i].type != TAG_PSYM) {
      err_puts("pdf_buf_write_dictionary: key must be psym");
      assert(! "pdf_buf_write_dictionary: key must be psym");
      return -1;
    }
    if ((r = pdf_buf_write_name(buf, src->key[i].data.psym)) < 0
      || (r = pdf_buf_write_token(buf, " ")) < 0
      || (r = pdf_buf_write(buf, &src->value[i])) < 0
      || (r = pdf_buf_write_token(buf, " ")) < 0) {
      return r;
    }
  }
  if ((r = pdf_buf_write_token(buf, ">>")) < 0) {
    return r;
  }
}

sw pdf_buf_write_float (s_buf *buf, s_tag *src)
{
  if (! src) {
      err_puts("pdf_buf_write: invalid tag");
      assert(! "pdf_buf_write: invalid tag");
      return -1;
  }
  switch (src->type) {
    case TAG_F32:  return buf_inspect_f32(buf, src->data.f32);
    case TAG_F64:  return buf_inspect_f64(buf, src->data.f64);
    case TAG_F128: return buf_inspect_f128(buf, src->data.f128);
    default:
      err_puts("pdf_buf_write_float: tag not a float");
      assert(! "pdf_buf_write_float: tag not a float");
      return -1;
  }
}

sw pdf_buf_write_integer (s_buf *buf, s_tag *src)
{
  if (! src) {
      err_puts("pdf_buf_write: invalid tag");
      assert(! "pdf_buf_write: invalid tag");
      return -1;
  }
  switch (src->type) {
    case TAG_INTEGER:
      return buf_inspect_integer(buf, &src->data.integer);
    case TAG_SW:  return buf_inspect_sw(buf, src->data.sw);
    case TAG_S64: return buf_inspect_s64(buf, src->data.s64);
    case TAG_S32: return buf_inspect_s32(buf, src->data.s32);
    case TAG_S16: return buf_inspect_s16(buf, src->data.s16);
    case TAG_S8:  return buf_inspect_s8(buf, src->data.s8);
    case TAG_U8:  return buf_inspect_u8(buf, src->data.u8);
    case TAG_U16: return buf_inspect_u16(buf, src->data.u16);
    case TAG_U32: return buf_inspect_u32(buf, src->data.u32);
    case TAG_U64: return buf_inspect_u64(buf, src->data.u64);
    case TAG_UW:  return buf_inspect_uw(buf, src->data.uw);
    default:
      err_puts("pdf_buf_write_integer: tag is not an integer");
      assert(! "pdf_buf_write_integer: tag is not an integer");
      return -1;
  }
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

  if ((r = pdf_buf_write_token(buf, "<")) < 0) {
    goto cleanup;
  }
  for (i = 0; i < dest->size; i++) {
    if ((r = buf_read_u8(&read_buf, &c)) < 0
      || (r = buf_write_u8(buf, base[c & 0xf])) < 0
      || (r = buf_write_u8(buf, base[(c >> 4) & 0xf])) < 0) {
      goto cleanup;
    }
  }
  if ((r = pdf_buf_write_token(buf, ">")) < 0) {
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
