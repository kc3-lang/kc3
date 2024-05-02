/* c3
 * Copyright 2022-2024 kmx.io <contact@kmx.io>
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
#ifndef LIBC3_VAR_H
#define LIBC3_VAR_H

#include "types.h"

s_tag * var_init_cast (s_tag *tag, const s_sym * const *type,
                       const s_tag *src);
s_tag * var_init_copy (s_tag *tag, const s_tag *src);

#endif /* LIBC3_VAR_H */
