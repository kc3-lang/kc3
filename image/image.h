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
#ifndef KC3_IMAGE_H
#define KC3_IMAGE_H

#include "types.h"

/* Stack-allocation compatible functions, call image_clean after use. */
void      image_clean (s_image *image);
s_image * image_init (s_image *image);
s_image * image_init_alloc (s_image *image, uw w, uw h, u8 components,
                            u8 pixel_size);

/* Observers. */
bool image_to_jpeg_file (s_image *image, s_str *path);
bool image_to_png_file (s_image *image, s_str *path);

#endif /* KC3_IMAGE_H */
