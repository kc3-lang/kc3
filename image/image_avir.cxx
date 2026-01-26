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
extern "C" {
#include "../libkc3/kc3.h"
#include "image.h"
#include "image_avir.h"
}

#include "avir/avir.h"

extern "C" {

  s_image * image_avir_resize_8 (s_image *src,
                                 s_image *dest)
  {
    avir::CImageResizer<> ir(8);
    if (src->components != dest->components) {
      err_puts("image_avir_resize_8: invalid components");
      return NULL;
    }
    ir.resizeImage((u8 *) src->data, src->w, src->h, 0,
                   (u8 *) dest->data, dest->w, dest->h, src->components,
                   0);
    return src;
  }

}
