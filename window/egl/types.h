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
 *  @brief Module KC3.Window.EGL
 *
 *  Struct for all GUI window with EGL graphics operations.
 */
#ifndef LIBKC3_WINDOW_EGL_TYPES_H
#define LIBKC3_WINDOW_EGL_TYPES_H

#include <GL/glew.h>
#include <EGL/egl.h>
#include <GL/gl.h>
#include <ft2build.h>
#include FT_FREETYPE_H
#include <libkc3/types.h>
#include "../types.h"

typedef struct gl_font      s_gl_font;
typedef struct gl_object    s_gl_object;
typedef struct gl_ortho     s_gl_ortho;
typedef struct gl_square    s_gl_square;
typedef struct gl_text      s_gl_text;
typedef struct gl_triangle  s_gl_triangle;
typedef struct gl_vertex    s_gl_vertex;
typedef struct mat4         s_mat4;
typedef struct rgb          s_rgb;
typedef struct rgba         s_rgba;
typedef struct sequence_egl s_sequence_egl;
typedef struct vec3         s_vec3;
typedef struct window_egl   s_window_egl;

/* return false to break event loop */
typedef bool (*f_sequence_egl) (s_sequence_egl *seq);
typedef bool (*f_sequence_egl_button) (s_sequence_egl *seq, u8 button,
                                       s64 x, s64 y);
typedef bool (*f_sequence_egl_key) (s_sequence_egl *seq, u32 keysym);
typedef bool (*f_window_egl_button) (s_window_egl *window,
                                     u8 button, s64 x, s64 y);
typedef bool (*f_window_egl_key) (s_window_egl *window, u32 keysym);
typedef bool (*f_window_egl_load) (s_window_egl *window);
typedef bool (*f_window_egl_motion) (s_window_egl *window, s64 x,
                                     s64 y);
typedef bool (*f_window_egl_render) (s_window_egl *window);
typedef bool (*f_window_egl_resize) (s_window_egl *window,
                                     u64 w, u64 h);
typedef void (*f_window_egl_unload) (s_window_egl *window);

struct rgb {
  f32 r;
  f32 g;
  f32 b;
};

struct rgba {
  f32 r;
  f32 g;
  f32 b;
  f32 a;
};

struct vec3 {
  f32 x;
  f32 y;
  f32 z;
};

struct gl_font {
  FT_Face ft_face;
  s_str path;
  f32 point_size;
  f32 point_per_pixel;
  s_str real_path;
};

struct gl_object {
  s_array vertex;
  s_array triangle;
  u32 gl_mode;
  u32 gl_vao;
  u32 gl_vbo;
  u32 gl_ebo;
};

struct gl_square {
  s_gl_object object;
  u32 segments_u;
  u32 segments_v;
};

struct gl_text {
  const s_gl_font *font;
  uw pix_w;
  uw pix_h;
  f32 pt_w;
  f32 pt_h;
  s_str str;
  GLuint texture;
};

struct gl_triangle {
  GLuint a;
  GLuint b;
  GLuint c;
};

struct gl_vertex {
  f32 pos_x;
  f32 pos_y;
  f32 pos_z;
  f32 normal_x;
  f32 normal_y;
  f32 normal_z;
  f32 tex_coord_x;
  f32 tex_coord_y;
};

struct mat4 {
  f32 xx;
  f32 xy;
  f32 xz;
  f32 xt;
  f32 yx;
  f32 yy;
  f32 yz;
  f32 yt;
  f32 zx;
  f32 zy;
  f32 zz;
  f32 zt;
  f32 tx;
  f32 ty;
  f32 tz;
  f32 tt;
};

struct gl_ortho {
  f32 x1;
  f32 x2;
  f32 y1;
  f32 y2;
  f32 clip_z_near;
  f32 clip_z_far;
  s_vec3 position;
  s_vec3 rotation;
  s_vec3 scale;
  s_mat4 projection_matrix;
  GLuint gl_projection_matrix_loc;
  s_mat4 view_matrix;
  GLuint gl_view_matrix_loc;
  s_mat4 model_matrix;
  GLuint gl_model_matrix_loc;
  GLuint gl_enable_tex2d_loc;
  GLuint gl_tex2d_loc;
  GLuint gl_color_loc;
  GLuint gl_shader_program;
  s_gl_square square;
};

/* Subtype of s_sequence. See libkc3/types.h */
struct sequence_egl {
  s_tag tag;
  f64 dt;
  f64 duration;
  u64 frame;
  f64 t;
  s_timespec t0;
  const char *title;
  s_window_egl *window;
  f_sequence_egl load;
  f_sequence_egl render;
  f_sequence_egl unload;
  f_sequence_egl_button button;
  f_sequence_egl_key key;
};

/* Subtype of s_window. See libkc3/window/types.h */
struct window_egl {
  s64                 x;
  s64                 y;
  u64                 w;
  u64                 h;
  bool                fullscreen;
  f_window_egl_button button;
  f_window_egl_key    key;
  f_window_egl_load   load;
  f_window_egl_motion motion;
  f_window_egl_render render;
  EGLContext          egl_context;
  f_window_egl_resize resize;
  s_sequence_egl     *seq;
  s_sequence_egl     *sequence;
  u32                 sequence_count;
  u32                 sequence_pos;
  s_tag               tag; // TODO: move sequence to tag
  const char         *title;
  f_window_egl_unload unload;
  u64                 gl_w;
  u64                 gl_h;
  EGLDisplay          egl_display;
  EGLConfig           egl_config;
  EGLSurface          egl_surface;
};

#endif /* LIBKC3_WINDOW_EGL_TYPES_H */
