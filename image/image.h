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
s_image * image_init_file (s_image *image, const s_str *path);
s_image * image_init_jpeg (s_image *image, const s_str *path, FILE *fp);
s_image * image_init_png (s_image *image, const s_str *path, FILE *fp);

/* Heap allocation functions, call image_delete after use. */
void      image_delete (s_image *image);
s_image * image_new_alloc (uw w, uw h, u8 components, u8 pixel_size);

/* Operators. */
s_image * image_resize_to_fill (s_image *src, s_image *dest);

/* Observers. */
s_image * image_to_jpeg_file (s_image *image, s_str *path);
s_image * image_to_png_file (s_image *image, s_str *path);
s_str *   image_to_png_str (s_image *image, s_str *dest);

/* KC3 interface. */
void       kc3_image_delete (s_image **image);
s_image ** kc3_image_new_alloc (s_image **image, s_tag *w, s_tag *h,
                                u8 components, u8 pixel_size);
s_image ** kc3_image_new_resize_to_fill_file (s_image **image,
                                              s_tag *tag_w,
                                              s_tag *tag_h,
                                              s_str *src_path);
s_image ** kc3_image_to_png_file (s_image **image, s_str *path);
s_str *    kc3_image_to_png_str (s_image **image, s_str *dest);

#endif /* KC3_IMAGE_H */
