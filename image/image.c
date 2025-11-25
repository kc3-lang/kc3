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
#include <stdio.h>
#include <string.h>
#include "../libkc3/kc3.h"
#include <jpeglib.h>
#include <png.h>
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

s_image * image_init_file (s_image *image, const s_str *path)
{
  FILE *fp;
  u8 header[8];
  fp = fopen(path->ptr.pchar, "rb");
  if (! fp) {
    err_write_1("image_init_file: fopen: ");
    err_inspect_str(path);
    err_write_1("\n");
    return NULL;
  }
  if (fread(header, 1, sizeof(header), fp) != sizeof(header)) {
    err_write_1("image_init_file: fread: ");
    err_inspect_str(path);
    err_write_1("\n");
    fclose(fp);
    return NULL;
  }
  rewind(fp);
  if (! png_sig_cmp(header, 0, sizeof(header))) {
    if (! image_init_png(image, path, fp)) {
      fclose(fp);
      return NULL;
    }
    fclose(fp);
    return image;
  }
  if (header[0] == 0xff && header[1] == 0xd8) {
    if (! image_init_jpeg(image, path, fp)) {
      fclose(fp);
      return NULL;
    }
    fclose(fp);
    return image;
  }
  fclose(fp);
  err_puts("image_init_file: unsupported file format");
  assert(! "image_init_file: unsupported file format");
  return NULL;
}

s_image * image_init_jpeg (s_image *image, const s_str *path, FILE *fp)
{
  u8                           *jpeg_buffer;
  u8                            jpeg_components;
  struct jpeg_error_mgr         jpeg_err;
  uw                            jpeg_h;
  struct jpeg_decompress_struct jpeg_info;
  uw                            jpeg_pixel_size;
  uw                            jpeg_w;
  assert(image);
  assert(path);
  assert(fp);
  jpeg_info.err = jpeg_std_error(&jpeg_err);
  jpeg_create_decompress(&jpeg_info);
  jpeg_stdio_src(&jpeg_info, fp);
  jpeg_read_header(&jpeg_info, TRUE);
  jpeg_start_decompress(&jpeg_info);
  jpeg_w = jpeg_info.output_width;
  jpeg_h = jpeg_info.output_height;
  jpeg_components = jpeg_info.output_components;
  jpeg_pixel_size = jpeg_components;
  if (jpeg_h > SIZE_MAX / (jpeg_w * jpeg_pixel_size)) {
    err_write_1("image_init_jpeg: image too large: ");
    err_inspect_str(path);
    err_write_1("\n");
    jpeg_destroy_decompress(&jpeg_info);
    return NULL;
  }
  if (! image_init_alloc(image, jpeg_w, jpeg_h, jpeg_components,
                         jpeg_pixel_size)) {
    err_write_1("image_init_jpeg: image_init_alloc failed: ");
    err_inspect_str(path);
    err_write_1("\n");
    jpeg_destroy_decompress(&jpeg_info);
    return NULL;
  }
  while (jpeg_info.output_scanline < jpeg_info.output_height) {
    jpeg_buffer = image->data +
      jpeg_info.output_scanline * jpeg_w * jpeg_pixel_size;
    jpeg_read_scanlines(&jpeg_info, &jpeg_buffer, 1);
  }
  jpeg_finish_decompress(&jpeg_info);
  jpeg_destroy_decompress(&jpeg_info);
  return image;
}

s_image * image_init_png (s_image *image, const s_str *path, FILE *fp)
{
  uw i;
  s32 png_bit_depth;
  s32 png_color_type;
  u8 png_components;
  png_bytep png_data;
  u32 png_h;
  u8 png_header[8];
  png_infop png_info;
  uw png_pixel_size;
  png_structp png_read;
  png_bytep *png_row;
  u32 png_w;
  assert(image);
  assert(path);
  assert(fp);
  if (fread(png_header, 1, sizeof(png_header), fp) !=
      sizeof(png_header)) {
    err_write_1("image_init_png: fread: ");
    err_inspect_str(path);
    err_write_1("\n");
    return NULL;
  }
  if (png_sig_cmp(png_header, 0, sizeof(png_header))) {
    err_write_1("image_init_png: not a png: ");
    err_inspect_str(path);
    err_write_1("\n");
    return NULL;
  }
  png_read = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL,
                                    NULL, NULL);
  if (! png_read) {
    err_write_1("image_init_png: png_create_read_struct: ");
    err_inspect_str(path);
    err_write_1("\n");
    return NULL;
  }
  png_info = png_create_info_struct(png_read);
  if (! png_info) {
    err_write_1("image_init_png: png_create_info_struct: ");
    err_inspect_str(path);
    err_write_1("\n");
    png_destroy_read_struct(&png_read, NULL, NULL);
    return NULL;
  }
  if (setjmp(png_jmpbuf(png_read))) {
    png_destroy_read_struct(&png_read, &png_info, NULL);
    return NULL;
  }
  png_set_sig_bytes(png_read, sizeof(png_header));
  png_init_io(png_read, fp);
  png_read_info(png_read, png_info);
  png_get_IHDR(png_read, png_info, &png_w, &png_h,
               &png_bit_depth, &png_color_type,
               NULL, NULL, NULL);
  if (png_color_type == PNG_COLOR_TYPE_PALETTE)
    png_set_palette_to_rgb(png_read);
  switch (png_color_type) {
  case PNG_COLOR_TYPE_GRAY:       png_components = 1; break;
  case PNG_COLOR_TYPE_GRAY_ALPHA: png_components = 2; break;
  case PNG_COLOR_TYPE_RGB:        png_components = 3; break;
  case PNG_COLOR_TYPE_RGBA:       png_components = 4; break;
  default:
    err_write_1("image_init_png: unknown PNG color type ");
    err_inspect_s32(png_color_type);
    err_write_1(": ");
    err_inspect_str(path);
    err_write_1("\n");
    png_destroy_read_struct(&png_read, &png_info, NULL);
    return NULL;
  }
  png_pixel_size = (png_bit_depth / 8) * png_components;
  if (! png_pixel_size)
    png_error(png_read, "unknown png pixel size");
  if (png_h > PNG_SIZE_MAX / (png_w * png_pixel_size))
    png_error(png_read, "image_data buffer would be too large");
  png_data = png_malloc(png_read, png_h * png_w * png_pixel_size);
  png_row = png_malloc(png_read, png_h * sizeof(png_bytep));
  i = 0;
  while (i < png_h) {
    png_row[i] = png_data + i * png_w * png_pixel_size;
    i++;
  }
  if (png_bit_depth < 8)
    png_set_packing(png_read);
  png_read_image(png_read, png_row);
  if (! image_init_alloc(image, png_w, png_h, png_components,
                         png_pixel_size)) {
    err_write_1("image_init_png: image_init_alloc failed: ");
    err_inspect_str(path);
    err_write_1("\n");
    free(png_data);
    free(png_row);
    png_destroy_read_struct(&png_read, &png_info, NULL);
    return NULL;
  }
  memcpy(image->data, png_data, png_h * png_w * png_pixel_size);
  free(png_data);
  free(png_row);
  png_destroy_read_struct(&png_read, &png_info, NULL);
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
