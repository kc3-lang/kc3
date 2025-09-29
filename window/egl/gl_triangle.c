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
#include <libkc3/kc3.h>
#include "gl_triangle.h"

s_gl_triangle * gl_triangle_init (s_gl_triangle *triangle, GLuint a,
                                  GLuint b, GLuint c)
{
  assert(triangle);
  triangle->a = a;
  triangle->b = b;
  triangle->c = c;
  return triangle;
}
