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
#ifndef KC3_IMAGE_EGL_H
#define KC3_IMAGE_EGL_H

#include "types.h"

/* Stack-allocation compatible functions, call image_egl_clean
   after use. */
void          image_egl_clean (s_image_egl *image);
s_image_egl * image_egl_init (s_image_egl *image, uw w, uw h);

/* Operators. */
void          image_egl_read (s_image_egl *image);
s_image_egl * image_egl_resize_to_fill_file (s_image_egl *image,
                                             s_str *path);
s_image_egl * image_egl_resize_to_fill_sprite (s_image_egl *image,
                                               s_gl_sprite *sprite);

#endif /* KC3_IMAGE_EGL_H */
