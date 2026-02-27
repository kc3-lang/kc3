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
#include "kc3_pdf_write.h"
#include "pdf_buf_write.h"
#include "pdf_write.h"
#include "pdf_write_page.h"
#include "jpeglib.h"
#include <ft2build.h>
#include FT_FREETYPE_H
#include <string.h>

s_pdf_write ** kc3_pdf_write_add_page (s_pdf_write **pdf,
                                       s_pdf_write_page **page)
{
  s_buf *buf;
  s_buf *content_buf;
  s_list *l;
  s_pdf_write_page *p;
  assert(pdf);
  assert(*pdf);
  assert(page);
  assert(*page);
  buf = (*pdf)->buf;
  p = *page;
  if (! (l = list_new_u32(p->object_number, (*pdf)->pages)))
    return NULL;
  (*pdf)->pages = l;
  pdf_write_indirect_object(*pdf, p->object_number, 0);
  pdf_buf_write_dictionnary(buf, &p->map);
  buf_write_1(buf, "\nendobj\n");
  content_buf = p->contents.stream.stream.buf;
  pdf_write_indirect_object(*pdf, p->contents.stream.object_number, 0);
  buf_write_1(buf, "<< /Length ");
  buf_inspect_sw_decimal(buf, (sw) content_buf->wpos);
  buf_write_1(buf, " >>\nstream\n");
  buf_write(buf, content_buf->ptr.p_pchar, content_buf->wpos);
  buf_write_1(buf, "endstream\nendobj\n");
  return pdf;
}

void kc3_pdf_write_close (s_pdf_write **pdf)
{
  pdf_write_close(*pdf);
}

void kc3_pdf_write_delete (s_pdf_write **pdf)
{
  pdf_write_delete(*pdf);
}

u32 kc3_pdf_write_font_from_file (s_pdf_write **pdf, s_str *path)
{
  s16 ascender;
  s_buf *buf;
  s16 descender;
  u32 descriptor_obj;
  u32  file_obj;
  long file_size;
  u32 font_obj;
  FILE *fp;
  FT_Face    ft_face;
  FT_Library ft_library;
  s_list *l;
  u16 units_per_em;
  FT_BBox bbox;
  const char *font_name;
  assert(pdf);
  assert(*pdf);
  assert(path);
  buf = (*pdf)->buf;
  if (FT_Init_FreeType(&ft_library)) {
    err_puts("kc3_pdf_write_font_from_file: FT_Init_FreeType failed");
    return 0;
  }
  if (FT_New_Face(ft_library, path->ptr.p_pchar, 0, &ft_face)) {
    err_write_1("kc3_pdf_write_font_from_file: "
                "error loading font: ");
    err_puts(path->ptr.p_pchar);
    FT_Done_FreeType(ft_library);
    return 0;
  }
  fp = fopen(path->ptr.p_pchar, "rb");
  if (! fp) {
    err_write_1("kc3_pdf_write_font_from_file: "
                "error opening font file: ");
    err_puts(path->ptr.p_pchar);
    FT_Done_Face(ft_face);
    FT_Done_FreeType(ft_library);
    return 0;
  }
  fseek(fp, 0, SEEK_END);
  file_size = ftell(fp);
  fseek(fp, 0, SEEK_SET);
  font_name = ft_face->family_name ? ft_face->family_name : "Unknown";
  units_per_em = ft_face->units_per_EM;
  ascender = ft_face->ascender;
  descender = ft_face->descender;
  bbox = ft_face->bbox;
  font_obj = pdf_write_object_number_register(*pdf);
  descriptor_obj = pdf_write_object_number_register(*pdf);
  file_obj = pdf_write_object_number_register(*pdf);
  kc3_pdf_write_ttf_font(buf, font_obj, descriptor_obj,
                         font_name);
  kc3_pdf_write_ttf_descriptor(buf, descriptor_obj, file_obj,
                               font_name, units_per_em,
                               ascender, descender, &bbox,
                               file_size);
  kc3_pdf_write_ttf_file(buf, file_obj, fp, file_size);
  FT_Done_Face(ft_face);
  FT_Done_FreeType(ft_library);
  fclose(fp);
  if (! (l = list_new_u32(font_obj, (*pdf)->fonts)))
    return 0;
  (*pdf)->fonts = l;
  return font_obj;
}

