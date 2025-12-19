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
#include "image_avir.h"

void image_clean (s_image *image)
{
  if (image && image->data)
    free(image->data);
}

void image_delete (s_image *image)
{
  image_clean(image);
  free(image);
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

s_image * image_new_alloc (uw w, uw h, u8 components, u8 pixel_size)
{
  s_image *image;
  if (! (image = alloc(sizeof(s_image))))
    return NULL;
  if (! image_init_alloc(image, w, h, components, pixel_size)) {
    free(image);
    return NULL;
  }
  return image;
}

s_image * image_resize_to_fill (s_image *src, s_image *dest)
{
  f32 dest_aspect;
  s_image resized = {0};
  uw resize_h;
  uw resize_w;
  f32 src_aspect;
  sw x;
  sw y;
  assert(src);
  assert(dest);
  assert(src->data);
  assert(dest->data);
  if (! src->w || ! src->h || ! dest->w || ! dest->h) {
    err_puts("image_resize_to_fill: invalid dimensions");
    return NULL;
  }
  if (src->components != dest->components) {
    err_puts("image_resize_to_fill: invalid components");
    return NULL;
  }
  if (src->pixel_size != dest->pixel_size) {
    err_puts("image_resize_to_fill: invalid pixel size");
    return NULL;
  }
  src_aspect = (f32) src->w / (f32) src->h;
  dest_aspect = (f32) dest->w / (f32) dest->h;
  if (src_aspect == dest_aspect) {
    if (! image_avir_resize_8(src, dest)) {
      err_puts("image_resize_to_fill: 1:1 image_avir_resize_8");
      return NULL;
    }
    return dest;
  }
  else if (src_aspect < dest_aspect) {
    resize_w = dest->w;
    resize_h = (uw) ((f32) resize_w / src_aspect);
    x = 0;
    assert(resize_h >= dest->h);
    y = (resize_h - dest->h) / 2;
  }
  else {
    resize_h = dest->h;
    resize_w = (uw) ((f32) resize_h * src_aspect);
    y = 0;
    assert(resize_w >= dest->w);
    x = (resize_w - dest->w) / 2;
  }
  if (! image_init_alloc(&resized, resize_w, resize_h, src->components,
                         src->pixel_size)) {
    err_puts("image_resize_to_fill: image_init_alloc failed");
    return NULL;
  }
  if (! image_avir_resize_8(src, &resized)) {
    err_puts("image_resize_to_fill: image_avir_resize_8 failed");
    image_clean(&resized);
    return NULL;
  }
  // memset(dest->data, 0, dest->h * dest->w * dest->pixel_size);
  uw v = 0;
  while (v < dest->h) {
    memcpy(dest->data + v * dest->w * dest->pixel_size,
           resized.data + (((v + y) * resize_w + x) *
                           resized.pixel_size),
           dest->w * dest->pixel_size);
    v++;
  }
  image_clean(&resized);
  return dest;
}

s_image * image_to_png_file (s_image *image, s_str *path)
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
    return NULL;
  }
  fp = fopen(path->ptr.pchar, "wb");
  if (! fp) {
    err_write_1("image_to_png_file: fopen: ");
    err_puts(path->ptr.pchar);
    return NULL;
  }
  png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING,
                                    NULL, NULL, NULL);
  if (! png_ptr) {
    err_puts("image_to_png_file: png_create_write_struct");
    fclose(fp);
    return NULL;
  }
  info_ptr = png_create_info_struct(png_ptr);
  if (! info_ptr) {
    err_puts("image_to_png_file: png_create_info_struct");
    png_destroy_write_struct(&png_ptr, NULL);
    fclose(fp);
    return NULL;
  }
  if (setjmp(png_jmpbuf(png_ptr))) {
    err_puts("image_to_png_file: png error");
    png_destroy_write_struct(&png_ptr, &info_ptr);
    fclose(fp);
    return NULL;
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
    return NULL;
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
    return NULL;
  }
  for (y = 0; y < image->h; y++)
    row_pointers[y] = (png_byte *) image->data +
      y * image->w * image->pixel_size;
  png_write_image(png_ptr, row_pointers);
  png_write_end(png_ptr, NULL);
  free(row_pointers);
  png_destroy_write_struct(&png_ptr, &info_ptr);
  fclose(fp);
  return image;
}

