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
#ifndef FLIES_H
#define FLIES_H

#include "demo.h"

extern s_gl_font   g_font_flies;
extern s_gl_sprite g_sprite_dead_fly;
extern s_gl_sprite g_sprite_fly;

u8 flies_load (s_sequence *seq);
u8 flies_render (s_sequence *seq);
u8 flies_unload (s_sequence *seq);

#endif /* FLIES_H */
