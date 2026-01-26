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
/** @file types.h
 *  @brief Module C3.Window.Cairo
 *
 *  Struct for all GUI window Cairo graphics operations.
 */
#ifndef LIBKC3_WINDOW_CAIRO_TYPES_H
#define LIBKC3_WINDOW_CAIRO_TYPES_H

#include <cairo.h>

#if ! HAVE_COCOA
#include <cairo-ft.h>
#include <ft2build.h>
#include FT_FREETYPE_H
#endif

#include <png.h>
#include "../../libkc3/types.h"
#include "../types.h"

typedef struct cairo_font   s_cairo_font;
typedef struct cairo_sprite s_cairo_sprite;
typedef struct rgb          s_rgb;
typedef struct rgba         s_rgba;
typedef struct window_cairo s_window_cairo;

/* return false to break event loop */
typedef bool (*f_window_cairo_button) (s_window_cairo *window,
                                       u8 button, s64 x, s64 y);

/* return false to break event loop */
typedef bool (*f_window_cairo_key) (s_window_cairo *window, u32 keysym);

/* return false to break event loop */
typedef bool (*f_window_cairo_load) (s_window_cairo *window);

/* return false to break event loop */
typedef bool (*f_window_cairo_motion) (s_window_cairo *window, s64 x,
                                       s64 y);

/* return false to break event loop */
typedef bool (*f_window_cairo_render) (s_window_cairo *window);

/* return false to break event loop */
typedef bool (*f_window_cairo_resize) (s_window_cairo *window,
                                       u64 w, u64 h);

typedef void (*f_window_cairo_unload) (s_window_cairo *window);

struct cairo_font {
  cairo_font_face_t *cairo_font_face;
#if ! HAVE_COCOA
  FT_Face ft_face;
#endif
  s_str path;
  s_str real_path;
};

struct cairo_sprite {
  s_str path;
  s_str real_path;
  u64 total_w;
  u64 total_h;
  u64 dim_x;
  u64 dim_y;
  u64 frame_count;
  u64 w;
  u64 h;
  //ILuint *il_image;
  cairo_surface_t **surface;
};

struct rgb {
  double r;
  double g;
  double b;
};

struct rgba {
  double r;
  double g;
  double b;
  double a;
};

/* Subtype of s_window. See libkc3/window/types.h */
struct window_cairo {
  s64                   x;
  s64                   y;
  u64                   w;
  u64                   h;
  u64                   pixel_w;
  u64                   pixel_h;
  bool                  fullscreen;
  f_window_cairo_button button;
  f_window_cairo_key    key;
  f_window_cairo_load   load;
  f_window_cairo_motion motion;
  f_window_cairo_render render;
  cairo_t              *cr;
  f_window_cairo_resize resize;
  s_sequence           *seq;
  s_sequence           *sequence;
  u32                   sequence_count;
  u32                   sequence_pos;
  s_tag                 tag; // TODO: move sequence to tag
  const char           *title;
  f_window_cairo_unload unload;
};

#endif /* LIBKC3_WINDOW_CAIRO_TYPES_H */
