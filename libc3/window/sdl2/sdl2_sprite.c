/* c3
 * Copyright 2022,2023 kmx.io <contact@kmx.io>
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
#include <assert.h>
#include <err.h>
#include <unistd.h>
#include <libc3/c3.h>
#include "sdl2_sprite.h"

void sdl2_sprite_clean (s_sdl2_sprite *sprite)
{
  assert(sprite);
  str_clean(&sprite->path);
  str_clean(&sprite->real_path);
  glDeleteTextures(sprite->frame_count, sprite->texture);
  free(sprite->texture);
}

static GLenum png_info_to_gl_format (int png_color_type,
                                     int png_bit_depth)
{
  switch (png_color_type) {
  case PNG_COLOR_TYPE_GRAY:
    switch (png_bit_depth) {
    case 8:  return GL_LUMINANCE8;
    case 16: return GL_LUMINANCE16;
    }
    break;
  case PNG_COLOR_TYPE_GRAY_ALPHA:
    switch (png_bit_depth) {
    case 8:  return GL_LUMINANCE8_ALPHA8;
    case 16: return GL_LUMINANCE16_ALPHA16;
    }
    break;
  case PNG_COLOR_TYPE_RGB:
    switch (png_bit_depth) {
    case 8:  return GL_RGB8;
    case 16: return GL_RGB16;
    }
    break;
  case PNG_COLOR_TYPE_RGBA:
    switch (png_bit_depth) {
    case 8:  return GL_RGBA8;
    case 16: return GL_RGBA16;
    }
    break;
  }
  return 0;
}

static GLenum png_info_to_gl_type (int png_bit_depth)
{
  switch (png_bit_depth) {
  case 8:
    return GL_UNSIGNED_BYTE;
  case 16:
    return GL_UNSIGNED_SHORT;
  }
  return 0;
}

s_sdl2_sprite * sdl2_sprite_init (s_sdl2_sprite *sprite,
                                  const s8 *path,
                                  uw dim_x, uw dim_y,
                                  uw frame_count)
{
  u8 *data;
  FILE *fp;
  GLenum gl_format;
  GLenum gl_type;
  uw i;
  u8          png_header[8]; // maximum size is 8
  png_infop   png_info;
  png_structp png_read;
  png_bytep  *png_row;
  u32         png_w;
  u32         png_h;
  s32         png_bit_depth;
  s32         png_color_type;
  u8 *sprite_data;
  uw  sprite_stride;
  uw x;
  uw y;
  uw v;
  assert(sprite);
  assert(path);
  assert(dim_x);
  assert(dim_y);
  frame_count = (frame_count > 0) ? frame_count : (dim_x * dim_y);
  str_init_copy_1(&sprite->path, path);
  if (! file_search(&sprite->path, sym_1("r"), &sprite->real_path)) {
    warnx("sdl2_sprite_init: file not found: %s", path);
    str_clean(&sprite->path);
    return NULL;
  }
  fp = fopen(sprite->real_path.ptr.ps8, "rb");
  if (! fp) {
    warn("sdl2_sprite_init: %s", sprite->real_path.ptr.ps8);
    str_clean(&sprite->path);
    str_clean(&sprite->real_path);
    return NULL;
  }
  if (fread(png_header, 1, sizeof(png_header), fp) !=
      sizeof(png_header)) {
    warn("sdl2_sprite_init: %s", sprite->real_path.ptr.ps8);
    fclose(fp);
    str_clean(&sprite->path);
    str_clean(&sprite->real_path);
    return NULL;
  }
  if (png_sig_cmp(png_header, 0, sizeof(png_header))) {
    warn("sdl2_sprite_init: %s: not a png", sprite->real_path.ptr.ps8);
    fclose(fp);
    str_clean(&sprite->path);
    str_clean(&sprite->real_path);
    return NULL;
  }
  png_read = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL,
                                    NULL);
  if (!png_read) {
    warn("sdl2_sprite_init: %s: png_create_read_struct",
         sprite->real_path.ptr.ps8);
    fclose(fp);
    str_clean(&sprite->path);
    str_clean(&sprite->real_path);
    return NULL;
  }
  if (setjmp(png_jmpbuf(png_read))) {
    png_destroy_read_struct(&png_read, &png_info, NULL);
    fclose(fp);
    str_clean(&sprite->path);
    str_clean(&sprite->real_path);
    return NULL;
  }
  png_init_io(png_read, fp);
  png_set_sig_bytes(png_read, sizeof(png_header));
  //png_set_compression_buffer_size(png_read, buffer_size);
  png_read_png(png_read, png_info,
               PNG_TRANSFORM_PACKING, NULL);
  png_get_IHDR(png_read, png_info, &png_w, &png_h,
               &png_bit_depth, &png_color_type,
               NULL, NULL, NULL);
  if (png_color_type == PNG_COLOR_TYPE_PALETTE) {
    warnx("sdl2_sprite_init: %s: PNG_COLOR_TYPE_PALETTE",
          sprite->real_path.ptr.ps8);
    png_destroy_read_struct(&png_read, &png_info, NULL);
    fclose(fp);
    str_clean(&sprite->path);
    str_clean(&sprite->real_path);
    return NULL;
  }
  png_row = png_get_rows(png_read, png_info);
  if (! png_row) {
    warnx("sdl2_sprite_init: %s: png_get_rows => NULL",
          sprite->real_path.ptr.ps8);
    png_destroy_read_struct(&png_read, &png_info, NULL);
    fclose(fp);
    str_clean(&sprite->path);
    str_clean(&sprite->real_path);
    return NULL;
  }
  sprite->total_w = png_w;
  sprite->total_h = png_h;
  sprite->dim_x = dim_x;
  sprite->dim_y = dim_y;
  sprite->w = sprite->total_w / dim_x;
  sprite->h = sprite->total_h / dim_y;
  sprite->frame_count = frame_count;
  sprite->texture = malloc(frame_count * sizeof(GLuint));
  if (! sprite->texture) {
    warn("sdl2_sprite_init: sprite->texture");
    png_destroy_read_struct(&png_read, &png_info, NULL);
    fclose(fp);
    str_clean(&sprite->path);
    str_clean(&sprite->real_path);
    return NULL;
  }
  gl_format = png_info_to_gl_format(png_color_type, png_bit_depth);
  if (! gl_format) {
    warnx("sdl2_sprite_init: %s: unknown OpenGL format",
          sprite->real_path.ptr.ps8);
    free(sprite->texture);
    png_destroy_read_struct(&png_read, &png_info, NULL);
    fclose(fp);
    str_clean(&sprite->path);
    str_clean(&sprite->real_path);
    return NULL;
  }
  gl_type = png_info_to_gl_type(png_bit_depth);
  if (! gl_type) {
    warnx("sdl2_sprite_init: %s: unknown OpenGL type",
          sprite->real_path.ptr.ps8);
    free(sprite->texture);
    png_destroy_read_struct(&png_read, &png_info, NULL);
    fclose(fp);
    str_clean(&sprite->path);
    str_clean(&sprite->real_path);
    return NULL;
  }
  sprite_stride = sprite->w * (png_bit_depth / 8);
  data = malloc(sprite->h * sprite_stride);
  if (! data) {
    warn("sdl2_sprite_init: %s", sprite->real_path.ptr.ps8);
    free(sprite->texture);
    png_destroy_read_struct(&png_read, &png_info, NULL);
    fclose(fp);
    str_clean(&sprite->path);
    str_clean(&sprite->real_path);
    return NULL;
  }
  i = 0;
  y = 0;
  while (i < sprite->frame_count && y < dim_y) {
    x = 0;
    while (i < sprite->frame_count && x < dim_x) {
      sprite_data = data;
      v = 0;
      while (v < sprite->h) {
        memcpy(sprite_data, png_row[v], sprite_stride);
        v++;
      }
      glBindTexture(GL_TEXTURE_2D, sprite->texture[i]);
      glTexImage2D(GL_TEXTURE_2D, 0, gl_format, sprite->w, sprite->h,
                   0, gl_format, gl_type, data);
      i++;
      x++;
    }
    y++;
  }
  free(data);
  png_destroy_read_struct(&png_read, &png_info, NULL);
  fclose(fp);
  return sprite;
}
