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
#ifndef LIBKC3_WINDOW_EGL_VIRTUAL_TYPES_H
#define LIBKC3_WINDOW_EGL_VIRTUAL_TYPES_H

#include "../../../libkc3/types.h"

typedef struct virtual_context s_virtual_context;

struct virtual_context {
  u8 *buffer;
};

#endif /* LIBKC3_WINDOW_EGL_VIRTUAL_TYPES_H */
