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
#ifndef FLIES_H
#define FLIES_H

#include "../types.h"

extern s_cairo_sprite g_sprite_dead_fly;
extern s_cairo_sprite g_sprite_fly;

bool flies_load (s_sequence *seq);
bool flies_render (s_sequence *seq);
bool flies_unload (s_sequence *seq);

#endif /* FLIES_H */
