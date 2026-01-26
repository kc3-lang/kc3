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
#ifndef LIBKC3_WINDOW_SDL2_H
#define LIBKC3_WINDOW_SDL2_H

#include "types.h"

extern const char *g_gray_3_bits_utf8[];

/* Stack-allocation compatible functions, call window_sdl2_clean
   after use. */
void            window_sdl2_clean (s_window_sdl2 *window);
s_window_sdl2 * window_sdl2_init (s_window_sdl2 *window,
                                  s64 x, s64 y, u64 w, u64 h,
                                  const char *title,
                                  u32 sequence_count);

/* Operators. */
bool             window_sdl2_run (s_window_sdl2 *window);

/* Callbacks. */
bool window_sdl2_default_button_cb (s_window_sdl2 *window, u8 button,
                                    s64 x, s64 y);
bool window_sdl2_default_key_cb (s_window_sdl2 *window,
                                 SDL_Keysym *keysym);
bool window_sdl2_default_load_cb (s_window_sdl2 *window);
bool window_sdl2_default_motion_cb (s_window_sdl2 *window,
                                    s64 x, s64 y);
bool window_sdl2_default_render_cb (s_window_sdl2 *window);
bool window_sdl2_default_resize_cb (s_window_sdl2 *window,
                                    u64 w, u64 h);
void window_sdl2_default_unload_cb (s_window_sdl2 *window);

#endif /* LIBKC3_WINDOW_SDL2_H */
