/* kc3
 * Copyright from 2022 to 2025 kmx.io <contact@kmx.io>
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
#ifndef EARTH_H
#define EARTH_H

#include "window_egl_demo.h"

extern s_gl_sprite g_sprite_earth;
extern s_gl_sprite g_sprite_earth_night;

u8 earth_load (s_sequence_egl *seq);
u8 earth_render (s_sequence_egl *seq);
u8 earth_unload (s_sequence_egl *seq);

#endif /* EARTH_H */
