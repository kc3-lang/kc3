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
#ifndef LIBKC3_WINDOW_CAIRO_QUARTZ_H
#define LIBKC3_WINDOW_CAIRO_QUARTZ_H

#include "../../../libkc3/types.h"
#include "../types.h"
#include "../window_cairo.h"

cairo_font_face_t * window_cairo_quartz_load_font (const char *path);
bool                window_cairo_quartz_run (s_window_cairo *window);

#endif /* LIBKC3_WINDOW_CAIRO_XCB_H */
