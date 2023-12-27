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
#ifndef GL_SPRITE_H
#define GL_SPRITE_H

#include "types.h"

/* Stack-allocation compatible functions, call gl_sprite_clean
   after use. */
void          gl_sprite_clean (s_gl_sprite *sprite);
s_gl_sprite * gl_sprite_init (s_gl_sprite *sprite, const s8 *path,
                              uw dim_x, uw dim_y, uw frame_count);

/* Observers. */
void gl_sprite_bind (const s_gl_sprite *sprite, uw frame);
void gl_sprite_render (const s_gl_sprite *sprite, uw frame);

#endif /* GL_SPRITE_H */
