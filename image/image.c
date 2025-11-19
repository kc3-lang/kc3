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
#include <png.h>
#include <stdio.h>
#include "../libkc3/kc3.h"
#include "image.h"

void image_clean (s_image *image)
{
  if (image && image->data)
    free(image->data);
}

s_image * image_init (s_image *image)
{
  s_image tmp = {0};
  *image = tmp;
  return image;
}

s_image * image_init_alloc (s_image *image, uw w, uw h, u8 components,
                            u8 pixel_size)
{
  uw image_size;
  s_image tmp = {0};
  image_size = w * h * pixel_size;
  tmp.w = w;
  tmp.h = h;
  tmp.components = components;
  tmp.pixel_size = pixel_size;
  if (! (tmp.data = alloc(image_size)))
    return NULL;
  *image = tmp;
  return image;
}

bool image_to_png_file (s_image *image, s_str *path)
{
  FILE *fp;
  png_structp png_ptr;
  png_infop info_ptr;
  png_byte color_type;
  png_byte **row_pointers;
  uw y;
  if (! image ||
      ! image->w ||
      ! image->h ||
      ! image->components ||
      ! image->pixel_size ||
      ! image->data ||
      ! path) {
    ERROR("invalid argument");
    return false;
  }
  fp = fopen(path->ptr.pchar, "wb");
  if (! fp) {
    err_write_1("image_to_png_file: fopen: ");
    err_puts(path->ptr.pchar);
    return false;
  }
  png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING,
                                    NULL, NULL, NULL);
  if (! png_ptr) {
    err_puts("image_to_png_file: png_create_write_struct");
    fclose(fp);
    return false;
  }
  info_ptr = png_create_info_struct(png_ptr);
  if (! info_ptr) {
    err_puts("image_to_png_file: png_create_info_struct");
    png_destroy_write_struct(&png_ptr, NULL);
    fclose(fp);
    return false;
  }
  if (setjmp(png_jmpbuf(png_ptr))) {
    err_puts("image_to_png_file: png error");
    png_destroy_write_struct(&png_ptr, &info_ptr);
    fclose(fp);
    return false;
  }
  png_init_io(png_ptr, fp);
  switch (image->components) {
  case 1: color_type = PNG_COLOR_TYPE_GRAY;       break;
  case 2: color_type = PNG_COLOR_TYPE_GRAY_ALPHA; break;
  case 3: color_type = PNG_COLOR_TYPE_RGB;        break;
  case 4: color_type = PNG_COLOR_TYPE_RGBA;       break;
  default:
    err_puts("image_to_png_file: invalid components");
    png_destroy_write_struct(&png_ptr, &info_ptr);
    fclose(fp);
    return false;
  }
  png_set_IHDR(png_ptr, info_ptr, image->w, image->h, 8,
               color_type, PNG_INTERLACE_NONE,
               PNG_COMPRESSION_TYPE_DEFAULT,
               PNG_FILTER_TYPE_DEFAULT);
  png_write_info(png_ptr, info_ptr);
  row_pointers = alloc(image->h * sizeof(png_byte *));
  if (! row_pointers) {
    err_puts("image_to_png_file: alloc");
    png_destroy_write_struct(&png_ptr, &info_ptr);
    fclose(fp);
    return false;
  }
  for (y = 0; y < image->h; y++)
    row_pointers[y] = (png_byte *) image->data +
      y * image->w * image->pixel_size;
  png_write_image(png_ptr, row_pointers);
  png_write_end(png_ptr, NULL);
  free(row_pointers);
  png_destroy_write_struct(&png_ptr, &info_ptr);
  fclose(fp);
  return true;
}
