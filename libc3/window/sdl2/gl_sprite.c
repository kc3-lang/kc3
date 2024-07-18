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
#include <libc3/c3.h>
#include "gl_object.h"
#include "gl_point_2d.h"
#include "gl_point_3d.h"
#include "gl_sprite.h"
#include "gl_triangle.h"

void gl_sprite_bind (const s_gl_sprite *sprite, uw frame)
{
  assert(sprite);
  assert(frame < sprite->frame_count);
  frame %= sprite->frame_count;
  assert(glGetError() == GL_NO_ERROR);
  glBindTexture(GL_TEXTURE_2D, sprite->texture[frame]);
  assert(glGetError() == GL_NO_ERROR);
}

void gl_sprite_clean (s_gl_sprite *sprite)
{
  assert(sprite);
  gl_object_clean(&sprite->object);
  str_clean(&sprite->path);
  str_clean(&sprite->real_path);
  glDeleteTextures(sprite->frame_count, sprite->texture);
  free(sprite->texture);
}

static bool png_info_to_gl_info (s32 png_color_type,
                                 s32 png_bit_depth,
                                 GLenum *gl_format,
                                 GLint  *gl_internal_format,
                                 GLenum *gl_type,
                                 u8 *components)
{
  switch (png_bit_depth) {
  case 8:  *gl_type = GL_UNSIGNED_BYTE;  break;
  case 16: *gl_type = GL_UNSIGNED_SHORT; break;
  default: *gl_type = 0; return false;
  }
  switch (png_color_type) {
  case PNG_COLOR_TYPE_GRAY:
    *components = 1;
    *gl_format = GL_RED;
    switch (png_bit_depth) {
    case 8:  *gl_internal_format = GL_RED;  break;
    case 16: *gl_internal_format = GL_RED; break;
    default: *gl_internal_format = 0; return false;
    }
    break;
  case PNG_COLOR_TYPE_GRAY_ALPHA:
    *components = 2;
    *gl_format = GL_RG;
    switch (png_bit_depth) {
    case 8:  *gl_internal_format = GL_RG;  break;
    case 16: *gl_internal_format = GL_RG; break;
    default: *gl_internal_format = 0; return false;
    }
    break;
  case PNG_COLOR_TYPE_RGB:
    *components = 3;
    *gl_format = GL_RGB;
    switch (png_bit_depth) {
    case 8:  *gl_internal_format = GL_RGB;  break;
    case 16: *gl_internal_format = GL_RGB; break;
    default: *gl_internal_format = 0; return false;
    }
    break;
  case PNG_COLOR_TYPE_RGBA:
    *components = 4;
    *gl_format = GL_RGBA;
    switch (png_bit_depth) {
    case 8:  *gl_internal_format = GL_RGBA8;  break;
    case 16: *gl_internal_format = GL_RGBA16; break;
    default: *gl_internal_format = 0; return false;
    }
    break;
  default:
    *components = 0;
    *gl_format = 0;
    *gl_internal_format = 0;
    return false;
  }
  return true;
}

