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
#ifndef LIBKC3_WINDOW_SDL2_SDL2_FONT_H
#define LIBKC3_WINDOW_SDL2_SDL2_FONT_H

#include "types.h"

/* Stack-allocation compatible functions, call sdl2_font_clean after
   use. */
void          sdl2_font_clean (s_sdl2_font *font);
s_sdl2_font * sdl2_font_init (s_sdl2_font *font, const s8 *path);

/* Operators. */
void sdl2_font_set_size (s_sdl2_font *font, u32 size, u32 dpi);

/* Observers */
void sdl2_font_render_text (const s_sdl2_font *font, const s8 *p);

#endif /* LIBKC3_WINDOW_SDL2_SDL2_FONT_H */
