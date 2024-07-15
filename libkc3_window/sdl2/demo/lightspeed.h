/* kc3
 * Copyright 2022,2023,2024 kmx.io <contact@kmx.io>
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
#ifndef LIGHTSPEED_H
#define LIGHTSPEED_H

#include "../types.h"

#define LIGHTSPEED_STAR_MAX (1024 * 1024)
#define LIGHTSPEED_STAR_PROBABILITY 0.005

extern s_gl_lines g_lines_stars;

bool lightspeed_load (s_sequence *seq);
bool lightspeed_render (s_sequence *seq);
bool lightspeed_unload (s_sequence *seq);

#endif /* LIGHTSPEED_H */
