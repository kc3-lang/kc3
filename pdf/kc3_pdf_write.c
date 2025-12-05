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
#include "kc3_pdf_write.h"
#include "pdf_write.h"
#include "jpeglib.h"
#include <string.h>

void kc3_pdf_write_delete (s_pdf_write **pdf)
{
  pdf_write_delete(*pdf);
}

s_pdf_write ** kc3_pdf_write_font_from_file (s_pdf_write **pdf,
                                             s_str *path)
{
  (void) path;
  err_puts("kc3_pdf_write_font_from_file: not implemented");
  return pdf;
}

s_pdf_write ** kc3_pdf_write_image_from_file (s_pdf_write **doc,
                                              s_str *path)
{
  FILE *fp;
  struct jpeg_decompress_struct cinfo;
  struct jpeg_error_mgr jerr;
  s_buf *buf;
  long file_size;
  u8 *jpeg_data = NULL;
  size_t bytes_read;
  u32 object_number;
  assert(doc);
  assert(*doc);
  assert(path);
  buf = (*doc)->buf;
  fp = fopen(path->ptr.pchar, "rb");
  if (! fp) {
    err_write_1("kc3_pdf_write_image_from_file: "
                "error opening JPEG file: ");
    err_puts(path->ptr.pchar);
    return NULL;
  }
  cinfo.err = jpeg_std_error(&jerr);
  jpeg_create_decompress(&cinfo);
  jpeg_stdio_src(&cinfo, fp);
  jpeg_read_header(&cinfo, TRUE);
  fseek(fp, 0, SEEK_END);
  file_size = ftell(fp);
  fseek(fp, 0, SEEK_SET);
  if (! (jpeg_data = alloc(file_size))) {
    jpeg_destroy_decompress(&cinfo);
    fclose(fp);
    return NULL;
  }
  bytes_read = fread(jpeg_data, 1, file_size, fp);
  if (bytes_read != (size_t) file_size) {
    err_puts("kc3_pdf_write_image_from_file: failed to read JPEG file");
    free(jpeg_data);
    jpeg_destroy_decompress(&cinfo);
    fclose(fp);
    return NULL;
  }
  object_number = pdf_write_object_number_register(*doc);
  kc3_pdf_write_jpeg_xobject(buf, object_number,
                             cinfo.image_width, cinfo.image_height,
                             cinfo.num_components,
                             jpeg_data, file_size);
  free(jpeg_data);
  jpeg_destroy_decompress(&cinfo);
  fclose(fp);
  return doc;
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

sw kc3_pdf_write_jpeg_xobject (s_buf *buf, u32 object_number,
                               u32 width, u32 height,
                               u32 num_components,
                               const u8 *data, uw length)
{
  sw r;
  sw result = 0;
  const char *colorspace;
  assert(buf);
  assert(data);
  colorspace = (num_components == 1) ? "/DeviceGray" :
               (num_components == 3) ? "/DeviceRGB" :
               (num_components == 4) ? "/DeviceCMYK" : "/DeviceRGB";
  if ((r = buf_inspect_u32(buf, object_number)) < 0)
    return r;
  result += r;
  if ((r = buf_write_1(buf, " 0 obj\n")) < 0)
    return r;
  result += r;
  if ((r = buf_write_1(buf, "<<\n/Type /XObject\n"
                       "/Subtype /Image\n")) < 0)
    return r;
  result += r;
  if ((r = buf_write_1(buf, "/Width ")) < 0)
    return r;
  result += r;
  if ((r = buf_inspect_u32(buf, width)) < 0)
    return r;
  result += r;
  if ((r = buf_write_1(buf, "\n/Height ")) < 0)
    return r;
  result += r;
  if ((r = buf_inspect_u32(buf, height)) < 0)
    return r;
  result += r;
  if ((r = buf_write_1(buf, "\n/ColorSpace ")) < 0)
    return r;
  result += r;
  if ((r = buf_write_1(buf, colorspace)) < 0)
    return r;
  result += r;
  if ((r = buf_write_1(buf, "\n/BitsPerComponent 8\n"
                       "/Filter /DCTDecode\n"
                       "/Length ")) < 0)
    return r;
  result += r;
  if ((r = buf_inspect_uw(buf, length)) < 0)
    return r;
  result += r;
  if ((r = buf_write_1(buf, "\n>>\nstream\n")) < 0)
    return r;
  result += r;
  if ((r = buf_write(buf, data, length)) < 0)
    return r;
  result += r;
  if ((r = buf_write_1(buf, "\nendstream\nendobj\n")) < 0)
    return r;
  result += r;
  return result;
}
