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
/** @file types.h
 *  @brief Module C3.Window.Sdl2
 *
 *  Struct for all GUI window Sdl2 graphics operations.
 */
#ifndef LIBC3_WINDOW_SDL2_TYPES_H
#define LIBC3_WINDOW_SDL2_TYPES_H

#include <SDL.h>
#include <GL/glew.h>
#if defined(__APPLE__)
# include <OpenGL/glcorearb.h>
# include <OpenGL/glu.h>
#else
#endif
#include <FTGL/ftgl.h>
#include <png.h>
#include <libc3/types.h>
#include "../types.h"

typedef struct gl_camera    s_gl_camera;
typedef struct gl_cylinder  s_gl_cylinder;
typedef struct gl_matrix_3d s_gl_matrix_3d;
typedef struct gl_matrix_4d s_gl_matrix_4d;
typedef struct gl_object    s_gl_object;
typedef struct gl_point_2d  s_gl_point_2d;
typedef struct gl_point_3d  s_gl_point_3d;
typedef struct gl_sphere    s_gl_sphere;
typedef struct gl_vertex    s_gl_vertex;
typedef struct sdl2_font    s_sdl2_font;
typedef struct sdl2_sprite  s_sdl2_sprite;
typedef struct rgb          s_rgb;
typedef struct rgba         s_rgba;
typedef struct window_sdl2  s_window_sdl2;

/* return false to break event loop */
typedef bool (*f_window_sdl2_button) (s_window_sdl2 *window,
                                      u8 button, sw x, sw y);

/* return false to break event loop */
typedef bool (*f_window_sdl2_key) (s_window_sdl2 *window,
                                   SDL_Keysym *keysym);

/* return false to break event loop */
typedef bool (*f_window_sdl2_load) (s_window_sdl2 *window);

/* return false to break event loop */
typedef bool (*f_window_sdl2_motion) (s_window_sdl2 *window, sw x,
                                      sw y);

/* return false to break event loop */
typedef bool (*f_window_sdl2_render) (s_window_sdl2 *window,
                                      void *context);

/* return false to break event loop */
typedef bool (*f_window_sdl2_resize) (s_window_sdl2 *window,
                                      uw w, uw h);

typedef bool (*f_window_sdl2_sequence_load) (s_sequence *seq,
                                             s_window_sdl2 *window);

typedef bool (*f_window_sdl2_sequence_render) (s_sequence *seq,
                                               s_window_sdl2 *window,
                                               void *context);

typedef void (*f_window_sdl2_unload) (s_window_sdl2 *window);

/* 1 */
struct gl_point_2d {
  f64 x;
  f64 y;
};

struct gl_point_3d {
  f64 x;
  f64 y;
  f64 z;
};

struct gl_matrix_3d {
  f64 xx;
  f64 xy;
  f64 xz;
  f64 yx;
  f64 yy;
  f64 yz;
  f64 zx;
  f64 zy;
  f64 zz;
};

struct gl_matrix_4d {
  f64 xx;
  f64 xy;
  f64 xz;
  f64 xt;
  f64 yx;
  f64 yy;
  f64 yz;
  f64 yt;
  f64 zx;
  f64 zy;
  f64 zz;
  f64 zt;
  f64 tx;
  f64 ty;
  f64 tz;
  f64 tt;
};

struct rgb {
  f64 r;
  f64 g;
  f64 b;
};

struct rgba {
  f64 r;
  f64 g;
  f64 b;
  f64 a;
};

struct sdl2_font {
  FTGLfont *ftgl_font;
  u32 size;
  u32 dpi;
  s_str path;
  s_str real_path;
};

struct gl_vertex {
  s_gl_point_3d position;
  s_gl_point_3d normal;
  s_gl_point_2d tex_coord;
};

struct gl_object {
  s_array vertex;
  s_array index;
  u32 gl_mode;
  u32 gl_vao;
  u32 gl_vbo;
  u32 gl_ebo;
};

/* Subtype of s_window. See libc3/window/types.h */
struct window_sdl2 {
  sw                   x;
  sw                   y;
  uw                   w;
  uw                   h;
  bool                 fullscreen;
  f_window_sdl2_button button;
  f_window_sdl2_key    key;
  f_window_sdl2_load   load;
  f_window_sdl2_motion motion;
  f_window_sdl2_render render;
  SDL_Window          *sdl_window;
  f_window_sdl2_resize resize;
  s_sequence          *sequence;
  uw                   sequence_count;
  uw                   sequence_pos;
  s_tag                tag; // TODO: move sequence to tag
  const s8            *title;
  f_window_sdl2_unload unload;
  uw gl_w;
  uw gl_h;
  float dpi;
  float dpi_w;
  float dpi_h;
};

/* 2 */
struct gl_camera {
  f64 aspect_ratio;
  f64 clip_z_far;
  f64 clip_z_near;
  f64 fov_y;
  s_gl_point_3d position;
  s_gl_point_3d rotation;
  s_gl_matrix_4d matrix;
  u32 gl_matrix_loc;
  u32 gl_shader_program;
};

struct gl_cylinder {
  s_gl_object object;
  uw segments_u;
  uw segments_v;
};

struct gl_sphere {
  s_gl_object object;
  uw segments_u;
  uw segments_v;
};

struct sdl2_sprite {
  s_gl_object object;
  s_str path;
  s_str real_path;
  uw total_w;
  uw total_h;
  uw dim_x;
  uw dim_y;
  uw frame_count;
  uw w;
  uw h;
  uw tex_w;
  uw tex_h;
  GLuint *texture;
};

#endif /* LIBC3_WINDOW_SDL2_TYPES_H */
