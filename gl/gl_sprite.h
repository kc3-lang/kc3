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
#ifndef GL_SPRITE_H
#define GL_SPRITE_H

#include "types.h"

/* Stack-allocation compatible functions, call gl_sprite_clean
   after use. */
void          gl_sprite_clean (s_gl_sprite *sprite);
s_gl_sprite * gl_sprite_init (s_gl_sprite *sprite, const char *path,
                              uw dim_x, uw dim_y, uw frame_count,
                              f32 point_per_pixel);
s_gl_sprite * gl_sprite_init_jpeg (s_gl_sprite *sprite,
                                   const char *path,
                                   uw dim_x, uw dim_y, uw frame_count,
                                   f32 point_per_pixel);
s_gl_sprite * gl_sprite_init_png (s_gl_sprite *sprite, const char *path,
                                  uw dim_x, uw dim_y, uw frame_count,
                                  f32 point_per_pixel);

/* Observers. */
u32 gl_sprite_texture (const s_gl_sprite *sprite, uw frame);

#endif /* GL_SPRITE_H */