u32 kc3_pdf_write_image_from_file (s_pdf_write **doc,
                                   s_str *path)
{
  FILE *fp;
  struct jpeg_decompress_struct cinfo;
  struct jpeg_error_mgr jerr;
  long file_size;
  u32 object_number;
  u32 width;
  u32 height;
  u32 num_components;
  assert(doc);
  assert(*doc);
  assert(path);
  fp = fopen(path->ptr.p_pchar, "rb");
  if (! fp) {
    err_write_1("kc3_pdf_write_image_from_file: "
                "error opening JPEG file: ");
    err_puts(path->ptr.p_pchar);
    return 0;
  }
  fseek(fp, 0, SEEK_END);
  file_size = ftell(fp);
  if (file_size <= 0) {
    err_puts("kc3_pdf_write_image_from_file: invalid file size");
    fclose(fp);
    return 0;
  }
  fseek(fp, 0, SEEK_SET);
  cinfo.err = jpeg_std_error(&jerr);
  jpeg_create_decompress(&cinfo);
  jpeg_stdio_src(&cinfo, fp);
  jpeg_read_header(&cinfo, TRUE);
  width = cinfo.image_width;
  height = cinfo.image_height;
  num_components = cinfo.num_components;
  jpeg_destroy_decompress(&cinfo);
  if (! width || ! height || ! num_components) {
    err_puts("kc3_pdf_write_image_from_file: "
             "invalid JPEG dimensions");
    fclose(fp);
    return 0;
  }
  fseek(fp, 0, SEEK_SET);
  if (! (object_number = pdf_write_object_number_register(*doc))) {
    err_puts("kc3_pdf_write_image_from_file:"
             " pdf_write_object_number_register");
    fclose(fp);
    return 0;
  }
  if (pdf_buf_write_jpeg_xobject((*doc)->buf, object_number,
                                 width, height, num_components,
                                 fp, file_size) < 0) {
    err_puts("kc3_pdf_write_image_from_file: "
             "kc3_pdf_write_jpeg_xobject");
    fclose(fp);
    return 0;
  }
  fclose(fp);
  /* Track image object number. */
  {
    s_list *l;
    l = list_new((*doc)->images);
    tag_init_u32(&l->tag, object_number);
    (*doc)->images = l;
  }
  return object_number;
}

s_pdf_write ** kc3_pdf_write_new (s_pdf_write **pdf)
{
  s_pdf_write *tmp;
  if (! (tmp = pdf_write_new()))
    return NULL;
  *pdf = tmp;
  return pdf;
}

s_pdf_write ** kc3_pdf_write_new_file (s_pdf_write **pdf,
                                       s_str *path)
{
  s_pdf_write *tmp;
  if (! (tmp = pdf_write_new_file(path)))
    return NULL;
  *pdf = tmp;
  return pdf;
}

s_str * kc3_pdf_write_to_str (s_pdf_write **pdf, s_str *dest)
{
  assert(pdf);
  assert(*pdf);
  assert(dest);
  return pdf_write_to_str(*pdf, dest);
}

sw kc3_pdf_write_ttf_font (s_buf *buf, u32 font_obj,
                           u32 descriptor_obj, const char *font_name)
{
  sw r;
  sw result = 0;
  assert(buf);
  assert(font_name);
  if ((r = buf_inspect_u32_decimal(buf, font_obj)) < 0)
    return r;
  result += r;
  if ((r = buf_write_1(buf, " 0 obj\n<<\n/Type /Font\n"
                       "/Subtype /TrueType\n"
                       "/BaseFont /")) < 0)
    return r;
  result += r;
  if ((r = buf_write_1(buf, font_name)) < 0)
    return r;
  result += r;
  if ((r = buf_write_1(buf, "\n/Encoding /WinAnsiEncoding\n"
                       "/FontDescriptor ")) < 0)
    return r;
  result += r;
  if ((r = buf_inspect_u32_decimal(buf, descriptor_obj)) < 0)
    return r;
  result += r;
  if ((r = buf_write_1(buf, " 0 R\n>>\nendobj\n")) < 0)
    return r;
  result += r;
  return result;
}

