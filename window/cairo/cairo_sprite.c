/* kc3
 * Copyright from 2022 to 2026 kmx.io <contact@kmx.io>
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
#include "../../libkc3/kc3.h"
#include "cairo_sprite.h"

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
  cairo_set_source_rgb(cr, 0.0, 0.0, 0.0);
}

void cairo_sprite_clean (s_cairo_sprite *sprite)
{
  void *data;
  uw i;
  assert(sprite);
  i = 0;
  while (i < sprite->frame_count) {
    data = cairo_image_surface_get_data(sprite->surface[i]);
    cairo_surface_destroy(sprite->surface[i]);
    alloc_free(data);
    i++;
  }
  alloc_free(sprite->surface);
}

s_cairo_sprite * cairo_sprite_init (s_cairo_sprite *sprite,
                                    const char *path,
                                    uw dim_x, uw dim_y,
                                    uw frame_count)
{
  assert(sprite);
  assert(dim_x);
  assert(dim_y);
  u8 *dest_data;
  uw  dest_stride;
  uw i;
  cairo_surface_t *src;
  u8              *src_data;
  uw               src_stride;
  uw u;
  uw v;
  uw x;
  uw y;
  assert(sprite);
  assert(path);
  assert(dim_x);
  assert(dim_y);
  str_init_copy_1(&sprite->path, path);
  if (! file_search(&sprite->path, sym_1("r"), &sprite->real_path)) {
    err_write_1("cairo_sprite_init: file not found: ");
    err_puts(path);
    str_clean(&sprite->path);
    return NULL;
  }
  src =
    cairo_image_surface_create_from_png(sprite->real_path.ptr.pchar);
  if (! src) {
    err_write_1("cairo_sprite_init: error loading image: ");
    err_puts(sprite->real_path.ptr.pchar);
    str_clean(&sprite->path);
    str_clean(&sprite->real_path);
    return NULL;
  }
  sprite->total_w = cairo_image_surface_get_width(src);
  sprite->total_h = cairo_image_surface_get_height(src);
  sprite->dim_x = dim_x;
  sprite->dim_y = dim_y;
  sprite->w = sprite->total_w / dim_x;
  sprite->h = sprite->total_h / dim_y;
  sprite->frame_count = frame_count ? frame_count : (dim_x * dim_y);
  sprite->surface = calloc(frame_count, sizeof(cairo_surface_t *));
  if (! sprite->surface) {
    err_puts("cairo_sprite_init: sprite->surface:"
             " failed to allocate memory");
    cairo_surface_destroy(src);
    str_clean(&sprite->path);
    str_clean(&sprite->real_path);
    return NULL;
  }
  i = 0;
  y = 0;
  while (i < sprite->frame_count && y < dim_y) {
    x = 0;
    while (i < sprite->frame_count && x < dim_x) {
      src_data = cairo_image_surface_get_data(src);
      src_stride = cairo_format_stride_for_width(CAIRO_FORMAT_ARGB32,
                                                 sprite->total_w);
      dest_stride =
        cairo_format_stride_for_width(CAIRO_FORMAT_ARGB32, sprite->w);
      dest_data = malloc(sprite->h * dest_stride);
      sprite->surface[i] =
        cairo_image_surface_create_for_data(dest_data,
                                            CAIRO_FORMAT_ARGB32,
                                            sprite->w, sprite->h,
                                            dest_stride);
      v = 0;
      while (v < sprite->h) {
        u = 0;
        while (u < sprite->w) {
          u8 *dest_pixel = dest_data + v * dest_stride +
            u * 4;
          u8 *src_pixel = src_data +
            (y * sprite->h + v) * src_stride +
            (x * sprite->w + u) * 4;
          dest_pixel[0] = src_pixel[0];
          dest_pixel[1] = src_pixel[1];
          dest_pixel[2] = src_pixel[2];
          dest_pixel[3] = src_pixel[3];
          u++;
        }
        v++;
      }
      i++;
      x++;
    }
    y++;
  }
  cairo_surface_destroy(src);
  return sprite;
}
