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
#ifndef GL_SPHERE_H
#define GL_SPHERE_H

#include "types.h"

/* Stack-allocation compatible functions, call gl_sphere_clean after
   use. */
void          gl_sphere_clean (s_gl_sphere *sphere);
s_gl_sphere * gl_sphere_init (s_gl_sphere *sphere, uw segments_u,
                              uw segments_v);

/* Heap-allocation functions, call gl_sphere_delete after use. */
void          gl_sphere_delete (s_gl_sphere *sphere);
s_gl_sphere * gl_sphere_new (uw segments_u, uw segments_v);

/* Observers. */
void          gl_sphere_render (const s_gl_sphere *sphere);
void          gl_sphere_render_wireframe (const s_gl_sphere *sphere);

#endif /* GL_SPHERE_H */
