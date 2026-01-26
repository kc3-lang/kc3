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
#ifndef GL_OBJECT_H
#define GL_OBJECT_H

#include "types.h"

/* Stack-allocation compatible functions, call gl_object_clean
   after use. */
void          gl_object_clean (s_gl_object *object);
s_gl_object * gl_object_init (s_gl_object *object);

/* Heap-allocation functions, call gl_object_delete after use. */
void          gl_object_delete (s_gl_object *object);
s_gl_object * gl_object_new (void);

/* Operators. */
s_gl_object * gl_object_allocate (s_gl_object *object, uw vertex_count,
                                  uw index_count);
void          gl_object_transform (s_gl_object *object,
                                   const s_mat4 *matrix);
bool          gl_object_update (s_gl_object *object);

/* Observers. */
void          gl_object_render (const s_gl_object *object);
void          gl_object_render_points (const s_gl_object *object,
                                       u32 count);
void          gl_object_render_triangles (const s_gl_object *object,
                                          u32 count);
void          gl_object_render_wireframe (const s_gl_object *object);

#endif /* GL_OBJECT_H */