sw kc3_pdf_write_ttf_descriptor (s_buf *buf, u32 descriptor_obj,
                                 u32 file_obj, const char *font_name,
                                 u16 units_per_em, s16 ascender,
                                 s16 descender, const FT_BBox *bbox,
                                 uw file_length)
{
  sw r;
  sw result = 0;
  s32 scale;
  assert(buf);
  assert(font_name);
  assert(bbox);
  scale = 1000 / (units_per_em ? units_per_em : 1000);
  if ((r = buf_inspect_u32_decimal(buf, descriptor_obj)) < 0)
    return r;
  result += r;
  if ((r = buf_write_1(buf, " 0 obj\n<<\n/Type /FontDescriptor\n"
                       "/FontName /")) < 0)
    return r;
  result += r;
  if ((r = buf_write_1(buf, font_name)) < 0)
    return r;
  result += r;
  if ((r = buf_write_1(buf, "\n/Flags 32\n/FontBBox [")) < 0)
    return r;
  result += r;
  if ((r = buf_inspect_s32_decimal(buf, (s32) bbox->xMin * scale)) < 0)
    return r;
  result += r;
  if ((r = buf_write_1(buf, " ")) < 0)
    return r;
  result += r;
  if ((r = buf_inspect_s32_decimal(buf, (s32) bbox->yMin * scale)) < 0)
    return r;
  result += r;
  if ((r = buf_write_1(buf, " ")) < 0)
    return r;
  result += r;
  if ((r = buf_inspect_s32_decimal(buf, (s32) bbox->xMax * scale)) < 0)
    return r;
  result += r;
  if ((r = buf_write_1(buf, " ")) < 0)
    return r;
  result += r;
  if ((r = buf_inspect_s32_decimal(buf, (s32) bbox->yMax * scale)) < 0)
    return r;
  result += r;
  if ((r = buf_write_1(buf, "]\n/ItalicAngle 0\n/Ascent ")) < 0)
    return r;
  result += r;
  if ((r = buf_inspect_s32_decimal(buf, (s32) ascender * scale)) < 0)
    return r;
  result += r;
  if ((r = buf_write_1(buf, "\n/Descent ")) < 0)
    return r;
  result += r;
  if ((r = buf_inspect_s32_decimal(buf, (s32) descender * scale)) < 0)
    return r;
  result += r;
  if ((r = buf_write_1(buf, "\n/CapHeight ")) < 0)
    return r;
  result += r;
  if ((r = buf_inspect_s32_decimal(buf, (s32) ascender * scale)) < 0)
    return r;
  result += r;
  if ((r = buf_write_1(buf, "\n/StemV 80\n/FontFile2 ")) < 0)
    return r;
  result += r;
  if ((r = buf_inspect_u32_decimal(buf, file_obj)) < 0)
    return r;
  result += r;
  if ((r = buf_write_1(buf, " 0 R\n/Length1 ")) < 0)
    return r;
  result += r;
  if ((r = buf_inspect_uw_decimal(buf, file_length)) < 0)
    return r;
  result += r;
  if ((r = buf_write_1(buf, "\n>>\nendobj\n")) < 0)
    return r;
  result += r;
  return result;
}

sw kc3_pdf_write_ttf_file (s_buf *buf, u32 file_obj,
                           FILE *fp, uw length)
{
  size_t bytes_read;
  u8 chunk[BUF_SIZE];
  uw chunk_size;
  uw remaining;
  sw r;
  sw result = 0;
  assert(buf);
  assert(fp);
  if ((r = buf_inspect_u32_decimal(buf, file_obj)) < 0)
    return r;
  result += r;
  if ((r = buf_write_1(buf, " 0 obj\n<<\n/Length ")) < 0)
    return r;
  result += r;
  if ((r = buf_inspect_uw_decimal(buf, length)) < 0)
    return r;
  result += r;
  if ((r = buf_write_1(buf, "\n/Length1 ")) < 0)
    return r;
  result += r;
  if ((r = buf_inspect_uw_decimal(buf, length)) < 0)
    return r;
  result += r;
  if ((r = buf_write_1(buf, "\n>>\nstream\n")) < 0)
    return r;
  result += r;
  remaining = length;
  while (remaining > 0) {
    chunk_size = remaining > sizeof(chunk) ? sizeof(chunk) : remaining;
    bytes_read = fread(chunk, 1, chunk_size, fp);
    if (bytes_read != chunk_size) {
      err_puts("kc3_pdf_write_ttf_file: fread");
      return -1;
    }
    if ((r = buf_flush(buf)) < 0) {
      err_puts("kc3_pdf_write_ttf_file: buf_flush");
      return -1;
    }
    if ((r = buf_write(buf, chunk, chunk_size)) < 0) {
      err_puts("kc3_pdf_write_ttf_file: buf_write");
      return -1;
    }
    result += r;
    remaining -= chunk_size;
  }
  if ((r = buf_write_1(buf, "\nendstream\nendobj\n")) < 0)
    return r;
  result += r;
  return result;
}
