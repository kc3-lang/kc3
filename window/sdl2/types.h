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
/** @file types.h
 *  @brief Module KC3.Window.SDL2
 *
 *  Struct for all GUI window SDL2 graphics operations.
 */
#ifndef LIBKC3_WINDOW_SDL2_TYPES_H
#define LIBKC3_WINDOW_SDL2_TYPES_H

#if defined(WIN32) || defined(WIN64)
# include <windef.h>
#endif

#include <SDL.h>
#include <png.h>
#include "../../gl/types.h"
#include <SDL_opengl.h>
#include "../types.h"

#define GL_CAMERA_LIGHT_MAX 16          // keep in sync with shader

typedef struct dmat3        s_dmat3;
typedef struct dmat4        s_dmat4;
typedef struct dvec2        s_dvec2;
typedef struct dvec3        s_dvec3;
typedef struct dvec4        s_dvec4;
typedef struct gl_box       s_gl_box;
typedef struct gl_camera    s_gl_camera;
typedef struct gl_cylinder  s_gl_cylinder;
typedef struct gl_font      s_gl_font;
typedef struct gl_light     s_gl_light;
typedef struct gl_lines     s_gl_lines;
typedef struct gl_material  s_gl_material;
typedef struct gl_object    s_gl_object;
typedef struct gl_ortho     s_gl_ortho;
typedef struct gl_sphere    s_gl_sphere;
typedef struct gl_sprite    s_gl_sprite;
typedef struct gl_square    s_gl_square;
typedef struct gl_text      s_gl_text;
typedef struct gl_triangle  s_gl_triangle;
typedef struct gl_vertex    s_gl_vertex;
typedef struct mat3         s_mat3;
typedef struct mat4         s_mat4;
typedef struct rgb          s_rgb;
typedef struct rgba         s_rgba;
typedef struct vec2         s_vec2;
typedef struct vec3         s_vec3;
typedef struct vec4         s_vec4;
typedef struct window_sdl2  s_window_sdl2;

/* return false to break event loop */
typedef bool (*f_window_sdl2_button) (s_window_sdl2 *window,
                                      u8 button, s64 x, s64 y);

/* return false to break event loop */
typedef bool (*f_window_sdl2_key) (s_window_sdl2 *window,
                                   SDL_Keysym *keysym);

/* return false to break event loop */
typedef bool (*f_window_sdl2_load) (s_window_sdl2 *window);

/* return false to break event loop */
typedef bool (*f_window_sdl2_motion) (s_window_sdl2 *window, s64 x,
                                      s64 y);

/* return false to break event loop */
typedef bool (*f_window_sdl2_render) (s_window_sdl2 *window);

/* return false to break event loop */
typedef bool (*f_window_sdl2_resize) (s_window_sdl2 *window,
                                      u64 w, u64 h);

typedef bool (*f_window_sdl2_sequence_load) (s_sequence *seq,
                                             s_window_sdl2 *window);

typedef bool (*f_window_sdl2_sequence_render) (s_sequence *seq,
                                               s_window_sdl2 *window,
                                               void *context);

typedef void (*f_window_sdl2_unload) (s_window_sdl2 *window);

/* Subtype of s_window, see window/types.h . */
struct window_sdl2 {
  s64                  x;
  s64                  y;
  u64                  w;
  u64                  h;
  bool                 fullscreen;
  f_window_sdl2_button button;
  f_window_sdl2_key    key;
  f_window_sdl2_load   load;
  f_window_sdl2_motion motion;
  f_window_sdl2_render render;
  SDL_Window          *sdl_window;
  f_window_sdl2_resize resize;
  s_sequence          *seq;
  s_sequence          *sequence;
  u32                  sequence_count;
  u32                  sequence_pos;
  s_tag                tag; // TODO: move sequence to tag
  const char          *title;
  f_window_sdl2_unload unload;
  u64 gl_w;
  u64 gl_h;
  float dpi;
  float dpi_w;
  float dpi_h;
  SDL_GLContext       *context;
};

/* 2 */

struct sdl2_sprite {
  s_gl_object object;
  s_str path;
  s_str real_path;
  u64 total_w;
  u64 total_h;
  u64 dim_x;
  u64 dim_y;
  u64 frame_count;
  u64 w;
  u64 h;
  u64 tex_w;
  u64 tex_h;
  GLuint *texture;
};

#endif /* LIBKC3_WINDOW_SDL2_TYPES_H */