s_gl_sprite * gl_sprite_init (s_gl_sprite *sprite, const s8 *path,
                              uw dim_x, uw dim_y, uw frame_count)
{
  u8 *data;
  FILE *fp;
  GLenum gl_format;
  GLint  gl_internal_format;
  GLenum gl_type;
  uw i;
  s32         png_bit_depth;
  s32         png_color_type;
  u8          png_components;
  png_bytep   png_data;
  u32         png_h;
  u8          png_header[8]; // maximum size is 8
  png_infop   png_info;
  uw          png_pixel_size;
  png_structp png_read;
  png_bytep  *png_row;
  u32         png_w;
  u8 *sprite_data;
  uw  sprite_stride;
  uw x;
  uw y;
  uw v;
  uw dimension;
  s_gl_sprite tmp = {0};
  s_gl_triangle *triangle;
  s_gl_vertex *vertex;
  assert(sprite);
  assert(path);
  assert(dim_x);
  assert(dim_y);
  assert(glGetError() == GL_NO_ERROR);
  tmp.frame_count = (frame_count > 0) ? frame_count :
    (dim_x * dim_y);
  str_init_copy_1(&tmp.path, path);
  if (! file_search(&tmp.path, sym_1("r"), &tmp.real_path)) {
    err_write_1("sdl2_sprite_init: file not found: ");
    err_puts(path);
    str_clean(&tmp.path);
    return NULL;
  }
  fp = fopen(tmp.real_path.ptr.ps8, "rb");
  if (! fp) {
    err_write_1("sdl2_sprite_init: fopen: ");
    err_puts(tmp.real_path.ptr.ps8);
    str_clean(&tmp.path);
    str_clean(&tmp.real_path);
    return NULL;
  }
  if (fread(png_header, 1, sizeof(png_header), fp) !=
      sizeof(png_header)) {
    err_write_1("sdl2_sprite_init: fread: ");
    err_puts(tmp.real_path.ptr.ps8);
    fclose(fp);
    str_clean(&tmp.path);
    str_clean(&tmp.real_path);
    return NULL;
  }
  if (png_sig_cmp(png_header, 0, sizeof(png_header))) {
    err_write_1("sdl2_sprite_init: not a png: ");
    err_puts(tmp.real_path.ptr.ps8);
    fclose(fp);
    str_clean(&tmp.path);
    str_clean(&tmp.real_path);
    return NULL;
  }
  png_read = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL,
				    NULL);
  if (! png_read) {
    err_write_1("sdl2_sprite_init: png_create_read_struct: ");
    err_puts(tmp.real_path.ptr.ps8);
    fclose(fp);
    str_clean(&tmp.path);
    str_clean(&tmp.real_path);
    return NULL;
  }
  png_info = png_create_info_struct(png_read);
  if (! png_info) {
    err_write_1("sdl2_sprite_init: png_create_info_struct: ");
    err_puts(tmp.real_path.ptr.ps8);
    png_destroy_read_struct(&png_read, NULL, NULL);
    fclose(fp);
    str_clean(&tmp.path);
    str_clean(&tmp.real_path);
    return NULL;
  }
  gl_internal_format = 0;
  gl_format = 0;
  gl_type = 0;
  png_components = 0;
  if (setjmp(png_jmpbuf(png_read))) {
    png_destroy_read_struct(&png_read, &png_info, NULL);
    fclose(fp);
    str_clean(&tmp.path);
    str_clean(&tmp.real_path);
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
  if (! png_info_to_gl_info(png_color_type, png_bit_depth, &gl_format,
			    &gl_internal_format, &gl_type,
			    &png_components)) {
    if (! gl_format || ! png_components) {
      err_write_1("sdl2_sprite_init: unknown PNG color type ");
      err_inspect_s32(&png_color_type);
      err_write_1(": ");
      err_puts(tmp.real_path.ptr.ps8);
    }
    if (! gl_internal_format) {
      err_write_1("sdl2_sprite_init: unknown OpenGL internal format: ");
      err_puts(tmp.real_path.ptr.ps8);
    }
    if (! gl_type) {
      err_write_1("sdl2_sprite_init: unknown OpenGL type: ");
      err_puts(tmp.real_path.ptr.ps8);
    }
    png_destroy_read_struct(&png_read, &png_info, NULL);
    fclose(fp);
    str_clean(&tmp.path);
    str_clean(&tmp.real_path);
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
  png_destroy_read_struct(&png_read, &png_info, NULL);
  fclose(fp);
  tmp.total_w = png_w;
  tmp.total_h = png_h;
  tmp.dim_x = dim_x;
  tmp.dim_y = dim_y;
  tmp.w = tmp.total_w / dim_x;
  tmp.h = tmp.total_h / dim_y;
  tmp.texture = calloc(tmp.frame_count, sizeof(GLuint));
  if (! tmp.texture) {
    err_puts("sdl2_sprite_init: tmp.texture:"
             " failed to allocate memory");
    str_clean(&tmp.path);
    str_clean(&tmp.real_path);
    return NULL;
  }
  glGenTextures(tmp.frame_count, tmp.texture);
  GLenum gl_error = glGetError();
  if (gl_error != GL_NO_ERROR) {
    err_write_1("sdl2_sprite_init: ");
    err_inspect_str(&tmp.real_path);
    err_write_1(": glGenTextures: ");
    err_puts((const s8 *) gluErrorString(gl_error));
    free(tmp.texture);
    str_clean(&tmp.path);
    str_clean(&tmp.real_path);
    return NULL;
  }
  sprite_stride = tmp.w * png_pixel_size;
  data = malloc(tmp.h * sprite_stride);
  if (! data) {
    err_write_1("sdl2_sprite_init: failed to allocate memory: ");
    err_puts(tmp.real_path.ptr.ps8);
    free(tmp.texture);
    str_clean(&tmp.path);
    str_clean(&tmp.real_path);
    return NULL;
  }
  i = 0;
  y = 0;
  while (i < tmp.frame_count && y < dim_y) {
    x = 0;
    while (i < tmp.frame_count && x < dim_x) {
      sprite_data = data + sprite_stride * tmp.h;
      v = 0;
      while (v < tmp.h) {
	sprite_data -= sprite_stride;
	memcpy(sprite_data,
	       png_row[y * tmp.h + v] + x * sprite_stride,
	       sprite_stride);
	v++;
      }
      glBindTexture(GL_TEXTURE_2D, tmp.texture[i]);
      gl_error = glGetError();
      if (gl_error != GL_NO_ERROR) {
	err_write_1("sdl2_sprite_init: ");
        err_inspect_str(&tmp.real_path);
        err_write_1(": glBindTexture: ");
        err_puts((const s8 *) gluErrorString(gl_error));
	return NULL;
      }
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,
                      GL_CLAMP_TO_EDGE);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,
                      GL_CLAMP_TO_EDGE);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                      GL_LINEAR_MIPMAP_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      gl_error = glGetError();
      if (gl_error != GL_NO_ERROR) {
	err_write_1("sdl2_sprite_init: ");
        err_inspect_str(&tmp.real_path);
        err_write_1(": glTexParameteri: ");
        err_puts((const s8 *) gluErrorString(gl_error));
	return NULL;
      }
      glTexImage2D(GL_TEXTURE_2D, 0, gl_format, tmp.w, tmp.h,
                   0, gl_format, gl_type, data);
      //glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tmp.w, tmp.h,
      //             0, GL_RGBA, GL_UNSIGNED_BYTE, data);
      gl_error = glGetError();
      if (gl_error != GL_NO_ERROR) {
	err_write_1("sdl2_sprite_init: ");
        err_inspect_str(&tmp.real_path);
        err_write_1(": glTexImage2D: ");
        err_puts((const s8 *) gluErrorString(gl_error));
	return NULL;
      }
      glGenerateMipmap(GL_TEXTURE_2D);
      assert(glGetError() == GL_NO_ERROR);
      i++;
      x++;
    }
    y++;
  }
  glBindTexture(GL_TEXTURE_2D, 0);
  free(data);
  free(png_data);
  free(png_row);
  assert(glGetError() == GL_NO_ERROR);
  gl_object_init(&tmp.object);
  dimension = 4;
  array_init(&tmp.object.vertex, sym_1("GL.Vertex"), 1,
             &dimension);
  array_allocate(&tmp.object.vertex);
  dimension = 2;
  array_init(&tmp.object.triangle, sym_1("GL.Triangle"), 1,
             &dimension);
  array_allocate(&tmp.object.triangle);
  vertex = tmp.object.vertex.data;
  gl_point_3d_init(&vertex[0].position, 0.0, tmp.h, 0.0);
  gl_point_3d_init(&vertex[0].normal, 0.0, 0.0, -1.0);
  gl_point_2d_init(&vertex[0].tex_coord, 0.0, 1.0);
  gl_point_3d_init(&vertex[1].position, 0.0, 0.0, 0.0);
  gl_point_3d_init(&vertex[1].normal, 0.0, 0.0, -1.0);
  gl_point_2d_init(&vertex[1].tex_coord, 0.0, 0.0);
  gl_point_3d_init(&vertex[2].position, tmp.w, tmp.h, 0.0);
  gl_point_3d_init(&vertex[2].normal, 0.0, 0.0, -1.0);
  gl_point_2d_init(&vertex[2].tex_coord, 1.0, 1.0);
  gl_point_3d_init(&vertex[3].position, tmp.w, 0.0, 0.0);
  gl_point_3d_init(&vertex[3].normal, 0.0, 0.0, -1.0);
  gl_point_2d_init(&vertex[3].tex_coord, 1.0, 0.0);
  triangle = tmp.object.triangle.data;
  gl_triangle_init(triangle + 0, 0, 1, 2);
  gl_triangle_init(triangle + 1, 1, 3, 2);
  gl_object_update(&tmp.object);
  *sprite = tmp;  
  return sprite;
}

void gl_sprite_render (const s_gl_sprite *sprite, uw frame)
{
  assert(sprite);
  assert(glGetError() == GL_NO_ERROR);
  gl_sprite_bind(sprite, frame);
  assert(glGetError() == GL_NO_ERROR);
  gl_object_render(&sprite->object);
  assert(glGetError() == GL_NO_ERROR);
  glBindTexture(GL_TEXTURE_2D, 0);
  assert(glGetError() == GL_NO_ERROR);
}
