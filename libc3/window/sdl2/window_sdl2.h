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
#ifndef LIBC3_WINDOW_SDL2_H
#define LIBC3_WINDOW_SDL2_H

#include "types.h"

s_window_sdl2 * window_sdl2_init (s_window_sdl2 *window,
                                  sw x, sw y, uw w, uw h,
                                  const s8 *title,
                                  uw sequence_count);
bool             window_sdl2_run (s_window_sdl2 *window);

s_sequence *     window_sdl2_sequence_init
(s_sequence *sequence, f64 duration, const s8 *title,
 f_window_sdl2_sequence_load load,
 f_window_sdl2_sequence_render render);

/* callbacks */
bool window_sdl2_button_default (s_window_sdl2 *window, u8 button,
                                 sw x, sw y);
bool window_sdl2_key_default (s_window_sdl2 *window, SDL_Keysym *keysym);
bool window_sdl2_load_default (s_window_sdl2 *window);
bool window_sdl2_motion_default (s_window_sdl2 *window, sw x, sw y);
bool window_sdl2_render_default (s_window_sdl2 *window,
                                 SDL_GLContext context);
bool window_sdl2_resize_default (s_window_sdl2 *window, uw w, uw h);

#endif /* LIBC3_WINDOW_SDL2_H */
