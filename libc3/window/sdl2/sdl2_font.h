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
#ifndef LIBC3_WINDOW_SDL2_SDL2_FONT_H
#define LIBC3_WINDOW_SDL2_SDL2_FONT_H

#include <assert.h>
#include <err.h>
#include "types.h"

/* Stack-allocation compatible functions, call sdl2_font_clean after
   use. */
void          sdl2_font_clean (s_sdl2_font *font);
s_sdl2_font * sdl2_font_init (s_sdl2_font *font, const s8 *path);

/* Operators. */
void sdl2_font_set_size (const s_sdl2_font *font, uw size);

/* Observers */
void sdl2_font_render_text (const s_sdl2_font *font, const s8 *p);

#endif /* LIBC3_WINDOW_SDL2_SDL2_FONT_H */
