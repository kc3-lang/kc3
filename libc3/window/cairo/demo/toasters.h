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
#ifndef TOASTERS_H
#define TOASTERS_H

#include "../types.h"

extern s_cairo_sprite g_sprite_toast;
extern s_cairo_sprite g_sprite_toaster;

bool toasters_load (s_sequence *seq, s_window_cairo *window);
bool toasters_render (s_sequence *seq, s_window_cairo *window,
                      cairo_t *cr);

#endif /* TOASTERS_H */
