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

s_sdl2_sprite * sdl2_sprite_init (s_sdl2_sprite *sprite,
                                  const s8 *path,
                                  uw dim_x, uw dim_y,
                                  uw frame_count)
{
  FILE *fp;
  uw i;
  png_infop png_info;
  png_structp png_read;
  uw x;
  uw y;
  assert(sprite);
  assert(path);
  assert(dim_x);
  assert(dim_y);
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
  png_read = png_create_read_struct
    (PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

   if (!png_ptr)
     return ERROR;
  sprite->total_w = ilGetInteger(IL_IMAGE_WIDTH);
  sprite->total_h = ilGetInteger(IL_IMAGE_HEIGHT);
  //bpp = ilGetInteger(IL_IMAGE_BPP);
  sprite->dim_x = dim_x;
  sprite->dim_y = dim_y;
  sprite->w = sprite->total_w / dim_x;
  sprite->h = sprite->total_h / dim_y;
  sprite->frame_count = frame_count ? frame_count : (dim_x * dim_y);
  sprite->texture = malloc(frame_count * sizeof(GLuint));
  if (! sprite->texture) {
    warn("sdl2_sprite_init: sprite->texture");
    ilDeleteImages(2, il_image);
    str_clean(&sprite->path);
    str_clean(&sprite->real_path);
    return NULL;
  }
  glGenTextures(frame_count, sprite->texture);
  ilBindImage(il_image[1]);
  ilTexImage(sprite->w, sprite->h, 1, 4, IL_RGBA, IL_UNSIGNED_BYTE,
             NULL);
  data[1] = ilGetData();
  ilBindImage(il_image[2]);
  sprite->tex_w = 1;
  while (sprite->tex_w < sprite->w)
    sprite->tex_w *= 2;
  sprite->tex_h = 1;
  while (sprite->tex_h < sprite->h)
    sprite->tex_h *= 2;
  ilTexImage(sprite->tex_w, sprite->tex_h, 1, 4, IL_RGBA,
             IL_UNSIGNED_BYTE, NULL);
  data[2] = ilGetData();
  ilBindImage(il_image[0]);
  i = 0;
  y = 0;
  while (i < sprite->frame_count && y < dim_y) {
    x = 0;
    while (i < sprite->frame_count && x < dim_x) {
      ilCopyPixels(x * sprite->w, y * sprite->h, 0,
                   sprite->w, sprite->h, 1, IL_RGBA, IL_UNSIGNED_BYTE,
                   data[1]);
      iluImageParameter(ILU_FILTER, ILU_SCALE_MITCHELL);
      ilBlit(il_image[1], 0, 0, 0, 0, 0, 0,
             sprite->tex_w, sprite->tex_h, 1);
      glBindTexture(GL_TEXTURE_2D, sprite->texture[i]);
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
                   sprite->tex_w, sprite->tex_h, 0, GL_RGBA,
                   GL_UNSIGNED_BYTE, data[2]);
      i++;
      x++;
    }
    y++;
  }
  ilDeleteImages(3, il_image);
  return sprite;
}
