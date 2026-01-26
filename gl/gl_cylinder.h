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
#ifndef GL_CYLINDER_H
#define GL_CYLINDER_H

#include "types.h"

s_gl_cylinder * gl_cylinder_init (s_gl_cylinder *cylinder,
                                  u32 segments_u, u32 segments_v);
void            gl_cylinder_render (const s_gl_cylinder *cylinder);

#endif /* GL_CYLINDER_H */
