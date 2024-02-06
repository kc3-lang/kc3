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
#ifndef GL_CAMERA_H
#define GL_CAMERA_H

#include "types.h"

/* Stack-allocation compatible functions, call gl_camera_clean
   after use. */
void          gl_camera_clean (s_gl_camera *camera);
s_gl_camera * gl_camera_init (s_gl_camera *camera, uw w, uw h);

/* Heap-allocation functions, call gl_camera_delete after use. */
void          gl_camera_delete (s_gl_camera *camera);
s_gl_camera * gl_camera_new (uw w, uw h);

/* Operators. */
void          gl_camera_bind_texture (s_gl_camera *camera,
                                      GLuint texture);
void          gl_camera_material (s_gl_camera *camera,
                                  const s_gl_material *material);
void          gl_camera_render (s_gl_camera *camera);
s_gl_camera * gl_camera_set_aspect_ratio (s_gl_camera *camera, uw w,
                                          uw h);

#endif /* GL_CAMERA_H */
