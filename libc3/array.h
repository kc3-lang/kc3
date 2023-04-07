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
#ifndef ARRAY_H
#define ARRAY_H

#include "types.h"

void      array_clean (s_array *a);
s_array * array_copy (const s_array *src, s_array *dest);
s_array * array_init (s_array *a, e_tag_type type, uw dimension,
                      const uw *sizes);
void *    array_data (const s_array *a, const uw *address);

#endif /* ARRAY_H */
