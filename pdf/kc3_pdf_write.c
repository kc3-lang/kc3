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

void kc3_pdf_jpeg_to_xobject(char *buf, u8 const *data,
                             size_t width, size_t height, size_t length)
{
    ssize_t nb = sprintf(buf,
        "<< /Type /XObject\n"
        "/Subtype /Image\n"
        "/Width %ld\n"
        "/Height %ld\n"
        "/ColorSpace /DeviceRGB\n"
        "/BitsPerComponent 8\n"
        "/Filter /DCTDecode\n"
        "/Length %ld\n"
        ">>\n",
        width, height, length);
    memcpy(buf + nb, (char *)data, length);
    memcpy(buf + nb + length,
        "\nEndstream\n"
        "EndXObject\n", 22);
}

s_pdf_write ** kc3_pdf_write_image_from_file (s_pdf_write **doc, s_str *filepath)
{
    struct jpeg_decompress_struct cinfo;
    struct jpeg_error_mgr jerr;
    FILE *infile = fopen(filepath->ptr.pchar, "rb");
    if (infile == NULL) {
        ERROR("Error opening JPEG file.");
        return NULL;
    }
    cinfo.err = jpeg_std_error(&jerr);
    jpeg_stdio_src(&cinfo, infile);
    jpeg_read_header(&cinfo, TRUE);

    int width = cinfo.output_width;
    int height = cinfo.output_height;
    int num_components = cinfo.output_components;

    // replace with pdf stream buffer
    u8 *image_data = alloc(width * height * num_components);
    u8 *row_pointer = image_data;

    while (cinfo.output_scanline < cinfo.output_height) {
        jpeg_read_scanlines(&cinfo, &row_pointer, 1);
        row_pointer += width * num_components;
    }

    char *buffer = (char *)alloc((width * height * num_components) << 1);

    kc3_pdf_jpeg_to_xobject(buffer, image_data,
                             width, height, width * height * num_components);

    (void)doc;
    // pdf_write_append(doc, buffer);

    free(image_data);
    fclose(infile);
    return doc;
}
