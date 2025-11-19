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
#ifndef KC3_IMAGE_TYPES_H
#define KC3_IMAGE_TYPES_H

#include "../libkc3/types.h"
#include <EGL/egl.h>
#include "../gl/types.h"

typedef struct image     s_image;
typedef struct image_egl s_image_egl;

/* 1 */

struct image {
  uw w;
  uw h;
  u8 components;
  u8 pixel_size;
  void *data;
};

/* 2 */

struct image_egl {
  s_image image;
  EGLConfig  egl_config;
  EGLContext egl_context;
  EGLDisplay egl_display;
  EGLSurface egl_surface;
};

#endif /* KC3_IMAGE_TYPES_H */
