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
#ifndef CAIRO_SPRITE_H
#define CAIRO_SPRITE_H

#include "types.h"
#include <cairo.h>

/* stack allocation compatible functions */
void             cairo_sprite_clean (s_cairo_sprite *sprite);
s_cairo_sprite * cairo_sprite_init (s_cairo_sprite *sprite,
                                    const char *path,
                                    uw dim_x, uw dim_y,
                                    uw frame_count);

/* operations */
void cairo_sprite_blit (const s_cairo_sprite *sprite, uw frame,
                        cairo_t *cr, uw x, uw y);

#endif /* CAIRO_SPRITE_H */
