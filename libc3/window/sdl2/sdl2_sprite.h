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
#ifndef SDL2_SPRITE_H
#define SDL2_SPRITE_H

#include "types.h"

/* stack allocation compatible functions */
void            sdl2_sprite_clean (s_sdl2_sprite *sprite);
s_sdl2_sprite * sdl2_sprite_init (s_sdl2_sprite *sprite,
                                  const s8 *path,
                                  uw dim_x, uw dim_y,
                                  uw frame_count);

/* operations */
void sdl2_sprite_render (const s_sdl2_sprite *sprite, uw frame,
                         uw x, uw y);

#endif /* CAIRO_SPRITE_H */
