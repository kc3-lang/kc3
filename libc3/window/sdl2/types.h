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

#include <GL/glew.h>
#include <SDL.h>
#include <ft2build.h>
#include FT_FREETYPE_H
#include <png.h>
#include <libc3/types.h>
#include "../types.h"

#define GL_CAMERA_LIGHT_MAX 16          // keep in sync with shader

typedef struct dmat3        s_dmat3;
typedef struct dmat4        s_dmat4;
typedef struct dvec2        s_dvec2;
typedef struct dvec3        s_dvec3;
typedef struct dvec4        s_dvec4;
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
typedef bool (*f_window_sdl2_render) (s_window_sdl2 *window);

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

struct dmat3 {
  f64 xx;
  f64 yx;
  f64 zx;
  f64 xy;
  f64 yy;
  f64 zy;
  f64 xz;
  f64 yz;
  f64 zz;
};

struct dmat4 {
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

struct dvec2 {
  f64 x;
  f64 y;
};

struct dvec3 {
  f64 x;
  f64 y;
  f64 z;
};

struct dvec4 {
  f64 x;
  f64 y;
  f64 z;
  f64 t;
};

struct gl_font {
  FT_Face ft_face;
  s_str path;
  f32 point_size;
  f32 point_per_pixel;
  s_str real_path;
};

struct gl_lines {
  s_array vertex;
  GLuint gl_vao;
  GLuint gl_vbo;
};

struct mat3 {
  f32 xx;
  f32 yx;
  f32 zx;
  f32 xy;
  f32 yy;
  f32 zy;
  f32 xz;
  f32 yz;
  f32 zz;
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

struct gl_object {
  s_array vertex;
  s_array triangle;
  u32 gl_mode;
  u32 gl_vao;
  u32 gl_vbo;
  u32 gl_ebo;
};

struct gl_sprite {
  uw dim_x;
  uw dim_y;
  uw frame_count;
  s_str path;
  uw pix_w;
  uw pix_h;
  f32 pt_w;
  f32 pt_h;
  s_str real_path;
  GLuint *texture;
  uw total_w;
  uw total_h;
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

struct vec2 {
  f32 x;
  f32 y;
};

struct vec3 {
  f32 x;
  f32 y;
  f32 z;
};

struct vec4 {
  f32 x;
  f32 y;
  f32 z;
  f32 t;
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
  s_sequence          *seq;
  s_sequence          *sequence;
  uw                   sequence_count;
  uw                   sequence_pos;
  s_tag                tag; // TODO: move sequence to tag
  const char          *title;
  f_window_sdl2_unload unload;
  uw gl_w;
  uw gl_h;
  float dpi;
  float dpi_w;
  float dpi_h;
};

/* 2 */

struct gl_cylinder {
  s_gl_object object;
  uw segments_u;
  uw segments_v;
};

struct gl_light {
  s_vec4 position; // in camera coordinates
  s_rgb intensity;
};

struct gl_material {
  f32 roughness;
  bool metal;
  s_rgba color;
};

struct gl_sphere {
  s_gl_object object;
  uw segments_u;
  uw segments_v;
};

struct gl_square {
  s_gl_object object;
  uw segments_u;
  uw segments_v;
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

/* 3 */

struct gl_camera {
  f32 aspect_ratio;
  f32 clip_z_far;
  f32 clip_z_near;
  f32 fov_y;
  s_vec3 position;
  s_vec3 rotation;
  s_vec3 scale;
  s_mat4 projection_matrix;
  s_mat4 view_matrix;
  s_mat4 model_matrix;
  GLint  light_count;
  s_vec3 light_pos[GL_CAMERA_LIGHT_MAX];
  s_vec3 light_pos_cam[GL_CAMERA_LIGHT_MAX];
  s_rgb  light_color[GL_CAMERA_LIGHT_MAX];
  GLuint gl_projection_matrix_loc;
  GLuint gl_view_matrix_loc;
  GLuint gl_model_matrix_loc;
  GLuint gl_enable_tex2d_loc;
  GLuint gl_tex2d_loc;
  GLuint gl_light_pos_loc;
  GLuint gl_light_color_loc;
  GLuint gl_material_rough_loc;
  GLuint gl_material_metal_loc;
  GLuint gl_material_color_loc;
  GLuint gl_shader_program;
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

#endif /* LIBC3_WINDOW_SDL2_TYPES_H */
