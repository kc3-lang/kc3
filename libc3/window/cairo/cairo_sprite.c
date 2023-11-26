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
#include "cairo_sprite.h"

void cairo_sprite_blit (const s_cairo_sprite *sprite, uw frame,
                        cairo_t *cr, uw x, uw y)
{
  uw frame_x;
  uw frame_y;
  cairo_matrix_t matrix;
  cairo_pattern_t *pattern;
  assert(sprite);
  assert(frame < sprite->frame_count);
  assert(cr);
  frame %= sprite->frame_count;
  frame_x = frame % sprite->dim_x;
  frame_y = frame / sprite->dim_x;
  frame_x *= sprite->w;
  frame_y *= sprite->h;
  pattern = cairo_pattern_create_for_surface(sprite->surface);
  cairo_matrix_init_translate(&matrix, frame_x, frame_y);
  cairo_pattern_set_matrix(pattern, &matrix);
  cairo_set_source(cr, pattern);
  cairo_rectangle(cr, x, y, sprite->w, sprite->h);
  cairo_fill(cr);
  cairo_pattern_destroy(pattern);
}

void cairo_sprite_clean (s_cairo_sprite *sprite)
{
  assert(sprite);
  cairo_surface_destroy(sprite->surface);
}

s_cairo_sprite * cairo_sprite_init (s_cairo_sprite *sprite,
                                    cairo_surface_t *surface,
                                    uw dim_x, uw dim_y,
                                    uw frame_count)
{
  assert(sprite);
  assert(dim_x);
  assert(dim_y);
  if (! (sprite->surface = surface))
    return NULL;
  sprite->surface_w = cairo_image_surface_get_width(surface);
  sprite->surface_h = cairo_image_surface_get_height(surface);
  sprite->dim_x = dim_x;
  sprite->dim_y = dim_y;
  sprite->w = sprite->surface_w / dim_x;
  sprite->h = sprite->surface_h / dim_y;
  sprite->frame_count = frame_count ? frame_count : (dim_x * dim_y);
  return sprite;
}
