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
#include <libc3/c3.h>
#include "cairo_sprite.h"

bool g_il_is_loaded = false;

static void ilEnsureInit (void)
{
  if (! g_il_is_loaded) {
    ilInit();
    iluInit();
    g_il_is_loaded = true;
  }
}

void ilClean (void)
{
  /*
  if (g_il_is_loaded) {
    iluDestroy();
    ilDestroy();
    g_il_is_loaded = false;
  }
  */
}

void cairo_sprite_blit (const s_cairo_sprite *sprite, uw frame,
                        cairo_t *cr, uw x, uw y)
{
  assert(sprite);
  assert(frame < sprite->frame_count);
  assert(cr);
  frame %= sprite->frame_count;
  cairo_set_source_surface(cr, sprite->surface[frame], 0, 0);
  cairo_rectangle(cr, x, y, sprite->w, sprite->h);
  cairo_fill(cr);
}

void cairo_sprite_clean (s_cairo_sprite *sprite)
{
  uw i;
  assert(sprite);
  i = 0;
  while (i < sprite->frame_count) {
    cairo_surface_destroy(sprite->surface[i]);
    i++;
  }
  free(sprite->surface);
}

s_cairo_sprite * cairo_sprite_init (s_cairo_sprite *sprite,
                                    const s8 *path,
                                    uw dim_x, uw dim_y,
                                    uw frame_count)
{
  assert(sprite);
  assert(dim_x);
  assert(dim_y);
    ILubyte *data;
  uw i;
  ILuint il_image[2];
  uw x;
  uw y;
  assert(sprite);
  assert(path);
  assert(dim_x);
  assert(dim_y);
  str_init_copy_1(&sprite->path, path);
  if (! file_search(&sprite->path, sym_1("r"), &sprite->real_path)) {
    warnx("cairo_sprite_init: file not found: %s", path);
    str_clean(&sprite->path);
    return NULL;
  }
  ilEnsureInit();
  ilGenImages(2, il_image);
  ilBindImage(il_image[0]);
  if (! ilLoadImage(sprite->path.ptr.ps8)) {
    warnx("cairo_sprite_init: error loading image: %s",
          iluErrorString(ilGetError()));
    str_clean(&sprite->path);
    str_clean(&sprite->real_path);
    return NULL;
  }
  sprite->total_w = ilGetInteger(IL_IMAGE_WIDTH);
  sprite->total_h = ilGetInteger(IL_IMAGE_HEIGHT);
  sprite->dim_x = dim_x;
  sprite->dim_y = dim_y;
  sprite->w = sprite->total_w / dim_x;
  sprite->h = sprite->total_h / dim_y;
  sprite->frame_count = frame_count ? frame_count : (dim_x * dim_y);
  sprite->surface = calloc(frame_count, sizeof(cairo_surface_t *));
  if (! sprite->surface) {
    warn("cairo_sprite_init: sprite->surface");
    ilDeleteImages(2, il_image);
    str_clean(&sprite->path);
    str_clean(&sprite->real_path);
    return NULL;
  }
  i = 0;
  y = 0;
  while (i < sprite->frame_count && y < dim_y) {
    x = 0;
    while (i < sprite->frame_count && x < dim_x) {
      ilBindImage(il_image[1]);
      ilTexImage(sprite->w, sprite->h, 1, 32, IL_RGBA, IL_UNSIGNED_BYTE,
                 NULL);
      data = ilGetData();
      ilBindImage(il_image[0]);
      ilCopyPixels(x * sprite->w, y * sprite->h, 0,
                   sprite->w, sprite->h, 1, IL_RGBA, IL_UNSIGNED_BYTE,
                   data);
      sprite->surface[i] = cairo_image_surface_create_for_data
        (data, CAIRO_FORMAT_ARGB32, sprite->w, sprite->h, sprite->w * 4);
      i++;
      x++;
    }
    y++;
  }
  ilDeleteImages(2, il_image);
  return sprite;
}
