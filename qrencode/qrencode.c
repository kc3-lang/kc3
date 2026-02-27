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
#include "../image/image.h"
#include "qrencode.h"
#include <qrencode.h>

s_str * qrencode_encode_png (const s_str *src, const s_tag *size,
                             s_str *dest)
{
  s_image image;
  QRcode *qrcode;
  uw size_uw;
  const s_sym *sym_Uw = &g_sym_Uw;
  s_str tmp = {0};
  uw u;
  uw v;
  uw x;
  uw y;
  if (! uw_init_cast(&size_uw, &sym_Uw, size))
    return NULL;
  if (! (qrcode = QRcode_encodeString(src->ptr.p_pchar, 0,
                                      QR_ECLEVEL_H,
                                      QR_MODE_8,
                                      1))) {
    err_puts("qrencode_encode_png: QRcode_encodeData");
    assert(! "qrencode_encode_png: QRcode_encodeData");
    return NULL;
  }
  if (size_uw < (uw) qrcode->width * 2)
    size_uw = qrcode->width * 2;
  if (! image_init_alloc(&image, size_uw, size_uw, 1, 1))
    return NULL;
  y = 0;
  while (y < size_uw) {
    v = (uw) ((f64) y / size_uw * qrcode->width);
    x = 0;
    while (x < size_uw) {
      u = (uw) ((f64) x / size_uw * qrcode->width);
      image.data[y * size_uw + x] =
        ~((qrcode->data[v * qrcode->width + u] & 1) * 0xff);
      x++;
    }
    y++;
  }
  QRcode_free(qrcode);
  if (! image_to_png_str(&image, &tmp)) {
    err_puts("qrencode_encode_png: image_to_png_str");
    assert(! "qrencode_encode_png: image_to_png_str");
    image_clean(&image);
    return NULL;
  }
  image_clean(&image);
  *dest = tmp;
  return dest;
}