static void image_png_write_callback (png_structp png_ptr, png_bytep data,
                                      png_size_t length)
{
  s_buf *out;
  out = (s_buf *) png_get_io_ptr(png_ptr);
  buf_write(out, data, length);
}

s_str * image_to_png_str (s_image *image, s_str *dest)
{
  png_structp png_ptr;
  png_infop info_ptr;
  png_byte color_type;
  png_byte **row_pointers;
  s_buf out;
  s_str tmp;
  uw y;
  if (! image ||
      ! image->w ||
      ! image->h ||
      ! image->components ||
      ! image->pixel_size ||
      ! image->data ||
      ! dest) {
    ERROR("invalid argument");
    return NULL;
  }
  if (! buf_init_alloc(&out, image->w * image->h * image->pixel_size))
    return NULL;
  png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING,
                                    NULL, NULL, NULL);
  if (! png_ptr) {
    err_puts("image_to_png_str: png_create_write_struct");
    buf_clean(&out);
    return NULL;
  }
  info_ptr = png_create_info_struct(png_ptr);
  if (! info_ptr) {
    err_puts("image_to_png_str: png_create_info_struct");
    png_destroy_write_struct(&png_ptr, NULL);
    buf_clean(&out);
    return NULL;
  }
  if (setjmp(png_jmpbuf(png_ptr))) {
    err_puts("image_to_png_str: png error");
    png_destroy_write_struct(&png_ptr, &info_ptr);
    buf_clean(&out);
    return NULL;
  }
  png_set_write_fn(png_ptr, &out, image_png_write_callback, NULL);
  switch (image->components) {
  case 1: color_type = PNG_COLOR_TYPE_GRAY;       break;
  case 2: color_type = PNG_COLOR_TYPE_GRAY_ALPHA; break;
  case 3: color_type = PNG_COLOR_TYPE_RGB;        break;
  case 4: color_type = PNG_COLOR_TYPE_RGBA;       break;
  default:
    err_puts("image_to_png_str: invalid components");
    png_destroy_write_struct(&png_ptr, &info_ptr);
    buf_clean(&out);
    return NULL;
  }
  png_set_IHDR(png_ptr, info_ptr, image->w, image->h, 8,
               color_type, PNG_INTERLACE_NONE,
               PNG_COMPRESSION_TYPE_DEFAULT,
               PNG_FILTER_TYPE_DEFAULT);
  png_write_info(png_ptr, info_ptr);
  row_pointers = alloc(image->h * sizeof(png_byte *));
  if (! row_pointers) {
    err_puts("image_to_png_str: alloc");
    png_destroy_write_struct(&png_ptr, &info_ptr);
    buf_clean(&out);
    return NULL;
  }
  for (y = 0; y < image->h; y++)
    row_pointers[y] = (png_byte *) image->data +
      y * image->w * image->pixel_size;
  png_write_image(png_ptr, row_pointers);
  png_write_end(png_ptr, NULL);
  free(row_pointers);
  png_destroy_write_struct(&png_ptr, &info_ptr);
  if (buf_read_to_str(&out, &tmp) < 0) {
    buf_clean(&out);
    return NULL;
  }
  buf_clean(&out);
  *dest = tmp;
  return dest;
}

void kc3_image_delete (s_image **image)
{
  assert(image);
  assert(*image);
  image_delete(*image);
}

s_image ** kc3_image_new_resize_to_fill_file (s_image **image,
                                              s_tag *tag_w,
                                              s_tag *tag_h,
                                              s_str *src_path)
{
  uw h;
  s_image src;
  const s_sym *sym_Uw = &g_sym_Uw;
  s_image *tmp = NULL;
  uw w;
  if (! uw_init_cast(&w, &sym_Uw, tag_w))
    return NULL;
  if (! uw_init_cast(&h, &sym_Uw, tag_h))
    return NULL;
  if (! image_init_file(&src, src_path))
    return NULL;
  if (! (tmp = image_new_alloc(w, h, src.components, src.pixel_size)))
    return NULL;
  if (! image_resize_to_fill(&src, tmp)) {
    image_clean(&src);
    return NULL;
  }
  image_clean(&src);
  *image = tmp;
  return image;
}

s_image ** kc3_image_to_png_file (s_image **image, s_str *path)
{
  if (! image_to_png_file(*image, path))
    return NULL;
  return image;
}

s_str * kc3_image_to_png_str (s_image **image, s_str *dest)
{
  return image_to_png_str(*image, dest);
}
